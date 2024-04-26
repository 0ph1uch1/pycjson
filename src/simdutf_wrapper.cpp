#include "simdutf.h"

namespace simdutf_wrapper {
    extern "C" {
        size_t count_utf8(const char *buf, size_t len) {
            return simdutf::count_utf8(buf, len);
        }
        size_t utf8_length_from_latin1(const char *buf, size_t len) {
            return simdutf::utf8_length_from_latin1(buf, len);
        }
    }
}