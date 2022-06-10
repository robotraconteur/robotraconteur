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
	r.Seek(8);
	uint16_t ver = r.ReadNumber<uint16_t>();
	r.Seek(0);
	boost::intrusive_ptr<Message> m = CreateMessage();
	switch (ver)
	{
	case 2:
		m->Read(r);
		break;
	case 4:
		m->Read4(r);
		break;
	default:
		throw InvalidArgumentException("Invalid message version");
	}
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
PyObject* MessageToBytes(boost::intrusive_ptr<RobotRaconteur::Message> m, uint32_t ver = 2)
{
	using namespace RobotRaconteur;
	if (m==NULL)
	{
		throw std::invalid_argument("m must not be null");
	}	

	switch (ver)
	{
	case 2:
	{
		size_t len = m->ComputeSize();
		boost::shared_array<uint8_t> buf(new uint8_t[len]);
		ArrayBinaryWriter w(buf.get(), 0, len);
		m->Write(w);
		return PyByteArray_FromStringAndSize((char*)buf.get(),len);
	}	
	case 4:		
	{
		size_t len = m->ComputeSize4();
		boost::shared_array<uint8_t> buf(new uint8_t[len]);
		ArrayBinaryWriter w(buf.get(), 0, len);
		m->Write4(w);
		return PyByteArray_FromStringAndSize((char*)buf.get(),len);
	}
	default:
		throw InvalidArgumentException("Invalid message version");
	}
}

boost::intrusive_ptr<RobotRaconteur::MessageElement> MessageElementFromBytes(const uint8_t* bytes, size_t bytes_len)
{
	using namespace RobotRaconteur;
	ArrayBinaryReader r(bytes, 0, bytes_len);
	boost::intrusive_ptr<MessageElement> m = CreateMessageElement();
	m->Read(r);
	return m;
}
size_t MessageElementLengthFromBytes(const uint8_t* bytes, size_t bytes_len)
{
	using namespace RobotRaconteur;
	ArrayBinaryReader r(bytes, 0, bytes_len);
	return r.ReadNumber<uint32_t>();
}
PyObject* MessageElementToBytes(const boost::intrusive_ptr<RobotRaconteur::MessageElement>& m, uint32_t ver = 2)
{
	using namespace RobotRaconteur;
	if (m==NULL)
	{
		throw std::invalid_argument("m must not be null");
	}
	m->UpdateData();
	size_t len = m->ComputeSize();
	boost::shared_array<uint8_t> buf(new uint8_t[len]);
	ArrayBinaryWriter w(buf.get(), 0, len);
	m->Write(w);
	return PyByteArray_FromStringAndSize((char*)buf.get(),len);
	
}

}