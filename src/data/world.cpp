#include "world.h"

using namespace cgvkp;

data::world::world() : meta() {
}

data::world::~world() {
}

void data::world::init() {
    meta.set_score(0);
}

void data::world::copy_from(const world& src) {
    meta.copy_from(src.meta);
}
