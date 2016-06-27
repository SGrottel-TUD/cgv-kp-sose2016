#pragma once
#include "abstract_renderer.hpp"

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
        virtual void render(const window& wnd);

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();
    private:
        SOCKET sock, client_sock;
    };
}
}