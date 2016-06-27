#include "remote_renderer_server.hpp"

#include <iostream>

namespace cgvkp {
namespace rendering {
remote_renderer_server::remote_renderer_server(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
    header(),  sock(INVALID_SOCKET), client_sock(INVALID_SOCKET) {}
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