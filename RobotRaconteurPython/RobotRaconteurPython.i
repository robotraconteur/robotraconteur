%module(directors="1") RobotRaconteurPython

#pragma SWIG nowarn=473

%include "CommonInclude.i"

%include "pyabc.i"
%{
#include "PythonTypeSupport.h"
%}

%define RR_PUBLIC_OVERRIDE_METHOD(a)
%enddef
%define RR_PUBLIC_VIRTUAL_METHOD(a)
%enddef
%define RR_PROPERTY(a) 
%rename(_Get ## a) Get ## a; %rename(_Set ## a) Set ## a;
%enddef
%define RR_MAKE_METHOD_PRIVATE(a) 
%rename(_ ## a) a;
%enddef

%define RR_RELEASE_GIL() 
RR_Py_Exception_GIL()
%enddef
%define RR_KEEP_GIL() 
RR_Py_Exception()
%enddef

%define RR_DIRECTOR_SHARED_PTR_RETURN_DEFAULT(TYPE)
%enddef

%define RR_DIRECTOR_SHARED_PTR_RETURN_RESET(TYPE)
%enddef

%init
{

PyEval_InitThreads();

RobotRaconteur::PythonTypeSupport_Init();

RobotRaconteur::RobotRaconteurNode::s()->SetDynamicServiceFactory(RR_MAKE_SHARED<RobotRaconteur::WrappedDynamicServiceFactory>());
RobotRaconteur::RobotRaconteurNode::s()->SetThreadPoolFactory(RR_MAKE_SHARED<RobotRaconteur::PythonThreadPoolFactory>());
//RobotRaconteur::RobotRaconteurNode::s()->UnregisterServiceType("RobotRaconteurServiceIndex");
//RobotRaconteur::RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<RobotRaconteur::WrappedServiceFactory>(RR_MAKE_SHARED<RobotRaconteurServiceIndex::RobotRaconteurServiceIndexFactory>()->DefString()));
}



%pythonbegin %{
	from __future__ import absolute_import
	from . import classproperty
%}

%include "rr_intrusive_ptr.i"
%define %rr_intrusive_ptr( TYPE )
%intrusive_ptr( TYPE )
%enddef
%include "PythonTypemaps.i"
%include "PythonExceptionTypemaps.i"
%include "RobotRaconteurConstants.i"
%include "DataTypes.i"

%include "NodeIDPython.i"

%include "MessagePython.i" 

%include "AsyncHandlerDirector.i"
%include "ServiceDefinitionPython.i"

%include "Transport.i"
%include "TcpTransportPython.i"
%include "LocalTransportPython.i"
%include "HardwareTransportPython.i"

%include "TimerPython.i"

%include "TimeSpecPython.i"

%include "PipeMember.i"
%include "WireMember.i"
%include "MemoryMember.i"
%include "Generator.i"


%include "ClientPython.i"

%include "ServicePython.i"
%include "ServiceSecurityPython.i"

%include "DiscoveryPython.i"
%include "Subscription.i"

%include "RobotRaconteurNodePython.i"

%include "PythonTypeSupport.i"

%include "NodeSetup.i"

namespace RobotRaconteur
{
class RRNativeDirectorSupport
{	
public:
	static void Start();		
	static void Stop();		
	static bool IsRunning();	
};

}

%pythoncode %{

import datetime
try:
	import numpy
except:
	pass

import weakref

import atexit
@atexit.register
def RobotRaconteur_cleanup():
    RobotRaconteurNode.s.Shutdown()
    RRNativeDirectorSupport.Stop()
    
RRNativeDirectorSupport.Start()
%}
