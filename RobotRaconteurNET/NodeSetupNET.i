
%typemap(csclassmodifiers) RobotRaconteur::RobotRaconteurNodeSetupFlags "[global::System.Flags] public enum";

%csmethodmodifiers RobotRaconteur::CommandLineConfigParser::ParseCommandLine "private";
%rename(_ParseCommandLine) RobotRaconteur::CommandLineConfigParser::ParseCommandLine;

%typemap(cscode) RobotRaconteur::CommandLineConfigParser
%{
    public void ParseCommandLine(string[] args)
    {
        vectorstring args2 = new vectorstring(args);
        _ParseCommandLine(args2);
    }
%}

%include "NodeSetup.i"
