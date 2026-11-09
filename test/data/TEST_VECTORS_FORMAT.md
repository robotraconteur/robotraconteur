# Test Vector Blob Format

Version 1

A test vector blob is one binary file that holds named numeric arrays for tests. Use one when several test programs,
often in different languages, must agree on the same large set of values. It replaces long array literals copied into
each language, and keeps one copy of the data.

A blob is worthwhile when the data is long or opaque, or whenever several languages must agree on it, however short it
is. Fixtures used by only one program can stay as ordinary literals.

The blob is frozen test data. Changing a value breaks interoperability with test programs built against the old data,
so add new vectors instead of editing existing ones.

## Getting started

1. Copy `test_vectors_tool.py` into your repository. It needs only the Python 3 standard library.
2. Write your data as JSON:

   ```json
   {
     "vectors": [
       { "name": "seq.double_constants", "type": "double", "values": [0.5, 0.25] },
       { "name": "ts1.i16_2.to_service", "type": "int16", "values": [-29064, 7306] }
     ]
   }
   ```

3. Create the blob and check it:

   ```sh
   ./test_vectors_tool.py pack vectors.json test_vectors.bin
   ./test_vectors_tool.py list test_vectors.bin
   ```

4. Commit `test_vectors.bin`. To inspect or extend it later, run `./test_vectors_tool.py dump test_vectors.bin
   vectors.json`, edit the JSON, and pack it again. The JSON does not need to be committed.
5. Add a reader and embed the blob as described below.

Before you remove any literals, compare each blob vector to the literals it replaces in every language. Test programs
that read the same blob agree with each other even if the blob is wrong, so a passing test does not prove the data is
right.

## File layout

The file has a header, an index, and then the array data. All integers and floating point values are little-endian.
Floating point values are IEEE 754. Big-endian hosts are not supported.

```text
+----------------------+  offset 0
| Header (24 bytes)    |
+----------------------+  offset 24
| Index entries        |  64 bytes each
+----------------------+  offset 24 + 64 * entry_count
| Array data           |  each array starts on an 8-byte boundary
+----------------------+  offset file_size
```

### Header

| Offset | Type      | Field         | Value                                    |
| ------ | --------- | ------------- | ---------------------------------------- |
| 0      | `char[8]` | `magic`       | ASCII `RRTVECTS`, with no terminator     |
| 8      | `u32`     | `version`     | `1`                                      |
| 12     | `u32`     | `entry_size`  | `64`                                     |
| 16     | `u32`     | `entry_count` | Number of index entries                  |
| 20     | `u32`     | `file_size`   | Total size of the file in bytes          |

### Index entry

| Offset | Type       | Field      | Notes                                          |
| ------ | ---------- | ---------- | ---------------------------------------------- |
| 0      | `char[48]` | `name`     | ASCII, 1 to 47 bytes, padded with NUL bytes    |
| 48     | `u32`      | `type`     | Type code from the table below                 |
| 52     | `u32`      | `count`    | Number of elements, not bytes                  |
| 56     | `u32`      | `offset`   | Start of the array, from the start of the file |
| 60     | `u32`      | `reserved` | `0`                                            |

Names are unique, and entries are sorted by name in bytewise order. This makes the file reproducible; readers may
still use a linear search.

Two entries may have the same `offset`, so identical data is stored once under several names. Readers must not assume
that arrays are unique or adjacent.

### Type codes

The codes match the Robot Raconteur `DataTypes` enumeration.

| Code | Type     | Element size |
| ---- | -------- | ------------ |
| 1    | `double` | 8            |
| 2    | `single` | 4            |
| 3    | `int8`   | 1            |
| 4    | `uint8`  | 1            |
| 5    | `int16`  | 2            |
| 6    | `uint16` | 2            |
| 7    | `int32`  | 4            |
| 8    | `uint32` | 4            |
| 9    | `int64`  | 8            |
| 10   | `uint64` | 8            |

### Array data

Arrays are stored as flat, packed elements. Writers pad between arrays with zero bytes so each array starts on an 8-byte
boundary. Readers must copy values out instead of casting into the blob.

Multidimensional arrays are stored flat. Store their dimensions as a separate `uint32` vector named `<name>.dims`. The
test code defines the storage order.

## Naming convention

Use dot-separated lowercase names in the form `<group>.<property>[.<direction>]`:

| Name                   | Meaning                                            |
| ---------------------- | -------------------------------------------------- |
| `seq.double_constants` | Lookup table for a sequence generator              |
| `ts1.i16_2.to_service` | Value that a client writes to a service property   |
| `ts1.i16_2.to_client`  | Value that a service returns from a property       |
| `ts1.d5.to_client.dims` | Dimensions of the multidimensional vector `ts1.d5.to_client` |

## Reader requirements

A reader must:

1. Check that the file is at least 24 bytes long and that `magic` matches.
2. Reject any `version` or `entry_size` that it does not support.
3. Check that `file_size` equals the actual size, and that the index fits inside the file.
4. Look up an entry by exact name, comparing up to the first NUL byte or all 48 bytes.
5. Fail if the entry's type code differs from the type the caller requested. Never convert silently.
6. Check that `offset + count * element_size` fits inside the file, using arithmetic that cannot overflow 32 bits.
7. Fail with a message that includes the vector name when a vector is missing or invalid.

A language with signed integers only should expose unsigned types through the next wider type, or document that it
returns the same bits as a signed value.

## Embedding the blob

Compiled test programs should embed the blob so they do not depend on the working directory or a path set at run time.
Interpreted tests can read it relative to their own source file.

### C++ with CMake

`#embed` needs C23 or C++26, and `.incbin` and `ld -r -b binary` are not available on MSVC. For portable builds, have
CMake generate a source file. The generated file belongs in the build directory.

```cmake
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${BLOB}")
file(READ "${BLOB}" hex HEX)
string(REGEX REPLACE "(................................)" "\\1\n" hex "${hex}")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," hex "${hex}")
file(WRITE "${OUT}.tmp"
     "#include <cstddef>\n"
     "extern const unsigned char test_vectors_data[];\n"
     "extern const std::size_t test_vectors_data_size;\n"
     "const unsigned char test_vectors_data[] = {\n${hex}\n};\n"
     "const std::size_t test_vectors_data_size = sizeof(test_vectors_data);\n")
configure_file("${OUT}.tmp" "${OUT}" COPYONLY)
```

Use a byte array here, because MSVC limits the length of string literals. The `extern` declarations are required
because a namespace-scope `const` otherwise has internal linkage. The array is constant-initialized, so it is safe to
read during static initialization.

### Java

Place the blob at the root of the class output directory before the jar is created, then read it from the class path:

```java
InputStream stream = TestVectors.class.getClassLoader().getResourceAsStream("test_vectors.bin");
```

Use a `ByteBuffer` with `ByteOrder.LITTLE_ENDIAN`.

### C\#

Embed the file under a fixed name, so the resource name does not depend on the project's default namespace:

```xml
<EmbeddedResource Include="path/to/test_vectors.bin" LogicalName="test_vectors.bin" />
```

```csharp
Stream stream = typeof(TestVectors).Assembly.GetManifestResourceStream("test_vectors.bin");
```

### Rust

```rust
static TEST_VECTORS: &[u8] = include_bytes!("../data/test_vectors.bin");
```

Decode values with `from_le_bytes`.

### Python

Read the file relative to `__file__`, and decode it with `struct` using formats that start with `<`.

### MATLAB

Find the file with `mfilename('fullpath')` and open it with `fopen(path, 'rb', 'ieee-le')`. Read with a precision such
as `'*uint64'`. Without the `*`, `fread` returns doubles, which cannot hold every 64-bit integer exactly.

## Changing the format

Readers reject versions they do not know, so increment `version` for any change that an existing reader would misread.
Adding vectors does not change the format.

## Reference implementations

The Robot Raconteur repository has a writer and readers for each language:

| Language | File                                                   |
| -------- | ------------------------------------------------------ |
| Tool     | `test/data/test_vectors_tool.py`                       |
| C++      | `test/cpp/common/include/test_vectors.h`               |
| CMake    | `RR_TEST_EMBED_BINARY` in `test/cmake/macros.cmake`    |
| Python   | `test/python/RobotRaconteurTest/testvectors.py`        |
| Java     | `test/java/test/com/robotraconteur/javatest/TestVectors.java` |
| C#       | `test/net/RobotRaconteurNETTest/TestVectors.cs`        |
| MATLAB   | `test/matlab/test_vectors_get.m`                       |
