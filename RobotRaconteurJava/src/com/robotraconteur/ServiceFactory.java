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
            String stub_type = RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s.getClass()))[1];
            IStructureStub stub = findStructureStub(stub_type);
            return stub.packStructure(s);
        }
        if (s instanceof RRPod)
        {
            String stub_type = RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s.getClass()))[1];
            IPodStub stub = findPodStub(stub_type);
            return stub.packStructure(s);
        }

        if (s instanceof RRPod[])
        {
            String stub_type = RobotRaconteurNode.splitQualifiedName(
                RobotRaconteurNode.getTypeString(s.getClass().getComponentType()))[1];
            IPodStub stub = findPodStub(stub_type);
            return stub.packStructure(s);
        }
        if (s instanceof PodMultiDimArray)
        {
            PodMultiDimArray s2 = (PodMultiDimArray)s;
            String stub_type = RobotRaconteurNode.splitQualifiedName(
                RobotRaconteurNode.getTypeString(s2.pod_array.getClass().getComponentType()))[1];
            IPodStub stub = findPodStub(stub_type);
            return stub.packStructure(s);
        }
        if (s instanceof RRNamedArray)
        {
            String stub_type = RobotRaconteurNode.splitQualifiedName(RobotRaconteurNode.getTypeString(s.getClass()))[1];
            INamedArrayStub stub = findNamedArrayStub(stub_type);
            return stub.packStructure(s);
        }

        if (s instanceof RRNamedArray[])
        {
            String stub_type = RobotRaconteurNode.splitQualifiedName(
                RobotRaconteurNode.getTypeString(s.getClass().getComponentType()))[1];
            INamedArrayStub stub = findNamedArrayStub(stub_type);
            return stub.packStructure(s);
        }
        if (s instanceof NamedMultiDimArray)
        {
            NamedMultiDimArray s2 = (NamedMultiDimArray)s;
            String stub_type = RobotRaconteurNode.splitQualifiedName(
                RobotRaconteurNode.getTypeString(s2.namedarray_array.getClass().getComponentType()))[1];
            INamedArrayStub stub = findNamedArrayStub(stub_type);
            return stub.packStructure(s);
        }

        throw new DataTypeException("Invalid structure type");
    }

    public <T> T unpackStructure(MessageElementData l)
    {
        switch (l.getTypeID())
        {
        case DataTypes_structure_t: {
            String stub_type = RobotRaconteurNode.splitQualifiedName(l.getTypeString())[1];
            IStructureStub stub = findStructureStub(stub_type);
            return stub.unpackStructure(l);
        }
        case DataTypes_pod_t:
        case DataTypes_pod_array_t:
        case DataTypes_pod_multidimarray_t: {
            String stub_type = RobotRaconteurNode.splitQualifiedName(l.getTypeString())[1];
            IPodStub stub = findPodStub(stub_type);
            return (T)stub.unpackStructure(l);
        }
        case DataTypes_namedarray_array_t:
        case DataTypes_namedarray_multidimarray_t: {
            String stub_type = RobotRaconteurNode.splitQualifiedName(l.getTypeString())[1];
            INamedArrayStub stub = findNamedArrayStub(stub_type);
            return (T)stub.unpackStructure(l);
        }
        default:
            throw new DataTypeException("Invalid structure type");
        }
    }

    public abstract IStructureStub findStructureStub(String objecttype);

    public abstract IPodStub findPodStub(String objecttype);

    public abstract INamedArrayStub findNamedArrayStub(String objecttype);

    public abstract ServiceStub createStub(WrappedServiceStub innerstub);

    public abstract ServiceSkel createSkel(Object innerskel);

    public abstract String getServiceName();

    public abstract RobotRaconteurException downCastException(RobotRaconteurException exp);
}