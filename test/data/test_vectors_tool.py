#!/usr/bin/env python3
"""Create and inspect Robot Raconteur test vector blobs.

The format is described in TEST_VECTORS_FORMAT.md. This tool has no dependencies
outside the Python standard library.

    test_vectors_tool.py pack vectors.json test_vectors.bin
    test_vectors_tool.py dump test_vectors.bin vectors.json
    test_vectors_tool.py list test_vectors.bin

The JSON form is {"vectors": [{"name": str, "type": str, "values": [...]}]}.
"""

import argparse
import json
import struct
import sys

MAGIC = b"RRTVECTS"
VERSION = 1
HEADER_SIZE = 24
ENTRY_SIZE = 64
NAME_SIZE = 48
HEADER = struct.Struct("<8sIIII")
ENTRY = struct.Struct("<48sIIII")

# name: (Robot Raconteur DataTypes code, struct format character)
TYPES = {
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
TYPES_BY_CODE = dict((code, (name, fmt)) for name, (code, fmt) in TYPES.items())


def pack(vectors):
    """Serialize a list of {"name", "type", "values"} dictionaries to blob bytes."""
    names = [v["name"] for v in vectors]
    if len(set(names)) != len(names):
        raise ValueError("duplicate vector names")

    records = []
    for vector in sorted(vectors, key=lambda v: v["name"].encode("ascii")):
        name = vector["name"].encode("ascii")
        if not name or len(name) >= NAME_SIZE:
            raise ValueError("name must be 1 to %d ASCII bytes: %s" % (NAME_SIZE - 1, vector["name"]))
        code, fmt = TYPES[vector["type"]]
        data = struct.pack("<%d%s" % (len(vector["values"]), fmt), *vector["values"])
        records.append((name, code, len(vector["values"]), data))

    data_start = HEADER_SIZE + ENTRY_SIZE * len(records)
    payload = bytearray()
    offsets_by_data = {}
    entries = []
    for name, code, count, data in records:
        # Identical bytes are stored once, and each name points at the same offset.
        if data not in offsets_by_data:
            payload += b"\0" * (-len(payload) % 8)
            offsets_by_data[data] = data_start + len(payload)
            payload += data
        entries.append(ENTRY.pack(name, code, count, offsets_by_data[data], 0))

    file_size = data_start + len(payload)
    header = HEADER.pack(MAGIC, VERSION, ENTRY_SIZE, len(records), file_size)
    return header + b"".join(entries) + bytes(payload)


def unpack(blob):
    """Parse and validate blob bytes, returning a list of vector dictionaries."""
    if len(blob) < HEADER_SIZE:
        raise ValueError("file is shorter than the header")
    magic, version, entry_size, entry_count, file_size = HEADER.unpack_from(blob, 0)
    if magic != MAGIC:
        raise ValueError("bad magic")
    if version != VERSION:
        raise ValueError("unsupported version %d" % version)
    if entry_size != ENTRY_SIZE:
        raise ValueError("unsupported entry size %d" % entry_size)
    if file_size != len(blob):
        raise ValueError("file_size is %d but the file has %d bytes" % (file_size, len(blob)))
    if HEADER_SIZE + ENTRY_SIZE * entry_count > len(blob):
        raise ValueError("index extends past the end of the file")

    vectors = []
    for i in range(entry_count):
        raw_name, code, count, offset, _ = ENTRY.unpack_from(blob, HEADER_SIZE + ENTRY_SIZE * i)
        name = raw_name.split(b"\0", 1)[0].decode("ascii")
        if code not in TYPES_BY_CODE:
            raise ValueError("%s has unknown type code %d" % (name, code))
        type_name, fmt = TYPES_BY_CODE[code]
        if offset + count * struct.calcsize("<" + fmt) > len(blob):
            raise ValueError("%s extends past the end of the file" % name)
        values = list(struct.unpack_from("<%d%s" % (count, fmt), blob, offset))
        vectors.append({"name": name, "type": type_name, "values": values, "offset": offset})
    return vectors


def read_blob(path):
    with open(path, "rb") as f:
        return f.read()


def command_pack(args):
    with open(args.input, "r") as f:
        document = json.load(f)
    with open(args.output, "wb") as f:
        f.write(pack(document["vectors"]))


def command_dump(args):
    vectors = unpack(read_blob(args.input))
    document = {"vectors": [dict((k, v[k]) for k in ("name", "type", "values")) for v in vectors]}
    with open(args.output, "w") as f:
        json.dump(document, f, indent=1)
        f.write("\n")


def command_list(args):
    for vector in unpack(read_blob(args.input)):
        print("%-48s %-7s count=%-7d offset=%d" %
              (vector["name"], vector["type"], len(vector["values"]), vector["offset"]))


def main(argv=None):
    parser = argparse.ArgumentParser(description="Create and inspect test vector blobs")
    commands = parser.add_subparsers(dest="command")
    commands.required = True

    command = commands.add_parser("pack", help="write a blob from JSON")
    command.add_argument("input")
    command.add_argument("output")
    command.set_defaults(function=command_pack)

    command = commands.add_parser("dump", help="write JSON from a blob")
    command.add_argument("input")
    command.add_argument("output")
    command.set_defaults(function=command_dump)

    command = commands.add_parser("list", help="validate a blob and print its index")
    command.add_argument("input")
    command.set_defaults(function=command_list)

    args = parser.parse_args(argv)
    args.function(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
