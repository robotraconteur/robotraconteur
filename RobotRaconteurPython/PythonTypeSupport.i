//Type support

namespace RobotRaconteur
{

%rename (_NewStructure) NewStructure;
PyObject* NewStructure(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);

%rename (_GetStructureType) GetStructureType;
PyObject* GetStructureType(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);


%rename (_GetNumPyDescrForType) GetNumPyDescrForType;
PyObject* GetNumPyDescrForType(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& e, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);
PyObject* GetNumPyDescrForType(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);
PyObject* GetNumPyDescrForType(const boost::shared_ptr<RobotRaconteur::TypeDefinition>& e, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);

%rename (_GetNamedTypeEntryType) GetNamedTypeEntryType;
DataTypes GetNamedTypeEntryType(const boost::shared_ptr<RobotRaconteur::TypeDefinition>& type1, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);

%rename(_PackMessageElement) PackMessageElement;
boost::intrusive_ptr<MessageElement>  PackMessageElement(PyObject* data, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteurNode>& node);
boost::intrusive_ptr<MessageElement>  PackMessageElement(PyObject* data, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServerContext>& obj, const boost::shared_ptr<RobotRaconteurNode>& node);

%rename(_UnpackMessageElement) UnpackMessageElement;
PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<WrappedServiceStub>& stub, const boost::shared_ptr<RobotRaconteurNode>& node);
PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServerContext>& obj, const boost::shared_ptr<RobotRaconteurNode>& node);

%rename(_PackToRRArray) PackToRRArray;
boost::intrusive_ptr<RRBaseArray> PackToRRArray(PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1,const boost::intrusive_ptr<RRBaseArray>& destrrarray);

%rename(_UnpackFromRRArray) UnpackFromRRArray;
PyObject* UnpackFromRRArray(const boost::intrusive_ptr<RRBaseArray>& rrarray, const boost::shared_ptr<TypeDefinition>& type1);

/*
%rename(_PackToRRArray_numpy) PackToRRArray_numpy;
boost::shared_ptr<RRBaseArray> PackToRRArray_numpy(PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<RRBaseArray>& destrrarray);

%rename(_UnpackFromRRArray_numpy) UnpackFromRRArray_numpy;
PyObject* UnpackFromRRArray_numpy(const boost::shared_ptr<RRBaseArray>& rrarray, const boost::shared_ptr<TypeDefinition>& type1);
*/

%rename(_RRTypeIdToNumPyDataType) RRTypeIdToNumPyDataType;
PyObject* RRTypeIdToNumPyDataType(DataTypes rr_type);

}
