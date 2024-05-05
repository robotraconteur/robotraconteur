package com.robotraconteur;

public abstract class Callback<T>
{

    protected String m_MemberName;

    public Callback(String name)
    {
        m_MemberName = name;
    }

    public abstract T getFunction();
    public abstract void setFunction(T value);

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public abstract T GetClientFunction(uint e);
    public abstract T getClientFunction(long e);
}
