
%shared_ptr(RobotRaconteur::WrappedServiceStub)

namespace RobotRaconteur
{
    class WrappedService_typed_packet
	{
	public:
		boost::intrusive_ptr<RobotRaconteur::MessageElement> packet;
		boost::shared_ptr<RobotRaconteur::TypeDefinition> type;
		boost::shared_ptr<RobotRaconteur::WrappedServiceStub> stub;
        uint32_t client;	
	};
}