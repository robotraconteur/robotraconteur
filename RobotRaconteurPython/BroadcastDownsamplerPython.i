%feature("shadow") RobotRaconteur::BroadcastDownsampler::BroadcastDownsampler()
%{
def __init__(self,context,default_downsample = 0):
    """
    Initialize the BroadcastDownsampler

    :param context: The ServerContext of the service
    :type context: ServerContext
    :param default_downsample: The default downsample for clients
    :type default_downsample: int
    """
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
    """
    Add a PipeBroadcaster to the downsampler

    Sets the predicate of the broadcaster to this downsampler

    :param broadcaster: The pipe broadcaster to add
    :type broadcaster: PipeBroadcaster
    """
    self._AddPipeBroadcaster(broadcaster._innerpipe)

def AddWireBroadcaster(self, broadcaster):
    """
    Add a WireBroadcaster to the downsampler

    Sets the predicate of the broadcaster to this downsampler

    :param broadcaster: The pipe broadcaster to add
    :type broadcaster: PipeBroadcaster
    """
    self._AddWireBroadcaster(broadcaster._innerpipe)

%}
}
