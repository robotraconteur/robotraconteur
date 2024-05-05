%typemap(csclassmodifiers) RobotRaconteur::UserAuthenticator "public partial class";
%typemap(csinterfaces) RobotRaconteur::UserAuthenticator "IUserAuthenticator, IDisposable";
%csmethodmodifiers AuthenticateUser "protected virtual";

%typemap(csclassmodifiers) RobotRaconteur::ServiceSecurityPolicy "public partial class";
%csmethodmodifiers ServiceSecurityPolicy "protected";

%include "ServiceSecurity.i"
