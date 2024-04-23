#include "dconv_wrapper.h"
#include "double-conversion.hpp"

#include <Python.h>
#include <pymath.h>

namespace double_conversion {
    extern "C" {
    int dconv_d2s(double value, char *buf, int buflen, int *strlength) {
        static auto d2s = DoubleToStringConverter(DoubleToStringConverter::NO_FLAGS, "Infinity", "NaN", 'e', -324, 308, 0, 0);
        StringBuilder sb(buf, buflen);
        int success = d2s.ToShortest(value, &sb);
        *strlength = success ? sb.position() : -1;
        return success;
    }


    double dconv_s2d(const char *buffer, int length, int *processed_characters_count) {
        static auto s2d = StringToDoubleConverter(StringToDoubleConverter::NO_FLAGS, 0.0, Py_NAN, "Infinity", "NaN");
        return s2d.StringToDouble(buffer, length, processed_characters_count);
    }
    }
} // namespace double_conversion