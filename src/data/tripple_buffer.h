#pragma once
#include <mutex>

namespace cgvkp {
namespace data {

    template<class T>
    class tripple_buffer {
    public:
        std::mutex sync;
        T buf[3];
        int p[2];
    };

    template<class T, int pi>
    class tripple_buffer_facade {
    public:
        static_assert((pi == 0) || (pi == 1), "index must be 0 or 1");

        tripple_buffer_facade(tripple_buffer<T>& b) : b(b) {
        }

        T& buffer(void) {
            return b.buf[b.p[pi]];
        }
        void sync(void) {
            std::lock_guard<std::mutex> l(b.sync);
            b.p[pi] = 3 - (b.p[pi] + b.p[1 - pi]);
        }

    private:
        tripple_buffer<T>& b;
    };

}
}
