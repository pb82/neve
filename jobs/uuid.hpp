#ifndef UUID_H
#define UUID_H

#include <random>

namespace UUID {
    static std::random_device r;
    uint create();
}

#endif // UUID_H
