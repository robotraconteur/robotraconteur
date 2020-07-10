%typemap("javacode") RobotRaconteur::BroadcastDownsampler
%{
    public <T> void addPipeBroadcaster(PipeBroadcaster<T> broadcaster)
    {
        _AddPipeBroadcaster(broadcaster.innerpipe);
    }

    public <T> void addWireBroadcaster(WireBroadcaster<T> broadcaster)
    {
        _AddWireBroadcaster(broadcaster.innerwire);
    }

    public BroadcastDownsampler(ServerContext context)
    {
        this();
        _Init(context);
    }

    public BroadcastDownsampler(ServerContext context, int default_downsample)
    {
        this();
        _Init(context,default_downsample);
    }

%}

%javamethodmodifiers RobotRaconteur::BroadcastDownsampler::BroadcastDownsampler "private"

%rename(_Init) RobotRaconteur::BroadcastDownsampler::Init;

%include "BroadcastDownsampler.i"