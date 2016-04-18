#pragma once
#include <mutex>

namespace cgvkp {
namespace data {

    template<class T, int pi>
    class tripple_buffer_facade;

    /*
     * WARNING!
     *  This tripple_buffer implementation is not safe.
     *  It only works if you ensure that each end-point facade exists exactly once.
     */
    template<class T>
    class tripple_buffer {
    public:
        tripple_buffer() : sync() {
            valid[0] = valid[1] = valid[2] = false;
            p[0] = 0;
            p[1] = 1;
        }
        tripple_buffer(const tripple_buffer& src) = delete;
    private:
        std::mutex sync;
        T buf[3];
        bool valid[3];
        int p[2];

        friend class ::cgvkp::data::tripple_buffer_facade<T, 0>;
        friend class ::cgvkp::data::tripple_buffer_facade<T, 1>;
    };

    template<class T, int pi>
    class tripple_buffer_facade {
    public:
        static_assert((pi == 0) || (pi == 1), "index must be 0 or 1");

        tripple_buffer_facade(tripple_buffer<T>& b) : b(b) {
        }
        tripple_buffer_facade(const tripple_buffer_facade& src) = delete;
        explicit tripple_buffer_facade(tripple_buffer_facade&& src) : b(src.b) {
        }

        T& buffer(void) {
            return b.buf[b.p[pi]];
        }
        bool& valid(void) {
            return b.valid[b.p[pi]];
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
