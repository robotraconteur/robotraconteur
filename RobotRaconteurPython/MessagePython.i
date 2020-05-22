%extend RobotRaconteur::MessageElement
{
%feature ("pythonappend") GetData() %{
	a=MessageElementDataUtil.ToRRBaseArray(val)
	if (not a is None):
		return a
	a=MessageElementDataUtil.ToMessageElementNestedElementList(val)
	if (not a is None):
		return a
	
%}

%pythoncode %{
	def GetData(self):
		return self._GetData()

	def SetData(self,data):
		return self._SetData(data)
%}
}

%include "Message.i"

%typemap(directorin) std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > %{
  //std::vector<boost::intrusive_ptr<T> > workaround
  
  $input = SWIG_NewPointerObj((void*)(new $1_ltype((const $1_ltype &)$1)), $&descriptor, SWIG_POINTER_OWN | 0);
%}

%typemap(directorin) std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > const& %{
  //std::vector<boost::intrusive_ptr<T> > workaround
  
  $input = SWIG_NewPointerObj((void*)(new $1_basetype(*(const $1_ltype)&$1)), $descriptor, SWIG_POINTER_OWN | 0);
%}

%include <pybuffer.i>

%pybuffer_binary(const uint8_t* bytes, size_t bytes_len);
%inline {
boost::intrusive_ptr<RobotRaconteur::Message> MessageFromBytes(const uint8_t* bytes, size_t bytes_len)
{
	using namespace RobotRaconteur;
	ArrayBinaryReader r(bytes, 0, bytes_len);
	boost::intrusive_ptr<Message> m = CreateMessage();
	m->Read(r);
	return m;
}
size_t MessageLengthFromBytes(const uint8_t* bytes, size_t bytes_len)
{
	using namespace RobotRaconteur;
	ArrayBinaryReader r(bytes, 0, bytes_len);
	std::string magic = r.ReadString8(4).str().to_string();
	if (magic != "RRAC")
		throw ProtocolException("Invalid message magic");

	return r.ReadNumber<uint32_t>();
}


}