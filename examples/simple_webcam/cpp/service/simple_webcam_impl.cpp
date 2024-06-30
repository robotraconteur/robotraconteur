#include "simple_webcam_impl.h"

// Implementation for a simple webcam service object

// Namespace alias for convenience. Do not use in header files!
namespace RR = RobotRaconteur;
namespace cam = ::experimental::simplewebcam3;

// Initialize the webcam and buffer objects
Webcam_impl::Webcam_impl(int cam, std::string name)
{
    capture = RR_MAKE_SHARED<cv::VideoCapture>(cam);
    if (!capture)
        throw std::runtime_error("Could not open camera");
    camopen = true;

    capture->set(cv::CAP_PROP_FRAME_WIDTH, 320);
    capture->set(cv::CAP_PROP_FRAME_HEIGHT, 240);

    m_buffer = RR::AllocateRRArray<uint8_t>(0);
    m_multidimbuffer = RR::AllocateEmptyRRMultiDimArray<uint8_t>({0});
    m_Name = name;
    streaming_timer.reset();
}

// Shutdown the webcams
void Webcam_impl::shutdown()
{
    if (camopen)
    {
        camopen = false;
    }
    streaming_timer.reset();
}
Webcam_impl::~Webcam_impl() { shutdown(); }

// Read the Webcam Name property
std::string Webcam_impl::get_name() { return m_Name; }

// Capture a frame and return a WebcamImage structure
cam::WebcamImagePtr Webcam_impl::capture_frame()
{
    boost::mutex::scoped_lock lock(this_lock);

    cam::WebcamImagePtr image(new cam::WebcamImage());
    cv::Mat mat;
    capture->read(mat);
    image->width = mat.cols;
    image->height = mat.rows;
    image->step = mat.step;
    image->data = RR::AttachRRArrayCopy((uint8_t*)mat.data, mat.step * mat.rows);
    return image;
}

// Start streaming webcam images to provide a live view
void Webcam_impl::start_streaming()
{
    boost::mutex::scoped_lock lock(this_lock);
    if (streaming_timer)
        throw RR::InvalidOperationException("Already streaming");
    RR_WEAK_PTR<Webcam_impl> weak_this = shared_from_this();
    streaming_timer =
        RR::RobotRaconteurNode::s()->CreateTimer(boost::posix_time::milliseconds(100), [weak_this](RR::TimerEvent ev) {
            auto shared_this = weak_this.lock();
            if (!shared_this)
                return;
            shared_this->send_frame_stream();
        });
    streaming_timer->Start();
}

// Stop streaming webcam images
void Webcam_impl::stop_streaming()
{
    boost::mutex::scoped_lock lock(this_lock);
    if (!streaming_timer)
        throw RR::InvalidOperationException("Not streaming");
    streaming_timer->TryStop();
    streaming_timer.reset();
}

void Webcam_impl::send_frame_stream()
{
    auto frame = capture_frame();
    this->rrvar_frame_stream->SendPacket(frame);
}

// Capture a frame and store in the "buffer" and "multidimbuffer" memories
cam::WebcamImage_sizePtr Webcam_impl::capture_frame_to_buffer()
{

    cam::WebcamImagePtr image = capture_frame();

    m_buffer = image->data;

    // Rearrange the data into the correct format for MATLAB arrays
    RR::RRMultiDimArrayPtr<uint8_t> mdbuf =
        RR::AllocateEmptyRRMultiDimArray<uint8_t>({(uint32_t)image->height, (uint32_t)image->width, 3});
    auto& mdata = *mdbuf->Array;
    auto& image_data = *image->data;

    for (int channel = 0; channel < 3; channel++)
    {
        int channel0 = image->height * image->width * channel;
        for (int x = 0; x < image->width; x++)
        {
            for (int y = 0; y < image->height; y++)
            {
                uint8_t value = image_data[(y * image->step + x * 3) + (2 - channel)];
                mdata[channel0 + x * image->height + y] = value;
            }
        }
    }
    m_multidimbuffer = mdbuf;

    // Return a WebcamImage_size structure to the client
    cam::WebcamImage_sizePtr size(new cam::WebcamImage_size());
    size->width = image->width;
    size->height = image->height;
    size->step = image->step;
    return size;
}

// Return an ArrayMemory for the "buffer" data containing the image.
RR::ArrayMemoryPtr<uint8_t> Webcam_impl::get_buffer()
{
    // In many cases this ArrayMemory would not be initialized every time,
    // but for this example return a new ArrayMemory
    return RR_MAKE_SHARED<RR::ArrayMemory<uint8_t> >(m_buffer);
}

// Return a MultiDimArray for the "multidimbuffer" data containing the image
RR::MultiDimArrayMemoryPtr<uint8_t> Webcam_impl::get_multidimbuffer()
{
    // In many cases this MultiDimArrayMemory would not be initialized every time,
    // but for this example return a new MultiDimArrayMemory
    return RR_MAKE_SHARED<RR::MultiDimArrayMemory<uint8_t> >(m_multidimbuffer);
}

// Override the RRServiceObjectInit function to set the maximum backlog for the FrameStream
// RRSerivceObjectInit is called after the service object is initialized by the Robot Raconteur node
void Webcam_impl::RRServiceObjectInit(RR_WEAK_PTR<RR::ServerContext> ctx, const std::string& service_path)
{
    this->rrvar_frame_stream->SetMaxBacklog(3);
}
