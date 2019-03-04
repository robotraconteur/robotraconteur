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

%typemap(directorin) std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > %{
  //std::vector<boost::intrusive_ptr<T> > workaround
  
  $input = SWIG_NewPointerObj((void*)(new $1_ltype((const $1_ltype &)$1)), $&descriptor, SWIG_POINTER_OWN | 0);
%}

%typemap(directorin) std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > const& %{
  //std::vector<boost::intrusive_ptr<T> > workaround
  
  $input = SWIG_NewPointerObj((void*)(new $1_basetype(*(const $1_ltype)&$1)), $descriptor, SWIG_POINTER_OWN | 0);
%}
