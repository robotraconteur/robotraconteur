%extend RobotRaconteur::MessageElement
{
%feature ("pythonappend") GetData() %{
	a=MessageElementDataUtil.ToRRBaseArray(val)
	if (not a is None):
		return a
	a=MessageElementDataUtil.ToMessageElementStructure(val)
	if (not a is None):
		return a
	a=MessageElementDataUtil.ToMessageElementMap_int32_t(val)
	if (not a is None):
		return a
	a=MessageElementDataUtil.ToMessageElementMap_string(val)
	if (not a is None):
		return a
	a=MessageElementDataUtil.ToMessageElementMultiDimArray(val)
	if (not a is None):
		return a
	a=MessageElementDataUtil.ToMessageElementList(val)
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