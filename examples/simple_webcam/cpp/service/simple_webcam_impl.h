#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdio.h>
#include <boost/enable_shared_from_this.hpp>

#pragma once

// Class to implement the "Webcam" object abstract class
// and also use "enable_shared_from_this" for shared_ptr support
class Webcam_impl : public ::experimental::simplewebcam3::Webcam_default_impl,
                    public boost::enable_shared_from_this<Webcam_impl>,
                    public virtual RobotRaconteur::IRRServiceObject
{
  public:
    Webcam_impl(int cam, std::string name);
    void shutdown();
    ~Webcam_impl();

    void RRServiceObjectInit(RR_WEAK_PTR<RobotRaconteur::ServerContext> ctx, const std::string& service_path) override;

    virtual std::string get_name() override;

    virtual ::experimental::simplewebcam3::WebcamImagePtr capture_frame() override;

    virtual void start_streaming() override;

    virtual void stop_streaming() override;

    virtual ::experimental::simplewebcam3::WebcamImage_sizePtr capture_frame_to_buffer() override;

    virtual RobotRaconteur::ArrayMemoryPtr<uint8_t> get_buffer() override;

    virtual RobotRaconteur::MultiDimArrayMemoryPtr<uint8_t> get_multidimbuffer() override;

  private:
    RobotRaconteur::RRArrayPtr<uint8_t> m_buffer;
    RobotRaconteur::RRMultiDimArrayPtr<uint8_t> m_multidimbuffer;
    RR_SHARED_PTR<cv::VideoCapture> capture;

    bool camopen;
    std::string m_Name;
    RobotRaconteur::TimerPtr streaming_timer;

    void send_frame_stream();

    boost::mutex this_lock;
};
