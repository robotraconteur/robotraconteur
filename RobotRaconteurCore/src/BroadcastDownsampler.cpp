
#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/WireMember.h"
#include "RobotRaconteur/PipeMember.h"
#include "RobotRaconteur/BroadcastDownsampler.h"

namespace RobotRaconteur
{
    BroadcastDownsampler::BroadcastDownsampler()
    {
        step_count = 0;
    }

    void BroadcastDownsampler::Init(RR_SHARED_PTR<ServerContext> context)
    {
        this->context=context;       
        RR_WEAK_PTR<BroadcastDownsampler> weak_this = shared_from_this();

        context->ServerServiceListener.connect(boost::signals2::signal<void(
            RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>
        )>::slot_type(
            boost::bind(&BroadcastDownsampler::server_event, weak_this, RR_BOOST_PLACEHOLDERS(_1),
            RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3))
        ).track(shared_from_this()));
    }

    uint32_t BroadcastDownsampler::GetClientDownsample(uint32_t ep)
    {
        boost::mutex::scoped_lock lock(this_lock);
        RR_UNORDERED_MAP<uint32_t, uint32_t>::iterator e = client_downsamples.find(ep);
        if (e == client_downsamples.end())
            return 0;
        return e->second;
    }

    void BroadcastDownsampler::SetClientDownsample(uint32_t ep, uint32_t downsample)
    {
        boost::mutex::scoped_lock lock(this_lock);
        client_downsamples[ep] = downsample;
    }

    void BroadcastDownsampler::BeginStep()
    {
        boost::mutex::scoped_lock lock(this_lock);
        ++step_count;
    }

    void BroadcastDownsampler::EndStep()
    {

    }

    void BroadcastDownsampler::AddPipeBroadcaster(RR_SHARED_PTR<PipeBroadcasterBase> broadcaster)
    {
        RR_WEAK_PTR<BroadcastDownsampler> weak_this = shared_from_this();
        boost::function<bool(RR_SHARED_PTR<PipeBroadcasterBase>&, uint32_t, int32_t) > pred =
            boost::bind(&BroadcastDownsampler::pipe_predicate, weak_this, RR_BOOST_PLACEHOLDERS(_1),
            RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3));

        broadcaster->SetPredicate(pred);
    }

    void BroadcastDownsampler::AddWireBroadcaster(RR_SHARED_PTR<WireBroadcasterBase> broadcaster)
    {
        RR_WEAK_PTR<BroadcastDownsampler> weak_this = shared_from_this();
        boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t) > pred =
            boost::bind(&BroadcastDownsampler::wire_predicate, weak_this, RR_BOOST_PLACEHOLDERS(_1),
            RR_BOOST_PLACEHOLDERS(_2));

        broadcaster->SetPredicate(pred);
    }

    bool BroadcastDownsampler::wire_predicate(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<WireBroadcasterBase>& wire, uint32_t ep)
    {
        RR_SHARED_PTR<BroadcastDownsampler> this1 = this_.lock();
        if (!this1)
            return true;
        
        boost::mutex::scoped_lock lock(this1->this_lock);

        RR_UNORDERED_MAP<uint32_t, uint32_t>::iterator e = this1->client_downsamples.find(ep);
        if (e == this1->client_downsamples.end())
            return true;

        uint64_t step_count = this1->step_count;
        uint32_t downsample = e->second + 1;
        bool drop = (step_count % downsample) == 0;

        return drop;
    }

    bool BroadcastDownsampler::pipe_predicate(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<PipeBroadcasterBase>& wire, uint32_t ep, uint32_t index)
    {
        RR_SHARED_PTR<BroadcastDownsampler> this1 = this_.lock();
        if (!this1)
            return true;
        
        boost::mutex::scoped_lock lock(this1->this_lock);

        RR_UNORDERED_MAP<uint32_t, uint32_t>::iterator e = this1->client_downsamples.find(ep);
        if (e == this1->client_downsamples.end())
            return true;

        uint64_t step_count = this1->step_count;
        uint32_t downsample = e->second + 1;
        bool drop = (step_count % downsample) == 0;

        return drop;
    }

    void BroadcastDownsampler::server_event(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<ServerContext> ctx, ServerServiceListenerEventType evt, RR_SHARED_PTR<void> p)
    {
        if (evt != ServerServiceListenerEventType_ClientDisconnected)
            return;

        RR_SHARED_PTR<BroadcastDownsampler> this1 = this_.lock();
        if (!this1)
            return;
        
        boost::mutex::scoped_lock lock(this1->this_lock);
        RR_SHARED_PTR<uint32_t> ep1 = RR_STATIC_POINTER_CAST<uint32_t>(p);
        uint32_t ep = *ep1;

        this1->client_downsamples.erase(ep);
    }

    BroadcastDownsamplerStep::BroadcastDownsamplerStep(RR_SHARED_PTR<BroadcastDownsampler>& downsampler)
     : _downsampler(downsampler)
    {
        _downsampler->BeginStep();
    }
    
    BroadcastDownsamplerStep::~BroadcastDownsamplerStep()
    {
        _downsampler->EndStep();
    }
}