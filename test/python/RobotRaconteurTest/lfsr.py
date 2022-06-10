import sys
import numpy as np
import struct


try:
    xrange
except NameError:
    xrange = range


def _clip_uint32(x):
    return x & 0xFFFFFFFF


def _robotraconteur_test_lfsr_next(lfsr):
    bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 21) ^ (lfsr >> 31)) & 1
    return _clip_uint32((lfsr >> 1) | (bit << 31))


def _robotraconteur_test_lfsr_initkey(seed, key):

    # Run the LFSR a few times on the seed to shuffle
    for i in xrange(128):
        seed = _robotraconteur_test_lfsr_next(seed)

    i = 0
    hash_ = _robotraconteur_test_lfsr_next(seed)
    while (i != len(key)):
        hash_ = _clip_uint32(hash_ + ord(key[i]))
        i += 1
        hash_ = _clip_uint32(hash_ + _clip_uint32(hash_ << 10))
        hash_ = hash_ ^ _clip_uint32(hash_ >> 6)

    hash_ = _clip_uint32(hash_ + _clip_uint32(hash_ << 3))
    hash_ = hash_ ^ (hash_ >> 11)
    hash_ = _clip_uint32(hash_ + _clip_uint32(hash_ << 15))

    # Shuffle again
    for i in xrange(128):
        hash_ = _robotraconteur_test_lfsr_next(hash_)

    return _robotraconteur_test_lfsr_next(hash_)


def _robotraconteur_test_lfsr_next_char(lfsr):
    chars = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    lfsr2 = _robotraconteur_test_lfsr_next(lfsr)
    ind_ = lfsr2 % len(chars)
    return lfsr2, chars[ind_]


def _robotraconteur_test_lfsr_next_len(lfsr, max_len):
    lfsr2 = _robotraconteur_test_lfsr_next(lfsr)
    if (max_len < 32):
        len_out = lfsr2 % max_len
    else:
        len_out = 8 + lfsr2 % (max_len - 8)
    return lfsr2, len_out


def _robotraconteur_test_lfsr_next_dist(lfsr, min_, max_):
    assert (max_ > min_)
    lfsr2 = _robotraconteur_test_lfsr_next(lfsr)
    val_out = (lfsr2 % ((max_ - min_) + 1)) + min_
    return lfsr2, val_out


def _reinterpret_cast(fmt, x):
    b = struct.pack("<I", x)
    return struct.unpack_from("<" + fmt, b)[0]


def _next_typed(lfsr, fmt):
    lfsr2 = _robotraconteur_test_lfsr_next(lfsr)
    val_out = _reinterpret_cast(fmt, lfsr2)
    return lfsr2, val_out


def _reinterpret_cast2(fmt, x):
    b = struct.pack("<Q", x)
    return struct.unpack_from("<" + fmt, b)[0]


def _next_typed2(lfsr, fmt):
    lfsr2 = _robotraconteur_test_lfsr_next(lfsr)
    lfsr3 = _robotraconteur_test_lfsr_next(lfsr2)
    val_uint64 = (lfsr2 << 32) | lfsr3
    val_out = _reinterpret_cast2(fmt, val_uint64)
    return lfsr3, val_out


def _next_array(fn, dtype, len_):
    ret = np.zeros((len_), dtype=dtype)
    for i in xrange(len_):
        ret[i] = fn()
    return ret


def _next_array_var_len(fn, dtype, max_len, len_fn):
    len_ = len_fn(max_len)
    return _next_array(fn, dtype, len_)


class LFSRSeqGen:
    def __init__(self, seed, key=None):
        if key is None:
            self.lfsr = seed
        else:
            self.lfsr = _robotraconteur_test_lfsr_initkey(seed, key)

    def _next_typed(self, fmt):
        self.lfsr, val_out = _next_typed(self.lfsr, fmt)
        return val_out

    def _next_typed2(self, fmt):
        self.lfsr, val_out = _next_typed2(self.lfsr, fmt)
        return val_out

    def next_int8(self):
        return self._next_typed("b")

    def next_uint8(self):
        return self._next_typed("B")

    def next_int16(self):
        return self._next_typed("h")

    def next_uint16(self):
        return self._next_typed("H")

    def next_int32(self):
        return self._next_typed("i")

    def next_uint32(self):
        return self._next_typed("I")

    def next_int64(self):
        return self._next_typed2("q")

    def next_uint64(self):
        return self._next_typed2("Q")

    def next_float(self):
        return self._next_typed("f")

    def next_double(self):
        return self._next_typed2("d")

    def next_bool(self):
        return (self._next_typed("I") & 0x1) != 0

    def next_cfloat(self):
        return self._next_typed("f") + (self._next_typed("f") * 1j)

    def next_cdouble(self):
        return self._next_typed2("d") + (self._next_typed2("d") * 1j)

    def next_char(self):
        self.lfsr, val_out = _robotraconteur_test_lfsr_next_char(self.lfsr)
        return val_out

    def next_len(self, max_len):
        self.lfsr, val_out = _robotraconteur_test_lfsr_next_len(
            self.lfsr, max_len)
        return val_out

    def next_dist(self, min_, max_):
        self.lfsr, val_out = _robotraconteur_test_lfsr_next_dist(
            self.lfsr, min_, max_)
        return val_out

    def next_int8_array(self, len_):
        return _next_array(self.next_int8, np.int8, len_)

    def next_uint8_array(self, len_):
        return _next_array(self.next_uint8, np.uint8, len_)

    def next_int16_array(self, len_):
        return _next_array(self.next_int16, np.int16, len_)

    def next_uint16_array(self, len_):
        return _next_array(self.next_uint16, np.uint16, len_)

    def next_int32_array(self, len_):
        return _next_array(self.next_int32, np.int32, len_)

    def next_uint32_array(self, len_):
        return _next_array(self.next_uint32, np.uint32, len_)

    def next_int64_array(self, len_):
        return _next_array(self.next_int64, np.int64, len_)

    def next_uint64_array(self, len_):
        return _next_array(self.next_uint64, np.uint64, len_)

    def next_float_array(self, len_):
        return _next_array(self.next_float, np.float32, len_)

    def next_double_array(self, len_):
        return _next_array(self.next_double, np.float64, len_)

    def next_bool_array(self, len_):
        return _next_array(self.next_bool, np.bool8, len_)

    def next_cfloat_array(self, len_):
        return _next_array(self.next_cfloat, np.complex64, len_)

    def next_cdouble_array(self, len_):
        return _next_array(self.next_cdouble, np.complex128, len_)

    def next_string(self, len_):
        out = ""
        for _ in xrange(len_):
            out += self.next_char()
        return out

    def next_int8_array_var_len(self, max_len):
        return _next_array_var_len(self.next_int8, np.int8, max_len, self.next_len)

    def next_uint8_array_var_len(self, max_len):
        return _next_array_var_len(self.next_uint8, np.uint8, max_len, self.next_len)

    def next_int16_array_var_len(self, max_len):
        return _next_array_var_len(self.next_int16, np.int16, max_len, self.next_len)

    def next_uint16_array_var_len(self, max_len):
        return _next_array_var_len(self.next_uint16, np.int16, max_len, self.next_len)

    def next_int32_array_var_len(self, max_len):
        return _next_array_var_len(self.next_int32, np.int32, max_len, self.next_len)

    def next_uint32_array_var_len(self, max_len):
        return _next_array_var_len(self.next_uint32, np.uint32, max_len, self.next_len)

    def next_int64_array_var_len(self, max_len):
        return _next_array_var_len(self.next_int64, np.int64, max_len, self.next_len)

    def next_uint64_array_var_len(self, max_len):
        return _next_array_var_len(self.next_uint64, np.uint64, max_len, self.next_len)

    def next_float_array_var_len(self, max_len):
        return _next_array_var_len(self.next_float, np.float32, max_len, self.next_len)

    def next_double_array_var_len(self, max_len):
        return _next_array_var_len(self.next_double, np.float64, max_len, self.next_len)

    def next_bool_array_var_len(self, max_len):
        return _next_array_var_len(self.next_bool, np.bool8, max_len, self.next_len)

    def next_cfloat_array_var_len(self, max_len):
        return _next_array_var_len(self.next_cfloat, np.complex64, max_len, self.next_len)

    def next_cdouble_array_var_len(self, max_len):
        return _next_array_var_len(self.next_cdouble, np.complex128, max_len, self.next_len)

    def next_string_var_len(self, max_len):
        len_ = self.next_len(max_len)
        out = ""
        for _ in xrange(len_):
            out += self.next_char()
        return out


if __name__ == "__main__":

    count = 8
    seed = 35913226
    key = "Hello World!"

    if (len(sys.argv) > 1):
        key = sys.argv[1]

    seqgen = LFSRSeqGen(seed, key)

    print(seqgen.next_int8_array(count))
    print(seqgen.next_uint8_array(count))
    print(seqgen.next_int16_array(count))
    print(seqgen.next_uint16_array(count))
    print(seqgen.next_int32_array(count))
    print(seqgen.next_uint32_array(count))
    print(seqgen.next_int64_array(count))
    print(seqgen.next_uint64_array(count))
    print(seqgen.next_float_array(count))
    print(seqgen.next_double_array(count))
    print(seqgen.next_string(count))
    print(seqgen.next_bool_array(count))
    print(seqgen.next_cfloat_array(count))
    print(seqgen.next_cdouble_array(count))
    print()
    print(seqgen.next_int8_array_var_len(count))
    print(seqgen.next_uint8_array_var_len(count))
    print(seqgen.next_int16_array_var_len(count))
    print(seqgen.next_uint16_array_var_len(count))
    print(seqgen.next_int32_array_var_len(count))
    print(seqgen.next_uint32_array_var_len(count))
    print(seqgen.next_int64_array_var_len(count))
    print(seqgen.next_uint64_array_var_len(count))
    print(seqgen.next_float_array_var_len(count))
    print(seqgen.next_double_array_var_len(count))
    print(seqgen.next_string_var_len(count))
    print(seqgen.next_bool_array_var_len(count))
    print(seqgen.next_cfloat_array_var_len(count))
    print(seqgen.next_cdouble_array_var_len(count))
