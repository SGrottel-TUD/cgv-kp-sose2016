#pragma once
#include "abstract_renderer.hpp"
#include "data/world.hpp"

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
        remote_renderer_server(const ::cgvkp::data::world& data);
        virtual ~remote_renderer_server();
        virtual void render();
        virtual void render(const window& wnd);

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();
    private:
        message_header header;
        SOCKET sock, client_sock;
    };
}
}