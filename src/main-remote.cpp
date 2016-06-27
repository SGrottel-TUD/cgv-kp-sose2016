#include <iostream>

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <WS2tcpip.h>


int main(int argc, char **argv)
{
    WSADATA WsaDat;
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
    {
        std::cout << "WSA Initialization failed!" << std::endl;
        WSACleanup();
        return false;
    }
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    //service.sin6_addr = in6addr_any;
    service.sin_port = htons(55500);
    //service.sin_scope_id = 0;

    while (true)
    {
        if (connect(sock, (SOCKADDR*)(&service), sizeof(service)) != 0)
        {
            std::cout << "Failed to establish connection with server" <<
                WSAGetLastError() << std::endl;
            //return 0;
        }
        else
            break;
        Sleep(1000);
    }
    while (true)
    {
        // Display message from server
        char buffer[1000];
        memset(buffer, 0, 999);
        int inDataLength = recv(sock, buffer, 1000, 0);
        std::cout << buffer;
    }

    // Shutdown our socket
    shutdown(sock, SD_BOTH);

    // Close our socket entirely
    closesocket(sock);

    // Cleanup Winsock
    WSACleanup();
}