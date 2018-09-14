package com.robotraconteur;

public abstract class ServiceFactory
{
	private ServiceDefinition sdef = null;

	public final ServiceDefinition serviceDef()
	{
		if (sdef == null)
		{
			sdef = new ServiceDefinition();
			sdef.fromString(defString());
		}
		return sdef;
	}

	public final static String removePath(String path)
	{		
		return RobotRaconteurNode.splitQualifiedName(path)[1];
	}
	
	public boolean compareNamespace(String qualified_typename)
	{
		String[] s = RobotRaconteurNode.splitQualifiedName(qualified_typename);
		return s[0] == getServiceName();
	}

	public abstract String defString();

	public MessageElementData packStructure(Object s)
	{
		if (s instanceof RRStructure)		
		{
			String stub_type=RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s.getClass()))[1];
			IStructureStub stub=findStructureStub(stub_type);
			return stub.packStructure(s);			
		}
		if (s instanceof RRCStructure)
		{
			String stub_type=RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s.getClass()))[1];
			ICStructureStub stub=findCStructureStub(stub_type);
			return stub.packStructure(s);			
		}
		
		if (s instanceof RRCStructure[])
		{
			String stub_type=RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s.getClass().getComponentType()))[1];
			ICStructureStub stub=findCStructureStub(stub_type);
			return stub.packStructure(s);			
		}
		
		if (s instanceof CStructureMultiDimArray)
		{
			CStructureMultiDimArray s2=(CStructureMultiDimArray)s;
			String stub_type=RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s2.cstruct_array.getClass().getComponentType()))[1];
			ICStructureStub stub=findCStructureStub(stub_type);
			return stub.packStructure(s);			
		}
		
		throw new DataTypeException("Invalid structure type");
	}

	public <T> T unpackStructure(MessageElementData l)
	{
		switch (l.getTypeID())
		{
		case DataTypes_structure_t:
		{
			String stub_type=RobotRaconteurNode.splitQualifiedName(l.getTypeString())[1];
			IStructureStub stub=findStructureStub(stub_type);
			return stub.unpackStructure(l);
		}
		case DataTypes_cstructure_t:
		case DataTypes_cstructure_array_t:
		case DataTypes_cstructure_multidimarray_t:
		{
			String stub_type=RobotRaconteurNode.splitQualifiedName(l.getTypeString())[1];
			ICStructureStub stub=findCStructureStub(stub_type);
			return (T)stub.unpackStructure(l);
		}
		default:
			throw new DataTypeException("Invalid structure type");
		}
		
	}

	public abstract IStructureStub findStructureStub(String objecttype);
	
	public abstract ICStructureStub findCStructureStub(String objecttype);

	public abstract ServiceStub createStub(WrappedServiceStub innerstub);

	public abstract ServiceSkel createSkel(Object innerskel);

	public abstract String getServiceName();
	
	public abstract RobotRaconteurException downCastException(RobotRaconteurException exp);
}