using System;
using System.IO;
using System.Text;

// Reader for the test vector blob. See test/data/TEST_VECTORS_FORMAT.md.
static class TestVectors
{
    private const int HeaderSize = 24;
    private const int EntrySize = 64;
    private const int NameSize = 48;
    private static readonly byte[] Blob = Load();

    // Returns a copy of the named vector. T must match the type stored in the blob.
    public static T[] Get<T>(string name)
        where T : struct
    {
        uint type;
        int elementSize;
        GetTypeInfo(typeof(T), out type, out elementSize);
        int offset;
        int count;
        Find(name, type, elementSize, out offset, out count);
        var result = new T[count];
        Buffer.BlockCopy(Blob, offset, result, 0, count * elementSize);
        return result;
    }

    private static void GetTypeInfo(Type t, out uint type, out int elementSize)
    {
        if (t == typeof(double))
        {
            type = 1;
            elementSize = 8;
        }
        else if (t == typeof(float))
        {
            type = 2;
            elementSize = 4;
        }
        else if (t == typeof(sbyte))
        {
            type = 3;
            elementSize = 1;
        }
        else if (t == typeof(byte))
        {
            type = 4;
            elementSize = 1;
        }
        else if (t == typeof(short))
        {
            type = 5;
            elementSize = 2;
        }
        else if (t == typeof(ushort))
        {
            type = 6;
            elementSize = 2;
        }
        else if (t == typeof(int))
        {
            type = 7;
            elementSize = 4;
        }
        else if (t == typeof(uint))
        {
            type = 8;
            elementSize = 4;
        }
        else if (t == typeof(long))
        {
            type = 9;
            elementSize = 8;
        }
        else if (t == typeof(ulong))
        {
            type = 10;
            elementSize = 8;
        }
        else
        {
            throw new ArgumentException("Unsupported test vector type: " + t);
        }
    }

    private static void Find(string name, uint type, int elementSize, out int offset, out int count)
    {
        uint entryCount = BitConverter.ToUInt32(Blob, 16);
        for (uint i = 0; i < entryCount; i++)
        {
            int entry = HeaderSize + (int)i * EntrySize;
            if (EntryName(entry) != name)
            {
                continue;
            }
            if (BitConverter.ToUInt32(Blob, entry + 48) != type)
            {
                throw new ArgumentException("Test vector type mismatch: " + name);
            }
            uint entryValueCount = BitConverter.ToUInt32(Blob, entry + 52);
            uint entryOffset = BitConverter.ToUInt32(Blob, entry + 56);
            if ((ulong)entryOffset + (ulong)entryValueCount * (ulong)elementSize > (ulong)Blob.Length)
            {
                throw new InvalidDataException("Test vector out of bounds: " + name);
            }
            offset = (int)entryOffset;
            count = (int)entryValueCount;
            return;
        }
        throw new ArgumentException("Test vector not found: " + name);
    }

    private static string EntryName(int entry)
    {
        int length = 0;
        while (length < NameSize && Blob[entry + length] != 0)
        {
            length++;
        }
        return Encoding.ASCII.GetString(Blob, entry, length);
    }

    private static byte[] Load()
    {
        using (Stream stream = typeof(TestVectors).Assembly.GetManifestResourceStream("test_vectors.bin"))
        {
            if (stream == null)
            {
                throw new InvalidOperationException("test_vectors.bin is not embedded in the assembly");
            }
            using (var memory = new MemoryStream())
            {
                stream.CopyTo(memory);
                byte[] blob = memory.ToArray();
                if (blob.Length < HeaderSize || Encoding.ASCII.GetString(blob, 0, 8) != "RRTVECTS" ||
                    BitConverter.ToUInt32(blob, 8) != 1 || BitConverter.ToUInt32(blob, 12) != EntrySize ||
                    BitConverter.ToUInt32(blob, 20) != (uint)blob.Length)
                {
                    throw new InvalidDataException("Invalid or unsupported test vector blob");
                }
                if (HeaderSize + (ulong)BitConverter.ToUInt32(blob, 16) * EntrySize > (ulong)blob.Length)
                {
                    throw new InvalidDataException("Truncated test vector index");
                }
                return blob;
            }
        }
    }
}
