#include "remote_renderer_server.hpp"

#include <iostream>

namespace cgvkp {
namespace rendering {
remote_renderer_server::remote_renderer_server(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
    header(),  sock(INVALID_SOCKET), client_sock(INVALID_SOCKET), mc_sock(INVALID_SOCKET) {}
remote_renderer_server::~remote_renderer_server() {}
void remote_renderer_server::render()
{
    if (sock == INVALID_SOCKET)
        return;
    if (client_sock == INVALID_SOCKET || client_sock == SOCKET_ERROR)
    {
        client_sock = accept(sock, nullptr, nullptr);
        if (client_sock == INVALID_SOCKET || client_sock == SOCKET_ERROR)
        {
            // If we still don't have a valid client, multicast our address after a while
            auto now_time = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(now_time - last_multicast).count();
            if (elapsed >= 5.0)
            {
                // Do multicast
                send_multicast();
                last_multicast = now_time;
            }
        }
        // If we got a valid socket, act upon it next frame.
        // Connection may not be complete on both ends.
        return;
    }
    // Prepare message
    header.w = data.get_config().width();
    header.h = data.get_config().height();
    header.pos_eps = data.get_config().positional_epsilon();
    header.score = static_cast<uint32_t>(data.get_score());
    auto data_hands = data.get_hands();
    std::vector<message_hand> hands;
    hands.reserve(data_hands.size());
    for (data::world::hand_ptr const &hand : data_hands)
    {
        message_hand h;
        h.id = hand->id;
        h.x = hand->x;
        h.y = hand->y;
        h.star_id = (hand->star) ? hand->star->id : -1;
        hands.push_back(h);
    }
    auto stars = data.get_stars();
    header.hands_count = static_cast<uint32_t>(hands.size());
    header.stars_count = static_cast<uint32_t>(stars.size());
    // Body: hands + stars
    size_t hands_size = sizeof(message_hand) * hands.size();
    size_t stars_size = sizeof(data::world::star) * stars.size();
    int buf_size = static_cast<int>(sizeof(message_header) + hands_size + stars_size);
    char *buf = static_cast<char*>(malloc(buf_size));
    std::memcpy(&buf[0], &header, sizeof(message_header));
    std::memcpy(&buf[sizeof(message_header)], hands.data(), hands_size);
    std::memcpy(&buf[sizeof(message_header) + hands_size], stars.data(), stars_size);

    // Do send
    send(client_sock, buf, buf_size, 0);
    // Release buffer
    free(buf);
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
void remote_renderer_server::send_multicast()
{
    char hostname[128];
    gethostname(hostname, 128);
    struct addrinfo *addrs;
    getaddrinfo(hostname, nullptr, nullptr, &addrs);
    char n[32];

    for (struct addrinfo *p = addrs; p != nullptr; p = p->ai_next)
    {
        if (p->ai_family != AF_INET)
            continue;
        inet_ntop(p->ai_family, &((struct sockaddr_in *)p->ai_addr)->sin_addr, n, sizeof(n));
        std::cout << "  - " << n << std::endl;
    }
    char buf[] = "hey hooo";
    sendto(mc_sock, buf, sizeof(buf), 0, (SOCKADDR *)& mc_addr, sizeof(mc_addr));
}
bool remote_renderer_server::init_impl(const window& wnd) {
    WSADATA WsaDat;
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
    {
        std::cout << "WSA Initialization failed!" << std::endl;
        WSACleanup();
        return false;
    }
    // Create multicast socket
    mc_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // And regular socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET || mc_sock == INVALID_SOCKET)
    {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }
    // Turn non-blocking
    u_long iMode = 1;
    ioctlsocket(mc_sock, FIONBIO, &iMode);
    ioctlsocket(sock, FIONBIO, &iMode);

    // Bind address
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    //inet_pton(AF_INET6, "::1", &service.sin_addr.s_addr);
    service.sin_addr = in4addr_any;//in6addr_any;
    service.sin_port = htons(55501);
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
    int namelen = sizeof(sock_name);
    if (getsockname(sock, (struct sockaddr *)&sock_name, &namelen) != 0)
    {
        deinit_impl();
        WSACleanup();
        return false;
    }
    char n[32];
    inet_ntop(sock_name.sin_family, &sock_name.sin_addr, n, sizeof(n));
    port = ntohs(sock_name.sin_port);
    std::cout << "- Listening on: " << n << ":" << port << std::endl;

    mc_addr.sin_family = AF_INET;
    mc_addr.sin_port = htons(mc_port);
    inet_pton(AF_INET, "239.77.7.7", &mc_addr.sin_addr);

    return true;
}
void remote_renderer_server::deinit_impl()
{
    if (mc_sock != INVALID_SOCKET)
    {
        shutdown(mc_sock, SD_BOTH);
        closesocket(mc_sock);
        mc_sock = INVALID_SOCKET;
    }
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