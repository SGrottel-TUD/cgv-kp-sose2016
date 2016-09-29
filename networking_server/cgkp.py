#/usr/bin/env python
# -*- coding: utf-8 -*-

from twisted.internet import protocol
from twisted.python import log
from collections import deque

import TwistedWebsocket.server

class ProxyProtocol(protocol.Protocol):
	def __init__(self, parent):
		self.server = parent.server
		self.peer = None
	def dataReceived(self, data):
		if self.peer is None and Factories[not self.server].children:
			self.peer = Factories[not self.server].children.popleft()
			Factories[self.server].children.remove(self)
			self.peer.peer = self
		if self.peer is not None:
			self.peer.transport.write(data)
		if self.server and Factories.get('web', False):
			Factories['web'].write(data)
	def connectionLost(self, reason):
		while self in Factories[self.server].children:
			Factories[self.server].children.remove(self)
		if self in Factories[self.server].allChildren:
			Factories[self.server].allChildren.remove(self)
		if self.peer is not None:
			self.peer.peer = None
			Factories[not self.server].children.append(self.peer)
			self.peer = None

Factories = dict()
class ProxyProtocolFactory(protocol.ServerFactory):
	def __init__(self, server = False):
		self.children = deque()
		self.allChildren = set()
		self.server = server
		Factories[self.server] = self
 
	def buildProtocol(self, addr):
		p = ProxyProtocol(self)
		self.children.append(p)
		self.allChildren.add(p)
		if self.server:
			log.msg('Server: ' + str(addr))
		else:
			log.msg('Client: ' + str(addr))
		return p

class WebSocketClientProtocol(TwistedWebsocket.server.Protocol):
	def __init__(self, parent):
		TwistedWebsocket.server.Protocol.__init__(self, parent.users)
		self.parent = parent
	def onConnect(self):
		log.msg('WebClient connected')
	def onDisconnect(self):
		log.msg('WebClient DISconnected')
	def onMessage(self, msg):
		log.msg(type(msg), len(msg))
		if Factories[True].allChildren and len(msg) > 2:
			list(Factories[True].allChildren)[0].transport.write(msg)
		
class WebClientFactory(protocol.Factory):
	def __init__(self):
		self.users = dict()
		Factories['web'] = self
	def buildProtocol(self, addr):
		return WebSocketClientProtocol(self)
	def write(self, data):
		for u in self.users.values():
			#pass
			u.sendMessage(data, binary = True)

PortServer = 6600
PortClient = 6601
PortWebSocket = 6602

def main(ssl_cert = False):
	from twisted.application import internet, service
	application = service.Application("CGKP_ProxyProtocol")
	services = service.IServiceCollection(application)

	serverProxy = ProxyProtocolFactory(server = True)
	clientProxy = ProxyProtocolFactory()

	AppServiceServer = internet.TCPServer(PortServer, serverProxy)
	AppServiceClient = internet.TCPServer(PortClient, clientProxy)

	if ssl_cert:
		try:
			from twisted.internet import ssl
		except ImportError as ex:
			print("This needs pyOpenSSL, run:\n")
			print("pip install pyOpenSSL\n")
			import sys
			sys.exit(1)
		with open(ssl_cert) as kf:
			cert = ssl.PrivateCertificate.loadPEM(kf.read())
		AppServiceWebSocket = internet.SSLServer(
			PortWebSocket, WebClientFactory(), cert.options())
	else:
		AppServiceWebSocket = internet.TCPServer(
			PortWebSocket, WebClientFactory())
	AppServiceServer.setServiceParent(services)
	AppServiceClient.setServiceParent(services)
	AppServiceWebSocket.setServiceParent(services)

	return application

if __name__ == "__main__":
	from optparse import OptionParser
	opts = OptionParser()
	opts.add_option("--ssl", "-s", help="Path to SSL certificate.")

	options, arguments = opts.parse_args()

	application = main(options.ssl)

	from twisted.application.app import startApplication
	startApplication(application, False)
	from twisted.internet import reactor
	reactor.run()
else:
	application = main()
