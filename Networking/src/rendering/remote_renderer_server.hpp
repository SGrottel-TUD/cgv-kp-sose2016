#pragma once
#include "abstract_renderer.hpp"
#include "vision/abstract_vision.hpp"
#include "data/world_remote.hpp"

#include <chrono>

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <WS2tcpip.h>

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {
    class remote_renderer_server : public abstract_renderer {
    public:
        remote_renderer_server(const ::cgvkp::data::world& data);
        virtual ~remote_renderer_server();
        virtual void render();
        inline virtual void render(const window& wnd) { render(); }

        bool update(data::input_layer &input_layer);
    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();
    private:
        bool setup_tunnel();
        void close_tunnel();
        data::message_header header;
        struct sockaddr_in tunnel_addr;
        SOCKET tunnel;
        bool is_disconnected = false;
        bool debug_warning = false;
        std::vector<data::input_layer::hand> hands;
    };
}
}