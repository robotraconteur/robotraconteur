//Typemap

%typemap(out) boost::posix_time::ptime {
	{
		TimeSpec timespec_result1 = RobotRaconteur::ptimeToTimeSpec($1);
		*(RobotRaconteur::TimeSpec **)&$result = new RobotRaconteur::TimeSpec((const RobotRaconteur::TimeSpec &)timespec_result1);
	}    
}

%typemap(in) boost::posix_time::ptime {
	{
		TimeSpec* temp_ptime = *(RobotRaconteur::TimeSpec **)&$input;
		if (!temp_ptime) 
		{
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "Attempt to dereference null TimeSpec");
			return $null;
		}
		$1 = TimeSpecToPTime(*temp_ptime);
	}    
}

%typemap(jstype) boost::posix_time::ptime "com.robotraconteur.TimeSpec";
%typemap(jni) boost::posix_time::ptime "jlong";
%typemap(jtype) boost::posix_time::ptime "long";

%typemap(javaout) boost::posix_time::ptime {
    return new TimeSpec($jnicall,true);
}

%typemap(javain) boost::posix_time::ptime "TimeSpec.getCPtr($javainput)"
    


%typemap(jstype) boost::posix_time::time_duration "int"
%typemap(jni) boost::posix_time::time_duration "jint"
%typemap(jtype) boost::posix_time::time_duration "int"

%typemap(out) boost::posix_time::time_duration {
	$result=(jint)$1.total_milliseconds();
}

%typemap(javaout) boost::posix_time::time_duration {
    int t= $jnicall;
    return t;
}

%typemap(jstype) const boost::posix_time::time_duration& "int"
%typemap(jni) const boost::posix_time::time_duration& "jint"
%typemap(jtype) const boost::posix_time::time_duration& "int"




%typemap(in) boost::posix_time::time_duration (boost::posix_time::time_duration tmpDur) {
	tmpDur=boost::posix_time::milliseconds($input);
	$1=&tmpDur;
}



%typemap(javain) boost::posix_time::time_duration "$javainput";


%typemap(in) const boost::posix_time::time_duration& = boost::posix_time::time_duration;
%typemap(javain) const boost::posix_time::time_duration& = boost::posix_time::time_duration;

%define %rr_intrusive_ptr( TYPE )
%intrusive_ptr( TYPE );

%typemap(directorin,descriptor="L$packagepath/$typemap(jstype, TYPE);") SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< TYPE > 
%{ 
//$input = $1 ? new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >($1) : 0; 
  if ($1) {
    intrusive_ptr_add_ref($1.get());
    *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > **)&$input = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >($1.get(), SWIG_intrusive_deleter< TYPE >());
  } else {
    *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > **)&$input = 0;
  }
%}

%typemap(directorout) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< TYPE > (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > * smartarg)
%{ 
//if ($input) {
//    SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartarg = (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *)$input;
//    $result = *smartarg;
//  }
  smartarg = *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >**)&$input;
  if (smartarg) {
    $result = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< TYPE >(smartarg->get(), true);
  }

%}

%typemap(javadirectorout) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< TYPE > "$typemap(jstype, TYPE).getCPtr($javacall)"
%typemap(javadirectorin) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr<  TYPE > "($jniinput == 0) ? null : new $typemap(jstype, TYPE)($jniinput, true)"


%enddef

namespace RobotRaconteur
{
	%naturalvar MessageStringPtr;
	class MessageStringPtr;

	%apply std::string { RobotRaconteur::MessageStringPtr }
	%apply const std::string& { const RobotRaconteur::MessageStringPtr& }

	%typemap(in, canthrow=1) MessageStringPtr 
	%{ if (!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null string");
		return $null;
	}
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
    if (!$1_pstr) return $null;
    $1.assign($1_pstr);
    jenv->ReleaseStringUTFChars($input, $1_pstr); %}
	%typemap(out) MessageStringPtr  %{ std::string temp_ret1 = $1.str().to_string(); $result = jenv->NewStringUTF(temp_ret1.c_str()); %}
	%typemap(in, canthrow=1) const MessageStringPtr &
	%{ if (!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null string");
		return $null;
	}
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
    if (!$1_pstr) return $null;
	$*1_ltype $1_str($1_pstr,false);
	$1 = &$1_str;
	jenv->ReleaseStringUTFChars($input, $1_pstr); %}
	%typemap(out) const MessageStringPtr&  %{ std::string temp_ret1 = $1->str().to_string(); $result = jenv->NewStringUTF(temp_ret1.c_str()); %}	

}

%{
#include <boost/filesystem/path.hpp>
%}

namespace boost
{
namespace filesystem
{

%rename(FilesystemPath) path;
class path
{
public:

path(const std::string& s);
std::string string();

};
}
}
