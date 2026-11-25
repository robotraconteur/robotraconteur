"""Reader for the test vector blob. See test/data/TEST_VECTORS_FORMAT.md."""
from __future__ import absolute_import

import os
import struct

_HEADER = struct.Struct("<8sIIII")
_ENTRY = struct.Struct("<48sIIII")
_FORMATS = {
    "double": (1, "d"),
    "single": (2, "f"),
    "int8": (3, "b"),
    "uint8": (4, "B"),
    "int16": (5, "h"),
    "uint16": (6, "H"),
    "int32": (7, "i"),
    "uint32": (8, "I"),
    "int64": (9, "q"),
    "uint64": (10, "Q"),
}
_BLOB_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "data", "test_vectors.bin")


def _load_blob():
    with open(_BLOB_PATH, "rb") as f:
        return f.read()


def get_test_vector(name, type_name):
    """Return the named vector as a list. type_name must match the type stored in the blob."""
    blob = _load_blob()
    if len(blob) < _HEADER.size:
        raise ValueError("Invalid test vector blob")
    magic, version, entry_size, entry_count, file_size = _HEADER.unpack_from(blob, 0)
    if magic != b"RRTVECTS" or version != 1 or entry_size != _ENTRY.size or file_size != len(blob):
        raise ValueError("Invalid or unsupported test vector blob")
    if _HEADER.size + _ENTRY.size * entry_count > len(blob):
        raise ValueError("Truncated test vector index")

    expected_code, fmt = _FORMATS[type_name]
    wanted = name.encode("ascii")
    for i in range(entry_count):
        raw_name, code, count, offset, _ = _ENTRY.unpack_from(blob, _HEADER.size + _ENTRY.size * i)
        if raw_name.split(b"\0", 1)[0] != wanted:
            continue
        if code != expected_code:
            raise ValueError("Test vector type mismatch: " + name)
        data_format = "<%d%s" % (count, fmt)
        if offset + struct.calcsize(data_format) > len(blob):
            raise ValueError("Test vector out of bounds: " + name)
        return list(struct.unpack_from(data_format, blob, offset))
    raise KeyError("Test vector not found: " + name)
