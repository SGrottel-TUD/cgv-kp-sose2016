#include <iostream>

#include "data/world_remote.hpp"
#include "rendering/debug_renderer.hpp"
#include "rendering/debug_user_input.hpp"
#include "vision/abstract_vision.hpp"
#include "rendering/window.hpp"

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <WS2tcpip.h>

#include <vector>

cgvkp::data::world_remote data;

int main(int argc, char **argv)
{
    data.init();
    data.get_config().set_size(4.0f, 3.0f); // 4x3 meter game area
    data.get_config().set_positional_epsilon(0.1f); // 10cm positional precision (used for hand matching)
    data.set_game_mode(cgvkp::data::world::game_mode::running);

    std::cout << "Size of message" << sizeof(cgvkp::data::message_header) << std::endl;
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
    // Turn non-blocking
    u_long iMode = 1;
    ioctlsocket(sock, FIONBIO, &iMode);
    // Set up target address
    struct sockaddr_in tunnel_addr;
    tunnel_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "46.19.33.155", &tunnel_addr.sin_addr.s_addr); // evilham.com
    tunnel_addr.sin_port = htons(6601);

    while (true)
    {
        if (connect(sock, (SOCKADDR*)(&tunnel_addr), sizeof(tunnel_addr)) != 0)
        {
            int eCode = WSAGetLastError();
            if (eCode == 10035) // Would block, it's normal
                break;
            std::cout << "Failed to establish connection with server" <<
                WSAGetLastError() << std::endl;
            //return 0;
        }
        else
            break;
        Sleep(1000);
    }

    if (::glfwInit() != GL_TRUE) {
        std::cerr << "glfwInit failed" << std::endl;
        return false;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    // create debug window
    auto debug_window = std::make_shared<cgvkp::rendering::window>(1280, 720, "CGV KP SoSe2016 - Remote Debug");
    std::shared_ptr<cgvkp::rendering::debug_renderer> debug_renderer;
    if (!debug_window || !debug_window->is_alive()) {
        debug_window.reset();
    }
    else {
        debug_renderer = std::make_shared<cgvkp::rendering::debug_renderer>(data);
        if (!debug_renderer || !debug_renderer->init(*debug_window)) {
            std::cout << "Failed to create Debug renderer" << std::endl;
            debug_renderer.reset();
            debug_window.reset();
        }
    }
    
    auto debug_vision = std::make_shared<cgvkp::rendering::debug_user_input>(data.get_input_layer());
    std::shared_ptr<cgvkp::vision::abstract_vision> vision = debug_vision;
    if (debug_window)
    {
        debug_window->set_user_input_object(debug_vision);
    }

    if (vision)
    {
        if (!vision->init()) {
            std::cout << "Failed to create Vision component" << std::endl;
            vision.reset();
        }
    }


    cgvkp::data::message_header header;
    uint64_t last_sim_frame = 0;
    while (debug_window) {

        std::chrono::duration<double> game_time_seconds(std::chrono::high_resolution_clock::now() - start_time);
        uint64_t game_time_sim_frames = static_cast<uint64_t>(game_time_seconds.count() * data.get_config().simulation_fps());

        // 0) Update with remote data
        int total_read_bytes = 0; int target_bytes = sizeof(cgvkp::data::message_header);
        int read_bytes = recv(sock, (char*)(&header), target_bytes, 0);
        if (read_bytes > 0) // Got some data, ensure we read whole frame before continuing
        {
            total_read_bytes = read_bytes;
            while (total_read_bytes < target_bytes)
            {
                read_bytes = recv(sock, (char*)(&header) + total_read_bytes, target_bytes - total_read_bytes, 0);
                if (read_bytes > 0)
                    total_read_bytes += read_bytes;
            }
            std::cout << "read: " << total_read_bytes << " " << header << std::endl;

            //std::vector<cgvkp::data::input_layer::hand> hands;
            /*
            auto &hands = data.get_input_layer().buffer();
            hands.resize(header.hands_count);
            total_read_bytes = 0; target_bytes = sizeof(cgvkp::data::input_layer::hand) * header.hands_count;
            while (total_read_bytes < target_bytes)
            {
                read_bytes = recv(sock, (char*)(hands.data()) + total_read_bytes, target_bytes, 0);
                if (read_bytes > 0)
                    total_read_bytes += read_bytes;
            }
            data.get_input_layer().sync_buffer();
            data.get_hands().clear();
            data.get_stars().reserve(header.hands_count);
            for (auto const &hand : hands)
            {
                auto h = std::make_shared<cgvkp::data::world::hand>();
                h->height = hand.h;
                h->x = hand.x;
                h->y = hand.y;
                data.get_hands().push_back(h);
            }*/

            std::vector<cgvkp::data::world::star> stars;
            stars.resize(header.stars_count);
            total_read_bytes = 0; target_bytes = sizeof(cgvkp::data::world::star) * header.stars_count;
            while (total_read_bytes < target_bytes)
            {
                read_bytes = recv(sock, (char*)(stars.data()), target_bytes - total_read_bytes, 0);
                if (read_bytes > 0)
                    total_read_bytes += read_bytes;
            }
            data.get_stars().clear();
            data.get_stars().reserve(header.stars_count);
            for (auto const &star : stars)
            {
                data.get_stars().push_back(std::make_shared<cgvkp::data::world::star>(star));
            }

            data.get_config().set_size(header.w, header.h);
            data.get_config().set_positional_epsilon(header.pos_eps);
            data.set_score(header.score);
            data.set_game_mode(header.game_mode);
        }

        // 1) vision in the main thread
        if (vision) {
            vision->update();
        }

        // 2') send vision data to server
        auto &hands = data.get_input_layer().buffer();
        auto hands_count = hands.size();
        if (hands_count > 0)
        {
            auto hands_size = hands_count * sizeof(cgvkp::data::input_layer::hand);
            int buf_size = static_cast<int>(sizeof(size_t) + hands_size);
            char *buf = static_cast<char*>(malloc(buf_size));
            memcpy(&buf[0], &hands_count, sizeof(size_t));
            memcpy(&buf[sizeof(size_t)], hands.data(), hands_size);
            send(sock, buf, buf_size, 0);
            std::cout << "\t sending " << buf_size << " h: " << hands.size() << std::endl;
            int nError = WSAGetLastError();
            if (nError != WSAEWOULDBLOCK && nError != 0)
            {
                std::cout << "Winsock error code: " << nError << std::endl;
            }
            free(buf);
        }

        // Release buffer

        // 2) updating data model
        data.merge_input(); // merge input from the input_layer (vision) into the data model

        // 3) and now for the rendering
        if (debug_window)
        {
            if (debug_window->is_alive())
            {
                debug_window->do_events();
                debug_renderer->render(*debug_window);
                debug_window->swap_buffers();
            }
            else
            {
                if (debug_renderer)
                {
                    debug_window->make_current();
                    debug_renderer->deinit();
                    debug_renderer.reset();
                }
                debug_window.reset();
            }
        }
    }

    if (vision) {
        vision->deinit();
        vision.reset();
    }

    // Shutdown our socket
    shutdown(sock, SD_BOTH);

    // Close our socket entirely
    closesocket(sock);

    // Cleanup Winsock
    WSACleanup();
}