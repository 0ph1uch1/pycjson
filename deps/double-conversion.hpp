// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/// fixed-dtoa.h START
#ifndef DOUBLE_CONVERSION_FIXED_DTOA_H_
#define DOUBLE_CONVERSION_FIXED_DTOA_H_
/// utils.h START
#ifndef DOUBLE_CONVERSION_UTILS_H_
#define DOUBLE_CONVERSION_UTILS_H_
#include <cstdlib>
#include <cstring>
#if __cplusplus >= 201103L
#define DOUBLE_CONVERSION_NULLPTR nullptr
#else
#define DOUBLE_CONVERSION_NULLPTR NULL
#endif
#include <cassert>
#ifndef DOUBLE_CONVERSION_ASSERT
#define DOUBLE_CONVERSION_ASSERT(condition)         \
    assert(condition)
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(ASSERT)
#define ASSERT DOUBLE_CONVERSION_ASSERT
#endif
#ifndef DOUBLE_CONVERSION_UNIMPLEMENTED
#define DOUBLE_CONVERSION_UNIMPLEMENTED() (abort())
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UNIMPLEMENTED)
#define UNIMPLEMENTED DOUBLE_CONVERSION_UNIMPLEMENTED
#endif
#ifndef DOUBLE_CONVERSION_NO_RETURN
#ifdef _MSC_VER
#define DOUBLE_CONVERSION_NO_RETURN __declspec(noreturn)
#else
#define DOUBLE_CONVERSION_NO_RETURN __attribute__((noreturn))
#endif
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(NO_RETURN)
#define NO_RETURN DOUBLE_CONVERSION_NO_RETURN
#endif
#ifndef DOUBLE_CONVERSION_UNREACHABLE
#ifdef _MSC_VER
void DOUBLE_CONVERSION_NO_RETURN abort_noreturn();
inline void abort_noreturn() { abort(); }
#define DOUBLE_CONVERSION_UNREACHABLE()   (abort_noreturn())
#else
#define DOUBLE_CONVERSION_UNREACHABLE()   (abort())
#endif
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UNREACHABLE)
#define UNREACHABLE DOUBLE_CONVERSION_UNREACHABLE
#endif
#ifdef __has_attribute
#   define DOUBLE_CONVERSION_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#   define DOUBLE_CONVERSION_HAS_ATTRIBUTE(x) 0
#endif
#ifndef DOUBLE_CONVERSION_UNUSED
#if DOUBLE_CONVERSION_HAS_ATTRIBUTE(unused)
#define DOUBLE_CONVERSION_UNUSED __attribute__((unused))
#else
#define DOUBLE_CONVERSION_UNUSED
#endif
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UNUSED)
#define UNUSED DOUBLE_CONVERSION_UNUSED
#endif
#if DOUBLE_CONVERSION_HAS_ATTRIBUTE(uninitialized)
#define DOUBLE_CONVERSION_STACK_UNINITIALIZED __attribute__((uninitialized))
#else
#define DOUBLE_CONVERSION_STACK_UNINITIALIZED
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(STACK_UNINITIALIZED)
#define STACK_UNINITIALIZED DOUBLE_CONVERSION_STACK_UNINITIALIZED
#endif
/*
// -- in div.c
double Div_double(double x, double y) { return x / y; }
// -- in main.c
double Div_double(double x, double y);  // Forward declaration.
int main(int argc, char** argv) {
  return Div_double(89255.0, 1e22) == 89255e-22;
}
*/
#if defined(_M_X64) || defined(__x86_64__) || \
    defined(__ARMEL__) || defined(__avr32__) || defined(_M_ARM) || defined(_M_ARM64) || \
    defined(__hppa__) || defined(__ia64__) || \
    defined(__mips__) || \
    defined(__loongarch__) || \
    defined(__nios2__) || defined(__ghs) || \
    defined(__powerpc__) || defined(__ppc__) || defined(__ppc64__) || \
    defined(_POWER) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || \
    defined(__sparc__) || defined(__sparc) || defined(__s390__) || \
    defined(__SH4__) || defined(__alpha__) || \
    defined(_MIPS_ARCH_MIPS32R2) || defined(__ARMEB__) ||\
    defined(__AARCH64EL__) || defined(__aarch64__) || defined(__AARCH64EB__) || \
    defined(__riscv) || defined(__e2k__) || \
    defined(__or1k__) || defined(__arc__) || defined(__ARC64__) || \
    defined(__microblaze__) || defined(__XTENSA__) || \
    defined(__EMSCRIPTEN__) || defined(__wasm32__)
#define DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS 1
#elif defined(__mc68000__) || \
    defined(__pnacl__) || defined(__native_client__)
#undef DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS
#elif defined(_M_IX86) || defined(__i386__) || defined(__i386)
#if defined(_WIN32)
#define DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS 1
#else
#undef DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS
#endif
#else
#error Target architecture was not detected as supported by Double-Conversion.
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(CORRECT_DOUBLE_OPERATIONS)
#define CORRECT_DOUBLE_OPERATIONS DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS
#endif
#if defined(_WIN32) && !defined(__MINGW32__)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif
typedef uint16_t uc16;
#define DOUBLE_CONVERSION_UINT64_2PART_C(a, b) (((static_cast<uint64_t>(a) << 32) + 0x##b##u))
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UINT64_2PART_C)
#define UINT64_2PART_C DOUBLE_CONVERSION_UINT64_2PART_C
#endif
#ifndef DOUBLE_CONVERSION_ARRAY_SIZE
#define DOUBLE_CONVERSION_ARRAY_SIZE(a)                                   \
  ((sizeof(a) / sizeof(*(a))) /                         \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(ARRAY_SIZE)
#define ARRAY_SIZE DOUBLE_CONVERSION_ARRAY_SIZE
#endif
#ifndef DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN
#define DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(TypeName)      \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(DC_DISALLOW_COPY_AND_ASSIGN)
#define DC_DISALLOW_COPY_AND_ASSIGN DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN
#endif
#ifndef DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS
#define DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                    \
  DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(DC_DISALLOW_IMPLICIT_CONSTRUCTORS)
#define DC_DISALLOW_IMPLICIT_CONSTRUCTORS DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS
#endif
namespace double_conversion {
inline int StrLength(const char* string) {
  size_t length = strlen(string);
  DOUBLE_CONVERSION_ASSERT(length == static_cast<size_t>(static_cast<int>(length)));
  return static_cast<int>(length);
}
template <typename T>
class Vector {
 public:
  Vector() : start_(DOUBLE_CONVERSION_NULLPTR), length_(0) {}
  Vector(T* data, int len) : start_(data), length_(len) {
    DOUBLE_CONVERSION_ASSERT(len == 0 || (len > 0 && data != DOUBLE_CONVERSION_NULLPTR));
  }
  Vector<T> SubVector(int from, int to) {
    DOUBLE_CONVERSION_ASSERT(to <= length_);
    DOUBLE_CONVERSION_ASSERT(from < to);
    DOUBLE_CONVERSION_ASSERT(0 <= from);
    return Vector<T>(start() + from, to - from);
  }
  int length() const { return length_; }
  bool is_empty() const { return length_ == 0; }
  T* start() const { return start_; }
  T& operator[](int index) const {
    DOUBLE_CONVERSION_ASSERT(0 <= index && index < length_);
    return start_[index];
  }
  T& first() { return start_[0]; }
  T& last() { return start_[length_ - 1]; }
  void pop_back() {
    DOUBLE_CONVERSION_ASSERT(!is_empty());
    --length_;
  }
 private:
  T* start_;
  int length_;
};
class StringBuilder {
 public:
  StringBuilder(char* buffer, int buffer_size)
      : buffer_(buffer, buffer_size), position_(0) { }
  ~StringBuilder() { if (!is_finalized()) Finalize(); }
  int size() const { return buffer_.length(); }
  int position() const {
    DOUBLE_CONVERSION_ASSERT(!is_finalized());
    return position_;
  }
  void Reset() { position_ = 0; }
  void AddCharacter(char c) {
    DOUBLE_CONVERSION_ASSERT(c != '\0');
    DOUBLE_CONVERSION_ASSERT(!is_finalized() && position_ < buffer_.length());
    buffer_[position_++] = c;
  }
  void AddString(const char* s) {
    AddSubstring(s, StrLength(s));
  }
  void AddSubstring(const char* s, int n) {
    DOUBLE_CONVERSION_ASSERT(!is_finalized() && position_ + n < buffer_.length());
    DOUBLE_CONVERSION_ASSERT(static_cast<size_t>(n) <= strlen(s));
    memmove(&buffer_[position_], s, static_cast<size_t>(n));
    position_ += n;
  }
  void AddPadding(char c, int count) {
    for (int i = 0; i < count; i++) {
      AddCharacter(c);
    }
  }
  char* Finalize() {
    DOUBLE_CONVERSION_ASSERT(!is_finalized() && position_ < buffer_.length());
    buffer_[position_] = '\0';
    DOUBLE_CONVERSION_ASSERT(strlen(buffer_.start()) == static_cast<size_t>(position_));
    position_ = -1;
    DOUBLE_CONVERSION_ASSERT(is_finalized());
    return buffer_.start();
  }
 private:
  Vector<char> buffer_;
  int position_;
  bool is_finalized() const { return position_ < 0; }
  DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(StringBuilder);
};
template <class Dest, class Source>
Dest BitCast(const Source& source) {
#if __cplusplus >= 201103L
  static_assert(sizeof(Dest) == sizeof(Source),
                "source and destination size mismatch");
#else
  DOUBLE_CONVERSION_UNUSED
  typedef char VerifySizesAreEqual[sizeof(Dest) == sizeof(Source) ? 1 : -1];
#endif
  Dest dest;
  memmove(&dest, &source, sizeof(dest));
  return dest;
}
template <class Dest, class Source>
Dest BitCast(Source* source) {
  return BitCast<Dest>(reinterpret_cast<uintptr_t>(source));
}
}
#endif
/// utils.h END

namespace double_conversion {
bool FastFixedDtoa(double v, int fractional_count,
                   Vector<char> buffer, int* length, int* decimal_point);
}
#endif
/// fixed-dtoa.h END
/// fast-dtoa.h START
#ifndef DOUBLE_CONVERSION_FAST_DTOA_H_
#define DOUBLE_CONVERSION_FAST_DTOA_H_
namespace double_conversion {
enum FastDtoaMode {
  FAST_DTOA_SHORTEST,
  FAST_DTOA_SHORTEST_SINGLE,
  FAST_DTOA_PRECISION
};
static const int kFastDtoaMaximalLength = 17;
static const int kFastDtoaMaximalSingleLength = 9;
bool FastDtoa(double d,
              FastDtoaMode mode,
              int requested_digits,
              Vector<char> buffer,
              int* length,
              int* decimal_point);
}
#endif
/// fast-dtoa.h END
/// bignum-dtoa.h START
#ifndef DOUBLE_CONVERSION_BIGNUM_DTOA_H_
#define DOUBLE_CONVERSION_BIGNUM_DTOA_H_
namespace double_conversion {
enum BignumDtoaMode {
  BIGNUM_DTOA_SHORTEST,
  BIGNUM_DTOA_SHORTEST_SINGLE,
  BIGNUM_DTOA_FIXED,
  BIGNUM_DTOA_PRECISION
};
void BignumDtoa(double v, BignumDtoaMode mode, int requested_digits,
                Vector<char> buffer, int* length, int* point);
}
#endif
/// bignum-dtoa.h END
/// cached-powers.h START
#ifndef DOUBLE_CONVERSION_CACHED_POWERS_H_
#define DOUBLE_CONVERSION_CACHED_POWERS_H_
/// diy-fp.h START
#ifndef DOUBLE_CONVERSION_DIY_FP_H_
#define DOUBLE_CONVERSION_DIY_FP_H_
namespace double_conversion {
class DiyFp {
 public:
  static const int kSignificandSize = 64;
  DiyFp() : f_(0), e_(0) {}
  DiyFp(const uint64_t significand, const int32_t exponent) : f_(significand), e_(exponent) {}
  void Subtract(const DiyFp& other) {
    DOUBLE_CONVERSION_ASSERT(e_ == other.e_);
    DOUBLE_CONVERSION_ASSERT(f_ >= other.f_);
    f_ -= other.f_;
  }
  static DiyFp Minus(const DiyFp& a, const DiyFp& b) {
    DiyFp result = a;
    result.Subtract(b);
    return result;
  }
  void Multiply(const DiyFp& other) {
    const uint64_t kM32 = 0xFFFFFFFFU;
    const uint64_t a = f_ >> 32;
    const uint64_t b = f_ & kM32;
    const uint64_t c = other.f_ >> 32;
    const uint64_t d = other.f_ & kM32;
    const uint64_t ac = a * c;
    const uint64_t bc = b * c;
    const uint64_t ad = a * d;
    const uint64_t bd = b * d;
    const uint64_t tmp = (bd >> 32) + (ad & kM32) + (bc & kM32) + (1U << 31);
    e_ += other.e_ + 64;
    f_ = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
  }
  static DiyFp Times(const DiyFp& a, const DiyFp& b) {
    DiyFp result = a;
    result.Multiply(b);
    return result;
  }
  void Normalize() {
    DOUBLE_CONVERSION_ASSERT(f_ != 0);
    uint64_t significand = f_;
    int32_t exponent = e_;
    const uint64_t k10MSBits = DOUBLE_CONVERSION_UINT64_2PART_C(0xFFC00000, 00000000);
    while ((significand & k10MSBits) == 0) {
      significand <<= 10;
      exponent -= 10;
    }
    while ((significand & kUint64MSB) == 0) {
      significand <<= 1;
      exponent--;
    }
    f_ = significand;
    e_ = exponent;
  }
  static DiyFp Normalize(const DiyFp& a) {
    DiyFp result = a;
    result.Normalize();
    return result;
  }
  uint64_t f() const { return f_; }
  int32_t e() const { return e_; }
  void set_f(uint64_t new_value) { f_ = new_value; }
  void set_e(int32_t new_value) { e_ = new_value; }
 private:
  static const uint64_t kUint64MSB = DOUBLE_CONVERSION_UINT64_2PART_C(0x80000000, 00000000);
  uint64_t f_;
  int32_t e_;
};
}
#endif
/// diy-fp.h END

namespace double_conversion {
namespace PowersOfTenCache {
  static const int kDecimalExponentDistance = 8;
  static const int kMinDecimalExponent = -348;
  static const int kMaxDecimalExponent = 340;
  void GetCachedPowerForBinaryExponentRange(int min_exponent,
                                            int max_exponent,
                                            DiyFp* power,
                                            int* decimal_exponent);
  void GetCachedPowerForDecimalExponent(int requested_exponent,
                                        DiyFp* power,
                                        int* found_exponent);
}
}
#endif
/// cached-powers.h END
/// double-conversion.h START
#ifndef DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_
#define DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_
/// string-to-double.h START
#ifndef DOUBLE_CONVERSION_STRING_TO_DOUBLE_H_
#define DOUBLE_CONVERSION_STRING_TO_DOUBLE_H_
namespace double_conversion {
class StringToDoubleConverter {
 public:
  enum Flags {
    NO_FLAGS = 0,
    ALLOW_HEX = 1,
    ALLOW_OCTALS = 2,
    ALLOW_TRAILING_JUNK = 4,
    ALLOW_LEADING_SPACES = 8,
    ALLOW_TRAILING_SPACES = 16,
    ALLOW_SPACES_AFTER_SIGN = 32,
    ALLOW_CASE_INSENSITIVITY = 64,
    ALLOW_CASE_INSENSIBILITY = 64,
    ALLOW_HEX_FLOATS = 128,
  };
  static const uc16 kNoSeparator = '\0';
  StringToDoubleConverter(int flags,
                          double empty_string_value,
                          double junk_string_value,
                          const char* infinity_symbol,
                          const char* nan_symbol,
                          uc16 separator = kNoSeparator)
      : flags_(flags),
        empty_string_value_(empty_string_value),
        junk_string_value_(junk_string_value),
        infinity_symbol_(infinity_symbol),
        nan_symbol_(nan_symbol),
        separator_(separator) {
  }
  double StringToDouble(const char* buffer,
                        int length,
                        int* processed_characters_count) const;
  double StringToDouble(const uc16* buffer,
                        int length,
                        int* processed_characters_count) const;
  float StringToFloat(const char* buffer,
                      int length,
                      int* processed_characters_count) const;
  float StringToFloat(const uc16* buffer,
                      int length,
                      int* processed_characters_count) const;
  template <typename T>
  T StringTo(const char* buffer,
             int length,
             int* processed_characters_count) const;
  template <typename T>
  T StringTo(const uc16* buffer,
             int length,
             int* processed_characters_count) const;
 private:
  const int flags_;
  const double empty_string_value_;
  const double junk_string_value_;
  const char* const infinity_symbol_;
  const char* const nan_symbol_;
  const uc16 separator_;
  template <class Iterator>
  double StringToIeee(Iterator start_pointer,
                      int length,
                      bool read_as_double,
                      int* processed_characters_count) const;
  DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(StringToDoubleConverter);
};
}
#endif
/// string-to-double.h END

/// double-to-string.h START
#ifndef DOUBLE_CONVERSION_DOUBLE_TO_STRING_H_
#define DOUBLE_CONVERSION_DOUBLE_TO_STRING_H_
namespace double_conversion {
class DoubleToStringConverter {
 public:
  static const int kMaxFixedDigitsBeforePoint = 60;
  static const int kMaxFixedDigitsAfterPoint = 100;
  static const int kMaxExponentialDigits = 120;
  static const int kMinPrecisionDigits = 1;
  static const int kMaxPrecisionDigits = 120;
  static const int kBase10MaximalLength = 17;
  static const int kBase10MaximalLengthSingle = 9;
  static const int kMaxCharsEcmaScriptShortest = 25;
  enum Flags {
    NO_FLAGS = 0,
    EMIT_POSITIVE_EXPONENT_SIGN = 1,
    EMIT_TRAILING_DECIMAL_POINT = 2,
    EMIT_TRAILING_ZERO_AFTER_POINT = 4,
    UNIQUE_ZERO = 8,
    NO_TRAILING_ZERO = 16,
    EMIT_TRAILING_DECIMAL_POINT_IN_EXPONENTIAL = 32,
    EMIT_TRAILING_ZERO_AFTER_POINT_IN_EXPONENTIAL = 64
  };
  DoubleToStringConverter(int flags,
                          const char* infinity_symbol,
                          const char* nan_symbol,
                          char exponent_character,
                          int decimal_in_shortest_low,
                          int decimal_in_shortest_high,
                          int max_leading_padding_zeroes_in_precision_mode,
                          int max_trailing_padding_zeroes_in_precision_mode,
                          int min_exponent_width = 0)
      : flags_(flags),
        infinity_symbol_(infinity_symbol),
        nan_symbol_(nan_symbol),
        exponent_character_(exponent_character),
        decimal_in_shortest_low_(decimal_in_shortest_low),
        decimal_in_shortest_high_(decimal_in_shortest_high),
        max_leading_padding_zeroes_in_precision_mode_(
            max_leading_padding_zeroes_in_precision_mode),
        max_trailing_padding_zeroes_in_precision_mode_(
            max_trailing_padding_zeroes_in_precision_mode),
        min_exponent_width_(min_exponent_width) {
    DOUBLE_CONVERSION_ASSERT(((flags & EMIT_TRAILING_DECIMAL_POINT) != 0) ||
        !((flags & EMIT_TRAILING_ZERO_AFTER_POINT) != 0));
  }
  static const DoubleToStringConverter& EcmaScriptConverter();
  bool ToShortest(double value, StringBuilder* result_builder) const {
    return ToShortestIeeeNumber(value, result_builder, SHORTEST);
  }
  bool ToShortestSingle(float value, StringBuilder* result_builder) const {
    return ToShortestIeeeNumber(value, result_builder, SHORTEST_SINGLE);
  }
  bool ToFixed(double value,
               int requested_digits,
               StringBuilder* result_builder) const;
  bool ToExponential(double value,
                     int requested_digits,
                     StringBuilder* result_builder) const;
  bool ToPrecision(double value,
                   int precision,
                   StringBuilder* result_builder) const;
  enum DtoaMode {
    SHORTEST,
    SHORTEST_SINGLE,
    FIXED,
    PRECISION
  };
  static void DoubleToAscii(double v,
                            DtoaMode mode,
                            int requested_digits,
                            char* buffer,
                            int buffer_length,
                            bool* sign,
                            int* length,
                            int* point);
 private:
  bool ToShortestIeeeNumber(double value,
                            StringBuilder* result_builder,
                            DtoaMode mode) const;
  bool HandleSpecialValues(double value, StringBuilder* result_builder) const;
  void CreateExponentialRepresentation(const char* decimal_digits,
                                       int length,
                                       int exponent,
                                       StringBuilder* result_builder) const;
  void CreateDecimalRepresentation(const char* decimal_digits,
                                   int length,
                                   int decimal_point,
                                   int digits_after_point,
                                   StringBuilder* result_builder) const;
  const int flags_;
  const char* const infinity_symbol_;
  const char* const nan_symbol_;
  const char exponent_character_;
  const int decimal_in_shortest_low_;
  const int decimal_in_shortest_high_;
  const int max_leading_padding_zeroes_in_precision_mode_;
  const int max_trailing_padding_zeroes_in_precision_mode_;
  const int min_exponent_width_;
  DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(DoubleToStringConverter);
};
}
#endif
/// double-to-string.h END

#endif
/// double-conversion.h END
/// ieee.h START
#ifndef DOUBLE_CONVERSION_DOUBLE_H_
#define DOUBLE_CONVERSION_DOUBLE_H_
namespace double_conversion {
static uint64_t double_to_uint64(double d) { return BitCast<uint64_t>(d); }
static double uint64_to_double(uint64_t d64) { return BitCast<double>(d64); }
static uint32_t float_to_uint32(float f) { return BitCast<uint32_t>(f); }
static float uint32_to_float(uint32_t d32) { return BitCast<float>(d32); }
class Double {
 public:
  static const uint64_t kSignMask = DOUBLE_CONVERSION_UINT64_2PART_C(0x80000000, 00000000);
  static const uint64_t kExponentMask = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF00000, 00000000);
  static const uint64_t kSignificandMask = DOUBLE_CONVERSION_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
  static const uint64_t kHiddenBit = DOUBLE_CONVERSION_UINT64_2PART_C(0x00100000, 00000000);
  static const uint64_t kQuietNanBit = DOUBLE_CONVERSION_UINT64_2PART_C(0x00080000, 00000000);
  static const int kPhysicalSignificandSize = 52;
  static const int kSignificandSize = 53;
  static const int kExponentBias = 0x3FF + kPhysicalSignificandSize;
  static const int kMaxExponent = 0x7FF - kExponentBias;
  Double() : d64_(0) {}
  explicit Double(double d) : d64_(double_to_uint64(d)) {}
  explicit Double(uint64_t d64) : d64_(d64) {}
  explicit Double(DiyFp diy_fp)
    : d64_(DiyFpToUint64(diy_fp)) {}
  DiyFp AsDiyFp() const {
    DOUBLE_CONVERSION_ASSERT(Sign() > 0);
    DOUBLE_CONVERSION_ASSERT(!IsSpecial());
    return DiyFp(Significand(), Exponent());
  }
  DiyFp AsNormalizedDiyFp() const {
    DOUBLE_CONVERSION_ASSERT(value() > 0.0);
    uint64_t f = Significand();
    int e = Exponent();
    while ((f & kHiddenBit) == 0) {
      f <<= 1;
      e--;
    }
    f <<= DiyFp::kSignificandSize - kSignificandSize;
    e -= DiyFp::kSignificandSize - kSignificandSize;
    return DiyFp(f, e);
  }
  uint64_t AsUint64() const {
    return d64_;
  }
  double NextDouble() const {
    if (d64_ == kInfinity) return Double(kInfinity).value();
    if (Sign() < 0 && Significand() == 0) {
      return 0.0;
    }
    if (Sign() < 0) {
      return Double(d64_ - 1).value();
    } else {
      return Double(d64_ + 1).value();
    }
  }
  double PreviousDouble() const {
    if (d64_ == (kInfinity | kSignMask)) return -Infinity();
    if (Sign() < 0) {
      return Double(d64_ + 1).value();
    } else {
      if (Significand() == 0) return -0.0;
      return Double(d64_ - 1).value();
    }
  }
  int Exponent() const {
    if (IsDenormal()) return kDenormalExponent;
    uint64_t d64 = AsUint64();
    int biased_e =
        static_cast<int>((d64 & kExponentMask) >> kPhysicalSignificandSize);
    return biased_e - kExponentBias;
  }
  uint64_t Significand() const {
    uint64_t d64 = AsUint64();
    uint64_t significand = d64 & kSignificandMask;
    if (!IsDenormal()) {
      return significand + kHiddenBit;
    } else {
      return significand;
    }
  }
  bool IsDenormal() const {
    uint64_t d64 = AsUint64();
    return (d64 & kExponentMask) == 0;
  }
  bool IsSpecial() const {
    uint64_t d64 = AsUint64();
    return (d64 & kExponentMask) == kExponentMask;
  }
  bool IsNan() const {
    uint64_t d64 = AsUint64();
    return ((d64 & kExponentMask) == kExponentMask) &&
        ((d64 & kSignificandMask) != 0);
  }
  bool IsQuietNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
    return IsNan() && ((AsUint64() & kQuietNanBit) == 0);
#else
    return IsNan() && ((AsUint64() & kQuietNanBit) != 0);
#endif
  }
  bool IsSignalingNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
    return IsNan() && ((AsUint64() & kQuietNanBit) != 0);
#else
    return IsNan() && ((AsUint64() & kQuietNanBit) == 0);
#endif
  }
  bool IsInfinite() const {
    uint64_t d64 = AsUint64();
    return ((d64 & kExponentMask) == kExponentMask) &&
        ((d64 & kSignificandMask) == 0);
  }
  int Sign() const {
    uint64_t d64 = AsUint64();
    return (d64 & kSignMask) == 0? 1: -1;
  }
  DiyFp UpperBoundary() const {
    DOUBLE_CONVERSION_ASSERT(Sign() > 0);
    return DiyFp(Significand() * 2 + 1, Exponent() - 1);
  }
  void NormalizedBoundaries(DiyFp* out_m_minus, DiyFp* out_m_plus) const {
    DOUBLE_CONVERSION_ASSERT(value() > 0.0);
    DiyFp v = this->AsDiyFp();
    DiyFp m_plus = DiyFp::Normalize(DiyFp((v.f() << 1) + 1, v.e() - 1));
    DiyFp m_minus;
    if (LowerBoundaryIsCloser()) {
      m_minus = DiyFp((v.f() << 2) - 1, v.e() - 2);
    } else {
      m_minus = DiyFp((v.f() << 1) - 1, v.e() - 1);
    }
    m_minus.set_f(m_minus.f() << (m_minus.e() - m_plus.e()));
    m_minus.set_e(m_plus.e());
    *out_m_plus = m_plus;
    *out_m_minus = m_minus;
  }
  bool LowerBoundaryIsCloser() const {
    bool physical_significand_is_zero = ((AsUint64() & kSignificandMask) == 0);
    return physical_significand_is_zero && (Exponent() != kDenormalExponent);
  }
  double value() const { return uint64_to_double(d64_); }
  static int SignificandSizeForOrderOfMagnitude(int order) {
    if (order >= (kDenormalExponent + kSignificandSize)) {
      return kSignificandSize;
    }
    if (order <= kDenormalExponent) return 0;
    return order - kDenormalExponent;
  }
  static double Infinity() {
    return Double(kInfinity).value();
  }
  static double NaN() {
    return Double(kNaN).value();
  }
 private:
  static const int kDenormalExponent = -kExponentBias + 1;
  static const uint64_t kInfinity = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF00000, 00000000);
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
  static const uint64_t kNaN = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF7FFFF, FFFFFFFF);
#else
  static const uint64_t kNaN = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF80000, 00000000);
#endif
  const uint64_t d64_;
  static uint64_t DiyFpToUint64(DiyFp diy_fp) {
    uint64_t significand = diy_fp.f();
    int exponent = diy_fp.e();
    while (significand > kHiddenBit + kSignificandMask) {
      significand >>= 1;
      exponent++;
    }
    if (exponent >= kMaxExponent) {
      return kInfinity;
    }
    if (exponent < kDenormalExponent) {
      return 0;
    }
    while (exponent > kDenormalExponent && (significand & kHiddenBit) == 0) {
      significand <<= 1;
      exponent--;
    }
    uint64_t biased_exponent;
    if (exponent == kDenormalExponent && (significand & kHiddenBit) == 0) {
      biased_exponent = 0;
    } else {
      biased_exponent = static_cast<uint64_t>(exponent + kExponentBias);
    }
    return (significand & kSignificandMask) |
        (biased_exponent << kPhysicalSignificandSize);
  }
  DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Double);
};
class Single {
 public:
  static const uint32_t kSignMask = 0x80000000;
  static const uint32_t kExponentMask = 0x7F800000;
  static const uint32_t kSignificandMask = 0x007FFFFF;
  static const uint32_t kHiddenBit = 0x00800000;
  static const uint32_t kQuietNanBit = 0x00400000;
  static const int kPhysicalSignificandSize = 23;
  static const int kSignificandSize = 24;
  Single() : d32_(0) {}
  explicit Single(float f) : d32_(float_to_uint32(f)) {}
  explicit Single(uint32_t d32) : d32_(d32) {}
  DiyFp AsDiyFp() const {
    DOUBLE_CONVERSION_ASSERT(Sign() > 0);
    DOUBLE_CONVERSION_ASSERT(!IsSpecial());
    return DiyFp(Significand(), Exponent());
  }
  uint32_t AsUint32() const {
    return d32_;
  }
  int Exponent() const {
    if (IsDenormal()) return kDenormalExponent;
    uint32_t d32 = AsUint32();
    int biased_e =
        static_cast<int>((d32 & kExponentMask) >> kPhysicalSignificandSize);
    return biased_e - kExponentBias;
  }
  uint32_t Significand() const {
    uint32_t d32 = AsUint32();
    uint32_t significand = d32 & kSignificandMask;
    if (!IsDenormal()) {
      return significand + kHiddenBit;
    } else {
      return significand;
    }
  }
  bool IsDenormal() const {
    uint32_t d32 = AsUint32();
    return (d32 & kExponentMask) == 0;
  }
  bool IsSpecial() const {
    uint32_t d32 = AsUint32();
    return (d32 & kExponentMask) == kExponentMask;
  }
  bool IsNan() const {
    uint32_t d32 = AsUint32();
    return ((d32 & kExponentMask) == kExponentMask) &&
        ((d32 & kSignificandMask) != 0);
  }
  bool IsQuietNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
    return IsNan() && ((AsUint32() & kQuietNanBit) == 0);
#else
    return IsNan() && ((AsUint32() & kQuietNanBit) != 0);
#endif
  }
  bool IsSignalingNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
    return IsNan() && ((AsUint32() & kQuietNanBit) != 0);
#else
    return IsNan() && ((AsUint32() & kQuietNanBit) == 0);
#endif
  }
  bool IsInfinite() const {
    uint32_t d32 = AsUint32();
    return ((d32 & kExponentMask) == kExponentMask) &&
        ((d32 & kSignificandMask) == 0);
  }
  int Sign() const {
    uint32_t d32 = AsUint32();
    return (d32 & kSignMask) == 0? 1: -1;
  }
  void NormalizedBoundaries(DiyFp* out_m_minus, DiyFp* out_m_plus) const {
    DOUBLE_CONVERSION_ASSERT(value() > 0.0);
    DiyFp v = this->AsDiyFp();
    DiyFp m_plus = DiyFp::Normalize(DiyFp((v.f() << 1) + 1, v.e() - 1));
    DiyFp m_minus;
    if (LowerBoundaryIsCloser()) {
      m_minus = DiyFp((v.f() << 2) - 1, v.e() - 2);
    } else {
      m_minus = DiyFp((v.f() << 1) - 1, v.e() - 1);
    }
    m_minus.set_f(m_minus.f() << (m_minus.e() - m_plus.e()));
    m_minus.set_e(m_plus.e());
    *out_m_plus = m_plus;
    *out_m_minus = m_minus;
  }
  DiyFp UpperBoundary() const {
    DOUBLE_CONVERSION_ASSERT(Sign() > 0);
    return DiyFp(Significand() * 2 + 1, Exponent() - 1);
  }
  bool LowerBoundaryIsCloser() const {
    bool physical_significand_is_zero = ((AsUint32() & kSignificandMask) == 0);
    return physical_significand_is_zero && (Exponent() != kDenormalExponent);
  }
  float value() const { return uint32_to_float(d32_); }
  static float Infinity() {
    return Single(kInfinity).value();
  }
  static float NaN() {
    return Single(kNaN).value();
  }
 private:
  static const int kExponentBias = 0x7F + kPhysicalSignificandSize;
  static const int kDenormalExponent = -kExponentBias + 1;
  static const int kMaxExponent = 0xFF - kExponentBias;
  static const uint32_t kInfinity = 0x7F800000;
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
  static const uint32_t kNaN = 0x7FBFFFFF;
#else
  static const uint32_t kNaN = 0x7FC00000;
#endif
  const uint32_t d32_;
  DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Single);
};
}
#endif
/// ieee.h END
/// strtod.h START
#ifndef DOUBLE_CONVERSION_STRTOD_H_
#define DOUBLE_CONVERSION_STRTOD_H_
namespace double_conversion {
double Strtod(Vector<const char> buffer, int exponent);
float Strtof(Vector<const char> buffer, int exponent);
double StrtodTrimmed(Vector<const char> trimmed, int exponent);
float StrtofTrimmed(Vector<const char> trimmed, int exponent);
inline Vector<const char> TrimTrailingZeros(Vector<const char> buffer) {
  for (int i = buffer.length() - 1; i >= 0; --i) {
    if (buffer[i] != '0') {
      return buffer.SubVector(0, i + 1);
    }
  }
  return Vector<const char>(buffer.start(), 0);
}
}
#endif
/// strtod.h END
/// bignum.h START
#ifndef DOUBLE_CONVERSION_BIGNUM_H_
#define DOUBLE_CONVERSION_BIGNUM_H_
namespace double_conversion {
class Bignum {
 public:
  static const int kMaxSignificantBits = 3584;
  Bignum() : used_bigits_(0), exponent_(0) {}
  void AssignUInt16(const uint16_t value);
  void AssignUInt64(uint64_t value);
  void AssignBignum(const Bignum& other);
  void AssignDecimalString(const Vector<const char> value);
  void AssignHexString(const Vector<const char> value);
  void AssignPowerUInt16(uint16_t base, const int exponent);
  void AddUInt64(const uint64_t operand);
  void AddBignum(const Bignum& other);
  void SubtractBignum(const Bignum& other);
  void Square();
  void ShiftLeft(const int shift_amount);
  void MultiplyByUInt32(const uint32_t factor);
  void MultiplyByUInt64(const uint64_t factor);
  void MultiplyByPowerOfTen(const int exponent);
  void Times10() { return MultiplyByUInt32(10); }
  uint16_t DivideModuloIntBignum(const Bignum& other);
  bool ToHexString(char* buffer, const int buffer_size) const;
  static int Compare(const Bignum& a, const Bignum& b);
  static bool Equal(const Bignum& a, const Bignum& b) {
    return Compare(a, b) == 0;
  }
  static bool LessEqual(const Bignum& a, const Bignum& b) {
    return Compare(a, b) <= 0;
  }
  static bool Less(const Bignum& a, const Bignum& b) {
    return Compare(a, b) < 0;
  }
  static int PlusCompare(const Bignum& a, const Bignum& b, const Bignum& c);
  static bool PlusEqual(const Bignum& a, const Bignum& b, const Bignum& c) {
    return PlusCompare(a, b, c) == 0;
  }
  static bool PlusLessEqual(const Bignum& a, const Bignum& b, const Bignum& c) {
    return PlusCompare(a, b, c) <= 0;
  }
  static bool PlusLess(const Bignum& a, const Bignum& b, const Bignum& c) {
    return PlusCompare(a, b, c) < 0;
  }
 private:
  typedef uint32_t Chunk;
  typedef uint64_t DoubleChunk;
  static const int kChunkSize = sizeof(Chunk) * 8;
  static const int kDoubleChunkSize = sizeof(DoubleChunk) * 8;
  static const int kBigitSize = 28;
  static const Chunk kBigitMask = (1 << kBigitSize) - 1;
  static const int kBigitCapacity = kMaxSignificantBits / kBigitSize;
  static void EnsureCapacity(const int size) {
    if (size > kBigitCapacity) {
      DOUBLE_CONVERSION_UNREACHABLE();
    }
  }
  void Align(const Bignum& other);
  void Clamp();
  bool IsClamped() const {
    return used_bigits_ == 0 || RawBigit(used_bigits_ - 1) != 0;
  }
  void Zero() {
    used_bigits_ = 0;
    exponent_ = 0;
  }
  void BigitsShiftLeft(const int shift_amount);
  int BigitLength() const { return used_bigits_ + exponent_; }
  Chunk& RawBigit(const int index);
  const Chunk& RawBigit(const int index) const;
  Chunk BigitOrZero(const int index) const;
  void SubtractTimes(const Bignum& other, const int factor);
  int16_t used_bigits_;
  int16_t exponent_;
  Chunk bigits_buffer_[kBigitCapacity];
  DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Bignum);
};
}
#endif
/// bignum.h END
