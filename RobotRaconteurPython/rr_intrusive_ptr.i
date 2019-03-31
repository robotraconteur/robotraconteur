// Users can provide their own SWIG_INTRUSIVE_PTR_TYPEMAPS or SWIG_INTRUSIVE_PTR_TYPEMAPS_NO_WRAP macros before including this file to change the
// visibility of the constructor and getCPtr method if desired to public if using multiple modules.
#ifndef SWIG_INTRUSIVE_PTR_TYPEMAPS_NO_WRAP
#define SWIG_INTRUSIVE_PTR_TYPEMAPS_NO_WRAP(CONST, TYPE...) SWIG_INTRUSIVE_PTR_TYPEMAPS_NO_WRAP_IMPLEMENTATION(protected, protected, CONST, TYPE)
#endif
#ifndef SWIG_INTRUSIVE_PTR_TYPEMAPS
#define SWIG_INTRUSIVE_PTR_TYPEMAPS(CONST, TYPE...) SWIG_INTRUSIVE_PTR_TYPEMAPS_IMPLEMENTATION(protected, protected, CONST, TYPE)
#endif

%include <intrusive_ptr.i>

// Language specific macro implementing all the customisations for handling the smart pointer
%define SWIG_INTRUSIVE_PTR_TYPEMAPS_IMPLEMENTATION(PTRCTOR_VISIBILITY, CPTR_VISIBILITY, CONST, TYPE...)

// %naturalvar is as documented for member variables
%naturalvar TYPE;
%naturalvar SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >;
// destructor wrapper customisation
%feature("unref") TYPE "(void)arg1; delete smartarg1;"

%typemap(in) SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > (void* argp, int res = 0, int newmem = 0) %{
  // intrusive_ptr by value
  //smartarg = *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >**)&$input;
  //if (smartarg) {
  //	$1 = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(smartarg->get(), true);
  //}
  
  res = SWIG_ConvertPtrAndOwn($input, &argp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), %convertptr_flags, &newmem);
  if (!SWIG_IsOK(res)) {
    %argument_fail(res, "$type", $symname, $argnum);
  }
  if (argp) $1 = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(%reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*)->get(),true);
  if (newmem & SWIG_CAST_NEW_MEMORY) delete %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*);
%}
%typemap(in) SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > & (void* argp, int res = 0, $*1_ltype tempnull, $*1_ltype temp, int newmem = 0) %{
  // intrusive_ptr by reference
  //if ( $input ) {
  //	smartarg = *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >**)&$input;
  //	temp = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(smartarg->get(), true);
  //	$1 = &temp;
  //} else {
  //  $1 = &tempnull;
  //}
  
  res = SWIG_ConvertPtrAndOwn($input, &argp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), %convertptr_flags, &newmem);
  if (!SWIG_IsOK(res)) {
    %argument_fail(res, "$type", $symname, $argnum);
  }
  if (argp) {
	temp = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(%reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*)->get(),true);
	if (newmem & SWIG_CAST_NEW_MEMORY) delete %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*);
	$1 = &temp;
  }
  else {
	$1 = &tempnull;  
  }  
%}
%typemap(in) SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > * (void* argp, int res = 0, $*1_ltype tempnull, $*1_ltype temp, int newmem = 0) %{
  // intrusive_ptr by pointer
  //if ( $input ) {
  //	smartarg = *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >**)&$input;
  //	temp = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(smartarg->get(), true);
  //	$1 = &temp;
  //} else {
  //  $1 = &tempnull;
  //}
  
  res = SWIG_ConvertPtrAndOwn($input, &argp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), %convertptr_flags, &newmem);
  if (!SWIG_IsOK(res)) {
    %argument_fail(res, "$type", $symname, $argnum);
  }
  if (argp) {
	temp = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(%reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*)->get(),true);
	if (newmem & SWIG_CAST_NEW_MEMORY) delete %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*);
	$1 = &temp;
  }
  else {
	$1 = &tempnull;  
  }  
%}
%typemap(out, fragment="SWIG_intrusive_deleter") SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartresult = 0) %{
  //if ($1) {
  //	intrusive_ptr_add_ref(result.get());
  //	*(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >(result.get(), SWIG_intrusive_deleter< CONST TYPE >());
  //} else {
  // 	*(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = 0;
  //}
  
  if(*(&$1)) {
	  intrusive_ptr_add_ref((&result)->get());
	  smartresult = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >((&$1)->get(),SWIG_intrusive_deleter< CONST TYPE >());
  }  
  
  %set_output(SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), SWIG_POINTER_OWN));
%}
%typemap(out, fragment="SWIG_intrusive_deleter") SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > & (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartresult = 0) %{
  //if (*$1) {
  //  intrusive_ptr_add_ref($1->get());
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1->get(), SWIG_intrusive_deleter< CONST TYPE >());
  //} else {
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = 0;
  //}
  
  if(*($1)) {
	  intrusive_ptr_add_ref(result->get());
	  smartresult = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1->get(),SWIG_intrusive_deleter< CONST TYPE >());
  } 
  %set_output(SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), SWIG_POINTER_OWN));
%}
%typemap(out, fragment="SWIG_intrusive_deleter") SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > * (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartresult = 0) %{
  //if ($1 && *$1) {
  //  intrusive_ptr_add_ref($1->get());
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1->get(), SWIG_intrusive_deleter< CONST TYPE >());
  //} else {
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = 0;
  //}
  //if ($owner) delete $1;
  
  if ($1 && *$1) {
	  intrusive_ptr_add_ref(result->get());
	  smartresult = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1->get(),SWIG_intrusive_deleter< CONST TYPE >());
  }  
  if ($owner) delete $1;
  %set_output(SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), SWIG_POINTER_OWN));
%}

/*%typemap(in) SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > swigSharedPtrUpcast ($&1_type smartarg) %{
  // shared_ptr by value
  smartarg = *($&1_ltype*)&$input;
  if (smartarg) $1 = *smartarg;
%}
%typemap(out) SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > ANY_TYPE_SWIGSharedPtrUpcast %{
  *($&1_ltype*)&$result = $1 ? new $1_ltype($1) : 0;
%}*/


/*%typemap(memberin) SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > & %{
  delete &($1);
  if ($self) {
    SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > * temp = new SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(*$input);
    $1 = *temp;
  }
%}*/

%typemap(in) CONST TYPE * (void  *argp = 0, int res = 0, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > tempshared, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartarg = 0, int newmem = 0) {
  
  res = SWIG_ConvertPtrAndOwn($input, &argp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), SHARED_PTR_DISOWN | %convertptr_flags, &newmem);
  if (!SWIG_IsOK(res)) {
    %argument_fail(res, "$type", $symname, $argnum);
  }
  if (newmem & SWIG_CAST_NEW_MEMORY) {
    tempshared = *%reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *);
    delete %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *);
    $1 = %const_cast(tempshared.get(), $1_ltype);
  } else {
    smartarg = %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *);
    $1 = %const_cast((smartarg ? smartarg->get() : 0), $1_ltype);
  }
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER, equivalent="TYPE *", noblock=1)
                      
                      SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >,
                      SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > &,
                      SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > *,
                      SWIG_SHARED_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > *& {
  int res = SWIG_ConvertPtr($input, 0, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), 0);
  $1 = SWIG_CheckState(res);
}

%typemap(directorin, fragment="SWIG_intrusive_deleter") SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > (SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartarg = 0) %{
  // intrusive_ptr by value directorin
  
  if(*(&$1)) {
	  intrusive_ptr_add_ref((&$1)->get());
	  smartarg = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >((&$1)->get(),SWIG_intrusive_deleter< CONST TYPE >());
  }
  $input = SWIG_NewPointerObj(%as_voidptr(smartarg), $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), SWIG_POINTER_OWN);
%}

%typemap(directorin) std::vector< SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > > const %{
  //std::vector<boost::intrusive_ptr<T> > workaround
  
  $input = SWIG_NewPointerObj(%as_voidptr($1), $1_descriptor, 0);
%}

%typemap(directorin) std::vector< SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > > & %{
  //std::vector<boost::intrusive_ptr<T> > workaround
  
  $input = SWIG_NewPointerObj(%as_voidptr($1), $1_descriptor, 0);
%}

%typemap(directorout) SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE > (void* argp, int swig_res = 0, int newmem = 0) %{
  // intrusive_ptr by value directorout
  //smartarg = *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >**)&$input;
  //if (smartarg) {
  //	$1 = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(smartarg->get(), true);
  //}
  
  swig_res = SWIG_ConvertPtrAndOwn($input, &argp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), %convertptr_flags, &newmem);
  if (!SWIG_IsOK(swig_res)) {
    %dirout_fail(swig_res, "$type");
  }
  if (argp) $result = SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >(%reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*)->get(),true);
  if (newmem & SWIG_CAST_NEW_MEMORY) delete %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE >*);
%}

%typemap(in) CONST TYPE * (void  *argp = 0, int res = 0, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > tempshared, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartarg = 0, int newmem = 0) %{
  // plain pointer
  // smartarg = *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$input;
  // $1 = (TYPE *)(smartarg ? smartarg->get() : 0);
  
  
  res = SWIG_ConvertPtrAndOwn($input, &argp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), SHARED_PTR_DISOWN | %convertptr_flags, &newmem);
  if (!SWIG_IsOK(res)) {
    %argument_fail(res, "$type", $symname, $argnum);
  }
  if (newmem & SWIG_CAST_NEW_MEMORY) {
    tempshared = *%reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *);
    delete %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *);
    $1 = %const_cast(tempshared.get(), $1_ltype);
  } else {
    smartarg = %reinterpret_cast(argp, SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *);
    $1 = %const_cast((smartarg ? smartarg->get() : 0), $1_ltype);
  }  
%}

%typemap(out, fragment="SWIG_null_deleter_python") CONST TYPE * {
  //#if ($owner)
  //if ($1) {
  //  intrusive_ptr_add_ref($1);
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1, SWIG_intrusive_deleter< CONST TYPE >());
  //} else {
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = 0;
  //}
  //#else
  //  *(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > **)&$result = $1 ? new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1 SWIG_NO_NULL_DELETER_0) : 0;
  //#endif
if ($owner) {
  if ($1) {
	  intrusive_ptr_add_ref($1);
  }
  SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartresult = $1 ? new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1, SWIG_intrusive_deleter< CONST TYPE >()) : 0;
  %set_output(SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), $owner | SWIG_POINTER_OWN));
} else {
	SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE > *smartresult = $1 ? new SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >($1 SWIG_NO_NULL_DELETER_0) : 0;
  %set_output(SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< TYPE > *), $owner | SWIG_POINTER_OWN));
}
}

%template() SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< CONST TYPE >;
%template() SWIG_INTRUSIVE_PTR_QNAMESPACE::intrusive_ptr< CONST TYPE >;

%enddef

