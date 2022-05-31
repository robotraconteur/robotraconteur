//NodeID

%feature("pythonappend") RobotRaconteur::NodeID::ToByteArray() %{
	return bytearray(val)
%}

%include "NodeID.i"

%extend RobotRaconteur::NodeID {
	static NodeID FromByteArray(const char bytes[16])
	{
		boost::array<uint8_t,16> b = {};
		memcpy(&b[0],bytes,16);
	
		NodeID n(b);
		return n;	
	}

	%pythoncode %{
	def __str__(self):
		return self.ToString()
	%}
}