#include "hand.h"

using namespace cgvkp;

data::hand::hand() : id(0), pos(), height(0), captured() {
}

data::hand::~hand() {
    captured.reset();
}
