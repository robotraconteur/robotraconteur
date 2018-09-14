package com.robotraconteur;

import java.util.Iterator;

public class IteratorGenerator<T> extends SyncGenerator2<T>
{
    boolean aborted = false;
    boolean closed = false;
    Iterator<T> enumerator;

    public IteratorGenerator(Iterator<T> enumerator)
    {
        this.enumerator = enumerator;
    }

    public synchronized void abort()
    {
        aborted = true;       
    }

    public synchronized void close()
    {
        closed = true;       
    }
    
    public synchronized T next()
    {        
        if (aborted) throw new OperationAbortedException("Generator aborted");
        if (closed) throw new StopIterationException("");
        if (!enumerator.hasNext()) throw new StopIterationException("");
        return enumerator.next();        
    }
}
