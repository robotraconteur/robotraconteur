%module(directors="1") RobotRaconteurJava

#pragma SWIG nowarn=473

%{
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-function"
#endif    

%}


%rename("%(firstlowercase)s", %$isfunction) "";
//%rename("$ignore",regextarget=1,fullname=1) "std::$";

%include "CommonInclude.i"

%include "typemaps.i"
%include "arrays_java.i"

%include "boost_intrusive_ptr.i"

#if defined(SWIGWORDSIZE64)
%apply unsigned int { size_t };
%apply const unsigned int & { const size_t & };
#endif

%define RR_PUBLIC_OVERRIDE_METHOD(a) 
%javamethodmodifiers a "public";
%enddef
%define RR_PUBLIC_VIRTUAL_METHOD(a)
%javamethodmodifiers a "public";
%enddef
%define RR_PROPERTY(a)
%enddef
%define RR_MAKE_METHOD_PRIVATE(a)
%javamethodmodifiers a "private"; %javamethodmodifiers _ ## a "private"; %rename(_ ## a) a;
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

%include "enums.swg"

%javaconst(1);

%rename("%(regex:/^(RobotRaconteur_LogLevel)_(.*)/LogLevel_\\2/)s", %$isenumitem) "";
%rename("%(regex:/^(RobotRaconteur_LogComponent)_(.*)/LogComponent_\\2/)s", %$isenumitem) "";
%include "RobotRaconteurConstants.i"

%include "JavaTypemaps.i"
%include "JavaExceptionTypemaps.i"

//%rename ToString toString;

%rename("add", match$name="push_back") ""; 

%include "DataTypes.i"


//Native loader

%pragma(java) jniclasscode=%{
	static class NativeLoader {
		public NativeLoader()
		{
			try
			{
				RobotRaconteurNativeLoader.load();
			}
			catch(Exception e) {}
		}
	}
	
	static NativeLoader myLoader=new NativeLoader();
%}


%shared_ptr(RobotRaconteur::RobotRaconteurNode)

%include "ServiceDefinitionJava.i"
%include "NodeIDJava.i"

#define RR_MESSAEGE_ELEMENT_DATA_UTIL_EXTRAS
%include "MessageJava.i"

%include "AsyncHandlerDirector.i"

%include "Transport.i"
%include "TcpTransportJava.i"
%include "LocalTransport.i"
%include "HardwareTransportJava.i"
%include "IntraTransport.i"

%include "TimerJava.i"
%include "TimeSpecJava.i"

%include "TypedPacket.i"

%include "PipeMember.i"
%include "WireMember.i"
%include "MemoryMember.i"
%include "Generator.i"

%include "Client.i"
%include "ServiceJava.i"
%include "ServiceSecurityJava.i"

%include "DiscoveryJava.i"
%include "Subscription.i"

%include "Logging.i"

%include "NodeSetupJava.i"

%include "BroadcastDownsamplerJava.i"

%inline
{

class RRMonitor
{
boost::recursive_timed_mutex mlock;
public:
	void lock()
	{
		mlock.lock();
	}
	
	bool tryLock(int timeout)
	{
		if (timeout==-1)
		{
			lock();
			return true;
		}
		return mlock.timed_lock(boost::posix_time::milliseconds(timeout));
	}
	
	void unlock()
	{
	mlock.unlock();
	}

};


}

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

	
}

%include "NodeDirectories.i";

%include "RobotRaconteurJavaNode.i";


//%include "Android.i"
