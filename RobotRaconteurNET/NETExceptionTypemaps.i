//Exception handling
%exception  %{
try {
  $action
} catch (RobotRaconteurException& e)
{
	if (robotRaconteurExceptionHelper!=NULL)
	{
		robotRaconteurExceptionHelper->SetRobotRaconteurException(e.ErrorCode,e.Error,e.Message);
		return $null;
	}
	else
	{
		SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, e.what());
		return $null;
	}
	

} catch (std::exception& e) {
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
virtual void SetRobotRaconteurException(MessageErrorType errorcode, std::string errorname, std::string errorstring) {};
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
	static void SetError(boost::intrusive_ptr<RobotRaconteur::MessageEntry> err);
	static bool IsErrorPending();
	static boost::intrusive_ptr<MessageEntry> GetError();

};
	
}