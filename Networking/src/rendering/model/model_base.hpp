#pragma once
#include <memory>

namespace cgvkp {
namespace rendering {
namespace model {

    class model_base {
    public:
        typedef std::shared_ptr<model_base> ptr;
        typedef std::weak_ptr<model_base> weak_ptr;

        model_base();
        virtual ~model_base();

    };

}
}
}
