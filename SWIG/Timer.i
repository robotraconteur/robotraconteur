// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

%shared_ptr(RobotRaconteur::Timer);
%shared_ptr(RobotRaconteur::WallTimer);
%shared_ptr(RobotRaconteur::Rate);
%shared_ptr(RobotRaconteur::WallRate);
%shared_ptr(RobotRaconteur::AutoResetEvent);
%feature("director") RobotRaconteur::AsyncTimerEventReturnDirector;

namespace RobotRaconteur
{


struct TimerEvent
{
	%immutable stopped; 
	bool stopped;
	%immutable last_expected; 
	boost::posix_time::ptime last_expected;
	%immutable last_real;
	boost::posix_time::ptime last_real;
	%immutable current_expected;
	boost::posix_time::ptime current_expected;
	%immutable current_real;
	boost::posix_time::ptime current_real;
};

class AsyncTimerEventReturnDirector
{
public:
	virtual ~AsyncTimerEventReturnDirector() {}
	virtual void handler(const RobotRaconteur::TimerEvent& ret, HandlerErrorInfo& error) = 0;
};

class Timer
{
public:

	virtual void Start()=0;

	virtual void Stop()=0;

	virtual boost::posix_time::time_duration GetPeriod()=0;

	virtual void SetPeriod(const boost::posix_time::time_duration& period)=0;

	virtual bool IsRunning()=0;
	
	virtual ~Timer() {}

};

class WallTimer : public Timer
{
public:
	
	
	virtual void Start();

	virtual void Stop();
	
	virtual boost::posix_time::time_duration GetPeriod();

	virtual void SetPeriod(const boost::posix_time::time_duration& period);

	virtual bool IsRunning();

	virtual ~WallTimer() {}

};

class Rate
{
public:
RR_RELEASE_GIL()
	virtual void Sleep()=0;
RR_KEEP_GIL()

	virtual ~Rate() {};
};

class WallRate : public Rate
{
public:
	
	WallRate(double frequency, boost::shared_ptr<RobotRaconteurNode> node=boost::shared_ptr<RobotRaconteurNode>());
	
RR_RELEASE_GIL()
	virtual void Sleep();
RR_KEEP_GIL()
	
	virtual ~WallRate() {}
};

class AutoResetEvent
{
public:

	AutoResetEvent();
	virtual ~AutoResetEvent();

	virtual void Set();

	virtual void Reset();
RR_RELEASE_GIL()
	virtual void WaitOne();
	
	virtual bool WaitOne(int32_t timeout);
RR_KEEP_GIL()
};

}