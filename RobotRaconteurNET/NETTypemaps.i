%include "boost_intrusive_ptr.i"

%typemap(cstype) void* "IntPtr"
%typemap(csin) void* "new HandleRef(null, $csinput)"
%typemap(csout, excode=SWIGEXCODE) void* "{IntPtr res = $imcall; $excode; return res;}"
%typemap(csvarout, excode=SWIGEXCODE2) void* "get{IntPtr res = $imcall; $excode; return res;}"
%typemap(csdirectorin) void* "$iminput"
%typemap(csdirectorout) void* "$cscall"

#if defined(SWIGWORDSIZE64)
%apply unsigned int { size_t };
%apply const unsigned int & { const size_t & };
#endif

%typemap(cstype) boost::posix_time::ptime "DateTime";
%typemap(ctype) boost::posix_time::ptime "int64_t";
%typemap(imtype) boost::posix_time::ptime "long";

%typemap(in) boost::posix_time::ptime {
    $1 = boost::posix_time::ptime(boost::gregorian::date(1970,1,1),boost::posix_time::time_duration(0,0,0)) + boost::posix_time::microseconds($input/10);
}

%typemap(out) boost::posix_time::ptime {
    {
	boost::posix_time::time_duration ptime_diff = ($1 - boost::posix_time::ptime(boost::gregorian::date(1970,1,1),boost::posix_time::time_duration(0,0,0)));
	$result=ptime_diff.total_microseconds()*10;
    }
}

%typemap(csout, excode=SWIGEXCODE) boost::posix_time::ptime, boost::posix_time::ptime* {
    long date_ticks = $imcall;$excode
    return new DateTime(date_ticks + (new DateTime(1970,1,1)).Ticks);
}

%typemap(csvarout, excode=SWIGEXCODE) boost::posix_time::ptime, boost::posix_time::ptime* %{
	get {
    long date_ticks = $imcall;$excode
    return new DateTime(date_ticks + (new DateTime(1970,1,1)).Ticks);
	}
%}

%typemap(csin, excode=SWIGEXCODE) boost::posix_time::ptime, boost::posix_time::ptime* %{
    $csinput.Ticks - (new DateTime(1970,1,1)).Ticks
%}

%typemap(cstype) boost::posix_time::time_duration "int"
%typemap(ctype) boost::posix_time::time_duration "int32_t"
%typemap(imtype) boost::posix_time::time_duration "int"

%typemap(out) boost::posix_time::time_duration {
	$result=(int32_t)$1.total_milliseconds();
}

%typemap(csout, excode=SWIGEXCODE) boost::posix_time::time_duration {
    int t= $imcall;$excode
    return t;
}

%typemap(cstype) const boost::posix_time::time_duration& "int"
%typemap(ctype) const boost::posix_time::time_duration& "int32_t"
%typemap(imtype) const boost::posix_time::time_duration& "int"




%typemap(in) boost::posix_time::time_duration (boost::posix_time::time_duration tmpDur) {
	tmpDur=boost::posix_time::milliseconds($input);
	$1=&tmpDur;
}



%typemap(csin) boost::posix_time::time_duration "$csinput";


%typemap(in) const boost::posix_time::time_duration& = boost::posix_time::time_duration;
%typemap(csin) const boost::posix_time::time_duration& = boost::posix_time::time_duration;

%typemap(cstype) DataTypes "DataTypes"
%typemap(ctype) DataTypes "uint32_t";
%typemap(imtype) DataTypes "uint";
%typemap(out) DataTypes {
	$result=$1;
}
%typemap(in) DataTypes {
	$1=(DataTypes)$input;
}
%typemap(csout, excode=SWIGEXCODE) DataTypes {
	uint type= $imcall;$excode
	return (DataTypes)type;
};
%typemap(csin) DataTypes "(uint)$csinput"
%typemap(csvarout, excode=SWIGEXCODE) DataTypes %{
	get {
		uint val=$imcall;$excode
		return (DataTypes)val;
	}	
%}

%typemap(cstype) MessageEntryType "MessageEntryType"
%typemap(ctype) MessageEntryType "uint32_t";
%typemap(imtype) MessageEntryType "uint";
%typemap(csout, excode=SWIGEXCODE) MessageEntryType {
	uint type= $imcall;$excode
	return (MessageEntryType)type;
};
%typemap(csin) MessageEntryType "(uint)$csinput"
%typemap(csvarout, excode=SWIGEXCODE) MessageEntryType %{
	get {
		uint val=$imcall;$excode
		return (MessageEntryType)val;
	}	
%}
%typemap(out) MessageEntryType {
	$result=$1;
}
%typemap(in) MessageEntryType {
	$1=(MessageEntryType)$input;
}

%typemap(cstype) MessageErrorType "MessageErrorType"
%typemap(ctype) MessageErrorType "uint32_t";
%typemap(imtype) MessageErrorType "uint";
%typemap(csout, excode=SWIGEXCODE) MessageErrorType {
	uint type= $imcall;$excode
	return (MessageErrorType)type;
};
%typemap(csin) MessageErrorType "(uint)$csinput"
%typemap(csvarout, excode=SWIGEXCODE) MessageErrorType %{
	get {
		uint val=$imcall;$excode
		return (MessageErrorType)val;
	}	
%}
%typemap(directorin) MessageErrorType "$input = $1;"
%typemap(csdirectorin) MessageErrorType "(MessageErrorType)$iminput";
%typemap(csdirectorout) MessageErrorType "$cscall";
%typemap(out) MessageErrorType {
	$result=$1;
}
%typemap(in) MessageErrorType {
	$1=(MessageErrorType)$input;
}

%typemap(cstype) RobotRaconteurObjectLockFlags "RobotRaconteurObjectLockFlags"
%typemap(ctype) RobotRaconteurObjectLockFlags "uint32_t";
%typemap(imtype) RobotRaconteurObjectLockFlags "uint";
%typemap(csout) RobotRaconteurObjectLockFlags {
	uint type= $imcall;$excode
	return (RobotRaconteurObjectLockFlags)type;
};
%typemap(csin) RobotRaconteurObjectLockFlags "(uint)$csinput"
%typemap(csvarout, excode=SWIGEXCODE) RobotRaconteurObjectLockFlags %{
	get {
		uint val=$imcall;$excode
		return (RobotRaconteurObjectLockFlags)val;
	}	
%}
%typemap(out) RobotRaconteurObjectLockFlags {
	$result=$1;
}
%typemap(in) RobotRaconteurObjectLockFlags {
	$1=(RobotRaconteurObjectLockFlags)$input;
}


%define %rr_weak_ptr( TYPE )
%typemap(ctype) boost::weak_ptr< TYPE > boost::shared_ptr< TYPE >;
%typemap(ctype) const boost::weak_ptr< TYPE >& boost::shared_ptr< TYPE >;
%typemap(ctype) boost::weak_ptr< TYPE >& boost::shared_ptr< TYPE >;
//%apply boost::shared_ptr< TYPE > { boost::weak_ptr< TYPE > }
%enddef

%define %rr_intrusive_ptr( TYPE )
%intrusive_ptr( TYPE );

%typemap(directorin) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< TYPE > 
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

%typemap(csdirectorout) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< TYPE > "$typemap(cstype, TYPE).getCPtr($cscall).Handle"
%typemap(csdirectorin) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr<  TYPE > "($iminput == global::System.IntPtr.Zero) ? null : new $typemap(cstype, TYPE)($iminput, true)"

%typemap(csvarout, excode=SWIGEXCODE) SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< TYPE >& %{
    get {		
      global::System.IntPtr cPtr = $imcall;
      $typemap(cstype, TYPE) ret = (cPtr == global::System.IntPtr.Zero) ? null : new $typemap(cstype, TYPE)(cPtr, true);$excode
      return ret;
    } %}


%enddef


namespace RobotRaconteur
{
	%naturalvar MessageStringPtr;
	class MessageStringPtr;

	%apply std::string { RobotRaconteur::MessageStringPtr }
	%apply const std::string& { const RobotRaconteur::MessageStringPtr& }

	%typemap(in, canthrow=1) MessageStringPtr 
	%{ if (!$input) {
		SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
		return $null;
	}
	$1.assign($input,false); %}
	%typemap(out) MessageStringPtr  %{ std::string temp_ret1 = $1.str().to_string(); $result = SWIG_csharp_string_callback(temp_ret1.c_str()); %}
	%typemap(in, canthrow=1) const MessageStringPtr &
	%{ if (!$input) {
		SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
		return $null;
	}
	$*1_ltype $1_str($input,false);
	$1 = &$1_str; %}
	%typemap(out) const MessageStringPtr&  %{ std::string temp_ret1 = $1->str().to_string(); $result = SWIG_csharp_string_callback(temp_ret1.c_str()); %}	



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
