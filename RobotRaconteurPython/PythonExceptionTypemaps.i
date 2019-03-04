//Allow all STL errors to fall through to the global error catcher
%typemap(throws) std::bad_exception     "throw _e;"
%typemap(throws) std::domain_error      "throw _e;"
%typemap(throws) std::exception         "throw _e;"
%typemap(throws) std::invalid_argument  "throw _e;"
%typemap(throws) std::length_error      "throw _e;"
%typemap(throws) std::logic_error       "throw _e;"
%typemap(throws) std::out_of_range      "throw _e;"
%typemap(throws) std::overflow_error    "throw _e;"
%typemap(throws) std::range_error       "throw _e;"
%typemap(throws) std::runtime_error     "throw _e;"
%typemap(throws) std::underflow_error   "throw _e;"

%{

void RRExceptionToPythonError(RobotRaconteurException& rrexp)
{
	PyObject* modules_dict= PyImport_GetModuleDict();
	if (modules_dict==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError module");
		return;
	}

	PyObject* err_module=PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonError");
	if (err_module==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError module");
		return;
	}

	PyObject* exceptionUtil=PyObject_GetAttrString(err_module, "RobotRaconteurExceptionUtil");
	if (exceptionUtil==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError.RobotRaconteurExceptionUtil class");
		return;
	}

	PyObject* errorCodeToException=PyObject_GetAttrString(exceptionUtil, "ErrorCodeToException");
	if (errorCodeToException==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturExceptionUtil.ErrorCodeToException function");
		return;
	}

	PyObject* pyErr=PyObject_CallFunction(errorCodeToException, "h,s#,s#", rrexp.ErrorCode, rrexp.Error.c_str(), rrexp.Error.size(), rrexp.Message.c_str(), rrexp.Message.size());
	PyObject* pyErrType=PyObject_Type(pyErr);

	PyErr_SetObject(pyErrType,pyErr);
	Py_XDECREF(pyErr);
	Py_XDECREF(pyErrType);	
}
	
%}

%define RR_Py_Exception_GIL()
%exception %{
    
    try {
		RR_Release_GIL gil_save;  
        $action       
    }
    
    catch (RobotRaconteurException &e) {		
		//PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
		RRExceptionToPythonError(e);
		goto fail;
	}    
    catch (std::exception &e) {    	
        PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
        goto fail;
    }
    /*catch (...)
    {    	
		PyErr_SetString(PyExc_Exception,"Unknown Robot Raconteur error");
		goto fail;
    }*/
    
%}
%enddef

%define RR_Py_Exception()
%exception %{	
    try {		    
        $action       
    }
    
    catch (RobotRaconteurException &e) {	
		//PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
		RRExceptionToPythonError(e);
		goto fail;
	}    
    catch (std::exception &e) {   	
        PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
        goto fail;
    }
    /*catch (...)
    {    	
		PyErr_SetString(PyExc_Exception,"Unknown Robot Raconteur error");
		goto fail;
    }*/
    
%}
%enddef

RR_Py_Exception()

%feature("director:except") {
    if ($error != NULL) {
    	
    	
    	PyObject* exc;
    	PyObject* val;
    	PyObject* tb;
    	PyErr_Fetch(&exc,&val,&tb);
        PyErr_NormalizeException(&exc,&val,&tb);

		PyObject* modules_dict= PyImport_GetModuleDict();
		if (modules_dict==NULL)
		{
			throw InternalErrorException("Could not load RobotRaconeturPythonError module");			
		}

		PyObject* err_module=PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonError");
		if (err_module==NULL)
		{
			throw InternalErrorException("Could not load RobotRaconeturPythonError module");			
		}

		PyObject* rr_py_RobotRaconteurException=PyObject_GetAttrString(err_module, "RobotRaconteurException");
		if (rr_py_RobotRaconteurException==NULL)
		{
			throw InternalErrorException("Could not load RobotRaconeturPythonError.RobotRaconteurException type");		
		}

        if (PyErr_GivenExceptionMatches(exc,rr_py_RobotRaconteurException))
        {
        	PyObject* message=PyObject_Str(val);
        	PyObject* errorname=PyObject_Str(PyObject_GetAttr(val,PyString_FromString("errorname")));
        	PyObject* errorcode=PyNumber_Long(PyObject_GetAttr(val,PyString_FromString("errorcode")));
        	
        	if (!message || !errorcode || !errorname)
        	{
        		throw InternalErrorException("Exception occured in Python code");
        	}
        	
        	std::string message2 = PyObjectToUTF8(message);
        	std::string errorname2=PyObjectToUTF8(errorname);
        	long errorcode2=PyLong_AsLong(errorcode);
        	if (errorcode2 > 65535 || errorcode2 < 0) errorcode2=MessageErrorType_UnknownError;
        	
        	boost::intrusive_ptr<MessageEntry> m=CreateMessageEntry();
        	m->Error=(MessageErrorType)(uint16_t)errorcode2;
        	m->AddElement("errorname",stringToRRArray(errorname2));
        	m->AddElement("errorstring", stringToRRArray(message2));			

        	RobotRaconteurExceptionUtil::ThrowMessageEntryException(m);        	
        }
        else
        {
			PyObject* str=PyObject_Str(val);
			PyObject* strname=PyObject_Str(PyObject_GetAttr(exc,PyString_FromString("__name__")));
			std::string str2=PyObjectToUTF8(str);
			std::string strname2=PyObjectToUTF8(strname);
					
			throw UnknownException(strname2, str2);
        }
    }
}

