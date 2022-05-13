#include "robotraconteur_test_lfsr.h"
#include <assert.h>

uint32_t robotraconteur_test_lfsr_initkey(uint32_t seed, const char* key, uint32_t key_len)
{

    // Run the LFSR a few times on the seed to shuffle
    for (size_t i=0; i<128; i++)
    {
        seed = robotraconteur_test_lfsr_next(seed);
    }

    size_t i = 0;
    uint32_t hash = robotraconteur_test_lfsr_next(seed);
    while (i != key_len) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    // Shuffle again
    for (size_t i = 0; i<128; i++)
    {
        hash = robotraconteur_test_lfsr_next(hash);
    }

    return robotraconteur_test_lfsr_next(hash);
}

uint32_t robotraconteur_test_lfsr_next(uint32_t lfsr)
{
    uint32_t bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 21) ^ (lfsr >> 31)) & 1u;
    return (lfsr >> 1) | (bit << 31);
}

uint32_t robotraconteur_test_lfsr_next_int8(uint32_t lfsr, int8_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((int8_t*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_uint8(uint32_t lfsr, uint8_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((uint8_t*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_int16(uint32_t lfsr, int16_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((int16_t*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_uint16(uint32_t lfsr, uint16_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((int16_t*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_int32(uint32_t lfsr, int32_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((int32_t*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_uint32(uint32_t lfsr, uint32_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((uint32_t*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_int64(uint32_t lfsr, int64_t* val_out)
{
    uint64_t val_out1;
    uint32_t lfsr2 = robotraconteur_test_lfsr_next_uint64(lfsr, &val_out1);
    *val_out = *((int64_t*)&val_out1);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_uint64(uint32_t lfsr, uint64_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    uint32_t lfsr3 = robotraconteur_test_lfsr_next(lfsr2);
    uint64_t val_out1 = lfsr2;
    val_out1 = (val_out1 << 32) | lfsr3;
    *val_out = val_out1;
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_float(uint32_t lfsr, float* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = *((float*)&lfsr2);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_double(uint32_t lfsr, double* val_out)
{
    uint64_t val_out1;
    uint32_t lfsr2 = robotraconteur_test_lfsr_next_uint64(lfsr, &val_out1);
    *val_out = *((double*)&val_out1);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_bool(uint32_t lfsr, uint8_t* val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = (uint8_t)(lfsr2 & 0x1);
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_cfloat(uint32_t lfsr, float* r_val_out, float* i_val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next_float(lfsr, r_val_out);
    return robotraconteur_test_lfsr_next_float(lfsr2, i_val_out);
}

uint32_t robotraconteur_test_lfsr_next_cdouble(uint32_t lfsr, double* r_val_out, double* i_val_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next_double(lfsr, r_val_out);
    return robotraconteur_test_lfsr_next_double(lfsr2, i_val_out);
}

uint32_t robotraconteur_test_lfsr_next_char(uint32_t lfsr, char* val_out)
{
    const char chars[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    uint32_t ind = lfsr2 % sizeof(chars);
    *val_out = chars[ind];
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_len(uint32_t lfsr, size_t max_len, size_t* len_out)
{
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    if (max_len < 32)
    {
        *len_out = ((size_t)lfsr2) % max_len;
    }
    else
    {
        *len_out = 8 + ((size_t)lfsr2) % (max_len - 8);
    }
    return lfsr2;
}

uint32_t robotraconteur_test_lfsr_next_dist(uint32_t lfsr, uint32_t min_, uint32_t max_, uint32_t* val_out)
{
    assert(max_ > min_);
    uint32_t lfsr2 = robotraconteur_test_lfsr_next(lfsr);
    *val_out = (lfsr2 % ((max_-min_)+1)) + min_;
    return lfsr2;
}
