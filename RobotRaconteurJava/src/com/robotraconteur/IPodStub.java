package com.robotraconteur;

public interface IPodStub
{
    MessageElementData packStructure(Object s);

    Object unpackStructure(MessageElementData m);
}
