import numpy as np


def fill_double_multidim_array(seqgen, dims):
    n_elems = np.prod(dims)
    a = seqgen.next_double_array(n_elems)
    return a.reshape(dims, order="F")


def fill_uint8_multidim_array(seqgen, dims):
    n_elems = np.prod(dims)
    a = seqgen.next_uint8_array(n_elems)
    return a.reshape(dims, order="F")


def get_multidim_array_sub(source, srcpos, count):
    d = []
    for s, c in zip(srcpos, count):
        d.append(slice(s, s + c))
    return source[tuple(d)]


def set_multidim_array_sub(dest, destpos, source):
    d = []
    for s, c in zip(destpos, source.shape):
        d.append(slice(s, s + c))
    dest[tuple(d)] = source
