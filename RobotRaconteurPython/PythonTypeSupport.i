//Type support

namespace RobotRaconteur
{

%rename (_NewStructure) NewStructure;
PyObject* NewStructure(const std::string& type, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);

%rename (_GetNumPyDescrForType) GetNumPyDescrForType;
PyObject* GetNumPyDescrForType(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> e, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);
PyObject* GetNumPyDescrForType(const std::string& type, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);
PyObject* GetNumPyDescrForType(boost::shared_ptr<RobotRaconteur::TypeDefinition> e, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);

%rename (_GetNamedTypeEntryType) GetNamedTypeEntryType;
DataTypes GetNamedTypeEntryType(boost::shared_ptr<RobotRaconteur::TypeDefinition> type1, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);

%rename(_PackMessageElement) PackMessageElement;
boost::intrusive_ptr<MessageElement>  PackMessageElement(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteurNode> node);
boost::intrusive_ptr<MessageElement>  PackMessageElement(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<ServerContext> obj, boost::shared_ptr<RobotRaconteurNode> node);

%rename(_UnpackMessageElement) UnpackMessageElement;
PyObject* UnpackMessageElement(boost::intrusive_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> stub, boost::shared_ptr<RobotRaconteurNode> node);
PyObject* UnpackMessageElement(boost::intrusive_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<ServerContext> obj, boost::shared_ptr<RobotRaconteurNode> node);

%rename(_PackToRRArray) PackToRRArray;
boost::intrusive_ptr<RRBaseArray> PackToRRArray(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::intrusive_ptr<RRBaseArray> destrrarray);

%rename(_UnpackFromRRArray) UnpackFromRRArray;
PyObject* UnpackFromRRArray(boost::intrusive_ptr<RRBaseArray> rrarray, boost::shared_ptr<TypeDefinition> type1);

/*
%rename(_PackToRRArray_numpy) PackToRRArray_numpy;
boost::shared_ptr<RRBaseArray> PackToRRArray_numpy(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<RRBaseArray> destrrarray);

%rename(_UnpackFromRRArray_numpy) UnpackFromRRArray_numpy;
PyObject* UnpackFromRRArray_numpy(boost::shared_ptr<RRBaseArray> rrarray, boost::shared_ptr<TypeDefinition> type1);
*/

%rename(_RRTypeIdToNumPyDataType) RRTypeIdToNumPyDataType;
PyObject* RRTypeIdToNumPyDataType(DataTypes rr_type);

}
