#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdio.h>
#include <boost/enable_shared_from_this.hpp>

#include "simple_webcam_impl.h"

#pragma once

class Webcam_name
{
  public:
    int index;
    std::string name;
};

// Class that implemnets the "WebcamHost" object abstract class
class WebcamHost_impl : public ::experimental::simplewebcam3::WebcamHost_default_impl
{
  public:
    WebcamHost_impl(std::vector<Webcam_name> names);
    ~WebcamHost_impl();
    void shutdown();

    virtual RobotRaconteur::RRMapPtr<int32_t, RobotRaconteur::RRArray<char> > get_webcam_names() override;

    virtual ::experimental::simplewebcam3::WebcamPtr get_webcams(int32_t ind) override;

  private:
    std::vector<RR_SHARED_PTR<Webcam_impl> > cameras;

    boost::mutex this_lock;
};
