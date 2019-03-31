package com.robotraconteur;

public interface INamedArrayStub {
	
    public MessageElementData packStructure(Object s);

    public Object unpackStructure(MessageElementData m);
    
}
