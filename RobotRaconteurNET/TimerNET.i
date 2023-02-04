
%typemap(csclassmodifiers) WallTimer_initstruct "internal class";

%inline {
struct WallTimer_initstruct
{
	RobotRaconteur::AsyncTimerEventReturnDirector* handler;
	int32_t id;
};

}

%typemap(csclassmodifiers) RobotRaconteur::WallTimer "public partial class";

%include "Timer.i"

%extend RobotRaconteur::WallTimer
{
%csmethodmodifiers WallTimer "internal";

WallTimer(const boost::posix_time::time_duration& period, bool oneshot, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node, WallTimer_initstruct init )
{
	boost::shared_ptr<AsyncTimerEventReturnDirector> sphandler(init.handler,boost::bind(&ReleaseDirector<AsyncTimerEventReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),init.id));
	return new WallTimer(period,boost::bind(&TimerHandlerFunc,RR_BOOST_PLACEHOLDERS(_1),sphandler),oneshot,node);
}
}