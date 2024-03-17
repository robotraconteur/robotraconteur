package com.robotraconteur;

public class CallbackClient<T> extends Callback<T>
{
    public CallbackClient(String name)
    {
        super(name);
    }

    private T function = null;
    @Override public T getFunction()
    {
        return function;
    }
    @Override public void setFunction(T value)
    {
        function = value;
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public override T GetClientFunction(uint e)
    @Override public T getClientFunction(long e)
    {
        throw new UnsupportedOperationException("Invalid for client side of callback");
    }
}
