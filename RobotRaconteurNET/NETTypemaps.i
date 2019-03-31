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

%typemap(out) boost::posix_time::ptime {
    //$result=PyString_FromString(boost::posix_time::to_iso_string($1).c_str());
    std::stringstream o;
	o << boost::gregorian::to_iso_string($1.date()) << "T";
    boost::posix_time::time_duration t=$1.time_of_day();
	o << std::setw(2) << std::setfill('0') << t.hours() << std::setw(2) << t.minutes() << std::setw(2) << t.seconds() << ".";
	std::stringstream o2;
	o2 << std::setw(t.num_fractional_digits()) << std::setfill('0') <<  t.fractional_seconds();
    std::string o3=o2.str();
	while (o3.size() < 6) o3 += "0";
	o << o3.substr(0,6);
	
	  $result=SWIG_csharp_string_callback(o.str().c_str());
}


%typemap(cstype) boost::posix_time::ptime "DateTime";

%typemap(ctype) boost::posix_time::ptime "char*";
%typemap(imtype) boost::posix_time::ptime "string";

%typemap(csout, excode=SWIGEXCODE) boost::posix_time::ptime {
    string date = $imcall;$excode
    return new DateTime(Int32.Parse(date.Substring(0, 4)),
                    Int32.Parse(date.Substring(4, 2)),
                    Int32.Parse(date.Substring(6, 2)),
                    Int32.Parse(date.Substring(9, 2)),
                    Int32.Parse(date.Substring(11, 2)),
                    Int32.Parse(date.Substring(13, 2)),
                    Int32.Parse(date.Substring(16, 3)));
}

%typemap(csvarout, excode=SWIGEXCODE) boost::posix_time::ptime %{
	get {
    string date = $imcall;$excode
    return new DateTime(Int32.Parse(date.Substring(0, 4)),
                    Int32.Parse(date.Substring(4, 2)),
                    Int32.Parse(date.Substring(6, 2)),
                    Int32.Parse(date.Substring(9, 2)),
                    Int32.Parse(date.Substring(11, 2)),
                    Int32.Parse(date.Substring(13, 2)),
                    Int32.Parse(date.Substring(16, 3)));
	}
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



%enddef

