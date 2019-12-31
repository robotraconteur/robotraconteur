package com.robotraconteur;

public interface INamedArrayStub2<T> extends INamedArrayStub
{
    MessageElementNestedElementList packNamedArrayToArray(T s2);

    MessageElementNestedElementList packNamedArray(T[] s2);

    MessageElementNestedElementList packNamedMultiDimArray(NamedMultiDimArray s3);

    T unpackNamedArrayFromArray(MessageElementNestedElementList s2);

    T[] unpackNamedArray(MessageElementNestedElementList s2);

    NamedMultiDimArray unpackNamedMultiDimArray(MessageElementNestedElementList s3);
}
