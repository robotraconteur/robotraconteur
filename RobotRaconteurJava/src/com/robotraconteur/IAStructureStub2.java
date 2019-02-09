package com.robotraconteur;

public interface IAStructureStub2<T> extends IAStructureStub
{
    MessageElementAStructureArray packAStructureToArray(T s2);

    MessageElementAStructureArray packAStructureArray(T[] s2);

    MessageElementAStructureMultiDimArray packAStructureMultiDimArray(AStructureMultiDimArray s3);

    T unpackAStructureFromArray(MessageElementAStructureArray s2);

    T[] unpackAStructureArray(MessageElementAStructureArray s2);

    AStructureMultiDimArray unpackAStructureMultiDimArray(MessageElementAStructureMultiDimArray s3);
}
