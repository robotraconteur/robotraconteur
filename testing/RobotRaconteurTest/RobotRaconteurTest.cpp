#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

// RobotRaconteurTest.cpp : Defines the entry point for the console application.
//


#include <RobotRaconteur.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "MessageSerializationTest.h"
#include "MessageSerializationTest3.h"

#include "AsyncMessageTest.h"

#include <RobotRaconteur.h>
/*#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"
#include "com__robotraconteur__testing__TestService2_stubskel.h"
#include "com__robotraconteur__testing__TestService3_stubskel.h"*/
#include "robotraconteur_generated.h"
#include "ServiceTestClient.h"
#include "MultiDimArrayTest.h"
#include "ServiceTest.h"
#include "ServiceTestClient.h"
#include "ServiceTest2.h"
#include "ServiceTestClient2.h"

#include "boost/foreach.hpp"

#ifndef RR_NO_BACKTRACE
#ifndef _WIN32
#include <execinfo.h>
#include <signal.h>
#endif
#endif

#ifdef _WIN32
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#endif


using namespace RobotRaconteur;
using namespace RobotRaconteurTest;
using namespace std;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace com::robotraconteur::testing::TestService3;


std::string ReadFile(const std::string& fname)
{
	ifstream file(fname.c_str(), ios_base::in);

	if (!file.is_open()) throw std::runtime_error("File not found");

	char bom1, bom2, bom3;
	file >> bom1 >> bom2 >> bom3;
	if (!(bom1 == -17 && bom2 == -69 && bom3 == -65))
	{
		file.seekg(0, ifstream::beg);
	}

	stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();

}


#ifndef _WIN32
void handler(int sig) {
  void *array[10];
  size_t size;
#ifndef RR_NO_BACKTRACE
  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
#endif
  exit(1);
}

#endif

void myterminate()
{
	while (true)
	{
	cout << "Terminate!" << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
}

void myhandler(const std::exception* e)
{

	if (!e) return;
	
	do
	{
	cout << "Handled Exception (new): " << e->what() << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(1));

    } while (std::string(e->what()) == "Not running");
}

uint64_t servicetest_connectcount;
bool servicetest_keepgoing;
uint64_t servicetest_count;

boost::mutex servicetest_lock;

void servicetest7(RR_SHARED_PTR<PipeEndpoint<double> > p)
{
	boost::mutex::scoped_lock lock(servicetest_lock);
	while (p->Available() > 0)
	{
		p->ReceivePacket();
	}
}

void servicetest6(uint32_t pnum, RR_SHARED_PTR<RobotRaconteurException> e)
{

}

void servicetest5(RR_SHARED_PTR<PipeEndpoint<double> > p, RR_SHARED_PTR<WireConnection<double> > w, const TimerEvent& ev)
{
	if (ev.stopped) return;
	p->AsyncSendPacket((double)servicetest_count,&servicetest6);

	for (size_t i=0; i<100; i++)
	{
		double d=(double)servicetest_count*100+i;
		w->SetOutValue(d);
	}

	servicetest_count++;


}

void servicetest3(std::string url1, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> exp);


void servicetest4(std::string url1)
{
	RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"",boost::bind(&servicetest3,url1,_1,_2));
}

void servicetest3(std::string url1, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> exp)
{
	if (exp)
	{
		cout << "Got exception" << endl;
		if (servicetest_keepgoing) std::terminate();
		return;
	}

	RobotRaconteurNode::s()->AsyncDisconnectService(obj,boost::bind(&servicetest4,url1));
}

void servicetest2(RR_SHARED_PTR<async_testroot> o, double d, RR_SHARED_PTR<RobotRaconteurException> exp)
{
	if (exp)
	{
		cout << "Got exception" << endl;
		if (servicetest_keepgoing) std::terminate();
		return;
	}

	o->async_func3(1,2,boost::bind(&servicetest2,o,_1,_2));
}

void servicetest1(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> exp)
{
	if (exp)
	{
		cout << "Got exception" << endl;
		if (servicetest_keepgoing) std::terminate();
		return;
	}

	try
	{
	RR_SHARED_PTR<async_testroot> o=rr_cast<async_testroot>(obj);
	o->async_func3(1,2,boost::bind(&servicetest2,o,_1,_2));
	}
	catch (std::exception&)
	{
		if (!servicetest_keepgoing) std::terminate();
	}
}

int latencyitercount=0;
RR_SHARED_PTR<AutoResetEvent> latency_event;

void latencytest1(RR_SHARED_PTR<async_testroot> obj, RR_SHARED_PTR<RobotRaconteurException> exp);

void latencytest1(RR_SHARED_PTR<async_testroot> obj, RR_SHARED_PTR<RobotRaconteurException> exp)
{
	if (exp)
	{
		cout << "Got exception" << endl;
		if (servicetest_keepgoing) std::terminate();
		return;
	}

	try
	{
	
	obj->async_func3(1,2,boost::bind(&latencytest1,obj,_2));
	if (latencyitercount>100000)
	{
		latency_event->Set();
		return;
	}
	latencyitercount++;
	}
	catch (std::exception&)
	{
		if (!servicetest_keepgoing) std::terminate();
	}
}

void servicesubscription_connected(RR_SHARED_PTR<ServiceSubscription> sub, const ServiceSubscriptionClientID& noden, RR_SHARED_PTR<RRObject> obj)
{
	cout << "Subscription Connected: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void servicesubscription_disconnected(RR_SHARED_PTR<ServiceSubscription> sub, const ServiceSubscriptionClientID& noden, RR_SHARED_PTR<RRObject> obj)
{
	cout << "Subscription Disconnected: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void serviceinfo2subscription_detected(RR_SHARED_PTR<ServiceInfo2Subscription> sub, const ServiceSubscriptionClientID& noden, const ServiceInfo2& info)
{
	cout << "ServiceInfo2 detected: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void serviceinfo2subscription_lost(RR_SHARED_PTR<ServiceInfo2Subscription> sub, const ServiceSubscriptionClientID& noden, const ServiceInfo2& info)
{
	cout << "ServiceInfo2 lost: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void subscribertest_waitwire(RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > w1)
{
	w1->WaitInValueValid();
	w1->WaitInValueValid();
}

void subscribertest_wirechanged(RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > w1, RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > val, const TimeSpec& time)
{
	//cout << "Wire value: " << RRArrayToScalar(val) << endl;	
}

void subscribertest_pipereceived(RR_SHARED_PTR<PipeSubscription<double > > p1)
{
	static int c = 0;

	double val;
	while (p1->TryReceivePacket(val))
	{
		c++;
		if (c > 10)
		{
			cout << "Pipe value: " << val << endl;
			c = 0;
		}
	}
}

bool subscribertest_filter_predicate(const ServiceInfo2& node)
{
	if (node.NodeName == "testprog")
	{
		return true;
	}

	return false;
}

int main(int argc, char* argv[])
{

#ifndef _WIN32
	//signal(SIGSEGV,handler);
#endif

#ifdef _WIN32
	HANDLE process = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 1 << 0;

   // BOOL success = SetProcessAffinityMask(process, processAffinityMask);


#ifdef _DEBUG
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetBreakAlloc(15427);
#endif
#endif

	/*NodeID id=NodeID::NewUniqueID();
	string sid=id.ToString();
	cout << sid << endl;
	NodeID id2=NodeID(sid);
	cout << id2.ToString() << endl;
	
	

	string dat;
	cin > > dat;*/

	//char* dat=new char[1000];
	//dat=0;
	
	string command;
	if (argc > 1) 
		command=string(argv[1]);
	else
		command="loopback";
	

	if (command == "messagetest")
	{
		MessageSerializationTest::Test();
		MessageSerializationTest3::Test();
		std::cout << "Done!" << std::endl;
		return 0;
	}

	if (command == "messagerandomtest")
	{
		size_t n = 100;
		if (argc > 2)
		{
			n = boost::lexical_cast<size_t>(string(argv[2]));
		}
		MessageSerializationTest::RandomTest(n);
		MessageSerializationTest3::RandomTest(n);
		std::cout << "Done!" << std::endl;
		return 0;
	}

	if (command == "asyncmessagerandomtest")
	{
		size_t n = 100;
		if (argc > 2)
		{
			n = boost::lexical_cast<size_t>(string(argv[2]));
		}
		AsyncMessageReaderTest::RandomTest(n);
		AsyncMessageReaderTest::RandomTest3(n);
		AsyncMessageWriterTest::RandomTest(n);
		AsyncMessageWriterTest::RandomTest3(n);
		std::cout << "Done!" << std::endl;
		return 0;
	}

	if (command=="loopback")
	{

		/*RR_SHARED_PTR<SuperNodeTransport> t=RR_MAKE_SHARED<SuperNodeTransport>();
		t->ConnectSuperNode();

		getchar();

		RobotRaconteurNode::s()->Shutdown();

		return 0;*/

		//std::set_terminate(myterminate);

		RobotRaconteurNode::s()->SetExceptionHandler(myhandler);
		
		RR_SHARED_PTR<TcpTransport> c=RR_MAKE_SHARED<TcpTransport>();
		c->StartServer(4565);
		
		//c->EnableNodeAnnounce();
		//c->EnableNodeDiscoveryListening();

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		RobotRaconteurTestServiceSupport s;
		s.RegisterServices(c);




		/*boost::this_thread::sleep(boost::posix_time::seconds(10));
		std::vector<std::string> tp;
		tp.push_back("tcp");
		std::vector<ServiceInfo2> sv=RobotRaconteurNode::s()->FindServiceByType("RobotRaconteurTestService.testroot",tp);

		std::vector<NodeInfo2> n1=RobotRaconteurNode::s()->FindNodeByID(NodeID("{f09b9b80-d71d-11e3-9c1a-0800200c9a66}"),tp);
		std::vector<NodeInfo2> n2=RobotRaconteurNode::s()->FindNodeByName("testprog",tp);*/

		//for (int i=0; i<5; i++)

		//_CrtMemState s1;
		//_CrtMemState s2;
		

		{
		ServiceTestClient cl;
		cl.RunFullTest("tcp://localhost:4565/{0}/RobotRaconteurTestService", "rr+tcp://localhost:4565/?service=RobotRaconteurTestService_auth");
		cl.RunFullTest("rr+ws://localhost:4565/?service=RobotRaconteurTestService", "rr+ws://localhost:4565/?service=RobotRaconteurTestService_auth");
		
		}
		
		//_CrtMemCheckpoint(&s1);

		int count = 1;

		if (argc >= 3)
		{
			std::string scount(argv[2]);
			count = boost::lexical_cast<int>(scount);
		}
		
		for (int j=0; j<count; j++)
		{
			ServiceTestClient cl;
			cl.RunFullTest("rr+tcp://localhost:4565?service=RobotRaconteurTestService", "rr+tcp://localhost:4565?service=RobotRaconteurTestService_auth");								
		}

			
		

		//boost::this_thread::sleep(boost::posix_time::minutes(5));

		

		/*_CrtMemState s3;
		_CrtMemCheckpoint(&s2);
		_CrtMemDifference(&s3,&s1,&s2);

		_CrtMemDumpStatistics(&s3);*/
#ifdef _DEBUG
		//cout << "Diff: " << (int)s3.lSizes[1] << " " << (int)s3.lSizes[2] << endl;
#endif
		/*HANDLE h=::GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS_EX p;
		::GetProcessMemoryInfo(h,(PPROCESS_MEMORY_COUNTERS)&p,sizeof(p));
		std::cout << "Mem: " << p.PrivateUsage << endl;

		if (p.WorkingSetSize > 60000000)
		{
			cout << "leak" << endl;
		}*/

		//boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

		

		//RobotRaconteurNode::s()->GetThreadPool()->get_io_service().stop();

		//boost::this_thread::sleep(boost::posix_time::milliseconds(30000));
		
		cout << "start shutdown" << endl;



		RobotRaconteurNode::s()->Shutdown();
		//cout << "end shutdown" << endl;

		//cout << "Test completed, no errors detected!" << endl;

		//boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
		cout << "Test completed, no errors detected!" << endl;

return 0;
		while (true)
		{
			std::vector<std::string> t;
			t.push_back("tcp");
			std::vector<ServiceInfo2> s= RobotRaconteurNode::s()->FindServiceByType("RobotRaconteurTestService.testroot",t);

			for (std::vector<ServiceInfo2>::const_iterator e=s.begin(); e!=s.end(); ++e)
			{
				cout << e->NodeID.ToString() << ", " << e->NodeName << endl;

				for (std::vector<std::string>::const_iterator ee=e->ConnectionURL.begin(); ee!=e->ConnectionURL.end(); ++ee)
				{
					cout << "	" << *ee << endl;
				}
				cout << endl;
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

		}



		getchar();

		return 0;

	}

	if (command == "loopback2")
	{
		RobotRaconteurNode::s()->SetExceptionHandler(myhandler);

		/*RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
		c->StartServer(4565);
		//c->SetDisableAsyncMessageIO(true);		
		//c->SetDisableMessage3(true);

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService3Factory>());
		*/
		{			
			ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "com.robotraconteur.testing.TestService2", 4565, RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER);
			RobotRaconteurTestService2Support s;
			s.RegisterServices(node_setup.GetTcpTransport());

			ServiceTestClient2 c2;
			c2.RunFullTest("rr+tcp://localhost:4565/?service=RobotRaconteurTestService2");

		}
		//RobotRaconteurNode::s()->Shutdown();

		cout << "Test completed, no errors detected!" << endl;

		return 0;
	}

	if (command=="client")
	{
		if (argc <4) 
		{
			cout << "Usage for client:  RobotRaconteurTest client url url_auth" << endl;
			return -1;
		}

		string url1(argv[2]);
		string url2(argv[3]);

		RR_SHARED_PTR<TcpTransport> c=RR_MAKE_SHARED<TcpTransport>();
		//c->EnableNodeAnnounce();
		c->EnableNodeDiscoveryListening();
		c->SetDefaultConnectTimeout(100000000);
		RobotRaconteurNode::s()->SetRequestTimeout(100000000);

		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		
		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterTransport(c2);		
		RobotRaconteurNode::s()->RegisterTransport(c5);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		cout << RobotRaconteurNode::s()->NodeID().ToString() << endl;

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		int count = 1;

		if (argc >= 5)
		{
			std::string scount(argv[4]);
			count = boost::lexical_cast<int>(scount);
		}

		for (int j = 0; j < count; j++)
		{
		ServiceTestClient cl;
		cl.RunFullTest(url1, url2);
		}
		
		RobotRaconteurNode::s()->Shutdown();
	
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		cout << "Test completed, no errors detected!" << endl;
		return 0;

	}

	if (command == "client2")
	{
		if (argc <3)
		{
			cout << "Usage for client2:  RobotRaconteurTest client2 url" << endl;
			return -1;
		}

		string url1(argv[2]);
		
		RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
		//c->EnableNodeAnnounce();
		c->EnableNodeDiscoveryListening();
		
		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterTransport(c2);
		RobotRaconteurNode::s()->RegisterTransport(c5);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService3Factory>());

		cout << RobotRaconteurNode::s()->NodeID().ToString() << endl;

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		int count = 1;

		if (argc >= 4)
		{
			std::string scount(argv[3]);
			count = boost::lexical_cast<int>(scount);
		}

		for (int j = 0; j < count; j++)
		{
			ServiceTestClient2 cl;
			cl.RunFullTest(url1);
		}

		RobotRaconteurNode::s()->Shutdown();

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		cout << "Test completed, no errors detected!" << endl;
		return 0;

	}

	if (command == "minimalclient")
	{
		if (argc <3)
		{
			cout << "Usage for minimalclient:  RobotRaconteurTest minimalclient url" << endl;
			return -1;
		}

		string url1(argv[2]);
		
		RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
		//c->EnableNodeAnnounce();
		//c->EnableNodeDiscoveryListening();
		c->SetDefaultConnectTimeout(100000000);
		RobotRaconteurNode::s()->SetRequestTimeout(100000000);

		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
		
		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterTransport(c2);		
		RobotRaconteurNode::s()->RegisterTransport(c5);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		cout << RobotRaconteurNode::s()->NodeID().ToString() << endl;

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		int count = 1;

		if (argc >= 4)
		{
			std::string scount(argv[3]);
			count = boost::lexical_cast<int>(scount);
		}

		for (int j = 0; j < count; j++)
		{
			ServiceTestClient cl;
			cl.RunMinimalTest(url1);
		}

		RobotRaconteurNode::s()->Shutdown();

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		cout << "Test completed, no errors detected!" << endl;
		return 0;

	}

	if (command=="server")
	{
		if (argc < 4)
		{
			cout << "Usage for server:  RobotRaconteurTest server port nodename" << endl;
			return -1;
		}

		int port;
		if (string(argv[2]) == "sharer")
		{
			port = -1;
		}
		else
		{
			port = boost::lexical_cast<int>(string(argv[2]));
		}
		//NodeID id=NodeID(string(argv[3]));
		string nodename(argv[3]);

		bool local_public = false;
		if (argc >= 5)
		{
			if (string(argv[4]) == "public")
			{
				local_public = true;
			}
		}

		//RobotRaconteurNode::s()->SetNodeID(id);
		//RobotRaconteurNode::s()->SetNodeName(nodename);

		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		c2->StartServerAsNodeName(nodename, local_public);

		RR_SHARED_PTR<TcpTransport> c=RR_MAKE_SHARED<TcpTransport>();
		
		try
		{
			c->LoadTlsNodeCertificate();
		}
		catch (std::exception&)
		{
			std::cout << "warning: Could not load node certificate" << std::endl;
		}
		if (port > 0)
		{
			c->StartServer(port);
		}
		else
		{
			c->StartServerUsingPortSharer();
		}

		c->EnableNodeAnnounce();
		c->EnableNodeDiscoveryListening();		

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterTransport(c2);		
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService3Factory>());

		RobotRaconteurTestServiceSupport s;
		s.RegisterServices(c);

		RobotRaconteurTestService2Support s2;
		s2.RegisterServices(c);

		cout << "Server started, press enter to quit" << endl;
		getchar();
		RobotRaconteurNode::s()->Shutdown();
		cout << "Test completed, no errors detected!" << endl;
		return 0;

	}

	if (command=="findservicebytype")
	{
		if (argc < 4)
		{
			cout << "Usage for findservicebytype:  RobotRaconteurTest findservicebytype servicetype schemes" << endl;
			return -1;
		}

		string servicetype=string(argv[2]);
		vector<string> schemes;
		boost::split(schemes,argv[3],boost::is_from_range(',',','));

		RR_SHARED_PTR<TcpTransport> t=RR_MAKE_SHARED<TcpTransport>();
		t->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(t);
		
		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();		
		RobotRaconteurNode::s()->RegisterTransport(c5);

		boost::this_thread::sleep(boost::posix_time::seconds(6));

		vector<ServiceInfo2> r=RobotRaconteurNode::s()->FindServiceByType(servicetype,schemes);

		for(vector<ServiceInfo2>::iterator e=r.begin(); e!=r.end(); e++)
		{
			cout << "Name: " << e->Name << endl;
			cout << "RootObjectType: " << e->RootObjectType << endl;
			cout << "RootObjectImplements: " << boost::join(e->RootObjectImplements, ", ") << endl;
			cout << "ConnectionURL: " << boost::join(e->ConnectionURL,", ") << endl;

			vector<string> a;
			for (std::map<std::string,RR_INTRUSIVE_PTR<RRValue> >::iterator ee=e->Attributes.begin(); ee!=e->Attributes.end(); ee++)
			{
				RR_INTRUSIVE_PTR<RRArray<char> > d=RR_DYNAMIC_POINTER_CAST<RRArray<char> >(ee->second);
				if (d)
				{
					a.push_back(ee->first + "=" + RRArrayToString(d));
				}

				RR_INTRUSIVE_PTR<RRArray<int32_t> > d2=RR_DYNAMIC_POINTER_CAST<RRArray<int32_t> >(ee->second);
				if (d2)
				{
					if (d2->size()>0)
					{
						a.push_back(ee->first + "=" + boost::lexical_cast<string>((*d2)[0]));
					}
				}
			}

			cout << "Attributes: " + boost::join(a,", ") << endl;
			cout << "NodeID: " << e->NodeID.ToString() << endl;
			cout << "NodeName: " << e->NodeName << endl;
			cout << endl;
		}

		RobotRaconteurNode::s()->Shutdown();

		return 0;
	}

	if (command=="findnodebyid")
	{
		if (argc < 4)
		{
			cout << "Usage for findnodebyid:  RobotRaconteurTest findnodebyid id schemes" << endl;
			return -1;
		}

		NodeID id=NodeID(string(argv[2]));
		vector<string> schemes;
		boost::split(schemes,argv[3],boost::is_from_range(',',','));

		RR_SHARED_PTR<TcpTransport> t=RR_MAKE_SHARED<TcpTransport>();
		t->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(t);

		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();		
		RobotRaconteurNode::s()->RegisterTransport(c5);

		boost::this_thread::sleep(boost::posix_time::seconds(6));

		vector<NodeInfo2> r=RobotRaconteurNode::s()->FindNodeByID(id,schemes);

		for(vector<NodeInfo2>::iterator e=r.begin(); e!=r.end(); e++)
		{
			cout << "NodeID: " << e->NodeID.ToString() << endl;
			cout << "NodeName: " << e->NodeName << endl;
			cout << "ConnectionURL: " << boost::join(e->ConnectionURL,", ") << endl;
			cout << endl;
		}

		RobotRaconteurNode::s()->Shutdown();

		return 0;
	}

	if (command=="findnodebyname")
	{
		if (argc < 4)
		{
			cout << "Usage for findnodebyname:  RobotRaconteurTest findnodebyname name schemes" << endl;
			return -1;
		}

		string name(argv[2]);
		vector<string> schemes;
		boost::split(schemes,argv[3],boost::is_from_range(',',','));

		RR_SHARED_PTR<TcpTransport> t=RR_MAKE_SHARED<TcpTransport>();
		t->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(t);

		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c5);

		boost::this_thread::sleep(boost::posix_time::seconds(6));

		vector<NodeInfo2> r=RobotRaconteurNode::s()->FindNodeByName(name,schemes);

		for(vector<NodeInfo2>::iterator e=r.begin(); e!=r.end(); e++)
		{
			cout << "NodeID: " << e->NodeID.ToString() << endl;
			cout << "NodeName: " << e->NodeName << endl;
			cout << "ConnectionURL: " << boost::join(e->ConnectionURL,", ") << endl;
			cout << endl;
		}

		RobotRaconteurNode::s()->Shutdown();

		return 0;
	}

	if (command=="stresstestclient")
	{
		if (argc <3) 
		{
			cout << "Usage for client:  RobotRaconteurTest stresstestclient url" << endl;
			return -1;
		}

		string url1(argv[2]);
		//string url2(argv[3]);

		RR_SHARED_PTR<TcpTransport> c=RR_MAKE_SHARED<TcpTransport>();
		c->EnableNodeAnnounce(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL);
		c->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL);

		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c3 = RR_MAKE_SHARED<HardwareTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c3);

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		servicetest_count=0;
		servicetest_connectcount=0;
		servicetest_keepgoing=true;

		//RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"",&servicetest1);
		//RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"",boost::bind(&servicetest3,url1,_1,_2));

		RR_SHARED_PTR<RRObject> obj=RobotRaconteurNode::s()->ConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"");
		RR_SHARED_PTR<async_testroot> o=rr_cast<async_testroot>(obj);
		RR_SHARED_PTR<testroot> o2=rr_cast<testroot>(obj);
		o->async_func3(1,2,boost::bind(&servicetest2,o,_1,_2));
		
		RR_SHARED_PTR<PipeEndpoint<double> > p=o2->get_broadcastpipe()->Connect(-1);
		p->PacketReceivedEvent.connect(&servicetest7);
		RR_SHARED_PTR<WireConnection<double> >w=o2->get_broadcastwire()->Connect();

		RR_SHARED_PTR<Timer> t=RobotRaconteurNode::s()->CreateTimer(boost::posix_time::milliseconds(40),boost::bind(&servicetest5,p,w,_1));
		t->Start();

		cout << "Press enter to quit" << endl;
		boost::this_thread::sleep(boost::posix_time::seconds(120));
		servicetest_keepgoing=false;
		t->Stop();
		RobotRaconteurNode::s()->DisconnectService(obj);
		RobotRaconteurNode::s()->Shutdown();
		cout << "Test completed, no errors detected!" << endl;
		boost::this_thread::sleep(boost::posix_time::seconds(10));
		
		return 0;

	}

	if (command=="latencytestclient")
	{
		if (argc <3) 
		{
			cout << "Usage for client:  RobotRaconteurTest stresstestclient url url_auth" << endl;
			return -1;
		}

		string url1(argv[2]);
		//string url2(argv[3]);

		RR_SHARED_PTR<TcpTransport> c=RR_MAKE_SHARED<TcpTransport>();
		c->EnableNodeAnnounce(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL);
		c->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL);

		RR_SHARED_PTR<LocalTransport> c2=RR_MAKE_SHARED<LocalTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		servicetest_count=0;
		servicetest_connectcount=0;
		servicetest_keepgoing=true;

		//RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"",&servicetest1);
		//RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"",boost::bind(&servicetest3,url1,_1,_2));

		RR_SHARED_PTR<RRObject> obj=RobotRaconteurNode::s()->ConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >(),NULL,"");
		RR_SHARED_PTR<async_testroot> o=rr_cast<async_testroot>(obj);
		RR_SHARED_PTR<testroot> o2=rr_cast<testroot>(obj);

		uint32_t iters=100000;

		RR_SHARED_PTR<sub1> o3=o2->get_o1();

		RR_INTRUSIVE_PTR<RRArray<double> > d=AllocateRRArray<double>(10);
		latencyitercount=0;
		latency_event=RR_MAKE_SHARED<AutoResetEvent>();

		boost::posix_time::ptime t1=RobotRaconteurNode::s()->NowUTC();

		//o->async_func3(1,2,boost::bind(&latencytest1,o,_2));

		for (uint32_t i=0; i<iters; i++)
		{
			o3->set_d1(d);
		}
		
		//latency_event->WaitOne();

		boost::posix_time::ptime t2=RobotRaconteurNode::s()->NowUTC();

		uint32_t diff=(uint32_t)(t2-t1).total_microseconds();
		
		double period=((double)diff) / ((double)iters);

		cout << "Period=" << period << endl;

		RobotRaconteurNode::s()->DisconnectService(obj);
		RobotRaconteurNode::s()->Shutdown();
		cout << "Test completed, no errors detected!" << endl;
		boost::this_thread::sleep(boost::posix_time::seconds(10));
		
		return 0;

	}

	if (command == "peeridentity")
	{
		if (argc <3)
		{
			cout << "Usage for peeridentity:  RobotRaconteurTest peeridentity url [nodeid]" << endl;
			return -1;
		}

		string url1(argv[2]);

		RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();

		if (argc > 3)
		{
			std::string nodeid1 = (argv[3]);
			NodeID id(nodeid1);

			RobotRaconteurNode::s()->SetNodeID(nodeid1);

			try
			{
				c->LoadTlsNodeCertificate();
			}
			catch (std::exception&)
			{
				std::cout << "warning: Could not load node certificate" << std::endl;
			}

		}
						
		c->EnableNodeAnnounce();
		c->EnableNodeDiscoveryListening();
		c->SetDefaultConnectTimeout(100000000);
		RobotRaconteurNode::s()->SetRequestTimeout(100000000);

		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
		
		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterTransport(c2);		
		RobotRaconteurNode::s()->RegisterTransport(c5);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		cout << RobotRaconteurNode::s()->NodeID().ToString() << endl;

		RR_SHARED_PTR<RRObject> o = RobotRaconteurNode::s()->ConnectService(url1);

		RR_SHARED_PTR<testroot> oo = rr_cast<testroot>(o);

		oo->func3(1.0, 2.3);

		//RR_SHARED_PTR<Endpoint> ep = rr_cast<ServiceStub>(o)->GetContext();
		if (c->IsTransportConnectionSecure(o))
		{
			std::cout << "Connection is secure" << std::endl;
			if (c->IsSecurePeerIdentityVerified(o))
			{
				std::cout << "Peer identity is verified: " << c->GetSecurePeerIdentity(o) << std::endl;

			}
			else
			{
				std::cout << "Peer identity is not verified" << std::endl;
			}


		}
		else
		{
			std::cout << "Connection is not secure" << std::endl;

		}

		RobotRaconteurNode::s()->Shutdown();

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		cout << "Test completed, no errors detected!" << endl;
		return 0;

	}

	if (command == "idleclient")
	{
		if (argc <3)
		{
			cout << "Usage for client:  RobotRaconteurTest idleclient url (seconds)" << endl;
			return -1;
		}

		

		string url1(argv[2]);
		int32_t seconds = 30;
		if (argc >= 4)
		{
			seconds = boost::lexical_cast<int32_t>(argv[3]);
		}
		//string url2(argv[3]);

		RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
		c->EnableNodeAnnounce(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL);
		c->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL);

		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c2);
		c2->EnableNodeDiscoveryListening();

		RobotRaconteurNode::s()->RegisterTransport(c);
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());
		
		
		RR_SHARED_PTR<RRObject> obj = RobotRaconteurNode::s()->ConnectService(url1, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), NULL, "");
		RR_SHARED_PTR<testroot> o = rr_cast<testroot>(obj);
		o->get_d1();
				
		boost::this_thread::sleep(boost::posix_time::seconds(seconds));
				
		RobotRaconteurNode::s()->DisconnectService(obj);
		RobotRaconteurNode::s()->Shutdown();
		cout << "Test completed, no errors detected!" << endl;
		
		return 0;

	}

	if (command == "multidimarraytest")
	{
		MultiDimArrayTest::TestDouble();
		MultiDimArrayTest::TestByte();
		return 0;
	}

	if (command == "subscribertest")
	{
		if (argc < 3)
		{
			cout << "Usage for subscribertest:  RobotRaconteurTest subscribertest servicetype" << endl;
			return -1;
		}

		string servicetype = string(argv[2]);
		//vector<string> schemes;
		//boost::split(schemes, argv[3], boost::is_from_range(',', ','));

		RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
		t->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(t);

		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
		c2->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c5);

		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		std::vector<std::string> servicetypes;
		servicetypes.push_back(servicetype);
		RR_SHARED_PTR<ServiceSubscription> subscription = RobotRaconteurNode::s()->SubscribeService(servicetypes);

		

		subscription->AddClientConnectListener(servicesubscription_connected);
		subscription->AddClientDisconnectListener(servicesubscription_disconnected);

		RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > broadcastwire=subscription->SubscribeWire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > >("broadcastwire");
		broadcastwire->AddWireValueChangedListener(subscribertest_wirechanged);

		RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > > > w1 = subscription->SubscribeWire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > >("w1");

		RR_SHARED_PTR<PipeSubscription<double> > broadcastpipe = subscription->SubscribePipe<double>("broadcastpipe", 100);
		broadcastpipe->AddPipePacketReceivedListener(subscribertest_pipereceived);

		boost::thread t1(boost::bind(&subscribertest_waitwire, w1));

		boost::this_thread::sleep(boost::posix_time::seconds(10));

		std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > clients = subscription->GetConnectedClients();
		typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;
		BOOST_FOREACH(e_type& e, clients)
		{
			cout << "Subscribed Node: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
		}

		RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > w1_value;
		TimeSpec w1_time;
		broadcastwire->TryGetInValue(w1_value, &w1_time);

		double pipe_val;
		while (broadcastpipe->TryReceivePacket(pipe_val))
		{
			cout << "Got pipe value: " << pipe_val << endl;
		}

		if (w1_value)
		{
			cout << "broadcastwire: len=" << w1_value->size() << " [0]=" << (*w1_value)[0] << endl;
		}

		cout << "Press enter to quit" << endl;
		
		getchar();

		subscription->Close();

		t1.join();

		RobotRaconteurNode::s()->Shutdown();

		return 0;
	}

	if (command == "subscriberfiltertest")
	{

		if (argc < 3)
		{
			cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest servicetype" << endl;
			return -1;
		}

		string servicetype = string(argv[2]);
		//vector<string> schemes;
		//boost::split(schemes, argv[3], boost::is_from_range(',', ','));

		RR_SHARED_PTR<ServiceSubscriptionFilter> f = RR_MAKE_SHARED<ServiceSubscriptionFilter>();
		
		if (argc >= 4)
		{
			std::string subcommand = string(argv[3]);

			if (subcommand == "nodeid")
			{
				if (argc < 5)
				{
					cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodeid <nodeid>" << endl;
					return -1;
				}

				RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
				n->NodeID = NodeID(std::string(argv[4]));
				f->Nodes.push_back(n);
			}
			else
			if (subcommand == "nodename")
			{
				if (argc < 5)
				{
					cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodename <nodename>" << endl;
					return -1;
				}

				RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
				n->NodeName = std::string(argv[4]);
				f->Nodes.push_back(n);
			}
			else
			if (subcommand == "nodeidscheme")
			{
				if (argc < 6)
				{
					cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodeidscheme <nodeid> <schemes>" << endl;
					return -1;
				}

				RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
				n->NodeID = NodeID(std::string(argv[4]));
				f->Nodes.push_back(n);
				std::vector<std::string> schemes;
				std::string schemes1(argv[5]);
				boost::split(schemes, schemes1, boost::is_any_of(","));
				f->TransportSchemes = schemes;
			}
			else
			if (subcommand == "nodeidauth")
			{
				if (argc < 7)
				{
					cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodeidauth <nodeid> <username> <password>" << endl;
					return -1;
				}

				RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
				n->NodeID = NodeID(std::string(argv[4]));
				n->Username = std::string(argv[5]);
				RR_INTRUSIVE_PTR<RRMap<std::string, RRValue > > cred = AllocateEmptyRRMap<std::string, RRValue >();
				cred->insert(std::make_pair("password", stringToRRArray(argv[6])));
				n->Credentials = cred;
				f->Nodes.push_back(n);
			}
			else
			if (subcommand == "servicename")
			{
				if (argc < 5)
				{
					cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest servicename <servicename>" << endl;
					return -1;
				}

				f->ServiceNames.push_back(argv[4]);
			}
			if (subcommand == "predicate")
			{
				if (argc < 4)
				{
					cout << "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest predicate" << endl;
					return -1;
				}

				f->Predicate = subscribertest_filter_predicate;
			}
			else
			{
				cout << "Unknown subscriberfiltertest command" << endl;
				return -1;
			}

			
		}
		
		RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
		t->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(t);

		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
		c2->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c5);

		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		std::vector<std::string> servicetypes;
		servicetypes.push_back(servicetype);
		RR_SHARED_PTR<ServiceSubscription> subscription = RobotRaconteurNode::s()->SubscribeService(servicetypes, f);
		
		subscription->AddClientConnectListener(servicesubscription_connected);
		subscription->AddClientDisconnectListener(servicesubscription_disconnected);
				
		std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > clients = subscription->GetConnectedClients();
		typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;
		BOOST_FOREACH(e_type& e, clients)
		{
			cout << "Subscribed Node: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
		}

		
		cout << "Press enter to quit" << endl;

		getchar();

		subscription->Close();
		
		RobotRaconteurNode::s()->Shutdown();

		return 0;
	}

	if (command == "serviceinfo2subscribertest")
	{
		if (argc < 3)
		{
			cout << "Usage for serviceinfo2subscribertest:  RobotRaconteurTest serviceinfo2subscribertest servicetype" << endl;
			return -1;
		}

		string servicetype = string(argv[2]);
		//vector<string> schemes;
		//boost::split(schemes, argv[3], boost::is_from_range(',', ','));

		RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
		t->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(t);

		RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
		c2->EnableNodeDiscoveryListening();
		RobotRaconteurNode::s()->RegisterTransport(c2);

		RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
		RobotRaconteurNode::s()->RegisterTransport(c5);

		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
		RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

		std::vector<std::string> servicetypes;
		servicetypes.push_back(servicetype);
		RR_SHARED_PTR<ServiceInfo2Subscription> subscription = RobotRaconteurNode::s()->SubscribeServiceInfo2(servicetypes);



		subscription->AddServiceDetectedListener(serviceinfo2subscription_detected);
		subscription->AddServiceLostListener(serviceinfo2subscription_lost);

		
		boost::this_thread::sleep(boost::posix_time::seconds(10));

		std::map<ServiceSubscriptionClientID, ServiceInfo2> clients = subscription->GetDetectedServiceInfo2();
		typedef std::map<ServiceSubscriptionClientID, ServiceInfo2 >::value_type e_type;
		BOOST_FOREACH(e_type& e, clients)
		{
			cout << "Service Detected: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
		}

		cout << "Press enter to quit" << endl;

		getchar();

		subscription->Close();
		
		RobotRaconteurNode::s()->Shutdown();

		return 0;
	}

	if (command == "messageperftest")
	{
		if (argc < 3)
		{
			cout << "Usage for messageperftest:  RobotRaconteurTest messageperftest iters" << endl;
			return -1;
		}

		boost::shared_array<uint8_t> buf(new uint8_t[10000]);

		size_t iters = boost::lexical_cast<size_t>(argv[2]);

		boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::universal_time();

		for (size_t j = 0; j < iters; j++)
		{
			std::list<RR_INTRUSIVE_PTR<RRValue> > o;
			for (size_t i = 0; i < 100; i++)
			{
				RR_INTRUSIVE_PTR<RRArray<double> > a = ScalarToRRArray((double)i);
				RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > a1 = AllocateEmptyRRMap<int32_t, RRArray<double> >();
				a1->insert(std::make_pair((int32_t)i, a));
				RR_INTRUSIVE_PTR<MessageElementData> a2=RobotRaconteurNode::s()->PackAnyType<RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > >(a1);

				RR_INTRUSIVE_PTR<MessageElement> a3 = CreateMessageElement("test", a2);
				ArrayBinaryWriter w(buf.get(), 0, 10000);
				a3->Write3(w, 0);

				o.push_back(a1);
			}
		}

		boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::universal_time();

		size_t m = (end_time - start_time).total_milliseconds();

		std::cout << "Iters: " << iters * 100 << " Milliseconds: " << m << std::endl;
		std::cout << "Milliseconds per iter: " << ((double)m) / ((double)iters * 100) << std::endl;

		return 0;
	}

	if (command == "robdeftest")
	{
		std::vector<std::string> robdef_filenames;
		for (size_t i = 2; i < argc; i++)
		{
			robdef_filenames.push_back(argv[i]);
		}

		std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
		std::vector<RR_SHARED_PTR<ServiceDefinition> > defs2;
		BOOST_FOREACH(const std::string& fname, robdef_filenames)
		{
			std::string robdef_text = ReadFile(fname);
			RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
			def->FromString(robdef_text);
			defs.push_back(def);
			std::string robdef_text2 = def->ToString();
			RR_SHARED_PTR<ServiceDefinition> def2 = RR_MAKE_SHARED<ServiceDefinition>();
			def2->FromString(robdef_text2);
			defs2.push_back(def2);
		}

		VerifyServiceDefinitions(defs);

		for (size_t i = 0; i < defs.size(); i++)
		{
			if (!CompareServiceDefinitions(defs[i], defs2[i]))
			{
				throw std::runtime_error("Service definition parse does not match");
			}
		}
		
		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> def, defs)
		{
			cout << def->ToString() << endl;
		}


		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> def, defs)
		{
			BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition> c, def->Constants)
			{
				if (c->Name == "strconst")
				{
					std::string strconst = c->ValueToString();
					std::cout << "strconst " << strconst << std::endl;

					std::string strconst2 = ConstantDefinition::EscapeString(strconst);
					std::string strconst3 = ConstantDefinition::UnescapeString(strconst2);

					if (strconst3 != strconst)
						throw std::runtime_error("");
				}

				if (c->Name == "int32const")
				{
					std::cout << "int32const: " << c->ValueToScalar<int32_t>() << std::endl;
				}

				if (c->Name == "int32const_array")
				{
					RR_INTRUSIVE_PTR<RRArray<int32_t> > a = c->ValueToArray<int32_t>();					
					std::cout << "int32const_array: " << a->size() << std::endl;
				}

				if (c->Name == "doubleconst_array")
				{
					RR_INTRUSIVE_PTR<RRArray<double> > a = c->ValueToArray<double>();
					std::cout << "doubleconst_array: " << a->size() << std::endl;
				}

				if (c->Name == "structconst")
				{
					std::vector<ConstantDefinition_StructField> s = c->ValueToStructFields();
					BOOST_FOREACH(const ConstantDefinition_StructField& f, s)
					{
						std::cout << f.Name << ": " << f.ConstantRefName << " ";
					}

					std::cout << std::endl;
				}

			}
		}

		RR_SHARED_PTR<ServiceDefinition> def1 = TryFindByName(defs, "com.robotraconteur.testing.TestService1");
		if (def1)
		{
			RR_SHARED_PTR<ServiceEntryDefinition> entry = TryFindByName(def1->Objects, "testroot");

			RR_SHARED_PTR<PropertyDefinition> p1 = rr_cast<PropertyDefinition>(TryFindByName(entry->Members, "d1"));
			if (p1->Direction() != MemberDefinition_Direction_both)
				throw std::exception();

			RR_SHARED_PTR<PipeDefinition> p2 = rr_cast<PipeDefinition>(TryFindByName(entry->Members, "p1"));
			if (p2->Direction() != MemberDefinition_Direction_both)
				throw std::exception();
			if (p2->IsUnreliable())
				throw std::exception();

			RR_SHARED_PTR<WireDefinition> w1 = rr_cast<WireDefinition>(TryFindByName(entry->Members, "w1"));
			if (w1->Direction() != MemberDefinition_Direction_both)
				throw std::exception();

			RR_SHARED_PTR<MemoryDefinition> m1 = rr_cast<MemoryDefinition>(TryFindByName(entry->Members, "m1"));
			if (m1->Direction() != MemberDefinition_Direction_both)
				throw std::exception();

		}

		RR_SHARED_PTR<ServiceDefinition> def2 = TryFindByName(defs, "com.robotraconteur.testing.TestService3");
		if (def2)
		{
			RR_SHARED_PTR<ServiceEntryDefinition> entry = TryFindByName(def2->Objects, "testroot3");

			RR_SHARED_PTR<PropertyDefinition> p1 = rr_cast<PropertyDefinition>(TryFindByName(entry->Members, "readme"));
			if (p1->Direction() != MemberDefinition_Direction_readonly) 
				throw std::exception();

			RR_SHARED_PTR<PropertyDefinition> p2 = rr_cast<PropertyDefinition>(TryFindByName(entry->Members, "writeme"));
			if (p2->Direction() != MemberDefinition_Direction_writeonly)
				throw std::exception();

			RR_SHARED_PTR<PipeDefinition> p3 = rr_cast<PipeDefinition>(TryFindByName(entry->Members, "unreliable1"));
			if (p3->Direction() != MemberDefinition_Direction_readonly)
				throw std::exception();
			if (!p3->IsUnreliable())
				throw std::exception();

			RR_SHARED_PTR<PipeDefinition> p4 = rr_cast<PipeDefinition>(TryFindByName(entry->Members, "unreliable2"));
			if (p4->Direction() != MemberDefinition_Direction_both)
				throw std::exception();
			if (!p4->IsUnreliable())
				throw std::exception();

			RR_SHARED_PTR<WireDefinition> w1 = rr_cast<WireDefinition>(TryFindByName(entry->Members, "peekwire"));
			if (w1->Direction() != MemberDefinition_Direction_readonly)
				throw std::exception();

			RR_SHARED_PTR<WireDefinition> w2 = rr_cast<WireDefinition>(TryFindByName(entry->Members, "pokewire"));
			if (w2->Direction() != MemberDefinition_Direction_writeonly)
				throw std::exception();

			RR_SHARED_PTR<MemoryDefinition> m1 = rr_cast<MemoryDefinition>(TryFindByName(entry->Members, "readmem"));
			if (m1->Direction() != MemberDefinition_Direction_readonly)
				throw std::exception();

			std::cout << "Found it" << std::endl;
		}
		
		return 0;
	}

	throw runtime_error("Unknown test command");

}
