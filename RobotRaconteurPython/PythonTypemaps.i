%{
#include <datetime.h>
%}

%typemap (in) void* {
	$1 = PyLong_AsVoidPtr($input);
}

%typemap(out) void* {
	$result=PyLong_FromVoidPtr($1);
}

%typemap(directorin) void* {
	$input=PyLong_FromVoidPtr( $1 );
}

%typemap (directorout) void* {
	$result = PyLong_AsVoidPtr($input);
}

//Pass raw pointers for PyObject

%typemap(in) PyObject* {
   $1 = $input;
}

%typemap(out) PyObject* {
   $result = $1;
} 

%typemap(out) boost::posix_time::ptime {
	try {
    $result = PyDateTime_FromDateAndTime((int)$1.date().year(), (int)$1.date().month(), (int)$1.date().day(),
		(int)$1.time_of_day().hours(), (int)$1.time_of_day().minutes(), (int)$1.time_of_day().seconds(),
		boost::numeric_cast<int32_t>(($1.time_of_day().fractional_seconds() * boost::numeric_cast<int32_t>(pow(10.0,(9-$1.time_of_day().num_fractional_digits())))))/1000);
	}
	catch (std::exception&)
	{

		$result = SWIG_Py_Void();
	}
}

%typemap(in) boost::posix_time::ptime {
	if (PyDateTime_Check($input)) {
		$1 = boost::posix_time::ptime(boost::gregorian::date((int)PyDateTime_GET_YEAR($input),(int)PyDateTime_GET_MONTH($input),(int)PyDateTime_GET_DAY($input)),
			boost::posix_time::time_duration((int)PyDateTime_DATE_GET_HOUR($input),(int)PyDateTime_DATE_GET_MINUTE($input),(int)PyDateTime_DATE_GET_SECOND($input)))
			 + boost::posix_time::microseconds((int)PyDateTime_DATE_GET_MICROSECOND($input));
	}
	else
	{
		PyErr_SetString(PyExc_TypeError, "not a datetime");
    	SWIG_fail;
	}
}

%typemap(out) boost::posix_time::time_duration %{
	$result=PyFloat_FromDouble(boost::lexical_cast<double>($1.total_microseconds())/1000000.0);
%}



%typemap(in) boost::posix_time::time_duration (boost::posix_time::time_duration tmpDur, int64_t pyinputval2=0, double pyinputval=0) %{

	
	if (PyInt_Check($input))
	{
		pyinputval=boost::lexical_cast<double>(PyInt_AsLong($input));
	}
	else if (PyLong_Check($input))
	{
		pyinputval=boost::lexical_cast<double>(PyLong_AsLong($input));
	}	
	else if (PyFloat_Check($input))
	{
		pyinputval=PyFloat_AsDouble($input);
	}
	else
	{
		PyErr_SetString(PyExc_TypeError,"Input must be an Integer, Long, or Float");
		return NULL;
	}
	
	pyinputval2=boost::lexical_cast<int64_t>(pyinputval*1000000.0);
	tmpDur=boost::posix_time::microseconds(pyinputval2);
	$1=&tmpDur;
	
%}

%typemap(in) const boost::posix_time::time_duration& = boost::posix_time::time_duration;

%typemap(out) boost::array<uint8_t,16> %{
	$result=PyByteArray_FromStringAndSize((const char*)&$1[0],16);
%}


namespace RobotRaconteur
{
	%naturalvar MessageStringPtr;
	class MessageStringPtr;

	%apply std::string { RobotRaconteur::MessageStringPtr }
	%apply const std::string& { const RobotRaconteur::MessageStringPtr& }

	%fragment(SWIG_AsVal_frag(MessageStringPtr),"header", fragment=SWIG_AsVal_frag(std::string)) {
		SWIGINTERN int
		SWIG_AsVal_dec(MessageStringPtr)(SWIG_Object obj, RobotRaconteur::MessageStringPtr *val)
		{
			std::string temp1;
			int res = SWIG_AsVal(std::string)(obj, &temp1);
			if (!SWIG_IsOK(res))
			{
				return res;
			}
			*val = temp1;
			return res;
		}
	}

	%fragment(SWIG_From_frag(MessageStringPtr),"header",fragment=SWIG_From_frag(std::string)) {
		SWIGINTERNINLINE SWIG_Object
		SWIG_From_dec(MessageStringPtr)  (const RobotRaconteur::MessageStringPtr& m)
		{
			return SWIG_From_std_string(m.str().to_string());
		}
	}

	%fragment(SWIG_AsPtr_frag(MessageStringPtr),"header",fragment=SWIG_AsVal_frag(std::string)) {
		SWIGINTERN int
		SWIG_AsPtr_dec(MessageStringPtr)(SWIG_Object obj, RobotRaconteur::MessageStringPtr **val) 
		{
			std::string temp1;
			int res = SWIG_AsVal(std::string)(obj, &temp1);
			if (!SWIG_IsOK(res))
			{
				return res;
			}
			*val = new MessageStringPtr(temp1);
			return SWIG_NEWOBJ;
		}
	}




	%typemaps_asptrfromn(%checkcode(STDSTRING),MessageStringPtr)
}
