package com.robotraconteur;

public class BroadcastDownsamplerStep
{
    protected BroadcastDownsampler downsampler;

    public BroadcastDownsamplerStep(BroadcastDownsampler downsampler)
    {
        this.downsampler = downsampler;
        downsampler.beginStep();
    }

    @Override public void finalize()
    {
        downsampler.endStep();
    }
}
