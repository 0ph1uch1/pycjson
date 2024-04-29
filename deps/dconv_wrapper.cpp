#include "double-conversion.hpp"

#include <Python.h>
#include <pymath.h>

enum FLAGS {
    // d2s/encoding flags
    NO_FLAGS = 0,
    EMIT_POSITIVE_EXPONENT_SIGN = 1,
    EMIT_TRAILING_DECIMAL_POINT = 2,
    EMIT_TRAILING_ZERO_AFTER_POINT = 4,
    UNIQUE_ZERO = 8,
    NO_TRAILING_ZERO = 16,
    EMIT_TRAILING_DECIMAL_POINT_IN_EXPONENTIAL = 32,
    EMIT_TRAILING_ZERO_AFTER_POINT_IN_EXPONENTIAL = 64,

    // s2d/decoding flags
    // NO_FLAGS = 0, // duplicated
    ALLOW_HEX = 1,
    ALLOW_OCTALS = 2,
    ALLOW_TRAILING_JUNK = 4,
    ALLOW_LEADING_SPACES = 8,
    ALLOW_TRAILING_SPACES = 16,
    ALLOW_SPACES_AFTER_SIGN = 32,
    ALLOW_CASE_INSENSITIVITY = 64,
    ALLOW_CASE_INSENSIBILITY = 64, // Deprecated
    ALLOW_HEX_FLOATS = 128,
};

namespace double_conversion {
    extern "C" {
    int dconv_d2s(double value, char *buf, int buflen, int *strlength, bool allow_nan) {
        DoubleToStringConverter *d2s;
        if (allow_nan) {
            static DoubleToStringConverter d2s_w_nan(DoubleToStringConverter::NO_FLAGS, "Infinity", "NaN", 'e', -324, 308, 0, 0);
            d2s = &d2s_w_nan;
        } else {
            static DoubleToStringConverter d2s_wo_nan(DoubleToStringConverter::NO_FLAGS, NULL, NULL, 'e', -324, 308, 0, 0);
            d2s = &d2s_wo_nan;
        }
        StringBuilder sb(buf, buflen);
        int success = d2s->ToShortest(value, &sb);
        *strlength = success ? sb.position() : -1;
        return success;
    }


    double dconv_s2d(const char *buffer, int length, int *processed_characters_count) {
        static StringToDoubleConverter s2d(StringToDoubleConverter::NO_FLAGS, 0.0, Py_NAN, "Infinity", "NaN");
        return s2d.StringToDouble(buffer, length, processed_characters_count);
    }
    }
} // namespace double_conversion