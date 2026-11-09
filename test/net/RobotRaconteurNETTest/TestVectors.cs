using System;
using System.IO;
using System.Text;

// Reader for the test vector blob. See test/data/TEST_VECTORS_FORMAT.md.
static class TestVectors
{
    private const int HeaderSize = 24;
    private const int EntrySize = 64;
    private const int NameSize = 48;
    private const uint TypeDouble = 1;
    private const uint TypeUInt32 = 8;
    private static readonly byte[] Blob = Load();

    public static double[] GetDoubles(string name)
    {
        int offset;
        int count;
        Find(name, TypeDouble, 8, out offset, out count);
        var result = new double[count];
        Buffer.BlockCopy(Blob, offset, result, 0, count * 8);
        return result;
    }

    public static uint[] GetUInt32s(string name)
    {
        int offset;
        int count;
        Find(name, TypeUInt32, 4, out offset, out count);
        var result = new uint[count];
        Buffer.BlockCopy(Blob, offset, result, 0, count * 4);
        return result;
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
