package com.robotraconteur;

public interface Generator1<ReturnType, ParamType>
{
    public ReturnType next(ParamType param);
    public void asyncNext(ParamType param, Action2<ReturnType, RuntimeException> handler, int timeout);
    public void abort();
    public void asyncAbort(Action1<RuntimeException> handler, int timeout);
    public void close();
    public void asyncClose(Action1<RuntimeException> handler, int timeout);
}
