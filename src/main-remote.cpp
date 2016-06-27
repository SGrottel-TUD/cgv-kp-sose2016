#include <iostream>

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <WS2tcpip.h>

#include <vector>

struct message_header {
    float w;
    float h;
    float pos_eps;
    uint32_t score;
    uint32_t hands_count;
    uint32_t stars_count;
};

struct message_hand {
    unsigned int id;    // unique hand id
    float x, y;         // position (meter)
    float height;       // height of hand over ground (meter)
    unsigned int star_id;      // pointer to the star in hand (or nullptr)
};

struct star {
    unsigned int id;    // unique star id
    float x, y;         // position (meter)
    float height;       // height of star over ground (meter)
    float dx, dy;       // movement direction
    bool in_hand;       // true if star is captured in a hand
};


int main(int argc, char **argv)
{
    std::cout << "Size of message" << sizeof(message_header) << std::endl;
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
    // Set up target address
    struct sockaddr_in tunnel_addr;
    tunnel_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "46.19.33.155", &tunnel_addr.sin_addr.s_addr); // evilham.com
    tunnel_addr.sin_port = htons(6601);

    while (true)
    {
        if (connect(sock, (SOCKADDR*)(&tunnel_addr), sizeof(tunnel_addr)) != 0)
        {
            std::cout << "Failed to establish connection with server" <<
                WSAGetLastError() << std::endl;
            //return 0;
        }
        else
            break;
        Sleep(1000);
    }

    message_header header;
    while (true)
    {
        // Display message from server
        //memset(&header, 0, sizeof(message_header));
        int inDataLength = recv(sock, (char*)(&header), sizeof(message_header), 0);
        std::cout << "read: " << inDataLength <<
            " score: " << header.score <<
            " hands: " << header.hands_count << " stars: " << header.stars_count <<
            " w: " << header.w << " h: " << header.h << " eps" << header.pos_eps << std::endl;
        std::vector<message_hand> hands;
        hands.reserve(header.hands_count);
        inDataLength = recv(sock, (char*)(hands.data()), sizeof(message_hand) * header.hands_count, 0);
        std::vector<star> stars;
        stars.reserve(header.stars_count);
        inDataLength = recv(sock, (char*)(stars.data()), sizeof(star) * header.stars_count, 0);
    }

    // Shutdown our socket
    shutdown(sock, SD_BOTH);

    // Close our socket entirely
    closesocket(sock);

    // Cleanup Winsock
    WSACleanup();
}