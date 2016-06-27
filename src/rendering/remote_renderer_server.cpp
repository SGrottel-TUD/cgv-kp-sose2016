#include "remote_renderer_server.hpp"

#include <iostream>

namespace cgvkp {
namespace rendering {
remote_renderer_server::remote_renderer_server(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
    header(), tunnel(INVALID_SOCKET), tunnel_addr() {}
remote_renderer_server::~remote_renderer_server() {}
void remote_renderer_server::render()
{
    if (tunnel == INVALID_SOCKET)
    {
        setup_tunnel();
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
    send(tunnel, buf, buf_size, 0);
    // Release buffer
    free(buf);
    int nError = WSAGetLastError();
    if (nError != WSAEWOULDBLOCK && nError != 0)
    {
        std::cout << "Winsock error code: " << nError << std::endl;

        close_tunnel();
    }
}
void remote_renderer_server::render(window const & wnd)
{
    return render();
}
bool remote_renderer_server::init_impl(const window& wnd) {
    // Set up target address
    tunnel_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "46.19.33.155", &tunnel_addr.sin_addr.s_addr); // evilham.com
    tunnel_addr.sin_port = htons(6600);

    return true;
}
bool remote_renderer_server::setup_tunnel()
{
    WSADATA WsaDat;
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
    {
        std::cout << "WSA Initialization failed!" << std::endl;
        WSACleanup();
        return false;
    }
    // And regular socket
    tunnel = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tunnel == INVALID_SOCKET)
    {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }
    // Turn non-blocking
    u_long iMode = 1;
    ioctlsocket(tunnel, FIONBIO, &iMode);
    // Connect
    if (connect(tunnel, (sockaddr*)&tunnel_addr, sizeof(tunnel_addr)) == SOCKET_ERROR)
    {
        int eCode = WSAGetLastError();
        if (eCode != 10035) // Would block, it's normal
        {
            std::cout << "Could not connect socket: " << eCode << std::endl;
            close_tunnel();
            return false;
        }
    }
    return true;
}
void remote_renderer_server::deinit_impl()
{
    close_tunnel();
}
void remote_renderer_server::close_tunnel()
{
    if (tunnel != INVALID_SOCKET)
    {
        shutdown(tunnel, SD_BOTH);
        closesocket(tunnel);
        tunnel = INVALID_SOCKET;
        WSACleanup();
    }
}

}
}