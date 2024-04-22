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

#include "double-conversion.hpp"
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
/// bignum-dtoa.cc START
#include <cmath>
namespace double_conversion {
static int NormalizedExponent(uint64_t significand, int exponent) {
  DOUBLE_CONVERSION_ASSERT(significand != 0);
  while ((significand & Double::kHiddenBit) == 0) {
    significand = significand << 1;
    exponent = exponent - 1;
  }
  return exponent;
}
static int EstimatePower(int exponent);
static void InitialScaledStartValues(uint64_t significand,
                                     int exponent,
                                     bool lower_boundary_is_closer,
                                     int estimated_power,
                                     bool need_boundary_deltas,
                                     Bignum* numerator,
                                     Bignum* denominator,
                                     Bignum* delta_minus,
                                     Bignum* delta_plus);
static void FixupMultiply10(int estimated_power, bool is_even,
                            int* decimal_point,
                            Bignum* numerator, Bignum* denominator,
                            Bignum* delta_minus, Bignum* delta_plus);
static void GenerateShortestDigits(Bignum* numerator, Bignum* denominator,
                                   Bignum* delta_minus, Bignum* delta_plus,
                                   bool is_even,
                                   Vector<char> buffer, int* length);
static void BignumToFixed(int requested_digits, int* decimal_point,
                          Bignum* numerator, Bignum* denominator,
                          Vector<char> buffer, int* length);
static void GenerateCountedDigits(int count, int* decimal_point,
                                  Bignum* numerator, Bignum* denominator,
                                  Vector<char> buffer, int* length);
void BignumDtoa(double v, BignumDtoaMode mode, int requested_digits,
                Vector<char> buffer, int* length, int* decimal_point) {
  DOUBLE_CONVERSION_ASSERT(v > 0);
  DOUBLE_CONVERSION_ASSERT(!Double(v).IsSpecial());
  uint64_t significand;
  int exponent;
  bool lower_boundary_is_closer;
  if (mode == BIGNUM_DTOA_SHORTEST_SINGLE) {
    float f = static_cast<float>(v);
    DOUBLE_CONVERSION_ASSERT(f == v);
    significand = Single(f).Significand();
    exponent = Single(f).Exponent();
    lower_boundary_is_closer = Single(f).LowerBoundaryIsCloser();
  } else {
    significand = Double(v).Significand();
    exponent = Double(v).Exponent();
    lower_boundary_is_closer = Double(v).LowerBoundaryIsCloser();
  }
  bool need_boundary_deltas =
      (mode == BIGNUM_DTOA_SHORTEST || mode == BIGNUM_DTOA_SHORTEST_SINGLE);
  bool is_even = (significand & 1) == 0;
  int normalized_exponent = NormalizedExponent(significand, exponent);
  int estimated_power = EstimatePower(normalized_exponent);
  if (mode == BIGNUM_DTOA_FIXED && -estimated_power - 1 > requested_digits) {
    buffer[0] = '\0';
    *length = 0;
    *decimal_point = -requested_digits;
    return;
  }
  Bignum numerator;
  Bignum denominator;
  Bignum delta_minus;
  Bignum delta_plus;
  DOUBLE_CONVERSION_ASSERT(Bignum::kMaxSignificantBits >= 324*4);
  InitialScaledStartValues(significand, exponent, lower_boundary_is_closer,
                           estimated_power, need_boundary_deltas,
                           &numerator, &denominator,
                           &delta_minus, &delta_plus);
  FixupMultiply10(estimated_power, is_even, decimal_point,
                  &numerator, &denominator,
                  &delta_minus, &delta_plus);
  switch (mode) {
    case BIGNUM_DTOA_SHORTEST:
    case BIGNUM_DTOA_SHORTEST_SINGLE:
      GenerateShortestDigits(&numerator, &denominator,
                             &delta_minus, &delta_plus,
                             is_even, buffer, length);
      break;
    case BIGNUM_DTOA_FIXED:
      BignumToFixed(requested_digits, decimal_point,
                    &numerator, &denominator,
                    buffer, length);
      break;
    case BIGNUM_DTOA_PRECISION:
      GenerateCountedDigits(requested_digits, decimal_point,
                            &numerator, &denominator,
                            buffer, length);
      break;
    default:
      DOUBLE_CONVERSION_UNREACHABLE();
  }
  buffer[*length] = '\0';
}
static void GenerateShortestDigits(Bignum* numerator, Bignum* denominator,
                                   Bignum* delta_minus, Bignum* delta_plus,
                                   bool is_even,
                                   Vector<char> buffer, int* length) {
  if (Bignum::Equal(*delta_minus, *delta_plus)) {
    delta_plus = delta_minus;
  }
  *length = 0;
  for (;;) {
    uint16_t digit;
    digit = numerator->DivideModuloIntBignum(*denominator);
    DOUBLE_CONVERSION_ASSERT(digit <= 9);
    buffer[(*length)++] = static_cast<char>(digit + '0');
    bool in_delta_room_minus;
    bool in_delta_room_plus;
    if (is_even) {
      in_delta_room_minus = Bignum::LessEqual(*numerator, *delta_minus);
    } else {
      in_delta_room_minus = Bignum::Less(*numerator, *delta_minus);
    }
    if (is_even) {
      in_delta_room_plus =
          Bignum::PlusCompare(*numerator, *delta_plus, *denominator) >= 0;
    } else {
      in_delta_room_plus =
          Bignum::PlusCompare(*numerator, *delta_plus, *denominator) > 0;
    }
    if (!in_delta_room_minus && !in_delta_room_plus) {
      numerator->Times10();
      delta_minus->Times10();
      if (delta_minus != delta_plus) {
        delta_plus->Times10();
      }
    } else if (in_delta_room_minus && in_delta_room_plus) {
      int compare = Bignum::PlusCompare(*numerator, *numerator, *denominator);
      if (compare < 0) {
      } else if (compare > 0) {
        DOUBLE_CONVERSION_ASSERT(buffer[(*length) - 1] != '9');
        buffer[(*length) - 1]++;
      } else {
        if ((buffer[(*length) - 1] - '0') % 2 == 0) {
        } else {
          DOUBLE_CONVERSION_ASSERT(buffer[(*length) - 1] != '9');
          buffer[(*length) - 1]++;
        }
      }
      return;
    } else if (in_delta_room_minus) {
      return;
    } else {
      DOUBLE_CONVERSION_ASSERT(buffer[(*length) -1] != '9');
      buffer[(*length) - 1]++;
      return;
    }
  }
}
static void GenerateCountedDigits(int count, int* decimal_point,
                                  Bignum* numerator, Bignum* denominator,
                                  Vector<char> buffer, int* length) {
  DOUBLE_CONVERSION_ASSERT(count >= 0);
  for (int i = 0; i < count - 1; ++i) {
    uint16_t digit;
    digit = numerator->DivideModuloIntBignum(*denominator);
    DOUBLE_CONVERSION_ASSERT(digit <= 9);
    buffer[i] = static_cast<char>(digit + '0');
    numerator->Times10();
  }
  uint16_t digit;
  digit = numerator->DivideModuloIntBignum(*denominator);
  if (Bignum::PlusCompare(*numerator, *numerator, *denominator) >= 0) {
    digit++;
  }
  DOUBLE_CONVERSION_ASSERT(digit <= 10);
  buffer[count - 1] = static_cast<char>(digit + '0');
  for (int i = count - 1; i > 0; --i) {
    if (buffer[i] != '0' + 10) break;
    buffer[i] = '0';
    buffer[i - 1]++;
  }
  if (buffer[0] == '0' + 10) {
    buffer[0] = '1';
    (*decimal_point)++;
  }
  *length = count;
}
static void BignumToFixed(int requested_digits, int* decimal_point,
                          Bignum* numerator, Bignum* denominator,
                          Vector<char> buffer, int* length) {
  if (-(*decimal_point) > requested_digits) {
    *decimal_point = -requested_digits;
    *length = 0;
    return;
  } else if (-(*decimal_point) == requested_digits) {
    DOUBLE_CONVERSION_ASSERT(*decimal_point == -requested_digits);
    denominator->Times10();
    if (Bignum::PlusCompare(*numerator, *numerator, *denominator) >= 0) {
      buffer[0] = '1';
      *length = 1;
      (*decimal_point)++;
    } else {
      *length = 0;
    }
    return;
  } else {
    int needed_digits = (*decimal_point) + requested_digits;
    GenerateCountedDigits(needed_digits, decimal_point,
                          numerator, denominator,
                          buffer, length);
  }
}
static int EstimatePower(int exponent) {
  const double k1Log10 = 0.30102999566398114;
  const int kSignificandSize = Double::kSignificandSize;
  double estimate = ceil((exponent + kSignificandSize - 1) * k1Log10 - 1e-10);
  return static_cast<int>(estimate);
}
static void InitialScaledStartValuesPositiveExponent(
    uint64_t significand, int exponent,
    int estimated_power, bool need_boundary_deltas,
    Bignum* numerator, Bignum* denominator,
    Bignum* delta_minus, Bignum* delta_plus) {
  DOUBLE_CONVERSION_ASSERT(estimated_power >= 0);
  numerator->AssignUInt64(significand);
  numerator->ShiftLeft(exponent);
  denominator->AssignPowerUInt16(10, estimated_power);
  if (need_boundary_deltas) {
    denominator->ShiftLeft(1);
    numerator->ShiftLeft(1);
    delta_plus->AssignUInt16(1);
    delta_plus->ShiftLeft(exponent);
    delta_minus->AssignUInt16(1);
    delta_minus->ShiftLeft(exponent);
  }
}
static void InitialScaledStartValuesNegativeExponentPositivePower(
    uint64_t significand, int exponent,
    int estimated_power, bool need_boundary_deltas,
    Bignum* numerator, Bignum* denominator,
    Bignum* delta_minus, Bignum* delta_plus) {
  numerator->AssignUInt64(significand);
  denominator->AssignPowerUInt16(10, estimated_power);
  denominator->ShiftLeft(-exponent);
  if (need_boundary_deltas) {
    denominator->ShiftLeft(1);
    numerator->ShiftLeft(1);
    delta_plus->AssignUInt16(1);
    delta_minus->AssignUInt16(1);
  }
}
static void InitialScaledStartValuesNegativeExponentNegativePower(
    uint64_t significand, int exponent,
    int estimated_power, bool need_boundary_deltas,
    Bignum* numerator, Bignum* denominator,
    Bignum* delta_minus, Bignum* delta_plus) {
  Bignum* power_ten = numerator;
  power_ten->AssignPowerUInt16(10, -estimated_power);
  if (need_boundary_deltas) {
    delta_plus->AssignBignum(*power_ten);
    delta_minus->AssignBignum(*power_ten);
  }
  DOUBLE_CONVERSION_ASSERT(numerator == power_ten);
  numerator->MultiplyByUInt64(significand);
  denominator->AssignUInt16(1);
  denominator->ShiftLeft(-exponent);
  if (need_boundary_deltas) {
    numerator->ShiftLeft(1);
    denominator->ShiftLeft(1);
  }
}
static void InitialScaledStartValues(uint64_t significand,
                                     int exponent,
                                     bool lower_boundary_is_closer,
                                     int estimated_power,
                                     bool need_boundary_deltas,
                                     Bignum* numerator,
                                     Bignum* denominator,
                                     Bignum* delta_minus,
                                     Bignum* delta_plus) {
  if (exponent >= 0) {
    InitialScaledStartValuesPositiveExponent(
        significand, exponent, estimated_power, need_boundary_deltas,
        numerator, denominator, delta_minus, delta_plus);
  } else if (estimated_power >= 0) {
    InitialScaledStartValuesNegativeExponentPositivePower(
        significand, exponent, estimated_power, need_boundary_deltas,
        numerator, denominator, delta_minus, delta_plus);
  } else {
    InitialScaledStartValuesNegativeExponentNegativePower(
        significand, exponent, estimated_power, need_boundary_deltas,
        numerator, denominator, delta_minus, delta_plus);
  }
  if (need_boundary_deltas && lower_boundary_is_closer) {
    denominator->ShiftLeft(1);
    numerator->ShiftLeft(1);
    delta_plus->ShiftLeft(1);
  }
}
static void FixupMultiply10(int estimated_power, bool is_even,
                            int* decimal_point,
                            Bignum* numerator, Bignum* denominator,
                            Bignum* delta_minus, Bignum* delta_plus) {
  bool in_range;
  if (is_even) {
    in_range = Bignum::PlusCompare(*numerator, *delta_plus, *denominator) >= 0;
  } else {
    in_range = Bignum::PlusCompare(*numerator, *delta_plus, *denominator) > 0;
  }
  if (in_range) {
    *decimal_point = estimated_power + 1;
  } else {
    *decimal_point = estimated_power;
    numerator->Times10();
    if (Bignum::Equal(*delta_minus, *delta_plus)) {
      delta_minus->Times10();
      delta_plus->AssignBignum(*delta_minus);
    } else {
      delta_minus->Times10();
      delta_plus->Times10();
    }
  }
}
}
/// bignum-dtoa.cc END
/// strtod.cc START
#include <climits>
#include <cstdarg>
namespace double_conversion {
#if defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
static const int kMaxExactDoubleIntegerDecimalDigits = 15;
#endif
static const int kMaxUint64DecimalDigits = 19;
static const int kMaxDecimalPower = 309;
static const int kMinDecimalPower = -324;
static const uint64_t kMaxUint64 = DOUBLE_CONVERSION_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF);
#if defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
static const double exact_powers_of_ten[] = {
  1.0,
  10.0,
  100.0,
  1000.0,
  10000.0,
  100000.0,
  1000000.0,
  10000000.0,
  100000000.0,
  1000000000.0,
  10000000000.0,
  100000000000.0,
  1000000000000.0,
  10000000000000.0,
  100000000000000.0,
  1000000000000000.0,
  10000000000000000.0,
  100000000000000000.0,
  1000000000000000000.0,
  10000000000000000000.0,
  100000000000000000000.0,
  1000000000000000000000.0,
  10000000000000000000000.0
};
static const int kExactPowersOfTenSize = DOUBLE_CONVERSION_ARRAY_SIZE(exact_powers_of_ten);
#endif
static const int kMaxSignificantDecimalDigits = 780;
static Vector<const char> TrimLeadingZeros(Vector<const char> buffer) {
  for (int i = 0; i < buffer.length(); i++) {
    if (buffer[i] != '0') {
      return buffer.SubVector(i, buffer.length());
    }
  }
  return Vector<const char>(buffer.start(), 0);
}
static void CutToMaxSignificantDigits(Vector<const char> buffer,
                                       int exponent,
                                       char* significant_buffer,
                                       int* significant_exponent) {
  for (int i = 0; i < kMaxSignificantDecimalDigits - 1; ++i) {
    significant_buffer[i] = buffer[i];
  }
  DOUBLE_CONVERSION_ASSERT(buffer[buffer.length() - 1] != '0');
  significant_buffer[kMaxSignificantDecimalDigits - 1] = '1';
  *significant_exponent =
      exponent + (buffer.length() - kMaxSignificantDecimalDigits);
}
static void TrimAndCut(Vector<const char> buffer, int exponent,
                       char* buffer_copy_space, int space_size,
                       Vector<const char>* trimmed, int* updated_exponent) {
  Vector<const char> left_trimmed = TrimLeadingZeros(buffer);
  Vector<const char> right_trimmed = TrimTrailingZeros(left_trimmed);
  exponent += left_trimmed.length() - right_trimmed.length();
  if (right_trimmed.length() > kMaxSignificantDecimalDigits) {
    (void) space_size;
    DOUBLE_CONVERSION_ASSERT(space_size >= kMaxSignificantDecimalDigits);
    CutToMaxSignificantDigits(right_trimmed, exponent,
                              buffer_copy_space, updated_exponent);
    *trimmed = Vector<const char>(buffer_copy_space,
                                 kMaxSignificantDecimalDigits);
  } else {
    *trimmed = right_trimmed;
    *updated_exponent = exponent;
  }
}
static uint64_t ReadUint64(Vector<const char> buffer,
                           int* number_of_read_digits) {
  uint64_t result = 0;
  int i = 0;
  while (i < buffer.length() && result <= (kMaxUint64 / 10 - 1)) {
    int digit = buffer[i++] - '0';
    DOUBLE_CONVERSION_ASSERT(0 <= digit && digit <= 9);
    result = 10 * result + digit;
  }
  *number_of_read_digits = i;
  return result;
}
static void ReadDiyFp(Vector<const char> buffer,
                      DiyFp* result,
                      int* remaining_decimals) {
  int read_digits;
  uint64_t significand = ReadUint64(buffer, &read_digits);
  if (buffer.length() == read_digits) {
    *result = DiyFp(significand, 0);
    *remaining_decimals = 0;
  } else {
    if (buffer[read_digits] >= '5') {
      significand++;
    }
    int exponent = 0;
    *result = DiyFp(significand, exponent);
    *remaining_decimals = buffer.length() - read_digits;
  }
}
static bool DoubleStrtod(Vector<const char> trimmed,
                         int exponent,
                         double* result) {
#if !defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
  (void) trimmed;
  (void) exponent;
  (void) result;
  return false;
#else
  if (trimmed.length() <= kMaxExactDoubleIntegerDecimalDigits) {
    int read_digits;
    if (exponent < 0 && -exponent < kExactPowersOfTenSize) {
      *result = static_cast<double>(ReadUint64(trimmed, &read_digits));
      DOUBLE_CONVERSION_ASSERT(read_digits == trimmed.length());
      *result /= exact_powers_of_ten[-exponent];
      return true;
    }
    if (0 <= exponent && exponent < kExactPowersOfTenSize) {
      *result = static_cast<double>(ReadUint64(trimmed, &read_digits));
      DOUBLE_CONVERSION_ASSERT(read_digits == trimmed.length());
      *result *= exact_powers_of_ten[exponent];
      return true;
    }
    int remaining_digits =
        kMaxExactDoubleIntegerDecimalDigits - trimmed.length();
    if ((0 <= exponent) &&
        (exponent - remaining_digits < kExactPowersOfTenSize)) {
      *result = static_cast<double>(ReadUint64(trimmed, &read_digits));
      DOUBLE_CONVERSION_ASSERT(read_digits == trimmed.length());
      *result *= exact_powers_of_ten[remaining_digits];
      *result *= exact_powers_of_ten[exponent - remaining_digits];
      return true;
    }
  }
  return false;
#endif
}
static DiyFp AdjustmentPowerOfTen(int exponent) {
  DOUBLE_CONVERSION_ASSERT(0 < exponent);
  DOUBLE_CONVERSION_ASSERT(exponent < PowersOfTenCache::kDecimalExponentDistance);
  DOUBLE_CONVERSION_ASSERT(PowersOfTenCache::kDecimalExponentDistance == 8);
  switch (exponent) {
    case 1: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xa0000000, 00000000), -60);
    case 2: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xc8000000, 00000000), -57);
    case 3: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xfa000000, 00000000), -54);
    case 4: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0x9c400000, 00000000), -50);
    case 5: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xc3500000, 00000000), -47);
    case 6: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xf4240000, 00000000), -44);
    case 7: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0x98968000, 00000000), -40);
    default:
      DOUBLE_CONVERSION_UNREACHABLE();
  }
}
static bool DiyFpStrtod(Vector<const char> buffer,
                        int exponent,
                        double* result) {
  DiyFp input;
  int remaining_decimals;
  ReadDiyFp(buffer, &input, &remaining_decimals);
  const int kDenominatorLog = 3;
  const int kDenominator = 1 << kDenominatorLog;
  exponent += remaining_decimals;
  uint64_t error = (remaining_decimals == 0 ? 0 : kDenominator / 2);
  int old_e = input.e();
  input.Normalize();
  error <<= old_e - input.e();
  DOUBLE_CONVERSION_ASSERT(exponent <= PowersOfTenCache::kMaxDecimalExponent);
  if (exponent < PowersOfTenCache::kMinDecimalExponent) {
    *result = 0.0;
    return true;
  }
  DiyFp cached_power;
  int cached_decimal_exponent;
  PowersOfTenCache::GetCachedPowerForDecimalExponent(exponent,
                                                     &cached_power,
                                                     &cached_decimal_exponent);
  if (cached_decimal_exponent != exponent) {
    int adjustment_exponent = exponent - cached_decimal_exponent;
    DiyFp adjustment_power = AdjustmentPowerOfTen(adjustment_exponent);
    input.Multiply(adjustment_power);
    if (kMaxUint64DecimalDigits - buffer.length() >= adjustment_exponent) {
      DOUBLE_CONVERSION_ASSERT(DiyFp::kSignificandSize == 64);
    } else {
      error += kDenominator / 2;
    }
  }
  input.Multiply(cached_power);
  int error_b = kDenominator / 2;
  int error_ab = (error == 0 ? 0 : 1);
  int fixed_error = kDenominator / 2;
  error += error_b + error_ab + fixed_error;
  old_e = input.e();
  input.Normalize();
  error <<= old_e - input.e();
  int order_of_magnitude = DiyFp::kSignificandSize + input.e();
  int effective_significand_size =
      Double::SignificandSizeForOrderOfMagnitude(order_of_magnitude);
  int precision_digits_count =
      DiyFp::kSignificandSize - effective_significand_size;
  if (precision_digits_count + kDenominatorLog >= DiyFp::kSignificandSize) {
    int shift_amount = (precision_digits_count + kDenominatorLog) -
        DiyFp::kSignificandSize + 1;
    input.set_f(input.f() >> shift_amount);
    input.set_e(input.e() + shift_amount);
    error = (error >> shift_amount) + 1 + kDenominator;
    precision_digits_count -= shift_amount;
  }
  DOUBLE_CONVERSION_ASSERT(DiyFp::kSignificandSize == 64);
  DOUBLE_CONVERSION_ASSERT(precision_digits_count < 64);
  uint64_t one64 = 1;
  uint64_t precision_bits_mask = (one64 << precision_digits_count) - 1;
  uint64_t precision_bits = input.f() & precision_bits_mask;
  uint64_t half_way = one64 << (precision_digits_count - 1);
  precision_bits *= kDenominator;
  half_way *= kDenominator;
  DiyFp rounded_input(input.f() >> precision_digits_count,
                      input.e() + precision_digits_count);
  if (precision_bits >= half_way + error) {
    rounded_input.set_f(rounded_input.f() + 1);
  }
  *result = Double(rounded_input).value();
  if (half_way - error < precision_bits && precision_bits < half_way + error) {
    return false;
  } else {
    return true;
  }
}
static int CompareBufferWithDiyFp(Vector<const char> buffer,
                                  int exponent,
                                  DiyFp diy_fp) {
  DOUBLE_CONVERSION_ASSERT(buffer.length() + exponent <= kMaxDecimalPower + 1);
  DOUBLE_CONVERSION_ASSERT(buffer.length() + exponent > kMinDecimalPower);
  DOUBLE_CONVERSION_ASSERT(buffer.length() <= kMaxSignificantDecimalDigits);
  DOUBLE_CONVERSION_ASSERT(((kMaxDecimalPower + 1) * 333 / 100) < Bignum::kMaxSignificantBits);
  Bignum buffer_bignum;
  Bignum diy_fp_bignum;
  buffer_bignum.AssignDecimalString(buffer);
  diy_fp_bignum.AssignUInt64(diy_fp.f());
  if (exponent >= 0) {
    buffer_bignum.MultiplyByPowerOfTen(exponent);
  } else {
    diy_fp_bignum.MultiplyByPowerOfTen(-exponent);
  }
  if (diy_fp.e() > 0) {
    diy_fp_bignum.ShiftLeft(diy_fp.e());
  } else {
    buffer_bignum.ShiftLeft(-diy_fp.e());
  }
  return Bignum::Compare(buffer_bignum, diy_fp_bignum);
}
static bool ComputeGuess(Vector<const char> trimmed, int exponent,
                         double* guess) {
  if (trimmed.length() == 0) {
    *guess = 0.0;
    return true;
  }
  if (exponent + trimmed.length() - 1 >= kMaxDecimalPower) {
    *guess = Double::Infinity();
    return true;
  }
  if (exponent + trimmed.length() <= kMinDecimalPower) {
    *guess = 0.0;
    return true;
  }
  if (DoubleStrtod(trimmed, exponent, guess) ||
      DiyFpStrtod(trimmed, exponent, guess)) {
    return true;
  }
  if (*guess == Double::Infinity()) {
    return true;
  }
  return false;
}
static bool IsDigit(const char d) {
  return ('0' <= d) && (d <= '9');
}
static bool IsNonZeroDigit(const char d) {
  return ('1' <= d) && (d <= '9');
}
#ifdef __has_cpp_attribute
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
#endif
static bool AssertTrimmedDigits(const Vector<const char>& buffer) {
  for(int i = 0; i < buffer.length(); ++i) {
    if(!IsDigit(buffer[i])) {
      return false;
    }
  }
  return (buffer.length() == 0) || (IsNonZeroDigit(buffer[0]) && IsNonZeroDigit(buffer[buffer.length()-1]));
}
double StrtodTrimmed(Vector<const char> trimmed, int exponent) {
  DOUBLE_CONVERSION_ASSERT(trimmed.length() <= kMaxSignificantDecimalDigits);
  DOUBLE_CONVERSION_ASSERT(AssertTrimmedDigits(trimmed));
  double guess;
  const bool is_correct = ComputeGuess(trimmed, exponent, &guess);
  if (is_correct) {
    return guess;
  }
  DiyFp upper_boundary = Double(guess).UpperBoundary();
  int comparison = CompareBufferWithDiyFp(trimmed, exponent, upper_boundary);
  if (comparison < 0) {
    return guess;
  } else if (comparison > 0) {
    return Double(guess).NextDouble();
  } else if ((Double(guess).Significand() & 1) == 0) {
    return guess;
  } else {
    return Double(guess).NextDouble();
  }
}
double Strtod(Vector<const char> buffer, int exponent) {
  char copy_buffer[kMaxSignificantDecimalDigits];
  Vector<const char> trimmed;
  int updated_exponent;
  TrimAndCut(buffer, exponent, copy_buffer, kMaxSignificantDecimalDigits,
             &trimmed, &updated_exponent);
  return StrtodTrimmed(trimmed, updated_exponent);
}
static float SanitizedDoubletof(double d) {
  DOUBLE_CONVERSION_ASSERT(d >= 0.0);
  float max_finite = 3.4028234663852885981170418348451692544e+38;
  double half_max_finite_infinity =
      3.40282356779733661637539395458142568448e+38;
  if (d >= max_finite) {
    if (d >= half_max_finite_infinity) {
      return Single::Infinity();
    } else {
      return max_finite;
    }
  } else {
    return static_cast<float>(d);
  }
}
float Strtof(Vector<const char> buffer, int exponent) {
  char copy_buffer[kMaxSignificantDecimalDigits];
  Vector<const char> trimmed;
  int updated_exponent;
  TrimAndCut(buffer, exponent, copy_buffer, kMaxSignificantDecimalDigits,
             &trimmed, &updated_exponent);
  exponent = updated_exponent;
  return StrtofTrimmed(trimmed, exponent);
}
float StrtofTrimmed(Vector<const char> trimmed, int exponent) {
  DOUBLE_CONVERSION_ASSERT(trimmed.length() <= kMaxSignificantDecimalDigits);
  DOUBLE_CONVERSION_ASSERT(AssertTrimmedDigits(trimmed));
  double double_guess;
  bool is_correct = ComputeGuess(trimmed, exponent, &double_guess);
  float float_guess = SanitizedDoubletof(double_guess);
  if (float_guess == double_guess) {
    return float_guess;
  }
  double double_next = Double(double_guess).NextDouble();
  double double_previous = Double(double_guess).PreviousDouble();
  float f1 = SanitizedDoubletof(double_previous);
  float f2 = float_guess;
  float f3 = SanitizedDoubletof(double_next);
  float f4;
  if (is_correct) {
    f4 = f3;
  } else {
    double double_next2 = Double(double_next).NextDouble();
    f4 = SanitizedDoubletof(double_next2);
  }
  (void) f2;
  DOUBLE_CONVERSION_ASSERT(f1 <= f2 && f2 <= f3 && f3 <= f4);
  if (f1 == f4) {
    return float_guess;
  }
  DOUBLE_CONVERSION_ASSERT((f1 != f2 && f2 == f3 && f3 == f4) ||
         (f1 == f2 && f2 != f3 && f3 == f4) ||
         (f1 == f2 && f2 == f3 && f3 != f4));
  float guess = f1;
  float next = f4;
  DiyFp upper_boundary;
  if (guess == 0.0f) {
    float min_float = 1e-45f;
    upper_boundary = Double(static_cast<double>(min_float) / 2).AsDiyFp();
  } else {
    upper_boundary = Single(guess).UpperBoundary();
  }
  int comparison = CompareBufferWithDiyFp(trimmed, exponent, upper_boundary);
  if (comparison < 0) {
    return guess;
  } else if (comparison > 0) {
    return next;
  } else if ((Single(guess).Significand() & 1) == 0) {
    return guess;
  } else {
    return next;
  }
}
}
/// strtod.cc END
/// fixed-dtoa.h START
#ifndef DOUBLE_CONVERSION_FIXED_DTOA_H_
#define DOUBLE_CONVERSION_FIXED_DTOA_H_
namespace double_conversion {
bool FastFixedDtoa(double v, int fractional_count,
                   Vector<char> buffer, int* length, int* decimal_point);
}
#endif
/// fixed-dtoa.h END
/// fixed-dtoa.cc START
#include <cmath>
namespace double_conversion {
class UInt128 {
 public:
  UInt128() : high_bits_(0), low_bits_(0) { }
  UInt128(uint64_t high, uint64_t low) : high_bits_(high), low_bits_(low) { }
  void Multiply(uint32_t multiplicand) {
    uint64_t accumulator;
    accumulator = (low_bits_ & kMask32) * multiplicand;
    uint32_t part = static_cast<uint32_t>(accumulator & kMask32);
    accumulator >>= 32;
    accumulator = accumulator + (low_bits_ >> 32) * multiplicand;
    low_bits_ = (accumulator << 32) + part;
    accumulator >>= 32;
    accumulator = accumulator + (high_bits_ & kMask32) * multiplicand;
    part = static_cast<uint32_t>(accumulator & kMask32);
    accumulator >>= 32;
    accumulator = accumulator + (high_bits_ >> 32) * multiplicand;
    high_bits_ = (accumulator << 32) + part;
    DOUBLE_CONVERSION_ASSERT((accumulator >> 32) == 0);
  }
  void Shift(int shift_amount) {
    DOUBLE_CONVERSION_ASSERT(-64 <= shift_amount && shift_amount <= 64);
    if (shift_amount == 0) {
      return;
    } else if (shift_amount == -64) {
      high_bits_ = low_bits_;
      low_bits_ = 0;
    } else if (shift_amount == 64) {
      low_bits_ = high_bits_;
      high_bits_ = 0;
    } else if (shift_amount <= 0) {
      high_bits_ <<= -shift_amount;
      high_bits_ += low_bits_ >> (64 + shift_amount);
      low_bits_ <<= -shift_amount;
    } else {
      low_bits_ >>= shift_amount;
      low_bits_ += high_bits_ << (64 - shift_amount);
      high_bits_ >>= shift_amount;
    }
  }
  int DivModPowerOf2(int power) {
    if (power >= 64) {
      int result = static_cast<int>(high_bits_ >> (power - 64));
      high_bits_ -= static_cast<uint64_t>(result) << (power - 64);
      return result;
    } else {
      uint64_t part_low = low_bits_ >> power;
      uint64_t part_high = high_bits_ << (64 - power);
      int result = static_cast<int>(part_low + part_high);
      high_bits_ = 0;
      low_bits_ -= part_low << power;
      return result;
    }
  }
  bool IsZero() const {
    return high_bits_ == 0 && low_bits_ == 0;
  }
  int BitAt(int position) const {
    if (position >= 64) {
      return static_cast<int>(high_bits_ >> (position - 64)) & 1;
    } else {
      return static_cast<int>(low_bits_ >> position) & 1;
    }
  }
 private:
  static const uint64_t kMask32 = 0xFFFFFFFF;
  uint64_t high_bits_;
  uint64_t low_bits_;
};
static const int kDoubleSignificandSize = 53;
static void FillDigits32FixedLength(uint32_t number, int requested_length,
                                    Vector<char> buffer, int* length) {
  for (int i = requested_length - 1; i >= 0; --i) {
    buffer[(*length) + i] = '0' + number % 10;
    number /= 10;
  }
  *length += requested_length;
}
static void FillDigits32(uint32_t number, Vector<char> buffer, int* length) {
  int number_length = 0;
  while (number != 0) {
    int digit = number % 10;
    number /= 10;
    buffer[(*length) + number_length] = static_cast<char>('0' + digit);
    number_length++;
  }
  int i = *length;
  int j = *length + number_length - 1;
  while (i < j) {
    char tmp = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = tmp;
    i++;
    j--;
  }
  *length += number_length;
}
static void FillDigits64FixedLength(uint64_t number,
                                    Vector<char> buffer, int* length) {
  const uint32_t kTen7 = 10000000;
  uint32_t part2 = static_cast<uint32_t>(number % kTen7);
  number /= kTen7;
  uint32_t part1 = static_cast<uint32_t>(number % kTen7);
  uint32_t part0 = static_cast<uint32_t>(number / kTen7);
  FillDigits32FixedLength(part0, 3, buffer, length);
  FillDigits32FixedLength(part1, 7, buffer, length);
  FillDigits32FixedLength(part2, 7, buffer, length);
}
static void FillDigits64(uint64_t number, Vector<char> buffer, int* length) {
  const uint32_t kTen7 = 10000000;
  uint32_t part2 = static_cast<uint32_t>(number % kTen7);
  number /= kTen7;
  uint32_t part1 = static_cast<uint32_t>(number % kTen7);
  uint32_t part0 = static_cast<uint32_t>(number / kTen7);
  if (part0 != 0) {
    FillDigits32(part0, buffer, length);
    FillDigits32FixedLength(part1, 7, buffer, length);
    FillDigits32FixedLength(part2, 7, buffer, length);
  } else if (part1 != 0) {
    FillDigits32(part1, buffer, length);
    FillDigits32FixedLength(part2, 7, buffer, length);
  } else {
    FillDigits32(part2, buffer, length);
  }
}
static void RoundUp(Vector<char> buffer, int* length, int* decimal_point) {
  if (*length == 0) {
    buffer[0] = '1';
    *decimal_point = 1;
    *length = 1;
    return;
  }
  buffer[(*length) - 1]++;
  for (int i = (*length) - 1; i > 0; --i) {
    if (buffer[i] != '0' + 10) {
      return;
    }
    buffer[i] = '0';
    buffer[i - 1]++;
  }
  if (buffer[0] == '0' + 10) {
    buffer[0] = '1';
    (*decimal_point)++;
  }
}
static void FillFractionals(uint64_t fractionals, int exponent,
                            int fractional_count, Vector<char> buffer,
                            int* length, int* decimal_point) {
  DOUBLE_CONVERSION_ASSERT(-128 <= exponent && exponent <= 0);
  if (-exponent <= 64) {
    DOUBLE_CONVERSION_ASSERT(fractionals >> 56 == 0);
    int point = -exponent;
    for (int i = 0; i < fractional_count; ++i) {
      if (fractionals == 0) break;
      fractionals *= 5;
      point--;
      int digit = static_cast<int>(fractionals >> point);
      DOUBLE_CONVERSION_ASSERT(digit <= 9);
      buffer[*length] = static_cast<char>('0' + digit);
      (*length)++;
      fractionals -= static_cast<uint64_t>(digit) << point;
    }
    DOUBLE_CONVERSION_ASSERT(fractionals == 0 || point - 1 >= 0);
    if ((fractionals != 0) && ((fractionals >> (point - 1)) & 1) == 1) {
      RoundUp(buffer, length, decimal_point);
    }
  } else {
    DOUBLE_CONVERSION_ASSERT(64 < -exponent && -exponent <= 128);
    UInt128 fractionals128 = UInt128(fractionals, 0);
    fractionals128.Shift(-exponent - 64);
    int point = 128;
    for (int i = 0; i < fractional_count; ++i) {
      if (fractionals128.IsZero()) break;
      fractionals128.Multiply(5);
      point--;
      int digit = fractionals128.DivModPowerOf2(point);
      DOUBLE_CONVERSION_ASSERT(digit <= 9);
      buffer[*length] = static_cast<char>('0' + digit);
      (*length)++;
    }
    if (fractionals128.BitAt(point - 1) == 1) {
      RoundUp(buffer, length, decimal_point);
    }
  }
}
static void TrimZeros(Vector<char> buffer, int* length, int* decimal_point) {
  while (*length > 0 && buffer[(*length) - 1] == '0') {
    (*length)--;
  }
  int first_non_zero = 0;
  while (first_non_zero < *length && buffer[first_non_zero] == '0') {
    first_non_zero++;
  }
  if (first_non_zero != 0) {
    for (int i = first_non_zero; i < *length; ++i) {
      buffer[i - first_non_zero] = buffer[i];
    }
    *length -= first_non_zero;
    *decimal_point -= first_non_zero;
  }
}
bool FastFixedDtoa(double v,
                   int fractional_count,
                   Vector<char> buffer,
                   int* length,
                   int* decimal_point) {
  const uint32_t kMaxUInt32 = 0xFFFFFFFF;
  uint64_t significand = Double(v).Significand();
  int exponent = Double(v).Exponent();
  if (exponent > 20) return false;
  if (fractional_count > 20) return false;
  *length = 0;
  if (exponent + kDoubleSignificandSize > 64) {
    const uint64_t kFive17 = DOUBLE_CONVERSION_UINT64_2PART_C(0xB1, A2BC2EC5);
    uint64_t divisor = kFive17;
    int divisor_power = 17;
    uint64_t dividend = significand;
    uint32_t quotient;
    uint64_t remainder;
    if (exponent > divisor_power) {
      dividend <<= exponent - divisor_power;
      quotient = static_cast<uint32_t>(dividend / divisor);
      remainder = (dividend % divisor) << divisor_power;
    } else {
      divisor <<= divisor_power - exponent;
      quotient = static_cast<uint32_t>(dividend / divisor);
      remainder = (dividend % divisor) << exponent;
    }
    FillDigits32(quotient, buffer, length);
    FillDigits64FixedLength(remainder, buffer, length);
    *decimal_point = *length;
  } else if (exponent >= 0) {
    significand <<= exponent;
    FillDigits64(significand, buffer, length);
    *decimal_point = *length;
  } else if (exponent > -kDoubleSignificandSize) {
    uint64_t integrals = significand >> -exponent;
    uint64_t fractionals = significand - (integrals << -exponent);
    if (integrals > kMaxUInt32) {
      FillDigits64(integrals, buffer, length);
    } else {
      FillDigits32(static_cast<uint32_t>(integrals), buffer, length);
    }
    *decimal_point = *length;
    FillFractionals(fractionals, exponent, fractional_count,
                    buffer, length, decimal_point);
  } else if (exponent < -128) {
    DOUBLE_CONVERSION_ASSERT(fractional_count <= 20);
    buffer[0] = '\0';
    *length = 0;
    *decimal_point = -fractional_count;
  } else {
    *decimal_point = 0;
    FillFractionals(significand, exponent, fractional_count,
                    buffer, length, decimal_point);
  }
  TrimZeros(buffer, length, decimal_point);
  buffer[*length] = '\0';
  if ((*length) == 0) {
    *decimal_point = -fractional_count;
  }
  return true;
}
}
/// fixed-dtoa.cc END
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
/// fast-dtoa.cc START

namespace double_conversion {
static const int kMinimalTargetExponent = -60;
static const int kMaximalTargetExponent = -32;
static bool RoundWeed(Vector<char> buffer,
                      int length,
                      uint64_t distance_too_high_w,
                      uint64_t unsafe_interval,
                      uint64_t rest,
                      uint64_t ten_kappa,
                      uint64_t unit) {
  uint64_t small_distance = distance_too_high_w - unit;
  uint64_t big_distance = distance_too_high_w + unit;
  DOUBLE_CONVERSION_ASSERT(rest <= unsafe_interval);
  while (rest < small_distance &&
         unsafe_interval - rest >= ten_kappa &&
         (rest + ten_kappa < small_distance ||
          small_distance - rest >= rest + ten_kappa - small_distance)) {
    buffer[length - 1]--;
    rest += ten_kappa;
  }
  if (rest < big_distance &&
      unsafe_interval - rest >= ten_kappa &&
      (rest + ten_kappa < big_distance ||
       big_distance - rest > rest + ten_kappa - big_distance)) {
    return false;
  }
  return (2 * unit <= rest) && (rest <= unsafe_interval - 4 * unit);
}
static bool RoundWeedCounted(Vector<char> buffer,
                             int length,
                             uint64_t rest,
                             uint64_t ten_kappa,
                             uint64_t unit,
                             int* kappa) {
  DOUBLE_CONVERSION_ASSERT(rest < ten_kappa);
  if (unit >= ten_kappa) return false;
  if (ten_kappa - unit <= unit) return false;
  if ((ten_kappa - rest > rest) && (ten_kappa - 2 * rest >= 2 * unit)) {
    return true;
  }
  if ((rest > unit) && (ten_kappa - (rest - unit) <= (rest - unit))) {
    buffer[length - 1]++;
    for (int i = length - 1; i > 0; --i) {
      if (buffer[i] != '0' + 10) break;
      buffer[i] = '0';
      buffer[i - 1]++;
    }
    if (buffer[0] == '0' + 10) {
      buffer[0] = '1';
      (*kappa) += 1;
    }
    return true;
  }
  return false;
}
static unsigned int const kSmallPowersOfTen[] =
    {0, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000,
     1000000000};
static void BiggestPowerTen(uint32_t number,
                            int number_bits,
                            uint32_t* power,
                            int* exponent_plus_one) {
  DOUBLE_CONVERSION_ASSERT(number < (1u << (number_bits + 1)));
  int exponent_plus_one_guess = ((number_bits + 1) * 1233 >> 12);
  exponent_plus_one_guess++;
  if (number < kSmallPowersOfTen[exponent_plus_one_guess]) {
    exponent_plus_one_guess--;
  }
  *power = kSmallPowersOfTen[exponent_plus_one_guess];
  *exponent_plus_one = exponent_plus_one_guess;
}
static bool DigitGen(DiyFp low,
                     DiyFp w,
                     DiyFp high,
                     Vector<char> buffer,
                     int* length,
                     int* kappa) {
  DOUBLE_CONVERSION_ASSERT(low.e() == w.e() && w.e() == high.e());
  DOUBLE_CONVERSION_ASSERT(low.f() + 1 <= high.f() - 1);
  DOUBLE_CONVERSION_ASSERT(kMinimalTargetExponent <= w.e() && w.e() <= kMaximalTargetExponent);
  uint64_t unit = 1;
  DiyFp too_low = DiyFp(low.f() - unit, low.e());
  DiyFp too_high = DiyFp(high.f() + unit, high.e());
  DiyFp unsafe_interval = DiyFp::Minus(too_high, too_low);
  DiyFp one = DiyFp(static_cast<uint64_t>(1) << -w.e(), w.e());
  uint32_t integrals = static_cast<uint32_t>(too_high.f() >> -one.e());
  uint64_t fractionals = too_high.f() & (one.f() - 1);
  uint32_t divisor;
  int divisor_exponent_plus_one;
  BiggestPowerTen(integrals, DiyFp::kSignificandSize - (-one.e()),
                  &divisor, &divisor_exponent_plus_one);
  *kappa = divisor_exponent_plus_one;
  *length = 0;
  while (*kappa > 0) {
    int digit = integrals / divisor;
    DOUBLE_CONVERSION_ASSERT(digit <= 9);
    buffer[*length] = static_cast<char>('0' + digit);
    (*length)++;
    integrals %= divisor;
    (*kappa)--;
    uint64_t rest =
        (static_cast<uint64_t>(integrals) << -one.e()) + fractionals;
    if (rest < unsafe_interval.f()) {
      return RoundWeed(buffer, *length, DiyFp::Minus(too_high, w).f(),
                       unsafe_interval.f(), rest,
                       static_cast<uint64_t>(divisor) << -one.e(), unit);
    }
    divisor /= 10;
  }
  DOUBLE_CONVERSION_ASSERT(one.e() >= -60);
  DOUBLE_CONVERSION_ASSERT(fractionals < one.f());
  DOUBLE_CONVERSION_ASSERT(DOUBLE_CONVERSION_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF) / 10 >= one.f());
  for (;;) {
    fractionals *= 10;
    unit *= 10;
    unsafe_interval.set_f(unsafe_interval.f() * 10);
    int digit = static_cast<int>(fractionals >> -one.e());
    DOUBLE_CONVERSION_ASSERT(digit <= 9);
    buffer[*length] = static_cast<char>('0' + digit);
    (*length)++;
    fractionals &= one.f() - 1;
    (*kappa)--;
    if (fractionals < unsafe_interval.f()) {
      return RoundWeed(buffer, *length, DiyFp::Minus(too_high, w).f() * unit,
                       unsafe_interval.f(), fractionals, one.f(), unit);
    }
  }
}
static bool DigitGenCounted(DiyFp w,
                            int requested_digits,
                            Vector<char> buffer,
                            int* length,
                            int* kappa) {
  DOUBLE_CONVERSION_ASSERT(kMinimalTargetExponent <= w.e() && w.e() <= kMaximalTargetExponent);
  DOUBLE_CONVERSION_ASSERT(kMinimalTargetExponent >= -60);
  DOUBLE_CONVERSION_ASSERT(kMaximalTargetExponent <= -32);
  uint64_t w_error = 1;
  DiyFp one = DiyFp(static_cast<uint64_t>(1) << -w.e(), w.e());
  uint32_t integrals = static_cast<uint32_t>(w.f() >> -one.e());
  uint64_t fractionals = w.f() & (one.f() - 1);
  uint32_t divisor;
  int divisor_exponent_plus_one;
  BiggestPowerTen(integrals, DiyFp::kSignificandSize - (-one.e()),
                  &divisor, &divisor_exponent_plus_one);
  *kappa = divisor_exponent_plus_one;
  *length = 0;
  while (*kappa > 0) {
    int digit = integrals / divisor;
    DOUBLE_CONVERSION_ASSERT(digit <= 9);
    buffer[*length] = static_cast<char>('0' + digit);
    (*length)++;
    requested_digits--;
    integrals %= divisor;
    (*kappa)--;
    if (requested_digits == 0) break;
    divisor /= 10;
  }
  if (requested_digits == 0) {
    uint64_t rest =
        (static_cast<uint64_t>(integrals) << -one.e()) + fractionals;
    return RoundWeedCounted(buffer, *length, rest,
                            static_cast<uint64_t>(divisor) << -one.e(), w_error,
                            kappa);
  }
  DOUBLE_CONVERSION_ASSERT(one.e() >= -60);
  DOUBLE_CONVERSION_ASSERT(fractionals < one.f());
  DOUBLE_CONVERSION_ASSERT(DOUBLE_CONVERSION_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF) / 10 >= one.f());
  while (requested_digits > 0 && fractionals > w_error) {
    fractionals *= 10;
    w_error *= 10;
    int digit = static_cast<int>(fractionals >> -one.e());
    DOUBLE_CONVERSION_ASSERT(digit <= 9);
    buffer[*length] = static_cast<char>('0' + digit);
    (*length)++;
    requested_digits--;
    fractionals &= one.f() - 1;
    (*kappa)--;
  }
  if (requested_digits != 0) return false;
  return RoundWeedCounted(buffer, *length, fractionals, one.f(), w_error,
                          kappa);
}
static bool Grisu3(double v,
                   FastDtoaMode mode,
                   Vector<char> buffer,
                   int* length,
                   int* decimal_exponent) {
  DiyFp w = Double(v).AsNormalizedDiyFp();
  DiyFp boundary_minus, boundary_plus;
  if (mode == FAST_DTOA_SHORTEST) {
    Double(v).NormalizedBoundaries(&boundary_minus, &boundary_plus);
  } else {
    DOUBLE_CONVERSION_ASSERT(mode == FAST_DTOA_SHORTEST_SINGLE);
    float single_v = static_cast<float>(v);
    Single(single_v).NormalizedBoundaries(&boundary_minus, &boundary_plus);
  }
  DOUBLE_CONVERSION_ASSERT(boundary_plus.e() == w.e());
  DiyFp ten_mk;
  int mk;
  int ten_mk_minimal_binary_exponent =
     kMinimalTargetExponent - (w.e() + DiyFp::kSignificandSize);
  int ten_mk_maximal_binary_exponent =
     kMaximalTargetExponent - (w.e() + DiyFp::kSignificandSize);
  PowersOfTenCache::GetCachedPowerForBinaryExponentRange(
      ten_mk_minimal_binary_exponent,
      ten_mk_maximal_binary_exponent,
      &ten_mk, &mk);
  DOUBLE_CONVERSION_ASSERT((kMinimalTargetExponent <= w.e() + ten_mk.e() +
          DiyFp::kSignificandSize) &&
         (kMaximalTargetExponent >= w.e() + ten_mk.e() +
          DiyFp::kSignificandSize));
  DiyFp scaled_w = DiyFp::Times(w, ten_mk);
  DOUBLE_CONVERSION_ASSERT(scaled_w.e() ==
         boundary_plus.e() + ten_mk.e() + DiyFp::kSignificandSize);
  DiyFp scaled_boundary_minus = DiyFp::Times(boundary_minus, ten_mk);
  DiyFp scaled_boundary_plus  = DiyFp::Times(boundary_plus,  ten_mk);
  int kappa;
  bool result = DigitGen(scaled_boundary_minus, scaled_w, scaled_boundary_plus,
                         buffer, length, &kappa);
  *decimal_exponent = -mk + kappa;
  return result;
}
static bool Grisu3Counted(double v,
                          int requested_digits,
                          Vector<char> buffer,
                          int* length,
                          int* decimal_exponent) {
  DiyFp w = Double(v).AsNormalizedDiyFp();
  DiyFp ten_mk;
  int mk;
  int ten_mk_minimal_binary_exponent =
     kMinimalTargetExponent - (w.e() + DiyFp::kSignificandSize);
  int ten_mk_maximal_binary_exponent =
     kMaximalTargetExponent - (w.e() + DiyFp::kSignificandSize);
  PowersOfTenCache::GetCachedPowerForBinaryExponentRange(
      ten_mk_minimal_binary_exponent,
      ten_mk_maximal_binary_exponent,
      &ten_mk, &mk);
  DOUBLE_CONVERSION_ASSERT((kMinimalTargetExponent <= w.e() + ten_mk.e() +
          DiyFp::kSignificandSize) &&
         (kMaximalTargetExponent >= w.e() + ten_mk.e() +
          DiyFp::kSignificandSize));
  DiyFp scaled_w = DiyFp::Times(w, ten_mk);
  int kappa;
  bool result = DigitGenCounted(scaled_w, requested_digits,
                                buffer, length, &kappa);
  *decimal_exponent = -mk + kappa;
  return result;
}
bool FastDtoa(double v,
              FastDtoaMode mode,
              int requested_digits,
              Vector<char> buffer,
              int* length,
              int* decimal_point) {
  DOUBLE_CONVERSION_ASSERT(v > 0);
  DOUBLE_CONVERSION_ASSERT(!Double(v).IsSpecial());
  bool result = false;
  int decimal_exponent = 0;
  switch (mode) {
    case FAST_DTOA_SHORTEST:
    case FAST_DTOA_SHORTEST_SINGLE:
      result = Grisu3(v, mode, buffer, length, &decimal_exponent);
      break;
    case FAST_DTOA_PRECISION:
      result = Grisu3Counted(v, requested_digits,
                             buffer, length, &decimal_exponent);
      break;
    default:
      DOUBLE_CONVERSION_UNREACHABLE();
  }
  if (result) {
    *decimal_point = *length + decimal_exponent;
    buffer[*length] = '\0';
  }
  return result;
}
}
/// fast-dtoa.cc END
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
/// double-conversion.h START
#ifndef DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_
#define DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_
#endif
/// double-conversion.h END
/// double-to-string.cc START
#include <algorithm>
#include <climits>
#include <cmath>
namespace double_conversion {
const DoubleToStringConverter& DoubleToStringConverter::EcmaScriptConverter() {
  int flags = UNIQUE_ZERO | EMIT_POSITIVE_EXPONENT_SIGN;
  static DoubleToStringConverter converter(flags,
                                           "Infinity",
                                           "NaN",
                                           'e',
                                           -6, 21,
                                           6, 0);
  return converter;
}
bool DoubleToStringConverter::HandleSpecialValues(
    double value,
    StringBuilder* result_builder) const {
  Double double_inspect(value);
  if (double_inspect.IsInfinite()) {
    if (infinity_symbol_ == DOUBLE_CONVERSION_NULLPTR) return false;
    if (value < 0) {
      result_builder->AddCharacter('-');
    }
    result_builder->AddString(infinity_symbol_);
    return true;
  }
  if (double_inspect.IsNan()) {
    if (nan_symbol_ == DOUBLE_CONVERSION_NULLPTR) return false;
    result_builder->AddString(nan_symbol_);
    return true;
  }
  return false;
}
void DoubleToStringConverter::CreateExponentialRepresentation(
    const char* decimal_digits,
    int length,
    int exponent,
    StringBuilder* result_builder) const {
  DOUBLE_CONVERSION_ASSERT(length != 0);
  result_builder->AddCharacter(decimal_digits[0]);
  if (length == 1) {
    if ((flags_ & EMIT_TRAILING_DECIMAL_POINT_IN_EXPONENTIAL) != 0) {
      result_builder->AddCharacter('.');
      if ((flags_ & EMIT_TRAILING_ZERO_AFTER_POINT_IN_EXPONENTIAL) != 0) {
          result_builder->AddCharacter('0');
      }
    }
  } else {
    result_builder->AddCharacter('.');
    result_builder->AddSubstring(&decimal_digits[1], length-1);
  }
  result_builder->AddCharacter(exponent_character_);
  if (exponent < 0) {
    result_builder->AddCharacter('-');
    exponent = -exponent;
  } else {
    if ((flags_ & EMIT_POSITIVE_EXPONENT_SIGN) != 0) {
      result_builder->AddCharacter('+');
    }
  }
  DOUBLE_CONVERSION_ASSERT(exponent < 1e4);
  const int kMaxExponentLength = 5;
  char buffer[kMaxExponentLength + 1];
  buffer[kMaxExponentLength] = '\0';
  int first_char_pos = kMaxExponentLength;
  if (exponent == 0) {
    buffer[--first_char_pos] = '0';
  } else {
    while (exponent > 0) {
      buffer[--first_char_pos] = '0' + (exponent % 10);
      exponent /= 10;
    }
  }
  while(kMaxExponentLength - first_char_pos < std::min(min_exponent_width_, kMaxExponentLength)) {
    buffer[--first_char_pos] = '0';
  }
  result_builder->AddSubstring(&buffer[first_char_pos],
                               kMaxExponentLength - first_char_pos);
}
void DoubleToStringConverter::CreateDecimalRepresentation(
    const char* decimal_digits,
    int length,
    int decimal_point,
    int digits_after_point,
    StringBuilder* result_builder) const {
  if (decimal_point <= 0) {
    result_builder->AddCharacter('0');
    if (digits_after_point > 0) {
      result_builder->AddCharacter('.');
      result_builder->AddPadding('0', -decimal_point);
      DOUBLE_CONVERSION_ASSERT(length <= digits_after_point - (-decimal_point));
      result_builder->AddSubstring(decimal_digits, length);
      int remaining_digits = digits_after_point - (-decimal_point) - length;
      result_builder->AddPadding('0', remaining_digits);
    }
  } else if (decimal_point >= length) {
    result_builder->AddSubstring(decimal_digits, length);
    result_builder->AddPadding('0', decimal_point - length);
    if (digits_after_point > 0) {
      result_builder->AddCharacter('.');
      result_builder->AddPadding('0', digits_after_point);
    }
  } else {
    DOUBLE_CONVERSION_ASSERT(digits_after_point > 0);
    result_builder->AddSubstring(decimal_digits, decimal_point);
    result_builder->AddCharacter('.');
    DOUBLE_CONVERSION_ASSERT(length - decimal_point <= digits_after_point);
    result_builder->AddSubstring(&decimal_digits[decimal_point],
                                 length - decimal_point);
    int remaining_digits = digits_after_point - (length - decimal_point);
    result_builder->AddPadding('0', remaining_digits);
  }
  if (digits_after_point == 0) {
    if ((flags_ & EMIT_TRAILING_DECIMAL_POINT) != 0) {
      result_builder->AddCharacter('.');
    }
    if ((flags_ & EMIT_TRAILING_ZERO_AFTER_POINT) != 0) {
      result_builder->AddCharacter('0');
    }
  }
}
bool DoubleToStringConverter::ToShortestIeeeNumber(
    double value,
    StringBuilder* result_builder,
    DoubleToStringConverter::DtoaMode mode) const {
  DOUBLE_CONVERSION_ASSERT(mode == SHORTEST || mode == SHORTEST_SINGLE);
  if (Double(value).IsSpecial()) {
    return HandleSpecialValues(value, result_builder);
  }
  int decimal_point;
  bool sign;
  const int kDecimalRepCapacity = kBase10MaximalLength + 1;
  char decimal_rep[kDecimalRepCapacity];
  int decimal_rep_length;
  DoubleToAscii(value, mode, 0, decimal_rep, kDecimalRepCapacity,
                &sign, &decimal_rep_length, &decimal_point);
  bool unique_zero = (flags_ & UNIQUE_ZERO) != 0;
  if (sign && (value != 0.0 || !unique_zero)) {
    result_builder->AddCharacter('-');
  }
  int exponent = decimal_point - 1;
  if ((decimal_in_shortest_low_ <= exponent) &&
      (exponent < decimal_in_shortest_high_)) {
    CreateDecimalRepresentation(decimal_rep, decimal_rep_length,
                                decimal_point,
                                (std::max)(0, decimal_rep_length - decimal_point),
                                result_builder);
  } else {
    CreateExponentialRepresentation(decimal_rep, decimal_rep_length, exponent,
                                    result_builder);
  }
  return true;
}
bool DoubleToStringConverter::ToFixed(double value,
                                      int requested_digits,
                                      StringBuilder* result_builder) const {
  DOUBLE_CONVERSION_ASSERT(kMaxFixedDigitsBeforePoint == 60);
  const double kFirstNonFixed = 1e60;
  if (Double(value).IsSpecial()) {
    return HandleSpecialValues(value, result_builder);
  }
  if (requested_digits > kMaxFixedDigitsAfterPoint) return false;
  if (value >= kFirstNonFixed || value <= -kFirstNonFixed) return false;
  int decimal_point;
  bool sign;
  const int kDecimalRepCapacity =
      kMaxFixedDigitsBeforePoint + kMaxFixedDigitsAfterPoint + 1;
  char decimal_rep[kDecimalRepCapacity];
  int decimal_rep_length;
  DoubleToAscii(value, FIXED, requested_digits,
                decimal_rep, kDecimalRepCapacity,
                &sign, &decimal_rep_length, &decimal_point);
  bool unique_zero = ((flags_ & UNIQUE_ZERO) != 0);
  if (sign && (value != 0.0 || !unique_zero)) {
    result_builder->AddCharacter('-');
  }
  CreateDecimalRepresentation(decimal_rep, decimal_rep_length, decimal_point,
                              requested_digits, result_builder);
  return true;
}
bool DoubleToStringConverter::ToExponential(
    double value,
    int requested_digits,
    StringBuilder* result_builder) const {
  if (Double(value).IsSpecial()) {
    return HandleSpecialValues(value, result_builder);
  }
  if (requested_digits < -1) return false;
  if (requested_digits > kMaxExponentialDigits) return false;
  int decimal_point;
  bool sign;
  const int kDecimalRepCapacity = kMaxExponentialDigits + 2;
  DOUBLE_CONVERSION_ASSERT(kDecimalRepCapacity > kBase10MaximalLength);
  char decimal_rep[kDecimalRepCapacity];
#ifndef NDEBUG
  memset(decimal_rep, 0, sizeof(decimal_rep));
#endif
  int decimal_rep_length;
  if (requested_digits == -1) {
    DoubleToAscii(value, SHORTEST, 0,
                  decimal_rep, kDecimalRepCapacity,
                  &sign, &decimal_rep_length, &decimal_point);
  } else {
    DoubleToAscii(value, PRECISION, requested_digits + 1,
                  decimal_rep, kDecimalRepCapacity,
                  &sign, &decimal_rep_length, &decimal_point);
    DOUBLE_CONVERSION_ASSERT(decimal_rep_length <= requested_digits + 1);
    for (int i = decimal_rep_length; i < requested_digits + 1; ++i) {
      decimal_rep[i] = '0';
    }
    decimal_rep_length = requested_digits + 1;
  }
  bool unique_zero = ((flags_ & UNIQUE_ZERO) != 0);
  if (sign && (value != 0.0 || !unique_zero)) {
    result_builder->AddCharacter('-');
  }
  int exponent = decimal_point - 1;
  CreateExponentialRepresentation(decimal_rep,
                                  decimal_rep_length,
                                  exponent,
                                  result_builder);
  return true;
}
bool DoubleToStringConverter::ToPrecision(double value,
                                          int precision,
                                          StringBuilder* result_builder) const {
  if (Double(value).IsSpecial()) {
    return HandleSpecialValues(value, result_builder);
  }
  if (precision < kMinPrecisionDigits || precision > kMaxPrecisionDigits) {
    return false;
  }
  int decimal_point;
  bool sign;
  const int kDecimalRepCapacity = kMaxPrecisionDigits + 1;
  char decimal_rep[kDecimalRepCapacity];
  int decimal_rep_length;
  DoubleToAscii(value, PRECISION, precision,
                decimal_rep, kDecimalRepCapacity,
                &sign, &decimal_rep_length, &decimal_point);
  DOUBLE_CONVERSION_ASSERT(decimal_rep_length <= precision);
  bool unique_zero = ((flags_ & UNIQUE_ZERO) != 0);
  if (sign && (value != 0.0 || !unique_zero)) {
    result_builder->AddCharacter('-');
  }
  int exponent = decimal_point - 1;
  int extra_zero = ((flags_ & EMIT_TRAILING_ZERO_AFTER_POINT) != 0) ? 1 : 0;
  bool as_exponential =
      (-decimal_point + 1 > max_leading_padding_zeroes_in_precision_mode_) ||
      (decimal_point - precision + extra_zero >
       max_trailing_padding_zeroes_in_precision_mode_);
  if ((flags_ & NO_TRAILING_ZERO) != 0) {
    int stop = as_exponential ? 1 : std::max(1, decimal_point);
    while (decimal_rep_length > stop && decimal_rep[decimal_rep_length - 1] == '0') {
      --decimal_rep_length;
    }
    precision = std::min(precision, decimal_rep_length);
  }
  if (as_exponential) {
    for (int i = decimal_rep_length; i < precision; ++i) {
      decimal_rep[i] = '0';
    }
    CreateExponentialRepresentation(decimal_rep,
                                    precision,
                                    exponent,
                                    result_builder);
  } else {
    CreateDecimalRepresentation(decimal_rep, decimal_rep_length, decimal_point,
                                (std::max)(0, precision - decimal_point),
                                result_builder);
  }
  return true;
}
static BignumDtoaMode DtoaToBignumDtoaMode(
    DoubleToStringConverter::DtoaMode dtoa_mode) {
  switch (dtoa_mode) {
    case DoubleToStringConverter::SHORTEST:  return BIGNUM_DTOA_SHORTEST;
    case DoubleToStringConverter::SHORTEST_SINGLE:
        return BIGNUM_DTOA_SHORTEST_SINGLE;
    case DoubleToStringConverter::FIXED:     return BIGNUM_DTOA_FIXED;
    case DoubleToStringConverter::PRECISION: return BIGNUM_DTOA_PRECISION;
    default:
      DOUBLE_CONVERSION_UNREACHABLE();
  }
}
void DoubleToStringConverter::DoubleToAscii(double v,
                                            DtoaMode mode,
                                            int requested_digits,
                                            char* buffer,
                                            int buffer_length,
                                            bool* sign,
                                            int* length,
                                            int* point) {
  Vector<char> vector(buffer, buffer_length);
  DOUBLE_CONVERSION_ASSERT(!Double(v).IsSpecial());
  DOUBLE_CONVERSION_ASSERT(mode == SHORTEST || mode == SHORTEST_SINGLE || requested_digits >= 0);
  if (Double(v).Sign() < 0) {
    *sign = true;
    v = -v;
  } else {
    *sign = false;
  }
  if (mode == PRECISION && requested_digits == 0) {
    vector[0] = '\0';
    *length = 0;
    return;
  }
  if (v == 0) {
    vector[0] = '0';
    vector[1] = '\0';
    *length = 1;
    *point = 1;
    return;
  }
  bool fast_worked;
  switch (mode) {
    case SHORTEST:
      fast_worked = FastDtoa(v, FAST_DTOA_SHORTEST, 0, vector, length, point);
      break;
    case SHORTEST_SINGLE:
      fast_worked = FastDtoa(v, FAST_DTOA_SHORTEST_SINGLE, 0,
                             vector, length, point);
      break;
    case FIXED:
      fast_worked = FastFixedDtoa(v, requested_digits, vector, length, point);
      break;
    case PRECISION:
      fast_worked = FastDtoa(v, FAST_DTOA_PRECISION, requested_digits,
                             vector, length, point);
      break;
    default:
      fast_worked = false;
      DOUBLE_CONVERSION_UNREACHABLE();
  }
  if (fast_worked) return;
  BignumDtoaMode bignum_mode = DtoaToBignumDtoaMode(mode);
  BignumDtoa(v, bignum_mode, requested_digits, vector, length, point);
  vector[*length] = '\0';
}
}
/// double-to-string.cc END
/// bignum.cc START
#include <algorithm>
#include <cstring>
namespace double_conversion {
Bignum::Chunk& Bignum::RawBigit(const int index) {
  DOUBLE_CONVERSION_ASSERT(static_cast<unsigned>(index) < kBigitCapacity);
  return bigits_buffer_[index];
}
const Bignum::Chunk& Bignum::RawBigit(const int index) const {
  DOUBLE_CONVERSION_ASSERT(static_cast<unsigned>(index) < kBigitCapacity);
  return bigits_buffer_[index];
}
template<typename S>
static int BitSize(const S value) {
  (void) value;
  return 8 * sizeof(value);
}
void Bignum::AssignUInt16(const uint16_t value) {
  DOUBLE_CONVERSION_ASSERT(kBigitSize >= BitSize(value));
  Zero();
  if (value > 0) {
    RawBigit(0) = value;
    used_bigits_ = 1;
  }
}
void Bignum::AssignUInt64(uint64_t value) {
  Zero();
  for(int i = 0; value > 0; ++i) {
    RawBigit(i) = value & kBigitMask;
    value >>= kBigitSize;
    ++used_bigits_;
  }
}
void Bignum::AssignBignum(const Bignum& other) {
  exponent_ = other.exponent_;
  for (int i = 0; i < other.used_bigits_; ++i) {
    RawBigit(i) = other.RawBigit(i);
  }
  used_bigits_ = other.used_bigits_;
}
static uint64_t ReadUInt64(const Vector<const char> buffer,
                           const int from,
                           const int digits_to_read) {
  uint64_t result = 0;
  for (int i = from; i < from + digits_to_read; ++i) {
    const int digit = buffer[i] - '0';
    DOUBLE_CONVERSION_ASSERT(0 <= digit && digit <= 9);
    result = result * 10 + digit;
  }
  return result;
}
void Bignum::AssignDecimalString(const Vector<const char> value) {
  static const int kMaxUint64DecimalDigits = 19;
  Zero();
  int length = value.length();
  unsigned pos = 0;
  while (length >= kMaxUint64DecimalDigits) {
    const uint64_t digits = ReadUInt64(value, pos, kMaxUint64DecimalDigits);
    pos += kMaxUint64DecimalDigits;
    length -= kMaxUint64DecimalDigits;
    MultiplyByPowerOfTen(kMaxUint64DecimalDigits);
    AddUInt64(digits);
  }
  const uint64_t digits = ReadUInt64(value, pos, length);
  MultiplyByPowerOfTen(length);
  AddUInt64(digits);
  Clamp();
}
static uint64_t HexCharValue(const int c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  }
  if ('a' <= c && c <= 'f') {
    return 10 + c - 'a';
  }
  DOUBLE_CONVERSION_ASSERT('A' <= c && c <= 'F');
  return 10 + c - 'A';
}
void Bignum::AssignHexString(Vector<const char> value) {
  Zero();
  EnsureCapacity(((value.length() * 4) + kBigitSize - 1) / kBigitSize);
  DOUBLE_CONVERSION_ASSERT(sizeof(uint64_t) * 8 >= kBigitSize + 4);
  uint64_t tmp = 0;
  for (int cnt = 0; !value.is_empty(); value.pop_back()) {
    tmp |= (HexCharValue(value.last()) << cnt);
    if ((cnt += 4) >= kBigitSize) {
      RawBigit(used_bigits_++) = (tmp & kBigitMask);
      cnt -= kBigitSize;
      tmp >>= kBigitSize;
    }
  }
  if (tmp > 0) {
    DOUBLE_CONVERSION_ASSERT(tmp <= kBigitMask);
    RawBigit(used_bigits_++) = static_cast<Bignum::Chunk>(tmp & kBigitMask);
  }
  Clamp();
}
void Bignum::AddUInt64(const uint64_t operand) {
  if (operand == 0) {
    return;
  }
  Bignum other;
  other.AssignUInt64(operand);
  AddBignum(other);
}
void Bignum::AddBignum(const Bignum& other) {
  DOUBLE_CONVERSION_ASSERT(IsClamped());
  DOUBLE_CONVERSION_ASSERT(other.IsClamped());
  Align(other);
  EnsureCapacity(1 + (std::max)(BigitLength(), other.BigitLength()) - exponent_);
  Chunk carry = 0;
  int bigit_pos = other.exponent_ - exponent_;
  DOUBLE_CONVERSION_ASSERT(bigit_pos >= 0);
  for (int i = used_bigits_; i < bigit_pos; ++i) {
    RawBigit(i) = 0;
  }
  for (int i = 0; i < other.used_bigits_; ++i) {
    const Chunk my = (bigit_pos < used_bigits_) ? RawBigit(bigit_pos) : 0;
    const Chunk sum = my + other.RawBigit(i) + carry;
    RawBigit(bigit_pos) = sum & kBigitMask;
    carry = sum >> kBigitSize;
    ++bigit_pos;
  }
  while (carry != 0) {
    const Chunk my = (bigit_pos < used_bigits_) ? RawBigit(bigit_pos) : 0;
    const Chunk sum = my + carry;
    RawBigit(bigit_pos) = sum & kBigitMask;
    carry = sum >> kBigitSize;
    ++bigit_pos;
  }
  used_bigits_ = static_cast<int16_t>(std::max(bigit_pos, static_cast<int>(used_bigits_)));
  DOUBLE_CONVERSION_ASSERT(IsClamped());
}
void Bignum::SubtractBignum(const Bignum& other) {
  DOUBLE_CONVERSION_ASSERT(IsClamped());
  DOUBLE_CONVERSION_ASSERT(other.IsClamped());
  DOUBLE_CONVERSION_ASSERT(LessEqual(other, *this));
  Align(other);
  const int offset = other.exponent_ - exponent_;
  Chunk borrow = 0;
  int i;
  for (i = 0; i < other.used_bigits_; ++i) {
    DOUBLE_CONVERSION_ASSERT((borrow == 0) || (borrow == 1));
    const Chunk difference = RawBigit(i + offset) - other.RawBigit(i) - borrow;
    RawBigit(i + offset) = difference & kBigitMask;
    borrow = difference >> (kChunkSize - 1);
  }
  while (borrow != 0) {
    const Chunk difference = RawBigit(i + offset) - borrow;
    RawBigit(i + offset) = difference & kBigitMask;
    borrow = difference >> (kChunkSize - 1);
    ++i;
  }
  Clamp();
}
void Bignum::ShiftLeft(const int shift_amount) {
  if (used_bigits_ == 0) {
    return;
  }
  exponent_ += static_cast<int16_t>(shift_amount / kBigitSize);
  const int local_shift = shift_amount % kBigitSize;
  EnsureCapacity(used_bigits_ + 1);
  BigitsShiftLeft(local_shift);
}
void Bignum::MultiplyByUInt32(const uint32_t factor) {
  if (factor == 1) {
    return;
  }
  if (factor == 0) {
    Zero();
    return;
  }
  if (used_bigits_ == 0) {
    return;
  }
  DOUBLE_CONVERSION_ASSERT(kDoubleChunkSize >= kBigitSize + 32 + 1);
  DoubleChunk carry = 0;
  for (int i = 0; i < used_bigits_; ++i) {
    const DoubleChunk product = static_cast<DoubleChunk>(factor) * RawBigit(i) + carry;
    RawBigit(i) = static_cast<Chunk>(product & kBigitMask);
    carry = (product >> kBigitSize);
  }
  while (carry != 0) {
    EnsureCapacity(used_bigits_ + 1);
    RawBigit(used_bigits_) = carry & kBigitMask;
    used_bigits_++;
    carry >>= kBigitSize;
  }
}
void Bignum::MultiplyByUInt64(const uint64_t factor) {
  if (factor == 1) {
    return;
  }
  if (factor == 0) {
    Zero();
    return;
  }
  if (used_bigits_ == 0) {
    return;
  }
  DOUBLE_CONVERSION_ASSERT(kBigitSize < 32);
  uint64_t carry = 0;
  const uint64_t low = factor & 0xFFFFFFFF;
  const uint64_t high = factor >> 32;
  for (int i = 0; i < used_bigits_; ++i) {
    const uint64_t product_low = low * RawBigit(i);
    const uint64_t product_high = high * RawBigit(i);
    const uint64_t tmp = (carry & kBigitMask) + product_low;
    RawBigit(i) = tmp & kBigitMask;
    carry = (carry >> kBigitSize) + (tmp >> kBigitSize) +
        (product_high << (32 - kBigitSize));
  }
  while (carry != 0) {
    EnsureCapacity(used_bigits_ + 1);
    RawBigit(used_bigits_) = carry & kBigitMask;
    used_bigits_++;
    carry >>= kBigitSize;
  }
}
void Bignum::MultiplyByPowerOfTen(const int exponent) {
  static const uint64_t kFive27 = DOUBLE_CONVERSION_UINT64_2PART_C(0x6765c793, fa10079d);
  static const uint16_t kFive1 = 5;
  static const uint16_t kFive2 = kFive1 * 5;
  static const uint16_t kFive3 = kFive2 * 5;
  static const uint16_t kFive4 = kFive3 * 5;
  static const uint16_t kFive5 = kFive4 * 5;
  static const uint16_t kFive6 = kFive5 * 5;
  static const uint32_t kFive7 = kFive6 * 5;
  static const uint32_t kFive8 = kFive7 * 5;
  static const uint32_t kFive9 = kFive8 * 5;
  static const uint32_t kFive10 = kFive9 * 5;
  static const uint32_t kFive11 = kFive10 * 5;
  static const uint32_t kFive12 = kFive11 * 5;
  static const uint32_t kFive13 = kFive12 * 5;
  static const uint32_t kFive1_to_12[] =
      { kFive1, kFive2, kFive3, kFive4, kFive5, kFive6,
        kFive7, kFive8, kFive9, kFive10, kFive11, kFive12 };
  DOUBLE_CONVERSION_ASSERT(exponent >= 0);
  if (exponent == 0) {
    return;
  }
  if (used_bigits_ == 0) {
    return;
  }
  int remaining_exponent = exponent;
  while (remaining_exponent >= 27) {
    MultiplyByUInt64(kFive27);
    remaining_exponent -= 27;
  }
  while (remaining_exponent >= 13) {
    MultiplyByUInt32(kFive13);
    remaining_exponent -= 13;
  }
  if (remaining_exponent > 0) {
    MultiplyByUInt32(kFive1_to_12[remaining_exponent - 1]);
  }
  ShiftLeft(exponent);
}
void Bignum::Square() {
  DOUBLE_CONVERSION_ASSERT(IsClamped());
  const int product_length = 2 * used_bigits_;
  EnsureCapacity(product_length);
  if ((1 << (2 * (kChunkSize - kBigitSize))) <= used_bigits_) {
    DOUBLE_CONVERSION_UNIMPLEMENTED();
  }
  DoubleChunk accumulator = 0;
  const int copy_offset = used_bigits_;
  for (int i = 0; i < used_bigits_; ++i) {
    RawBigit(copy_offset + i) = RawBigit(i);
  }
  for (int i = 0; i < used_bigits_; ++i) {
    int bigit_index1 = i;
    int bigit_index2 = 0;
    while (bigit_index1 >= 0) {
      const Chunk chunk1 = RawBigit(copy_offset + bigit_index1);
      const Chunk chunk2 = RawBigit(copy_offset + bigit_index2);
      accumulator += static_cast<DoubleChunk>(chunk1) * chunk2;
      bigit_index1--;
      bigit_index2++;
    }
    RawBigit(i) = static_cast<Chunk>(accumulator) & kBigitMask;
    accumulator >>= kBigitSize;
  }
  for (int i = used_bigits_; i < product_length; ++i) {
    int bigit_index1 = used_bigits_ - 1;
    int bigit_index2 = i - bigit_index1;
    while (bigit_index2 < used_bigits_) {
      const Chunk chunk1 = RawBigit(copy_offset + bigit_index1);
      const Chunk chunk2 = RawBigit(copy_offset + bigit_index2);
      accumulator += static_cast<DoubleChunk>(chunk1) * chunk2;
      bigit_index1--;
      bigit_index2++;
    }
    RawBigit(i) = static_cast<Chunk>(accumulator) & kBigitMask;
    accumulator >>= kBigitSize;
  }
  DOUBLE_CONVERSION_ASSERT(accumulator == 0);
  used_bigits_ = static_cast<int16_t>(product_length);
  exponent_ *= 2;
  Clamp();
}
void Bignum::AssignPowerUInt16(uint16_t base, const int power_exponent) {
  DOUBLE_CONVERSION_ASSERT(base != 0);
  DOUBLE_CONVERSION_ASSERT(power_exponent >= 0);
  if (power_exponent == 0) {
    AssignUInt16(1);
    return;
  }
  Zero();
  int shifts = 0;
  while ((base & 1) == 0) {
    base >>= 1;
    shifts++;
  }
  int bit_size = 0;
  int tmp_base = base;
  while (tmp_base != 0) {
    tmp_base >>= 1;
    bit_size++;
  }
  const int final_size = bit_size * power_exponent;
  EnsureCapacity(final_size / kBigitSize + 2);
  int mask = 1;
  while (power_exponent >= mask) mask <<= 1;
  mask >>= 2;
  uint64_t this_value = base;
  bool delayed_multiplication = false;
  const uint64_t max_32bits = 0xFFFFFFFF;
  while (mask != 0 && this_value <= max_32bits) {
    this_value = this_value * this_value;
    if ((power_exponent & mask) != 0) {
      DOUBLE_CONVERSION_ASSERT(bit_size > 0);
      const uint64_t base_bits_mask =
        ~((static_cast<uint64_t>(1) << (64 - bit_size)) - 1);
      const bool high_bits_zero = (this_value & base_bits_mask) == 0;
      if (high_bits_zero) {
        this_value *= base;
      } else {
        delayed_multiplication = true;
      }
    }
    mask >>= 1;
  }
  AssignUInt64(this_value);
  if (delayed_multiplication) {
    MultiplyByUInt32(base);
  }
  while (mask != 0) {
    Square();
    if ((power_exponent & mask) != 0) {
      MultiplyByUInt32(base);
    }
    mask >>= 1;
  }
  ShiftLeft(shifts * power_exponent);
}
uint16_t Bignum::DivideModuloIntBignum(const Bignum& other) {
  DOUBLE_CONVERSION_ASSERT(IsClamped());
  DOUBLE_CONVERSION_ASSERT(other.IsClamped());
  DOUBLE_CONVERSION_ASSERT(other.used_bigits_ > 0);
  if (BigitLength() < other.BigitLength()) {
    return 0;
  }
  Align(other);
  uint16_t result = 0;
  while (BigitLength() > other.BigitLength()) {
    DOUBLE_CONVERSION_ASSERT(other.RawBigit(other.used_bigits_ - 1) >= ((1 << kBigitSize) / 16));
    DOUBLE_CONVERSION_ASSERT(RawBigit(used_bigits_ - 1) < 0x10000);
    result += static_cast<uint16_t>(RawBigit(used_bigits_ - 1));
    SubtractTimes(other, RawBigit(used_bigits_ - 1));
  }
  DOUBLE_CONVERSION_ASSERT(BigitLength() == other.BigitLength());
  const Chunk this_bigit = RawBigit(used_bigits_ - 1);
  const Chunk other_bigit = other.RawBigit(other.used_bigits_ - 1);
  if (other.used_bigits_ == 1) {
    int quotient = this_bigit / other_bigit;
    RawBigit(used_bigits_ - 1) = this_bigit - other_bigit * quotient;
    DOUBLE_CONVERSION_ASSERT(quotient < 0x10000);
    result += static_cast<uint16_t>(quotient);
    Clamp();
    return result;
  }
  const int division_estimate = this_bigit / (other_bigit + 1);
  DOUBLE_CONVERSION_ASSERT(division_estimate < 0x10000);
  result += static_cast<uint16_t>(division_estimate);
  SubtractTimes(other, division_estimate);
  if (other_bigit * (division_estimate + 1) > this_bigit) {
    return result;
  }
  while (LessEqual(other, *this)) {
    SubtractBignum(other);
    result++;
  }
  return result;
}
template<typename S>
static int SizeInHexChars(S number) {
  DOUBLE_CONVERSION_ASSERT(number > 0);
  int result = 0;
  while (number != 0) {
    number >>= 4;
    result++;
  }
  return result;
}
static char HexCharOfValue(const int value) {
  DOUBLE_CONVERSION_ASSERT(0 <= value && value <= 16);
  if (value < 10) {
    return static_cast<char>(value + '0');
  }
  return static_cast<char>(value - 10 + 'A');
}
bool Bignum::ToHexString(char* buffer, const int buffer_size) const {
  DOUBLE_CONVERSION_ASSERT(IsClamped());
  DOUBLE_CONVERSION_ASSERT(kBigitSize % 4 == 0);
  static const int kHexCharsPerBigit = kBigitSize / 4;
  if (used_bigits_ == 0) {
    if (buffer_size < 2) {
      return false;
    }
    buffer[0] = '0';
    buffer[1] = '\0';
    return true;
  }
  const int needed_chars = (BigitLength() - 1) * kHexCharsPerBigit +
    SizeInHexChars(RawBigit(used_bigits_ - 1)) + 1;
  if (needed_chars > buffer_size) {
    return false;
  }
  int string_index = needed_chars - 1;
  buffer[string_index--] = '\0';
  for (int i = 0; i < exponent_; ++i) {
    for (int j = 0; j < kHexCharsPerBigit; ++j) {
      buffer[string_index--] = '0';
    }
  }
  for (int i = 0; i < used_bigits_ - 1; ++i) {
    Chunk current_bigit = RawBigit(i);
    for (int j = 0; j < kHexCharsPerBigit; ++j) {
      buffer[string_index--] = HexCharOfValue(current_bigit & 0xF);
      current_bigit >>= 4;
    }
  }
  Chunk most_significant_bigit = RawBigit(used_bigits_ - 1);
  while (most_significant_bigit != 0) {
    buffer[string_index--] = HexCharOfValue(most_significant_bigit & 0xF);
    most_significant_bigit >>= 4;
  }
  return true;
}
Bignum::Chunk Bignum::BigitOrZero(const int index) const {
  if (index >= BigitLength()) {
    return 0;
  }
  if (index < exponent_) {
    return 0;
  }
  return RawBigit(index - exponent_);
}
int Bignum::Compare(const Bignum& a, const Bignum& b) {
  DOUBLE_CONVERSION_ASSERT(a.IsClamped());
  DOUBLE_CONVERSION_ASSERT(b.IsClamped());
  const int bigit_length_a = a.BigitLength();
  const int bigit_length_b = b.BigitLength();
  if (bigit_length_a < bigit_length_b) {
    return -1;
  }
  if (bigit_length_a > bigit_length_b) {
    return +1;
  }
  for (int i = bigit_length_a - 1; i >= (std::min)(a.exponent_, b.exponent_); --i) {
    const Chunk bigit_a = a.BigitOrZero(i);
    const Chunk bigit_b = b.BigitOrZero(i);
    if (bigit_a < bigit_b) {
      return -1;
    }
    if (bigit_a > bigit_b) {
      return +1;
    }
  }
  return 0;
}
int Bignum::PlusCompare(const Bignum& a, const Bignum& b, const Bignum& c) {
  DOUBLE_CONVERSION_ASSERT(a.IsClamped());
  DOUBLE_CONVERSION_ASSERT(b.IsClamped());
  DOUBLE_CONVERSION_ASSERT(c.IsClamped());
  if (a.BigitLength() < b.BigitLength()) {
    return PlusCompare(b, a, c);
  }
  if (a.BigitLength() + 1 < c.BigitLength()) {
    return -1;
  }
  if (a.BigitLength() > c.BigitLength()) {
    return +1;
  }
  if (a.exponent_ >= b.BigitLength() && a.BigitLength() < c.BigitLength()) {
    return -1;
  }
  Chunk borrow = 0;
  const int min_exponent = (std::min)((std::min)(a.exponent_, b.exponent_), c.exponent_);
  for (int i = c.BigitLength() - 1; i >= min_exponent; --i) {
    const Chunk chunk_a = a.BigitOrZero(i);
    const Chunk chunk_b = b.BigitOrZero(i);
    const Chunk chunk_c = c.BigitOrZero(i);
    const Chunk sum = chunk_a + chunk_b;
    if (sum > chunk_c + borrow) {
      return +1;
    } else {
      borrow = chunk_c + borrow - sum;
      if (borrow > 1) {
        return -1;
      }
      borrow <<= kBigitSize;
    }
  }
  if (borrow == 0) {
    return 0;
  }
  return -1;
}
void Bignum::Clamp() {
  while (used_bigits_ > 0 && RawBigit(used_bigits_ - 1) == 0) {
    used_bigits_--;
  }
  if (used_bigits_ == 0) {
    exponent_ = 0;
  }
}
void Bignum::Align(const Bignum& other) {
  if (exponent_ > other.exponent_) {
    const int zero_bigits = exponent_ - other.exponent_;
    EnsureCapacity(used_bigits_ + zero_bigits);
    for (int i = used_bigits_ - 1; i >= 0; --i) {
      RawBigit(i + zero_bigits) = RawBigit(i);
    }
    for (int i = 0; i < zero_bigits; ++i) {
      RawBigit(i) = 0;
    }
    used_bigits_ += static_cast<int16_t>(zero_bigits);
    exponent_ -= static_cast<int16_t>(zero_bigits);
    DOUBLE_CONVERSION_ASSERT(used_bigits_ >= 0);
    DOUBLE_CONVERSION_ASSERT(exponent_ >= 0);
  }
}
void Bignum::BigitsShiftLeft(const int shift_amount) {
  DOUBLE_CONVERSION_ASSERT(shift_amount < kBigitSize);
  DOUBLE_CONVERSION_ASSERT(shift_amount >= 0);
  Chunk carry = 0;
  for (int i = 0; i < used_bigits_; ++i) {
    const Chunk new_carry = RawBigit(i) >> (kBigitSize - shift_amount);
    RawBigit(i) = ((RawBigit(i) << shift_amount) + carry) & kBigitMask;
    carry = new_carry;
  }
  if (carry != 0) {
    RawBigit(used_bigits_) = carry;
    used_bigits_++;
  }
}
void Bignum::SubtractTimes(const Bignum& other, const int factor) {
  DOUBLE_CONVERSION_ASSERT(exponent_ <= other.exponent_);
  if (factor < 3) {
    for (int i = 0; i < factor; ++i) {
      SubtractBignum(other);
    }
    return;
  }
  Chunk borrow = 0;
  const int exponent_diff = other.exponent_ - exponent_;
  for (int i = 0; i < other.used_bigits_; ++i) {
    const DoubleChunk product = static_cast<DoubleChunk>(factor) * other.RawBigit(i);
    const DoubleChunk remove = borrow + product;
    const Chunk difference = RawBigit(i + exponent_diff) - (remove & kBigitMask);
    RawBigit(i + exponent_diff) = difference & kBigitMask;
    borrow = static_cast<Chunk>((difference >> (kChunkSize - 1)) +
                                (remove >> kBigitSize));
  }
  for (int i = other.used_bigits_ + exponent_diff; i < used_bigits_; ++i) {
    if (borrow == 0) {
      return;
    }
    const Chunk difference = RawBigit(i) - borrow;
    RawBigit(i) = difference & kBigitMask;
    borrow = difference >> (kChunkSize - 1);
  }
  Clamp();
}
}
/// bignum.cc END
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
/// cached-powers.cc START
#include <climits>
#include <cmath>
#include <cstdarg>
namespace double_conversion {
namespace PowersOfTenCache {
struct CachedPower {
  uint64_t significand;
  int16_t binary_exponent;
  int16_t decimal_exponent;
};
static const CachedPower kCachedPowers[] = {
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xfa8fd5a0, 081c0288), -1220, -348},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xbaaee17f, a23ebf76), -1193, -340},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8b16fb20, 3055ac76), -1166, -332},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xcf42894a, 5dce35ea), -1140, -324},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9a6bb0aa, 55653b2d), -1113, -316},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xe61acf03, 3d1a45df), -1087, -308},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xab70fe17, c79ac6ca), -1060, -300},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xff77b1fc, bebcdc4f), -1034, -292},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xbe5691ef, 416bd60c), -1007, -284},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8dd01fad, 907ffc3c), -980, -276},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xd3515c28, 31559a83), -954, -268},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9d71ac8f, ada6c9b5), -927, -260},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xea9c2277, 23ee8bcb), -901, -252},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xaecc4991, 4078536d), -874, -244},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x823c1279, 5db6ce57), -847, -236},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xc2109436, 4dfb5637), -821, -228},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9096ea6f, 3848984f), -794, -220},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xd77485cb, 25823ac7), -768, -212},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xa086cfcd, 97bf97f4), -741, -204},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xef340a98, 172aace5), -715, -196},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xb23867fb, 2a35b28e), -688, -188},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x84c8d4df, d2c63f3b), -661, -180},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xc5dd4427, 1ad3cdba), -635, -172},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x936b9fce, bb25c996), -608, -164},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xdbac6c24, 7d62a584), -582, -156},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xa3ab6658, 0d5fdaf6), -555, -148},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xf3e2f893, dec3f126), -529, -140},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xb5b5ada8, aaff80b8), -502, -132},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x87625f05, 6c7c4a8b), -475, -124},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xc9bcff60, 34c13053), -449, -116},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x964e858c, 91ba2655), -422, -108},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xdff97724, 70297ebd), -396, -100},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xa6dfbd9f, b8e5b88f), -369, -92},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xf8a95fcf, 88747d94), -343, -84},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xb9447093, 8fa89bcf), -316, -76},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8a08f0f8, bf0f156b), -289, -68},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xcdb02555, 653131b6), -263, -60},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x993fe2c6, d07b7fac), -236, -52},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xe45c10c4, 2a2b3b06), -210, -44},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xaa242499, 697392d3), -183, -36},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xfd87b5f2, 8300ca0e), -157, -28},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xbce50864, 92111aeb), -130, -20},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8cbccc09, 6f5088cc), -103, -12},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xd1b71758, e219652c), -77, -4},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9c400000, 00000000), -50, 4},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xe8d4a510, 00000000), -24, 12},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xad78ebc5, ac620000), 3, 20},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x813f3978, f8940984), 30, 28},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xc097ce7b, c90715b3), 56, 36},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8f7e32ce, 7bea5c70), 83, 44},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xd5d238a4, abe98068), 109, 52},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9f4f2726, 179a2245), 136, 60},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xed63a231, d4c4fb27), 162, 68},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xb0de6538, 8cc8ada8), 189, 76},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x83c7088e, 1aab65db), 216, 84},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xc45d1df9, 42711d9a), 242, 92},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x924d692c, a61be758), 269, 100},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xda01ee64, 1a708dea), 295, 108},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xa26da399, 9aef774a), 322, 116},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xf209787b, b47d6b85), 348, 124},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xb454e4a1, 79dd1877), 375, 132},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x865b8692, 5b9bc5c2), 402, 140},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xc83553c5, c8965d3d), 428, 148},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x952ab45c, fa97a0b3), 455, 156},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xde469fbd, 99a05fe3), 481, 164},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xa59bc234, db398c25), 508, 172},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xf6c69a72, a3989f5c), 534, 180},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xb7dcbf53, 54e9bece), 561, 188},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x88fcf317, f22241e2), 588, 196},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xcc20ce9b, d35c78a5), 614, 204},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x98165af3, 7b2153df), 641, 212},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xe2a0b5dc, 971f303a), 667, 220},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xa8d9d153, 5ce3b396), 694, 228},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xfb9b7cd9, a4a7443c), 720, 236},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xbb764c4c, a7a44410), 747, 244},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8bab8eef, b6409c1a), 774, 252},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xd01fef10, a657842c), 800, 260},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9b10a4e5, e9913129), 827, 268},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xe7109bfb, a19c0c9d), 853, 276},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xac2820d9, 623bf429), 880, 284},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x80444b5e, 7aa7cf85), 907, 292},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xbf21e440, 03acdd2d), 933, 300},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x8e679c2f, 5e44ff8f), 960, 308},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xd433179d, 9c8cb841), 986, 316},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0x9e19db92, b4e31ba9), 1013, 324},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xeb96bf6e, badf77d9), 1039, 332},
  {DOUBLE_CONVERSION_UINT64_2PART_C(0xaf87023b, 9bf0ee6b), 1066, 340},
};
static const int kCachedPowersOffset = 348;
static const double kD_1_LOG2_10 = 0.30102999566398114;
void GetCachedPowerForBinaryExponentRange(
    int min_exponent,
    int max_exponent,
    DiyFp* power,
    int* decimal_exponent) {
  int kQ = DiyFp::kSignificandSize;
  double k = ceil((min_exponent + kQ - 1) * kD_1_LOG2_10);
  int foo = kCachedPowersOffset;
  int index =
      (foo + static_cast<int>(k) - 1) / kDecimalExponentDistance + 1;
  DOUBLE_CONVERSION_ASSERT(0 <= index && index < static_cast<int>(DOUBLE_CONVERSION_ARRAY_SIZE(kCachedPowers)));
  CachedPower cached_power = kCachedPowers[index];
  DOUBLE_CONVERSION_ASSERT(min_exponent <= cached_power.binary_exponent);
  (void) max_exponent;
  DOUBLE_CONVERSION_ASSERT(cached_power.binary_exponent <= max_exponent);
  *decimal_exponent = cached_power.decimal_exponent;
  *power = DiyFp(cached_power.significand, cached_power.binary_exponent);
}
void GetCachedPowerForDecimalExponent(int requested_exponent,
                                      DiyFp* power,
                                      int* found_exponent) {
  DOUBLE_CONVERSION_ASSERT(kMinDecimalExponent <= requested_exponent);
  DOUBLE_CONVERSION_ASSERT(requested_exponent < kMaxDecimalExponent + kDecimalExponentDistance);
  int index =
      (requested_exponent + kCachedPowersOffset) / kDecimalExponentDistance;
  CachedPower cached_power = kCachedPowers[index];
  *power = DiyFp(cached_power.significand, cached_power.binary_exponent);
  *found_exponent = cached_power.decimal_exponent;
  DOUBLE_CONVERSION_ASSERT(*found_exponent <= requested_exponent);
  DOUBLE_CONVERSION_ASSERT(requested_exponent < *found_exponent + kDecimalExponentDistance);
}
}
}
/// cached-powers.cc END
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
/// string-to-double.cc START
#include <climits>
#include <locale>
#include <cmath>
#ifdef _MSC_VER
#  if _MSC_VER >= 1900
 __pragma(warning(disable: 4244))
#  endif
#  if _MSC_VER <= 1700
#    define VS2012_RADIXWARN
#  endif
#endif
namespace double_conversion {
namespace {
inline char ToLower(char ch) {
  static const std::ctype<char>& cType =
      std::use_facet<std::ctype<char> >(std::locale::classic());
  return cType.tolower(ch);
}
inline char Pass(char ch) {
  return ch;
}
template <class Iterator, class Converter>
static inline bool ConsumeSubStringImpl(Iterator* current,
                                        Iterator end,
                                        const char* substring,
                                        Converter converter) {
  DOUBLE_CONVERSION_ASSERT(converter(**current) == *substring);
  for (substring++; *substring != '\0'; substring++) {
    ++*current;
    if (*current == end || converter(**current) != *substring) {
      return false;
    }
  }
  ++*current;
  return true;
}
template <class Iterator>
static bool ConsumeSubString(Iterator* current,
                             Iterator end,
                             const char* substring,
                             bool allow_case_insensitivity) {
  if (allow_case_insensitivity) {
    return ConsumeSubStringImpl(current, end, substring, ToLower);
  } else {
    return ConsumeSubStringImpl(current, end, substring, Pass);
  }
}
inline bool ConsumeFirstCharacter(char ch,
                                         const char* str,
                                         bool case_insensitivity) {
  return case_insensitivity ? ToLower(ch) == str[0] : ch == str[0];
}
}
const int kMaxSignificantDigits = 772;
static const char kWhitespaceTable7[] = { 32, 13, 10, 9, 11, 12 };
static const int kWhitespaceTable7Length = DOUBLE_CONVERSION_ARRAY_SIZE(kWhitespaceTable7);
static const uc16 kWhitespaceTable16[] = {
  160, 8232, 8233, 5760, 6158, 8192, 8193, 8194, 8195,
  8196, 8197, 8198, 8199, 8200, 8201, 8202, 8239, 8287, 12288, 65279
};
static const int kWhitespaceTable16Length = DOUBLE_CONVERSION_ARRAY_SIZE(kWhitespaceTable16);
static bool isWhitespace(int x) {
  if (x < 128) {
    for (int i = 0; i < kWhitespaceTable7Length; i++) {
      if (kWhitespaceTable7[i] == x) return true;
    }
  } else {
    for (int i = 0; i < kWhitespaceTable16Length; i++) {
      if (kWhitespaceTable16[i] == x) return true;
    }
  }
  return false;
}
template <class Iterator>
static inline bool AdvanceToNonspace(Iterator* current, Iterator end) {
  while (*current != end) {
    if (!isWhitespace(**current)) return true;
    ++*current;
  }
  return false;
}
static bool isDigit(int x, int radix) {
  return (x >= '0' && x <= '9' && x < '0' + radix)
      || (radix > 10 && x >= 'a' && x < 'a' + radix - 10)
      || (radix > 10 && x >= 'A' && x < 'A' + radix - 10);
}
static double SignedZero(bool sign) {
  return sign ? -0.0 : 0.0;
}
#ifdef VS2012_RADIXWARN
#pragma optimize("",off)
static bool IsDecimalDigitForRadix(int c, int radix) {
  return '0' <= c && c <= '9' && (c - '0') < radix;
}
#pragma optimize("",on)
#else
static bool inline IsDecimalDigitForRadix(int c, int radix) {
  return '0' <= c && c <= '9' && (c - '0') < radix;
}
#endif
static bool IsCharacterDigitForRadix(int c, int radix, char a_character) {
  return radix > 10 && c >= a_character && c < a_character + radix - 10;
}
template<class Iterator>
static bool Advance (Iterator* it, uc16 separator, int base, Iterator& end) {
  if (separator == StringToDoubleConverter::kNoSeparator) {
    ++(*it);
    return *it == end;
  }
  if (!isDigit(**it, base)) {
    ++(*it);
    return *it == end;
  }
  ++(*it);
  if (*it == end) return true;
  if (*it + 1 == end) return false;
  if (**it == separator && isDigit(*(*it + 1), base)) {
    ++(*it);
  }
  return *it == end;
}
template<class Iterator>
static bool IsHexFloatString(Iterator start,
                             Iterator end,
                             uc16 separator,
                             bool allow_trailing_junk) {
  DOUBLE_CONVERSION_ASSERT(start != end);
  Iterator current = start;
  bool saw_digit = false;
  while (isDigit(*current, 16)) {
    saw_digit = true;
    if (Advance(&current, separator, 16, end)) return false;
  }
  if (*current == '.') {
    if (Advance(&current, separator, 16, end)) return false;
    while (isDigit(*current, 16)) {
      saw_digit = true;
      if (Advance(&current, separator, 16, end)) return false;
    }
  }
  if (!saw_digit) return false;
  if (*current != 'p' && *current != 'P') return false;
  if (Advance(&current, separator, 16, end)) return false;
  if (*current == '+' || *current == '-') {
    if (Advance(&current, separator, 16, end)) return false;
  }
  if (!isDigit(*current, 10)) return false;
  if (Advance(&current, separator, 16, end)) return true;
  while (isDigit(*current, 10)) {
    if (Advance(&current, separator, 16, end)) return true;
  }
  return allow_trailing_junk || !AdvanceToNonspace(&current, end);
}
template <int radix_log_2, class Iterator>
static double RadixStringToIeee(Iterator* current,
                                Iterator end,
                                bool sign,
                                uc16 separator,
                                bool parse_as_hex_float,
                                bool allow_trailing_junk,
                                double junk_string_value,
                                bool read_as_double,
                                bool* result_is_junk) {
  DOUBLE_CONVERSION_ASSERT(*current != end);
  DOUBLE_CONVERSION_ASSERT(!parse_as_hex_float ||
      IsHexFloatString(*current, end, separator, allow_trailing_junk));
  const int kDoubleSize = Double::kSignificandSize;
  const int kSingleSize = Single::kSignificandSize;
  const int kSignificandSize = read_as_double? kDoubleSize: kSingleSize;
  *result_is_junk = true;
  int64_t number = 0;
  int exponent = 0;
  const int radix = (1 << radix_log_2);
  bool post_decimal = false;
  while (**current == '0') {
    if (Advance(current, separator, radix, end)) {
      *result_is_junk = false;
      return SignedZero(sign);
    }
  }
  while (true) {
    int digit;
    if (IsDecimalDigitForRadix(**current, radix)) {
      digit = static_cast<char>(**current) - '0';
      if (post_decimal) exponent -= radix_log_2;
    } else if (IsCharacterDigitForRadix(**current, radix, 'a')) {
      digit = static_cast<char>(**current) - 'a' + 10;
      if (post_decimal) exponent -= radix_log_2;
    } else if (IsCharacterDigitForRadix(**current, radix, 'A')) {
      digit = static_cast<char>(**current) - 'A' + 10;
      if (post_decimal) exponent -= radix_log_2;
    } else if (parse_as_hex_float && **current == '.') {
      post_decimal = true;
      Advance(current, separator, radix, end);
      DOUBLE_CONVERSION_ASSERT(*current != end);
      continue;
    } else if (parse_as_hex_float && (**current == 'p' || **current == 'P')) {
      break;
    } else {
      if (allow_trailing_junk || !AdvanceToNonspace(current, end)) {
        break;
      } else {
        return junk_string_value;
      }
    }
    number = number * radix + digit;
    int overflow = static_cast<int>(number >> kSignificandSize);
    if (overflow != 0) {
      int overflow_bits_count = 1;
      while (overflow > 1) {
        overflow_bits_count++;
        overflow >>= 1;
      }
      int dropped_bits_mask = ((1 << overflow_bits_count) - 1);
      int dropped_bits = static_cast<int>(number) & dropped_bits_mask;
      number >>= overflow_bits_count;
      exponent += overflow_bits_count;
      bool zero_tail = true;
      for (;;) {
        if (Advance(current, separator, radix, end)) break;
        if (parse_as_hex_float && **current == '.') {
          Advance(current, separator, radix, end);
          DOUBLE_CONVERSION_ASSERT(*current != end);
          post_decimal = true;
        }
        if (!isDigit(**current, radix)) break;
        zero_tail = zero_tail && **current == '0';
        if (!post_decimal) exponent += radix_log_2;
      }
      if (!parse_as_hex_float &&
          !allow_trailing_junk &&
          AdvanceToNonspace(current, end)) {
        return junk_string_value;
      }
      int middle_value = (1 << (overflow_bits_count - 1));
      if (dropped_bits > middle_value) {
        number++;
      } else if (dropped_bits == middle_value) {
        if ((number & 1) != 0 || !zero_tail) {
          number++;
        }
      }
      if ((number & ((int64_t)1 << kSignificandSize)) != 0) {
        exponent++;
        number >>= 1;
      }
      break;
    }
    if (Advance(current, separator, radix, end)) break;
  }
  DOUBLE_CONVERSION_ASSERT(number < ((int64_t)1 << kSignificandSize));
  DOUBLE_CONVERSION_ASSERT(static_cast<int64_t>(static_cast<double>(number)) == number);
  *result_is_junk = false;
  if (parse_as_hex_float) {
    DOUBLE_CONVERSION_ASSERT(**current == 'p' || **current == 'P');
    Advance(current, separator, radix, end);
    DOUBLE_CONVERSION_ASSERT(*current != end);
    bool is_negative = false;
    if (**current == '+') {
      Advance(current, separator, radix, end);
      DOUBLE_CONVERSION_ASSERT(*current != end);
    } else if (**current == '-') {
      is_negative = true;
      Advance(current, separator, radix, end);
      DOUBLE_CONVERSION_ASSERT(*current != end);
    }
    int written_exponent = 0;
    while (IsDecimalDigitForRadix(**current, 10)) {
      if (abs(written_exponent) <= 100 * Double::kMaxExponent) {
        written_exponent = 10 * written_exponent + **current - '0';
      }
      if (Advance(current, separator, radix, end)) break;
    }
    if (is_negative) written_exponent = -written_exponent;
    exponent += written_exponent;
  }
  if (exponent == 0 || number == 0) {
    if (sign) {
      if (number == 0) return -0.0;
      number = -number;
    }
    return static_cast<double>(number);
  }
  DOUBLE_CONVERSION_ASSERT(number != 0);
  double result = Double(DiyFp(number, exponent)).value();
  return sign ? -result : result;
}
template <class Iterator>
double StringToDoubleConverter::StringToIeee(
    Iterator input,
    int length,
    bool read_as_double,
    int* processed_characters_count) const {
  Iterator current = input;
  Iterator end = input + length;
  *processed_characters_count = 0;
  const bool allow_trailing_junk = (flags_ & ALLOW_TRAILING_JUNK) != 0;
  const bool allow_leading_spaces = (flags_ & ALLOW_LEADING_SPACES) != 0;
  const bool allow_trailing_spaces = (flags_ & ALLOW_TRAILING_SPACES) != 0;
  const bool allow_spaces_after_sign = (flags_ & ALLOW_SPACES_AFTER_SIGN) != 0;
  const bool allow_case_insensitivity = (flags_ & ALLOW_CASE_INSENSITIVITY) != 0;
  if (current == end) return empty_string_value_;
  if (allow_leading_spaces || allow_trailing_spaces) {
    if (!AdvanceToNonspace(&current, end)) {
      *processed_characters_count = static_cast<int>(current - input);
      return empty_string_value_;
    }
    if (!allow_leading_spaces && (input != current)) {
      return junk_string_value_;
    }
  }
  int exponent = 0;
  int significant_digits = 0;
  int insignificant_digits = 0;
  bool nonzero_digit_dropped = false;
  bool sign = false;
  if (*current == '+' || *current == '-') {
    sign = (*current == '-');
    ++current;
    Iterator next_non_space = current;
    if (!AdvanceToNonspace(&next_non_space, end)) return junk_string_value_;
    if (!allow_spaces_after_sign && (current != next_non_space)) {
      return junk_string_value_;
    }
    current = next_non_space;
  }
  if (infinity_symbol_ != DOUBLE_CONVERSION_NULLPTR) {
    if (ConsumeFirstCharacter(*current, infinity_symbol_, allow_case_insensitivity)) {
      if (!ConsumeSubString(&current, end, infinity_symbol_, allow_case_insensitivity)) {
        return junk_string_value_;
      }
      if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
        return junk_string_value_;
      }
      if (!allow_trailing_junk && AdvanceToNonspace(&current, end)) {
        return junk_string_value_;
      }
      *processed_characters_count = static_cast<int>(current - input);
      return sign ? -Double::Infinity() : Double::Infinity();
    }
  }
  if (nan_symbol_ != DOUBLE_CONVERSION_NULLPTR) {
    if (ConsumeFirstCharacter(*current, nan_symbol_, allow_case_insensitivity)) {
      if (!ConsumeSubString(&current, end, nan_symbol_, allow_case_insensitivity)) {
        return junk_string_value_;
      }
      if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
        return junk_string_value_;
      }
      if (!allow_trailing_junk && AdvanceToNonspace(&current, end)) {
        return junk_string_value_;
      }
      *processed_characters_count = static_cast<int>(current - input);
      return sign ? -Double::NaN() : Double::NaN();
    }
  }
  bool leading_zero = false;
  if (*current == '0') {
    if (Advance(&current, separator_, 10, end)) {
      *processed_characters_count = static_cast<int>(current - input);
      return SignedZero(sign);
    }
    leading_zero = true;
    if (((flags_ & ALLOW_HEX) || (flags_ & ALLOW_HEX_FLOATS)) &&
        (*current == 'x' || *current == 'X')) {
      ++current;
      if (current == end) return junk_string_value_;
      bool parse_as_hex_float = (flags_ & ALLOW_HEX_FLOATS) &&
                IsHexFloatString(current, end, separator_, allow_trailing_junk);
      if (!parse_as_hex_float && !isDigit(*current, 16)) {
        return junk_string_value_;
      }
      bool result_is_junk;
      double result = RadixStringToIeee<4>(&current,
                                           end,
                                           sign,
                                           separator_,
                                           parse_as_hex_float,
                                           allow_trailing_junk,
                                           junk_string_value_,
                                           read_as_double,
                                           &result_is_junk);
      if (!result_is_junk) {
        if (allow_trailing_spaces) AdvanceToNonspace(&current, end);
        *processed_characters_count = static_cast<int>(current - input);
      }
      return result;
    }
    while (*current == '0') {
      if (Advance(&current, separator_, 10, end)) {
        *processed_characters_count = static_cast<int>(current - input);
        return SignedZero(sign);
      }
    }
  }
  bool octal = leading_zero && (flags_ & ALLOW_OCTALS) != 0;
  const int kBufferSize = kMaxSignificantDigits + 10;
  DOUBLE_CONVERSION_STACK_UNINITIALIZED char
      buffer[kBufferSize];
  int buffer_pos = 0;
  while (*current >= '0' && *current <= '9') {
    if (significant_digits < kMaxSignificantDigits) {
      DOUBLE_CONVERSION_ASSERT(buffer_pos < kBufferSize);
      buffer[buffer_pos++] = static_cast<char>(*current);
      significant_digits++;
    } else {
      insignificant_digits++;
      nonzero_digit_dropped = nonzero_digit_dropped || *current != '0';
    }
    octal = octal && *current < '8';
    if (Advance(&current, separator_, 10, end)) goto parsing_done;
  }
  if (significant_digits == 0) {
    octal = false;
  }
  if (*current == '.') {
    if (octal && !allow_trailing_junk) return junk_string_value_;
    if (octal) goto parsing_done;
    if (Advance(&current, separator_, 10, end)) {
      if (significant_digits == 0 && !leading_zero) {
        return junk_string_value_;
      } else {
        goto parsing_done;
      }
    }
    if (significant_digits == 0) {
      while (*current == '0') {
        if (Advance(&current, separator_, 10, end)) {
          *processed_characters_count = static_cast<int>(current - input);
          return SignedZero(sign);
        }
        exponent--;
      }
    }
    while (*current >= '0' && *current <= '9') {
      if (significant_digits < kMaxSignificantDigits) {
        DOUBLE_CONVERSION_ASSERT(buffer_pos < kBufferSize);
        buffer[buffer_pos++] = static_cast<char>(*current);
        significant_digits++;
        exponent--;
      } else {
        nonzero_digit_dropped = nonzero_digit_dropped || *current != '0';
      }
      if (Advance(&current, separator_, 10, end)) goto parsing_done;
    }
  }
  if (!leading_zero && exponent == 0 && significant_digits == 0) {
    return junk_string_value_;
  }
  if (*current == 'e' || *current == 'E') {
    if (octal && !allow_trailing_junk) return junk_string_value_;
    if (octal) goto parsing_done;
    Iterator junk_begin = current;
    ++current;
    if (current == end) {
      if (allow_trailing_junk) {
        current = junk_begin;
        goto parsing_done;
      } else {
        return junk_string_value_;
      }
    }
    char exponen_sign = '+';
    if (*current == '+' || *current == '-') {
      exponen_sign = static_cast<char>(*current);
      ++current;
      if (current == end) {
        if (allow_trailing_junk) {
          current = junk_begin;
          goto parsing_done;
        } else {
          return junk_string_value_;
        }
      }
    }
    if (current == end || *current < '0' || *current > '9') {
      if (allow_trailing_junk) {
        current = junk_begin;
        goto parsing_done;
      } else {
        return junk_string_value_;
      }
    }
    const int max_exponent = INT_MAX / 2;
    DOUBLE_CONVERSION_ASSERT(-max_exponent / 2 <= exponent && exponent <= max_exponent / 2);
    int num = 0;
    do {
      int digit = *current - '0';
      if (num >= max_exponent / 10
          && !(num == max_exponent / 10 && digit <= max_exponent % 10)) {
        num = max_exponent;
      } else {
        num = num * 10 + digit;
      }
      ++current;
    } while (current != end && *current >= '0' && *current <= '9');
    exponent += (exponen_sign == '-' ? -num : num);
  }
  if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
    return junk_string_value_;
  }
  if (!allow_trailing_junk && AdvanceToNonspace(&current, end)) {
    return junk_string_value_;
  }
  if (allow_trailing_spaces) {
    AdvanceToNonspace(&current, end);
  }
  parsing_done:
  exponent += insignificant_digits;
  if (octal) {
    double result;
    bool result_is_junk;
    char* start = buffer;
    result = RadixStringToIeee<3>(&start,
                                  buffer + buffer_pos,
                                  sign,
                                  separator_,
                                  false,
                                  allow_trailing_junk,
                                  junk_string_value_,
                                  read_as_double,
                                  &result_is_junk);
    DOUBLE_CONVERSION_ASSERT(!result_is_junk);
    *processed_characters_count = static_cast<int>(current - input);
    return result;
  }
  if (nonzero_digit_dropped) {
    buffer[buffer_pos++] = '1';
    exponent--;
  }
  DOUBLE_CONVERSION_ASSERT(buffer_pos < kBufferSize);
  buffer[buffer_pos] = '\0';
  Vector<const char> chars(buffer, buffer_pos);
  chars = TrimTrailingZeros(chars);
  exponent += buffer_pos - chars.length();
  double converted;
  if (read_as_double) {
    converted = StrtodTrimmed(chars, exponent);
  } else {
    converted = StrtofTrimmed(chars, exponent);
  }
  *processed_characters_count = static_cast<int>(current - input);
  return sign? -converted: converted;
}
double StringToDoubleConverter::StringToDouble(
    const char* buffer,
    int length,
    int* processed_characters_count) const {
  return StringToIeee(buffer, length, true, processed_characters_count);
}
double StringToDoubleConverter::StringToDouble(
    const uc16* buffer,
    int length,
    int* processed_characters_count) const {
  return StringToIeee(buffer, length, true, processed_characters_count);
}
float StringToDoubleConverter::StringToFloat(
    const char* buffer,
    int length,
    int* processed_characters_count) const {
  return static_cast<float>(StringToIeee(buffer, length, false,
                                         processed_characters_count));
}
float StringToDoubleConverter::StringToFloat(
    const uc16* buffer,
    int length,
    int* processed_characters_count) const {
  return static_cast<float>(StringToIeee(buffer, length, false,
                                         processed_characters_count));
}
template<>
double StringToDoubleConverter::StringTo<double>(
    const char* buffer,
    int length,
    int* processed_characters_count) const {
    return StringToDouble(buffer, length, processed_characters_count);
}
template<>
float StringToDoubleConverter::StringTo<float>(
    const char* buffer,
    int length,
    int* processed_characters_count) const {
    return StringToFloat(buffer, length, processed_characters_count);
}
template<>
double StringToDoubleConverter::StringTo<double>(
    const uc16* buffer,
    int length,
    int* processed_characters_count) const {
    return StringToDouble(buffer, length, processed_characters_count);
}
template<>
float StringToDoubleConverter::StringTo<float>(
    const uc16* buffer,
    int length,
    int* processed_characters_count) const {
    return StringToFloat(buffer, length, processed_characters_count);
}
}
/// string-to-double.cc END
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
