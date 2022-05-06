#ifdef ANDROID
%{
//#include "HardwareTransport_android_private.h"
%}

%shared_ptr(RobotRaconteur::detail::AndroidBluetoothConnector_params);

namespace RobotRaconteur
{
namespace detail
{

%typemap(javaclassmodifiers) AndroidBluetoothConnector_params "class"
class AndroidBluetoothConnector_params
{
public:
	RR_SHARED_PTR<HardwareTransport> parent;
	NodeID target_nodeid;
	std::string target_nodename;
	std::string noden;
	uint32_t endpoint;

	//boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&) > handler;
};

%typemap(javaclassmodifiers) AndroidHardwareDirector "class"
%feature("director") AndroidHardwareDirector;
class AndroidHardwareDirector
{
public:

	static void SetDirector(AndroidHardwareDirector* d);
	static void ClearDirector(AndroidHardwareDirector* d);

	AndroidHardwareDirector();
	virtual ~AndroidHardwareDirector();

	//static void ConnectBluetooth(const RR_SHARED_PTR<HardwareTransport>& parent, const ParseConnectionURLResult& url, const std::string& noden, uint32_t endpoint, boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&) > handler);

protected:
	virtual void ConnectBluetooth2(void* p) = 0;

};

%typemap(javaclassmodifiers) AndroidHardwareHelper "class"
class AndroidHardwareHelper
{
public:

	static int32_t ConnectBluetooth_success(const boost::shared_ptr<AndroidBluetoothConnector_params>& p);
	static void ConnectBluetooth_error(const boost::shared_ptr<AndroidBluetoothConnector_params>& p, const std::string& message);
	static boost::shared_ptr<AndroidBluetoothConnector_params> VoidToAndroidBluetoothConnector_params(void* p);
	static std::vector<int8_t> MessageToVector(const boost::shared_ptr<Message>& m);
	static boost::shared_ptr<Message> VectorToMessage(std::vector<int8_t> v);
};

}

}



#endif