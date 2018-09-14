%typemap(csclassmodifiers) MessageElementDataUtil "public partial class";
%typemap(csclassmodifiers) RobotRaconteur::MessageElement "public partial class";
%typemap(csclassmodifiers) RobotRaconteur::MessageEntry "public partial class";

%include "Message.i"