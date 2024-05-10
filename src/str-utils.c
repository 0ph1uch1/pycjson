#include "str-utils.h"
#ifdef __aarch64__
// ref https://stackoverflow.com/questions/11870910/sse-mm-movemask-epi8-equivalent-method-for-arm-neon
// Use shifts to collect all of the sign bits.
// I'm not sure if this works on big endian, but big endian NEON is very
// rare.
int16_t _movemmask(uint8x16_t input) {
    // Example input (half scale):
    // 0x89 FF 1D C0 00 10 99 33

    // Shift out everything but the sign bits
    // 0x01 01 00 01 00 00 01 00
    uint16x8_t high_bits = vreinterpretq_u16_u8(vshrq_n_u8(input, 7));

    // Merge the even lanes together with vsra. The '??' bytes are garbage.
    // vsri could also be used, but it is slightly slower on aarch64.
    // 0x??03 ??02 ??00 ??01
    uint32x4_t paired16 = vreinterpretq_u32_u16(
            vsraq_n_u16(high_bits, high_bits, 7));
    // Repeat with wider lanes.
    // 0x??????0B ??????04
    uint64x2_t paired32 = vreinterpretq_u64_u32(
            vsraq_n_u32(paired16, paired16, 14));
    // 0x??????????????4B
    uint8x16_t paired64 = vreinterpretq_u8_u64(
            vsraq_n_u64(paired32, paired32, 28));
    // Extract the low 8 bits from each lane and join.
    // 0x4B
    return vgetq_lane_u8(paired64, 0) | ((int) vgetq_lane_u8(paired64, 8) << 8);
}
uint32_t movemask(uint8x16x2_t v) {
    // TODO fix _movemmask
    return _movemmask(v.val[0]) | (_movemmask(v.val[1]) >> 16);
}
#endif