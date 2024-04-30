#include "simdutf.h"

namespace simdutf_wrapper {
    extern "C" {
        size_t count_utf8(const char *buf, size_t len) {
            return simdutf::count_utf8(buf, len);
        }
    }
}