//Typemap

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
	
	  $result=jenv->NewStringUTF(o.str().c_str());
}


%typemap(jstype) boost::posix_time::ptime "java.util.GregorianCalendar";

%typemap(jni) boost::posix_time::ptime "jstring";
%typemap(jtype) boost::posix_time::ptime "String";


%typemap(javaout) boost::posix_time::ptime {
    String date=$jnicall;
	  java.util.GregorianCalendar o=new java.util.GregorianCalendar(
			  Integer.parseInt(date.substring(0, 4)),
			  Integer.parseInt(date.substring(4,6)),
			  Integer.parseInt(date.substring(6,8)),
			  Integer.parseInt(date.substring(9,11)),
			  Integer.parseInt(date.substring(11,13)),
			  Integer.parseInt(date.substring(13,15))			  			
			  );
	  o.set(java.util.Calendar.MILLISECOND, Integer.parseInt(date.substring(16,19)));
	  return o;
}

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