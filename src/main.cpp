#include "application.hpp"
#include <iostream>

namespace {
    const char * regionDescription(int regVal) {
        switch (regVal) {
        case -1: return "Failed to init the application: ";
        case -2: return "Application run exception: ";
        case -3: return "Failed to deinit the application: ";
        default: return "Unexpected exception: ";
        }
        return "";
    }
}

int main(int argc, char **argv) {
    cgvkp::application app;
    int retval;

    std::cout << "Started" << std::endl;

    try {

        retval = -1;

        if (!app.init()) {
            throw std::runtime_error("application::init returned false");
        }

        retval = -2;
        app.run();

        retval = -3;
        app.deinit();

        retval = 0;

    } catch (const std::exception &e) {
        std::cout << regionDescription(retval) << e.what() << std::endl;
    } catch (...) {
        std::cout << regionDescription(retval) << "unexpected exception" << std::endl;
    }

    std::cout << "Ended" << std::endl;
    return retval;

}
