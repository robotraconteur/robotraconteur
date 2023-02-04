package com.robotraconteur;

import java.util.List;

public interface Generator2<ReturnType>
{
    public ReturnType next();
    public void asyncNext(Action2<ReturnType, RuntimeException> handler, int timeout);
    public void abort();
    public void asyncAbort(Action1<RuntimeException> handler, int timeout);
    public void close();
    public void asyncClose(Action1<RuntimeException> handler, int timeout);
    public List<ReturnType> nextAll();
}
