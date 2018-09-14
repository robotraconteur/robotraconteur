
%typemap(csclassmodifiers) RobotRaconteur::RobotRaconteurNode "public partial class";
%nodefaultctor RobotRaconteur::RobotRaconteurNode;

%include "RobotRaconteurNode.i"