%rr_intrusive_ptr(RobotRaconteur::MessageElement)

namespace RobotRaconteur
{
class HandlerErrorInfo;
}

//Exception handling
%exception  %{
try {
  $action
} catch (RobotRaconteurException& e)
{
	RR_SHARED_PTR<RobotRaconteurNode> default_node = RobotRaconteurNode::weak_sp().lock();
	if (default_node)
	{			
		ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(default_node, Default, -1, "Exception raised in C++, passing to wrapped language: " << e.what());
	}
	if (robotRaconteurExceptionHelper!=NULL)
	{
		RobotRaconteur::HandlerErrorInfo err3(e);
		robotRaconteurExceptionHelper->SetRobotRaconteurException(err3);
		return $null;
	}
	else
	{
		SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, e.what());
		return $null;
	}
	

} catch (std::exception& e) {
	RR_SHARED_PTR<RobotRaconteurNode> default_node = RobotRaconteurNode::weak_sp().lock();
	if (default_node)
	{			
		ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(default_node, Default, -1, "Exception raised in C++, passing to wrapped language: " << e.what());
	}
  SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, e.what());
  return $null;
}
%}

%feature("director") RobotRaconteurExceptionHelper;

%inline
{
class RobotRaconteurExceptionHelper
{
public:
virtual void SetRobotRaconteurException(const RobotRaconteur::HandlerErrorInfo& error) {};
virtual ~RobotRaconteurExceptionHelper() {}
};

RobotRaconteurExceptionHelper* robotRaconteurExceptionHelper=NULL;

}

//Forward declarations of MessageElement
%rr_intrusive_ptr(RobotRaconteur::MessageEntry)

//RRDirectorExceptionHelper
namespace RobotRaconteur {

class MessageEntry;

class RRDirectorExceptionHelper
{

public:
	static void Reset();
	static void SetError(const boost::intrusive_ptr<RobotRaconteur::MessageEntry>& err, const std::string& exception_str);
	static bool IsErrorPending();
	static boost::intrusive_ptr<MessageEntry> GetError();

};
	
}