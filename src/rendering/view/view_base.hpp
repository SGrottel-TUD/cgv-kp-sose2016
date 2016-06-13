#pragma once
#include "rendering/model/model_base.hpp"
#include <memory>
#include <vector>

namespace cgvkp {
namespace rendering {
namespace view {

    class view_base {
    public:
        typedef std::shared_ptr<view_base> ptr;
        typedef std::weak_ptr<view_base> weak_ptr;

        view_base();
        virtual ~view_base();

        inline void set_model(model::model_base::weak_ptr m) {
            model = m;
        }
        inline model::model_base::ptr get_model() const {
            return model.lock();
        }

        // Initializes the object (if not already initialized) and returns the initialization state
        bool init();

        // Answer if the object is valid
        virtual bool is_valid() const;
        // Answer if the object has a model
        virtual bool has_model() const;

        virtual void render() = 0;

        // deinitializes the object (if initialized)
        void deinit();

        bool has_technique(std::string const technique_id) {
            auto t = techniques();
            return (std::find(t.begin(), t.end(), technique_id) != t.end());
        }

    protected:
        // return true on success
        virtual bool init_impl() = 0;
        virtual void deinit_impl() = 0;

        /** Set of ids for techniques that are allowed to run on this view */
        virtual std::vector<std::string> techniques() = 0;

        /** The main model used by this view */
        model::model_base::weak_ptr model;

    private:
        bool initialized;
    };

}
}
}
