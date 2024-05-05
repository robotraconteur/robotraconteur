%javamethodmodifiers RobotRaconteur::CommandLineConfigParser::ParseCommandLine "private";
%rename(_ParseCommandLine) RobotRaconteur::CommandLineConfigParser::ParseCommandLine;

%typemap(javacode) RobotRaconteur::CommandLineConfigParser
%{
    public void parseCommandLine(String[] args)
    {
        vectorstring args2 = new vectorstring(args);
        _ParseCommandLine(args2);
    }
%}

%include "NodeSetup.i"
