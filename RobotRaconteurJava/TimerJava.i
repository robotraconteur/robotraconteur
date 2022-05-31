%typemap(javaclassmodifiers) WallTimer_initstruct "class";

%inline {
struct WallTimer_initstruct
{
	RobotRaconteur::AsyncTimerEventReturnDirector* handler;
	int32_t id;
};

}

%typemap("javacode") RobotRaconteur::WallTimer
%{
	public WallTimer(int period, Action1<TimerEvent> handler, boolean oneshot)
	{
             this(period,oneshot,null,attachHandler(handler));
        
            

            
        }

	public WallTimer(int period, Action1<TimerEvent> handler, boolean oneshot, RobotRaconteurNode node)
	{
            this(period,oneshot,node,attachHandler(handler));
        
            

            
        }

        private static WallTimer_initstruct attachHandler(Action1<TimerEvent> handler)
        {
            WallTimer_initstruct s = new WallTimer_initstruct();
            AsyncTimerEventReturnDirectorImpl h=new AsyncTimerEventReturnDirectorImpl(handler);
            s.setHandler(h);
            s.setId(RRObjectHeap.addObject(h));
            return s;
        }
%}

%include "Timer.i"


	
%extend RobotRaconteur::WallTimer
{

%javamethodmodifiers WallTimer "";

WallTimer(const boost::posix_time::time_duration& period, bool oneshot, const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node, WallTimer_initstruct init )
{
	boost::shared_ptr<AsyncTimerEventReturnDirector> sphandler(init.handler,boost::bind(&ReleaseDirector<AsyncTimerEventReturnDirector>,RR_BOOST_PLACEHOLDERS(_1),init.id));
	return new WallTimer(period,boost::bind(&TimerHandlerFunc,RR_BOOST_PLACEHOLDERS(_1),sphandler),oneshot,node);
}
}
	
	