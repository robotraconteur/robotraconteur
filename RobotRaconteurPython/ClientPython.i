%rename(_FindObjRef) FindObjRef;
%rename(_FindObjRefTyped) FindObjRefTyped;

%include "Client.i"

%extend RobotRaconteur::WrappedServiceStub
{
%pythoncode %{
	def FindObjRef(self, name, index=None):
		if (index is None):
			stub=self._FindObjRef(name)
		else:
			stub=self._FindObjRef(name,str(index))

		return stub
%}
}

%rename(FindObjRef) FindObjRef;
%rename(FindObjRefTyped) FindObjRefTyped;
