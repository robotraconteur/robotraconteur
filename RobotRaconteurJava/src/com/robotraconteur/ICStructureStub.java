package com.robotraconteur;

public interface ICStructureStub
{
	MessageElementData packStructure(Object s);

	Object unpackStructure(MessageElementData m);
}