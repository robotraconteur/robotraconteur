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
    //$result=PyString_FromString(boost::posix_time::to_iso_string($1).c_str());
    std::stringstream o;
	o << boost::gregorian::to_iso_string($1.date()) << "T";
    boost::posix_time::time_duration t=$1.time_of_day();
	o << std::setw(2) << std::setfill('0') << t.hours() << std::setw(2) << t.minutes() <<  std::setw(2) << t.seconds() << ".";
	std::stringstream o2;
	o2 << std::setw(t.num_fractional_digits()) << std::setfill('0') <<  t.fractional_seconds();
    std::string o3=o2.str();
	while (o3.size() < 6) o3 += "0";
	o << o3.substr(0,6);
	std::string o4=o.str();
	
	PyObject* modules_dict= PyImport_GetModuleDict();
	if (modules_dict==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError module");
		goto fail;
	}

	PyObject* datetime_module=PyDict_GetItemString(modules_dict, "datetime");
	if (datetime_module==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load datetime module");
		goto fail;
	}

	PyObject* datetime=PyObject_GetAttrString(datetime_module, "datetime");
	if (datetime==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load datetime.datetime class");
		goto fail;
	}

	PyObject* strptime=PyObject_GetAttrString(datetime, "strptime");
	if (strptime==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load datetime.strptime function");
		goto fail;
	}

	PyObject* res=PyObject_CallFunction(strptime, "s#,s", o4.c_str(), o4.size(), "%Y%m%dT%H%M%S.%f");

	$result=res;
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
