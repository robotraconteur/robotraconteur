package com.robotraconteur.javatest;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;

/** Reader for the test vector blob. See test/data/TEST_VECTORS_FORMAT.md. */
final class TestVectors
{
    private static final int HEADER_SIZE = 24;
    private static final int ENTRY_SIZE = 64;
    private static final int NAME_SIZE = 48;
    private static final int TYPE_DOUBLE = 1;
    private static final int TYPE_UINT32 = 8;
    private static final ByteBuffer BLOB = load();

    private TestVectors()
    {}

    static double[] getDoubles(String name)
    {
        ByteBuffer data = find(name, TYPE_DOUBLE);
        double[] result = new double[data.remaining() / 8];
        data.asDoubleBuffer().get(result);
        return result;
    }

    /** Returns uint32 values widened to long so they keep their unsigned value. */
    static long[] getUInt32AsLongs(String name)
    {
        ByteBuffer data = find(name, TYPE_UINT32);
        long[] result = new long[data.remaining() / 4];
        for (int i = 0; i < result.length; i++)
        {
            result[i] = data.getInt() & 0xFFFFFFFFL;
        }
        return result;
    }

    private static ByteBuffer find(String name, int type)
    {
        ByteBuffer blob = BLOB.duplicate().order(ByteOrder.LITTLE_ENDIAN);
        int entryCount = blob.getInt(16);
        for (int i = 0; i < entryCount; i++)
        {
            int entry = HEADER_SIZE + i * ENTRY_SIZE;
            if (!entryName(blob, entry).equals(name))
            {
                continue;
            }
            if (blob.getInt(entry + 48) != type)
            {
                throw new IllegalArgumentException("Test vector type mismatch: " + name);
            }
            int count = blob.getInt(entry + 52);
            int offset = blob.getInt(entry + 56);
            long byteCount = ((long)count) * (type == TYPE_DOUBLE ? 8 : 4);
            if (count < 0 || offset < 0 || offset + byteCount > blob.capacity())
            {
                throw new IllegalStateException("Test vector out of bounds: " + name);
            }
            blob.position(offset);
            blob.limit(offset + (int)byteCount);
            return blob.slice().order(ByteOrder.LITTLE_ENDIAN);
        }
        throw new IllegalArgumentException("Test vector not found: " + name);
    }

    private static String entryName(ByteBuffer blob, int entry)
    {
        int length = 0;
        while (length < NAME_SIZE && blob.get(entry + length) != 0)
        {
            length++;
        }
        byte[] name = new byte[length];
        for (int i = 0; i < length; i++)
        {
            name[i] = blob.get(entry + i);
        }
        return new String(name, StandardCharsets.US_ASCII);
    }

    private static ByteBuffer load()
    {
        try (InputStream stream = TestVectors.class.getClassLoader().getResourceAsStream("test_vectors.bin"))
        {
            if (stream == null)
            {
                throw new IllegalStateException("test_vectors.bin is not on the class path");
            }
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            byte[] chunk = new byte[65536];
            int read;
            while ((read = stream.read(chunk)) != -1)
            {
                bytes.write(chunk, 0, read);
            }

            ByteBuffer blob = ByteBuffer.wrap(bytes.toByteArray()).order(ByteOrder.LITTLE_ENDIAN);
            byte[] magic = new byte[8];
            if (blob.capacity() >= HEADER_SIZE)
            {
                blob.duplicate().get(magic);
            }
            if (!"RRTVECTS".equals(new String(magic, StandardCharsets.US_ASCII)) || blob.getInt(8) != 1 ||
                blob.getInt(12) != ENTRY_SIZE || blob.getInt(20) != blob.capacity())
            {
                throw new IllegalStateException("Invalid or unsupported test vector blob");
            }
            if (HEADER_SIZE + ((long)blob.getInt(16)) * ENTRY_SIZE > blob.capacity())
            {
                throw new IllegalStateException("Truncated test vector index");
            }
            return blob;
        }
        catch (IOException e)
        {
            throw new IllegalStateException("Could not read test_vectors.bin", e);
        }
    }
}
