#include "remote_renderer_server.hpp"

#include <iostream>

namespace cgvkp {
namespace rendering {
remote_renderer_server::remote_renderer_server(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
    sock(INVALID_SOCKET), client_sock(INVALID_SOCKET) {}
remote_renderer_server::~remote_renderer_server() {}
void remote_renderer_server::render()
{
    if (sock == INVALID_SOCKET)
        return;
    if (client_sock == INVALID_SOCKET || client_sock == SOCKET_ERROR)
    {
        client_sock = accept(sock, nullptr, nullptr);
        // If we got a valid socket, act upon it next frame.
        // Connection may not be complete on both ends.
        return;
    }
    char *msg = "\nWhatuuup\n";
    send(client_sock, msg, strlen(msg), 0);
    int nError = WSAGetLastError();
    if (nError != WSAEWOULDBLOCK&&nError != 0)
    {
        std::cout << "Winsock error code: " << nError << "\r\n";
        std::cout << "Client disconnected!\r\n";

        // Shutdown our socket
        shutdown(client_sock, SD_SEND);

        // Close our socket entirely
        closesocket(client_sock);
        client_sock = INVALID_SOCKET;
    }
}
void remote_renderer_server::render(window const & wnd)
{
    return render();
}
bool remote_renderer_server::init_impl(const window& wnd) {
    WSADATA WsaDat;
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
    {
        std::cout << "WSA Initialization failed!" << std::endl;
        WSACleanup();
        return false;
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }
    // Turn non-blocking
    u_long iMode = 1;
    ioctlsocket(sock, FIONBIO, &iMode);
    // Bind address
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    //inet_pton(AF_INET6, "::1", &service.sin_addr.s_addr);
    service.sin_addr = in4addr_any;//in6addr_any;
    service.sin_port = htons(55500);
    //service.sin6_scope_id = 0;

    if (bind(sock, (SOCKADDR *)&service, sizeof(service)) != 0)
    {
        std::cout << "bind() failed with error " <<
            WSAGetLastError() << std::endl;
        sock = INVALID_SOCKET;
        WSACleanup();
        return false;
    }
    if (listen(sock, 1) != 0) // Limit to one client
    {
        std::cout << "Could not listen to socket" << std::endl;
        sock = INVALID_SOCKET;
        WSACleanup();
        return false;
    }

    return true;
}
void remote_renderer_server::deinit_impl()
{
    if (client_sock != INVALID_SOCKET)
    {
        shutdown(client_sock, SD_BOTH);
        closesocket(client_sock);
        client_sock = INVALID_SOCKET;
    }
    if (sock != INVALID_SOCKET)
    {
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        sock = INVALID_SOCKET;
        WSACleanup();
    }

}

}
}