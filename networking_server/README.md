# Networking

This code exists to enable rendering and vision to run in different computers.

# Installing:

* Start a python virtualenv: `virtualenv $DIR`
* Activate the virtualenv: `source $DIR/bin/activate`
* Install requirements: `pip -r $DIR/requirements.txt`
* If SSL is wanted, install pyOpenSSL: `pip install pyOpenSSL`
* Run the program with: `python cgvkp.py [--ssl $SSL_CERT.pem]`

## `cgvkp.py`
Uses the twisted networking framework to bridge rendering server and vision client.

Opens three TCP ports:
* *Server Port*: where the rendering program sends star position data and reads hand data sent from a vision client.
* *Client port*: where a vision client gets star position data and sends hand data.
* *WebSocket port*: where browsers that load the site currently hosted on https://evilham.com/kp interface with the rendering server as a vision client.

Passing an SSL certificate PEM bundle with `--ssl` will result in a https compatible WebSocket.

## `web`
A proof-of-concept built on ThreeJS and using WebSockets; multiple clients are easily implementable.

These files are static and can be served by any web server. WebSocket address must be adapted.
