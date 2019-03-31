package com.robotraconteur;

public interface INamedArrayStub2<T> extends INamedArrayStub
{
    MessageElementNamedArray packNamedArrayToArray(T s2);

    MessageElementNamedArray packNamedArray(T[] s2);

    MessageElementNamedMultiDimArray packNamedMultiDimArray(NamedMultiDimArray s3);

    T unpackNamedArrayFromArray(MessageElementNamedArray s2);

    T[] unpackNamedArray(MessageElementNamedArray s2);

    NamedMultiDimArray unpackNamedMultiDimArray(MessageElementNamedMultiDimArray s3);
}
