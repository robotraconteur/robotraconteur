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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/RobotRaconteurNode.h"

namespace RobotRaconteur
{	
	void WallTimer::timer_handler(const boost::system::error_code& ec)
	{
		TimerEvent ev;

		RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
		if (!n) throw InvalidOperationException("Node released");

		boost::function<void (const TimerEvent&)> h;

		{

			boost::mutex::scoped_lock lock(running_lock);
		
			if (ec)
			{
				running=false;
			}
			ev.stopped=!running;
			ev.last_expected=last_time;
			ev.last_real=actual_last_time;
			ev.current_expected=last_time+period;
			ev.current_real=n->NowNodeTime();
			h=handler;

			if (oneshot)
			{
				handler.clear();
			} 

			if (oneshot)
			{
				running=false;
			}
		}

		

		try
		{
			if (h) h(ev);
		}
		catch (std::exception& exp)
		{
			n->HandleException(&exp);
		}

		boost::mutex::scoped_lock lock(running_lock);
		if (!oneshot)
		{
			if (running)
			{
				last_time=ev.current_expected;
				actual_last_time=ev.current_real;
				
				while (last_time + period < actual_last_time)
				{
					last_time += period;
				}
				
				timer->expires_at(last_time + period);				
				RobotRaconteurNode::asio_async_wait(node, timer, boost::bind(&WallTimer::timer_handler,shared_from_this(),boost::asio::placeholders::error));
			}

		}
		else
		{
			running=false;
			timer.reset();
			
		}

	}

	WallTimer::WallTimer(const boost::posix_time::time_duration& period, boost::function<void (const TimerEvent&)> handler, bool oneshot, RR_SHARED_PTR<RobotRaconteurNode> node) 
	{
		this->period=period;
		this->oneshot=oneshot;
		this->handler=handler;
		running=false;
		if (!node) node=RobotRaconteurNode::sp();
		this->node=node;
	}

	void WallTimer::Start()
	{
		boost::mutex::scoped_lock lock(running_lock);
		if (running) throw InvalidOperationException("Already running");

		if (!handler) throw InvalidOperationException("Timer has expired");

		
		RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
		if (!n) throw InvalidOperationException("Node released");

		start_time=n->NowNodeTime();
		last_time=start_time;
		actual_last_time=last_time;

		timer.reset(new boost::asio::deadline_timer(n->GetThreadPool()->get_io_context()));

		timer->expires_at(last_time+period);
		if (!RobotRaconteurNode::asio_async_wait(node, timer, boost::bind(&WallTimer::timer_handler, shared_from_this(), boost::asio::placeholders::error)))
		{
			throw InvalidOperationException("Node released");
		}
		running=true;
	}

	void WallTimer::Stop()
	{
		boost::mutex::scoped_lock lock(running_lock);
		if (!running) throw InvalidOperationException("Not running");

		try
		{
			timer->cancel();
		}
		catch (std::exception&) {}
		timer.reset();
		running=false;

		if (oneshot) handler.clear();
		
	}

	boost::posix_time::time_duration WallTimer::GetPeriod()
	{
		boost::mutex::scoped_lock lock(running_lock);
		return this->period;
	}

	void WallTimer::SetPeriod(const boost::posix_time::time_duration& period)
	{
		boost::mutex::scoped_lock lock(running_lock);
		this->period=period;
	}

	bool WallTimer::IsRunning()
	{
		boost::mutex::scoped_lock lock(running_lock);
		return running;
	}

	void WallTimer::Clear()
	{
		boost::mutex::scoped_lock lock(running_lock);
		handler.clear();
	}


	WallRate::WallRate(double frequency, RR_SHARED_PTR<RobotRaconteurNode> node) : timer(node->GetThreadPool()->get_io_context())
	{
		if (!node) node=RobotRaconteurNode::sp();
		this->node=node;
		this->period=boost::posix_time::microseconds(boost::lexical_cast<int64_t>(1000000.0/frequency));
		start_time=node->NowNodeTime();
		last_time=node->NowNodeTime();
	}
		
	void WallRate::Sleep()
	{
		boost::posix_time::ptime p2=last_time+period;
		timer.expires_at(p2);
		timer.wait();
		last_time=p2;
	}
}
