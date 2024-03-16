%typemap("cscode") RobotRaconteur::BroadcastDownsampler
%{
    public void AddPipeBroadcaster<T>(PipeBroadcaster<T> broadcaster)
    {
        _AddPipeBroadcaster(broadcaster.innerpipe);
    }

    public void AddWireBroadcaster<T>(WireBroadcaster<T> broadcaster)
    {
        _AddWireBroadcaster(broadcaster.innerwire);
    }

    public BroadcastDownsampler(ServerContext context, uint default_downsample) : this()
    {
        _Init(context, default_downsample);
    }

%}

%csmethodmodifiers RobotRaconteur::BroadcastDownsampler::BroadcastDownsampler "private"

%rename(_Init) RobotRaconteur::BroadcastDownsampler::Init;

%include "BroadcastDownsampler.i"
