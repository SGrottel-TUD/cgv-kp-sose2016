#pragma once
#include "rendering/model/model_base.hpp"
#include <memory>

namespace cgvkp {
namespace rendering {
namespace view {
    enum VertexAttributeLocation
    {
        position = 0,
        normal,
        textureCoord,
        numAttributes
    };

    class view_base {
    public:
        typedef std::shared_ptr<view_base> ptr;
        typedef std::weak_ptr<view_base> weak_ptr;

		inline view_base() : initialized(false) {}
		virtual inline ~view_base() { deinit(); }

        inline void set_model(model::model_base::weak_ptr m) { model = m; }
        inline model::model_base::ptr get_model() const { return model.lock(); }

        // Initializes the object (if not already initialized) and returns the initialization state
        bool init();

        // Answer if the object is valid
        virtual inline bool is_valid() const { return initialized; }
        // Answer if the object has a model
        virtual inline bool has_model() const { return !model.expired(); }

        virtual void render() const = 0;

        // deinitializes the object (if initialized)
        void deinit();

        virtual inline bool casts_shadows() const { return true; }
        virtual inline bool light_source() const { return false; }

    protected:
        // return true on success
        virtual bool init_impl() = 0;
        virtual void deinit_impl() = 0;

        /** The main model used by this view */
        model::model_base::weak_ptr model;

    private:
        bool initialized;
    };

}
}
}
