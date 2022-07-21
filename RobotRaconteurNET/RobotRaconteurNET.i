%module(directors="1") RobotRaconteurNET

#pragma SWIG nowarn=473

%include "CommonInclude.i"

%include "typemaps.i"
%include "arrays_csharp.i"

//%apply void *VOID_INT_PTR { void * }

%define RR_PUBLIC_OVERRIDE_METHOD(a) 
%csmethodmodifiers a "public override";
%enddef
%define RR_PUBLIC_VIRTUAL_METHOD(a) 
%csmethodmodifiers a "public virtual";
%enddef
%define RR_PROPERTY(a) 
%rename(_Get ## a) Get ## a; %rename(_Set ## a) Set ## a; %csmethodmodifiers Get ## a "private"; %csmethodmodifiers Set ## a "private";
%enddef
%define RR_MAKE_METHOD_PRIVATE(a) 
%csmethodmodifiers a "private"; %csmethodmodifiers _ ## a "private"; %rename(_ ## a) a;
%enddef

%define RR_RELEASE_GIL()
%enddef
%define RR_KEEP_GIL()
%enddef

%define RR_DIRECTOR_SHARED_PTR_RETURN_DEFAULT(TYPE)
%typemap(directorout) SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >
%{ if ($input) {
    SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *smartarg = (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *)$input;
    $result = *smartarg;
  }
%}
%enddef

%define RR_DIRECTOR_SHARED_PTR_RETURN_RESET(TYPE)
%typemap(directorout) SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >
%{ if ($input) {
    SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *smartarg = (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *)$input;
    $result = *smartarg;
	smartarg->reset();
  }
%}
%enddef

%rename("%(regex:/^([A-Z][a-z]+)+_(.*)/\\2/)s", %$isenumitem) ""; // Colour_Red -> Red
//%rename("%(regex:/^(RobotRaconteur_LogLevel)_(.*)/\\2/)s", %$isenumitem) "";
//%rename("%(regex:/^(RobotRaconteur_LogComponent)_(.*)/\\2/)s", %$isenumitem) "";

%include "RobotRaconteurConstants.i"

%include "NETTypemaps.i"
%include "NETExceptionTypemaps.i"


%typemap(csclassmodifiers) std::map<std::string,std::string> "public partial class";
%include "DataTypes.i"

%include "ServiceDefinitionNET.i"

%include "NodeIDNET.i"

#define RR_MESSAEGE_ELEMENT_DATA_UTIL_EXTRAS
%include "MessageNET.i"

%include "AsyncHandlerDirector.i"

%typemap(csclassmodifiers) rr_memcpy "internal class";
%nodefault rr_memcpy;
%inline
{
class rr_memcpy
{
public:
static void memcpy(void* destination, void* source, int64_t num)
{
	::memcpy(destination, source, boost::numeric_cast<size_t>(num));
}
};
}

%include "Transport.i"
%include "TcpTransportNET.i"
%include "LocalTransportNET.i"
%include "HardwareTransportNET.i"
%include "IntraTransport.i"

%include "TimerNET.i"

%include "TimeSpecNET.i"

%include "TypedPacket.i"

%include "PipeMember.i"
%include "WireMember.i"
%include "MemoryMember.i"
%include "Generator.i"


%include "Client.i"
%include "ServiceNET.i"
%include "ServiceSecurityNET.i"

%include "DiscoveryNET.i"
%include "Subscription.i"

%include "LoggingNET.i"

%include "NodeDirectories.i"

%include "RobotRaconteurNodeNET.i"
%include "NodeSetupNET.i"

%include "BroadcastDownsamplerNET.i"

namespace RobotRaconteur {

%feature("director") RRNativeObjectHeapSupport;

class RRNativeObjectHeapSupport
	{
	
	public:
		
		static void Set_Support(RRNativeObjectHeapSupport* s);
		virtual void DeleteObject_i(int32_t id)=0;
		static void DeleteObject(int32_t id);
		virtual ~RRNativeObjectHeapSupport();

	};

};


