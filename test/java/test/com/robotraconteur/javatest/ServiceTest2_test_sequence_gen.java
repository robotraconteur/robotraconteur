package com.robotraconteur.javatest;

class ServiceTest2_test_sequence_gen
{
    private long counter;
    private long step;
    private static final double[] double_constants = TestVectors.getDoubles("seq.double_constants");
    private static final long[] uint32_constants = TestVectors.getUInt32AsLongs("seq.uint32_constants");

    public ServiceTest2_test_sequence_gen(long seed)
    {
        counter = seed % 7823;
        step = seed % 17 + 7;
    }

    public double get_double()
    {
        counter += step;
        return double_constants[(int)(counter % double_constants.length)];
    }

    public long get_uint32()
    {
        counter += step;
        return uint32_constants[(int)(counter % uint32_constants.length)];
    }

    public byte get_int8()
    {
        counter += step;
        return (byte)(((int)(counter % 0xFF)) - 128);
    }
}
