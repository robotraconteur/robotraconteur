package com.robotraconteur;

public interface IAStructureStub {
	
    public MessageElementData packStructure(Object s);

    public Object unpackStructure(MessageElementData m);
    
}
