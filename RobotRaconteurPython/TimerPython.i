namespace RobotRaconteur
{

%feature(shadow) WallTimer::WallTimer(const boost::posix_time::time_duration& period, bool oneshot, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node, RobotRaconteur::AsyncTimerEventReturnDirector* handler, int32_t id)
%{
	def __init__(self,*args):
		#This is a shadowed function
		
		from .RobotRaconteurPythonUtil import AsyncTimerEventReturnDirectorImpl

		if (not hasattr(args[1], '__call__')): raise Exception("Handler must be callable")
		director=AsyncTimerEventReturnDirectorImpl(args[1])
		
		oneshot=False
		
		if (len(args) > 2):
			oneshot=args[2]
		
		if (len(args) > 3):
			node=args[3]
		else:
			node=RobotRaconteurNode.s
		
		this = _RobotRaconteurPython.new_WallTimer(args[0],oneshot,node,director,0)
		director.__disown__()
		try: self.this.append(this)
		except: self.this = this
%}

}

%include "Timer.i"

%extend RobotRaconteur::WallTimer
{
	WallTimer(const boost::posix_time::time_duration& period, bool oneshot, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node, RobotRaconteur::AsyncTimerEventReturnDirector* handler, int32_t id )
	{
		boost::shared_ptr<AsyncTimerEventReturnDirector> sphandler(handler,boost::bind(&ReleaseDirector<AsyncTimerEventReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),id));
		return new WallTimer(period,boost::bind(&TimerHandlerFunc,RR_BOOST_PLACEHOLDERS(_1),sphandler),oneshot,node);
	}
}