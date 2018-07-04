#include "uuid.hpp"

namespace UUID {
    // TODO: create a real UUID instead of just a random number
    uint create() {
        std::mt19937 rnd;
        std::seed_seq seed { r(), r(), r() };
        rnd.seed(seed);

        std::uniform_int_distribution<std::mt19937::result_type> dst(1, RAND_MAX);
        return dst(rnd);
    }
}
