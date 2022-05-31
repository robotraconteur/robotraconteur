
%exception {
try
{
  $action
  }
  catch (RobotRaconteur::RobotRaconteurException& e)
  {
    
	jclass clazz2;
	jmethodID mid2;
	jthrowable jexp;
	jlong error_info_ptr=0;
    RobotRaconteur::HandlerErrorInfo error_info(e);
	clazz2=jenv->FindClass("com/robotraconteur/RobotRaconteurExceptionUtil");
	if (!clazz2) goto rrerrfail;
	mid2 = jenv->GetStaticMethodID(clazz2, "errorInfoToException" ,"(J)Ljava/lang/RuntimeException;");
	if (!mid2) goto rrerrfail;
	error_info_ptr=(jlong)&error_info;
	jexp=(jthrowable)jenv->CallStaticObjectMethod(clazz2,mid2,error_info_ptr);
	jenv->Throw(jexp);
	return $null;
	
	rrerrfail:
		jclass clazz = jenv->FindClass( "java/lang/RuntimeException");
		jenv->ThrowNew(clazz, e.what());
		return $null;
	
	
  }
  catch (std::exception& e) {
    jclass clazz = jenv->FindClass( "java/lang/RuntimeException");
    jenv->ThrowNew(clazz, e.what());
    return $null;
  }
}

%rr_intrusive_ptr(RobotRaconteur::MessageEntry)

//RRDirectorExceptionHelper

%typemap(javacode) RobotRaconteur::RRDirectorExceptionHelper %{
public static String exceptionToStackTraceString(Exception exp)
{
	java.io.StringWriter sw = new java.io.StringWriter();
	java.io.PrintWriter pw = new java.io.PrintWriter(sw);
	exp.printStackTrace(pw);
	return sw.toString();
}
%}

namespace RobotRaconteur {
class RRDirectorExceptionHelper
{


public:
	static void Reset();
	static void SetError(const boost::intrusive_ptr<RobotRaconteur::MessageEntry>& err, const std::string& exception_str);
	static bool IsErrorPending();
	static boost::intrusive_ptr<RobotRaconteur::MessageEntry> GetError();

};

}