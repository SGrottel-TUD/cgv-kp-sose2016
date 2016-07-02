#include "remote_renderer_server.hpp"
#include "data/input_layer.hpp"

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
    header.game_mode = data.get_game_mode();
    header.pos_eps = data.get_config().positional_epsilon();
    header.score = static_cast<uint32_t>(data.get_score());
    std::vector <data::world::star> stars;
    auto data_stars = data.get_stars();
    stars.reserve(data_stars.size());
    for (data::world::star_ptr const &star : data_stars)
    {
        data::world::star s;
        memcpy(&s, star.get(), sizeof(data::world::star));
        stars.push_back(s);
    }
    header.stars_count = static_cast<uint32_t>(stars.size());
    // Body: stars
    size_t stars_size = sizeof(data::world::star) * stars.size();
    int buf_size = static_cast<int>(sizeof(data::message_header) + stars_size);
    char *buf = static_cast<char*>(malloc(buf_size));
    std::memcpy(&buf[0], &header, sizeof(data::message_header));
    std::memcpy(&buf[sizeof(data::message_header)], stars.data(), stars_size);

#if 0
    std::cout << "Sending: " << header << std::endl;
#endif

    // Do send
    send(tunnel, buf, buf_size, 0);
    // Release buffer
    free(buf);
    int nError = WSAGetLastError();
    if (nError != WSAEWOULDBLOCK && nError != 0)
    {
        if (!is_disconnected)
        {
            std::cout << "Winsock error code: " << nError << std::endl;
            is_disconnected = true;
        }

        close_tunnel();
    }
    else
        is_disconnected = false;
}
bool remote_renderer_server::update(data::input_layer &input_layer)
{
    if (tunnel == INVALID_SOCKET)
        return false;
    uint32_t hands_count;
    int total_read_bytes = 0; int target_bytes = sizeof(uint32_t);
    int read_bytes = recv(tunnel, (char*)(&hands_count), target_bytes, 0);
    // We also use a while, to discard any in-between frames.
    while (read_bytes > 0) // Got some data, ensure we read whole frame before continuing
    {
        total_read_bytes = read_bytes;
        while (total_read_bytes < target_bytes)
        {
            read_bytes = recv(tunnel, (char*)(&hands_count) + total_read_bytes, target_bytes - total_read_bytes, 0);
            if (read_bytes > 0)
                total_read_bytes += read_bytes;
        }
#if 0
        std::cout << "read: " << total_read_bytes << " hands: " << hands_count << std::endl;
#endif

        if (hands_count > 0)
        {
            hands.resize(hands_count);
            total_read_bytes = 0; target_bytes = static_cast<int>(sizeof(cgvkp::data::input_layer::hand) * hands_count);
            while (total_read_bytes < target_bytes)
            {
                read_bytes = recv(tunnel, (char*)(hands.data()) + total_read_bytes, target_bytes - total_read_bytes, 0);
                if (read_bytes > 0)
                    total_read_bytes += read_bytes;
            }
        }
        else
            hands.clear();

        total_read_bytes = 0; target_bytes = sizeof(uint32_t);
        read_bytes = recv(tunnel, (char*)(&hands_count), target_bytes, 0);
    }
    if (input_layer.buffer().size() > 0)
    {
        // TODO: Remove this check if debug_user_input gets removed from the main pipeline
        if (!debug_warning && hands.size() > 0)
        {
            std::cout << "Warning: ignoring remote_vision since debug passed data" << std::endl;
            debug_warning = true;
        }
        return false;
    }
    debug_warning = false;
    input_layer.buffer().resize(hands.size());
    memcpy(input_layer.buffer().data(), hands.data(), hands.size() * sizeof(data::input_layer::hand));
    input_layer.sync_buffer();
    return true;
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
        if (eCode != WSAEWOULDBLOCK) // Would block, it's normal
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