%typemap(csclassmodifiers) MessageElementDataUtil "public partial class";
%typemap(csclassmodifiers) RobotRaconteur::MessageElement "public partial class";
%typemap(csclassmodifiers) RobotRaconteur::MessageEntry "public partial class";

%apply double INPUT[]  { double* array_in }
%apply double OUTPUT[]  { double* array_out }
%apply float INPUT[]  { float* array_in }
%apply float OUTPUT[]  { float* array_out }
%apply int8_t INPUT[]  { int8_t* array_in }
%apply int8_t OUTPUT[]  { int8_t* array_out }
%apply uint8_t INPUT[]  { uint8_t* array_in }
%apply uint8_t OUTPUT[]  { uint8_t* array_out }
%apply int16_t INPUT[]  { int16_t* array_in }
%apply int16_t OUTPUT[]  { int16_t* array_out }
%apply uint16_t INPUT[]  { uint16_t* array_in }
%apply uint16_t OUTPUT[]  { uint16_t* array_out }
%apply int32_t INPUT[]  { int32_t* array_in }
%apply int32_t OUTPUT[]  { int32_t* array_out }
%apply uint32_t INPUT[]  { uint32_t* array_in }
%apply uint32_t OUTPUT[]  { uint32_t* array_out }
%apply int64_t INPUT[]  { int64_t* array_in }
%apply int64_t OUTPUT[]  { int64_t* array_out }
%apply uint64_t INPUT[]  { uint64_t* array_in }
%apply uint64_t OUTPUT[]  { uint64_t* array_out }

%include "Message.i"