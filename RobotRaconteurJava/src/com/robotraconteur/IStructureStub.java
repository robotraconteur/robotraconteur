package com.robotraconteur;

public interface IStructureStub
{
    MessageElementData packStructure(Object s);

    <T> T unpackStructure(MessageElementData m);
}