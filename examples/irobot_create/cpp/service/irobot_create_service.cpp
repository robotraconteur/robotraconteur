#include <RobotRaconteur.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include "robotraconteur_generated.h"
#include <boost/asio/serial_port.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <map>
#include <cereal/archives/portable_binary.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <drekar_launch_process_cpp/drekar_launch_process_cpp.h>

// Only use namespace aliases in cpp files. Do not use aliases in header files.
namespace RR = RobotRaconteur;
namespace c3 = experimental::create3;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CREATE_OI_OP_START 128
#define CREATE_OI_OP_FULL 132
#define CREATE_OI_OP_DRIVE 137
#define CREATE_OI_OP_LEDS 139
#define CREATE_OI_OP_SONG 140
#define CREATE_OI_OP_DRIVE_DIRECT 145
#define CREATE_OI_OP_STREAM 148
#define CREATE_OI_OP_STREAM_RESUME 150

#define CREATE_OI_FLAGS_BUMP_RIGHT 0x1
#define CREATE_OI_FLAGS_BUMP_LEFT 0x2
#define CREATE_OI_FLAGS_WHEEL_DROP_RIGHT 0x4
#define CREATE_OI_FLAGS_WHEEL_DROP_LEFT 0x8
#define CREATE_OI_FLAGS_WHEEL_DROP_CASTER 0x10

#define CREATE_OI_FLAGS_PLAY_BUTTON 0x1
#define CREATE_OI_FLAGS_ADVANCE_BUTTON 0x4

#define CREATE_OI_SET_LED_PLAY 0x2
#define CREATE_OI_SET_LED_ADVANCE 0x8

class Create_impl : public c3::Create_default_impl, public boost::enable_shared_from_this<Create_impl>
{
  public:
    Create_impl() {}

    void Init(std::string port)
    {
        boost::mutex::scoped_lock lock(this_lock);

        serial_port.reset(
            new boost::asio::serial_port(RR::RobotRaconteurNode::s()->GetThreadPool()->get_io_context(), port));

        if (!serial_port->is_open())
        {
            throw std::runtime_error("Could not open port");
        }

        serial_port->set_option(boost::asio::serial_port::baud_rate(57600));
        serial_port->set_option(boost::asio::serial_port::stop_bits());
        serial_port->set_option(boost::asio::serial_port::flow_control());
        serial_port->set_option(boost::asio::serial_port::character_size());

        std::vector<uint8_t> start_cmd = {
            CREATE_OI_OP_START, CREATE_OI_OP_FULL, CREATE_OI_OP_STREAM_RESUME, 0, CREATE_OI_OP_LEDS, 0, 80, 255};
        boost::asio::write(*serial_port, boost::asio::buffer(start_cmd));

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));

        lock.unlock();
        StartStreaming();
    }

    void Shutdown()
    {
        StopStreaming();

        boost::mutex::scoped_lock lock(this_lock);

        std::vector<uint8_t> stop_cmd = {CREATE_OI_OP_START};
        boost::asio::write(*serial_port, boost::asio::buffer(stop_cmd));

        serial_port->close();
    }

    void StartStreaming()
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (streaming)
            return;

        streaming = true;

        // Start the thread that will receive serial data
        auto shared_this = shared_from_this();
        boost::thread([shared_this]() { shared_this->recv_thread_func(); });

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));

        std::vector<uint8_t> command = {CREATE_OI_OP_STREAM, 1, 6};
        boost::asio::write(*serial_port, boost::asio::buffer(command));
    }

    void StopStreaming()
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (!streaming)
            return;

        streaming = false;

        std::vector<uint8_t> command = {CREATE_OI_OP_STREAM_RESUME, 0};
        boost::asio::write(*serial_port, boost::asio::buffer(command));
    }

    void drive(double velocity, double radius) override
    {
        boost::mutex::scoped_lock lock(this_lock);

        int16_t v = boost::numeric_cast<int16_t>(velocity * 1000);
        int16_t r = boost::numeric_cast<int16_t>(radius * 1000);

        // Use cereal PortableBinary to serialize the data
        std::stringstream ss;
        cereal::PortableBinaryOutputArchive ar(ss, cereal::PortableBinaryOutputArchive::Options::BigEndian());
        // opcode
        ar((uint8_t)CREATE_OI_OP_DRIVE);
        ar(v);
        ar(r);

        std::string s = ss.str();
        boost::asio::write(*serial_port, boost::asio::buffer(s));
    }

    void drive_direct(double right_velocity, double left_velocity) override
    {
        boost::mutex::scoped_lock lock(this_lock);

        int16_t vr = boost::numeric_cast<int16_t>(right_velocity * 1000);
        int16_t vl = boost::numeric_cast<int16_t>(left_velocity * 1000);

        // Use cereal PortableBinary to serialize the data
        std::stringstream ss;
        cereal::PortableBinaryOutputArchive ar(ss, cereal::PortableBinaryOutputArchive::Options::BigEndian());
        // opcode
        ar((uint8_t)CREATE_OI_OP_DRIVE_DIRECT);
        ar(vr);
        ar(vl);

        std::string s = ss.str();
        boost::asio::write(*serial_port, boost::asio::buffer(s));
    }

    void stop() override { drive(0, 0); }

    void setf_leds(RR::rr_bool play, RR::rr_bool advance) override
    {
        boost::mutex::scoped_lock lock(this_lock);

        uint8_t bits = 0;
        if (play.value)
            bits |= CREATE_OI_SET_LED_PLAY;
        if (advance.value)
            bits |= CREATE_OI_SET_LED_ADVANCE;

        std::vector<uint8_t> command = {CREATE_OI_OP_LEDS, bits, 80, 255};
        boost::asio::write(*serial_port, boost::asio::buffer(command));
    }

    void claim_play_callback() override
    {
        boost::mutex::scoped_lock lock(this_lock);

        play_client = RR::ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint();
    }

  protected:
    void recv_thread_func()
    {
        size_t checksum_error_count = 0;
        try
        {
            while (streaming)
            {
                uint8_t magic;
                boost::asio::read(*serial_port, boost::asio::buffer(&magic, 1));

                if (magic != 19)
                    continue;

                uint8_t nbytes;
                boost::asio::read(*serial_port, boost::asio::buffer(&nbytes, 1));

                std::vector<uint8_t> data(nbytes);
                boost::asio::read(*serial_port, boost::asio::buffer(&data[0], nbytes));

                uint8_t checksum;
                boost::asio::read(*serial_port, boost::asio::buffer(&checksum, 1));

                uint32_t checksum_calc = 19 + nbytes;
                for (auto& b : data)
                {
                    checksum_calc += b;
                }
                checksum_calc += checksum;
                checksum_calc &= 0xff;

                if (checksum_calc != 0)
                {
                    checksum_error_count++;
                    if (checksum_error_count > 20)
                    {
                        std::cout << "Checksum error" << std::endl;
                        checksum_error_count = 0;
                    }
                    continue;
                }

                auto state = ParseSensorPackets(data);
                {
                    boost::mutex::scoped_lock lock(this_lock);
                    if (this->rrvar_create_state)
                    {
                        this->rrvar_create_state->SetOutValue(state);
                    }

                    if (state->create_state_flags &
                        (c3::CreateStateFlags::bump_right | c3::CreateStateFlags::bump_left))
                    {
                        if (!bump_fired)
                        {
                            this->FireBump();
                            bump_fired = true;
                        }
                    }
                    else
                    {
                        bump_fired = false;
                    }

                    rrvar_distance_traveled = state->distance_traveled;
                    rrvar_angle_traveled = state->angle_traveled;
                    rrvar_bumpers = state->create_state_flags &
                                    (c3::CreateStateFlags::bump_right | c3::CreateStateFlags::bump_left);

                    if (state->create_state_flags & c3::CreateStateFlags::play_button)
                    {
                        if (!play_pressed)
                        {
                            lock.unlock();
                            this->Play();
                            lock.lock();
                            play_pressed = true;
                        }
                    }
                    else
                    {
                        play_pressed = false;
                    }
                }
            }
        }
        catch (std::exception& exp)
        {
            std::cout << "Error in recv_thread_func: " << exp.what() << std::endl;
        }
    }

    c3::CreateStatePtr ParseSensorPackets(const std::vector<uint8_t>& data)
    {
        c3::CreateStatePtr ret(new c3::CreateState());

        // Use cereal PortableBinary to deserialize the data
        std::stringstream ss;
        ss.write((const char*)data.data(), data.size());
        ss.seekg(0);
        cereal::PortableBinaryInputArchive ar(ss, cereal::PortableBinaryInputArchive::Options::BigEndian());

        uint8_t bump_flags_b;
        ar(bump_flags_b);
        uint8_t wall_b;
        ar(wall_b);
        uint8_t cliff_left_b;
        ar(cliff_left_b);
        uint8_t cliff_front_left_b;
        ar(cliff_front_left_b);
        uint8_t cliff_front_right_b;
        ar(cliff_front_right_b);
        uint8_t cliff_right_b;
        ar(cliff_right_b);
        uint8_t virtual_wall_b;
        ar(virtual_wall_b);
        // skip four bytes
        uint8_t dummy;
        for (int i = 0; i < 4; i++)
        {
            ar(dummy);
        }

        uint8_t buttons_b;
        ar(buttons_b);
        int16_t distance_h;
        ar(distance_h);
        int16_t angle_h;
        ar(angle_h);

        // skip six bytes
        for (int i = 0; i < 6; i++)
        {
            ar(dummy);
        }

        uint16_t charge_H;
        ar(charge_H);
        uint16_t capacity_H;
        ar(capacity_H);

        // skip 18 bytes
        for (int i = 0; i < 18; i++)
        {
            ar(dummy);
        }

        int16_t velocity_h;
        ar(velocity_h);
        int16_t radius_h;
        ar(radius_h);
        int16_t velocity_right_h;
        ar(velocity_right_h);
        int16_t velocity_left_h;
        ar(velocity_left_h);

        int32_t state_flags = 0;
        if (bump_flags_b & CREATE_OI_FLAGS_BUMP_RIGHT)
        {
            state_flags |= c3::CreateStateFlags::bump_right;
        }
        if (bump_flags_b & CREATE_OI_FLAGS_BUMP_LEFT)
        {
            state_flags |= c3::CreateStateFlags::bump_left;
        }
        if (bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_RIGHT)
        {
            state_flags |= c3::CreateStateFlags::wheel_drop_right;
        }
        if (bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_LEFT)
        {
            state_flags |= c3::CreateStateFlags::wheel_drop_left;
        }
        if (bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_CASTER)
        {
            state_flags |= c3::CreateStateFlags::wheel_drop_caster;
        }

        if (wall_b != 0)
        {
            state_flags |= c3::CreateStateFlags::wall_sensor;
        }

        if (cliff_left_b != 0)
        {
            state_flags |= c3::CreateStateFlags::cliff_left;
        }
        if (cliff_front_left_b != 0)
        {
            state_flags |= c3::CreateStateFlags::cliff_front_left;
        }
        if (cliff_front_right_b != 0)
        {
            state_flags |= c3::CreateStateFlags::cliff_front_right;
        }
        if (cliff_right_b != 0)
        {
            state_flags |= c3::CreateStateFlags::cliff_right;
        }

        if (virtual_wall_b != 0)
        {
            state_flags |= c3::CreateStateFlags::virtual_wall;
        }

        if ((buttons_b & CREATE_OI_FLAGS_PLAY_BUTTON) != 0)
        {
            state_flags |= c3::CreateStateFlags::play_button;
        }

        if ((buttons_b & CREATE_OI_FLAGS_ADVANCE_BUTTON) != 0)
        {
            state_flags |= c3::CreateStateFlags::advance_button;
        }

        ret->create_state_flags = state_flags;

        RR::TimeSpec time = RR::RobotRaconteurNode::s()->NowTimeSpec();

        ret->time = boost::numeric_cast<double>(time.seconds) + (boost::numeric_cast<double>(time.nanoseconds) / 1e9);
        ret->velocity = boost::numeric_cast<double>(velocity_h) * 1e-3;
        ret->radius = boost::numeric_cast<double>(radius_h) * 1e-3;
        ret->right_wheel_velocity = boost::numeric_cast<double>(velocity_right_h) * 1e-3;
        ret->left_wheel_velocity = boost::numeric_cast<double>(velocity_left_h) * 1e-3;
        ret->distance_traveled = boost::numeric_cast<double>(distance_h) * 1e-3;
        ret->angle_traveled = boost::numeric_cast<double>(angle_h) * (M_PI / 180.0);
        ret->battery_charge = boost::numeric_cast<double>(charge_H);
        ret->battery_capacity = boost::numeric_cast<double>(capacity_H);

        return ret;
    }

    void FireBump() { rrvar_bump(); }

    void Play()
    {
        if (play_client == 0)
        {
            return;
        }

        try
        {
            if (!rrvar_play_callback)
            {
                return;
            }
            auto cb_func = rrvar_play_callback->GetClientFunction(play_client);
            auto notes = cb_func(rrvar_distance_traveled, rrvar_angle_traveled);

            std::vector<uint8_t> command = {CREATE_OI_OP_SONG, 0, (uint8_t)(notes->size() / 2)};
            boost::range::copy(*notes, std::back_inserter(command));
            command.push_back(141);
            command.push_back(0);

            boost::mutex::scoped_lock lock(this_lock);
            boost::asio::write(*serial_port, boost::asio::buffer(command));
        }
        catch (std::exception& exp)
        {
            std::cout << "Error in Play: " << exp.what() << std::endl;
        }
    }

    boost::shared_ptr<boost::asio::serial_port> serial_port;
    bool bump_fired = false;
    bool play_pressed = false;
    bool streaming = false;

    uint32_t play_client = 0;
};

int main(int argc, char* argv[])
{
    std::string port = "/dev/ttyUSB0";
    if (argc > 1)
    {
        port = argv[1];
    }

    boost::shared_ptr<Create_impl> create = boost::make_shared<Create_impl>();
    create->Init(port);

    RR::ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.create3", 22354);

    auto ctx = RR::RobotRaconteurNode::s()->RegisterService("create", "experimental.create3", create);

    // Print out some info for the user
    std::cout << "iRobot Create CPP Service Started" << std::endl << std::endl;

    std::cout << "Candidate connection urls:" << std::endl;
    ctx->PrintCandidateConnectionURLs();
    std::cout << std::endl;
    std::cout << "Press Ctrl-C to quit" << std::endl;

    // Use drekar_launch_process_cpp package to wait for exit
    drekar_launch_process_cpp::CWaitForExit wait_exit;
    wait_exit.WaitForExit();

    create->Shutdown();

    return 0;
}
