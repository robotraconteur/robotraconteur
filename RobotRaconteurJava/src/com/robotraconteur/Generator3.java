package com.robotraconteur;

public interface Generator3<ParamType> {
	public void next(ParamType param);
    public void asyncNext(ParamType param, Action1<RuntimeException> handler, int timeout);
    public void abort();
    public void asyncAbort(Action1<RuntimeException> handler, int timeout);
    public void close();
    public void asyncClose(Action1<RuntimeException> handler, int timeout);	
}
