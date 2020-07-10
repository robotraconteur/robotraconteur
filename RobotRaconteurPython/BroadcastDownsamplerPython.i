%feature("shadow") RobotRaconteur::BroadcastDownsampler::BroadcastDownsampler()
%{
def __init__(self,context,default_downsample = 0):
    #This is a shadowed function		
    _RobotRaconteurPython.BroadcastDownsampler_swiginit(self, _RobotRaconteurPython.new_BroadcastDownsampler())
    self._Init(context, default_downsample)
%}

%rename(_Init) RobotRaconteur::BroadcastDownsampler::Init;

%include "BroadcastDownsampler.i"

%extend RobotRaconteur::BroadcastDownsampler
{
%pythoncode %{

def AddPipeBroadcaster(self, broadcaster):
    self._AddPipeBroadcaster(broadcaster._innerpipe)

def AddWireBroadcaster(self, broadcaster):
    self._AddWireBroadcaster(broadcaster._innerpipe)

%}
}