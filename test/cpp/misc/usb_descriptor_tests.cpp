#include <gtest/gtest.h>
#include <RobotRaconteur.h>

#include <boost/thread.hpp>

#include "../../../RobotRaconteurCore/src/HardwareTransport_usbcommon_private.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::detail;

namespace
{
// Exposes the protected static ParseRRDescriptors for testing
class UsbDescriptorParser : public UsbDevice_Initialize
{
  public:
    using UsbDevice_Initialize::ParseRRDescriptors;
};

void push16(std::vector<uint8_t>& v, uint16_t x)
{
    v.push_back(static_cast<uint8_t>(x & 0xFF));
    v.push_back(static_cast<uint8_t>(x >> 8));
}

std::vector<uint8_t> rr_descriptors_header()
{
    std::vector<uint8_t> v;
    push16(v, 8);      // wLength
    push16(v, 0);      // wDescriptorType
    push16(v, 0x0100); // wRRVersionCode
    push16(v, 0);      // wTotalLength
    return v;
}

void push_interface_descriptor(std::vector<uint8_t>& v, uint16_t wLength, uint16_t wNumProtocols,
                               const std::vector<uint16_t>& protocols)
{
    push16(v, wLength);
    push16(v, RR_USB_INTERFACE_DESCRIPTOR_TYPE);
    push16(v, 0);      // wDescriptorSubType
    push16(v, 0x0100); // wVersion
    v.push_back(1);    // iNodeID
    v.push_back(2);    // iNodeName
    push16(v, wNumProtocols);
    for (size_t i = 0; i < protocols.size(); i++)
    {
        push16(v, protocols[i]);
    }
}

void push_unknown_descriptor(std::vector<uint8_t>& v)
{
    push16(v, 6);
    push16(v, 0x7F);
    push16(v, 0);
}

struct ParseResult
{
    UsbDeviceStatus status;
    RR_SHARED_PTR<UsbDevice_Settings> settings;
    boost::atomic<bool> done;
    std::vector<uint8_t> buf;
    ParseResult() : status(Error), done(false) {}
};

void run_parse(const RR_SHARED_PTR<ParseResult>& r)
{
    r->status = UsbDescriptorParser::ParseRRDescriptors(r->buf, r->settings);
    r->done.store(true);
}

// Runs ParseRRDescriptors on a separate thread so a parser that never returns fails the test instead of hanging
bool parse_with_timeout(const std::vector<uint8_t>& buf, UsbDeviceStatus& status,
                        RR_SHARED_PTR<UsbDevice_Settings>& settings)
{
    RR_SHARED_PTR<ParseResult> r = RR_MAKE_SHARED<ParseResult>();
    r->buf = buf;
    r->settings = RR_MAKE_SHARED<UsbDevice_Settings>();
    boost::thread t(boost::bind(&run_parse, r));
    if (!t.try_join_for(boost::chrono::seconds(5)))
    {
        t.detach();
        return false;
    }
    status = r->status;
    settings = r->settings;
    return true;
}
} // namespace

TEST(UsbDescriptors, ParseValidInterfaceDescriptor)
{
    std::vector<uint8_t> buf = rr_descriptors_header();
    std::vector<uint16_t> protocols;
    protocols.push_back(0x0101);
    protocols.push_back(0x0102);
    push_interface_descriptor(buf, 16, 2, protocols);
    push_unknown_descriptor(buf);

    UsbDeviceStatus status = Error;
    RR_SHARED_PTR<UsbDevice_Settings> settings;
    ASSERT_TRUE(parse_with_timeout(buf, status, settings));
    EXPECT_EQ(status, Open);
    EXPECT_EQ(settings->string_nodeid_index, 1);
    EXPECT_EQ(settings->string_nodename_index, 2);
    EXPECT_EQ(settings->supported_protocols, protocols);
}

TEST(UsbDescriptors, ParseZeroLengthDescriptor)
{
    // A sub-descriptor with wLength == 0 must not make the parser loop forever
    std::vector<uint8_t> buf = rr_descriptors_header();
    push16(buf, 0);
    push16(buf, 0x7F);
    push16(buf, 0);

    UsbDeviceStatus status = Open;
    RR_SHARED_PTR<UsbDevice_Settings> settings;
    ASSERT_TRUE(parse_with_timeout(buf, status, settings)) << "ParseRRDescriptors did not return";
    EXPECT_EQ(status, Error);
}

TEST(UsbDescriptors, ParseShortInterfaceDescriptor)
{
    // An interface descriptor shorter than its fixed fields must be rejected, not read past wLength
    std::vector<uint8_t> buf = rr_descriptors_header();
    push16(buf, 6);
    push16(buf, RR_USB_INTERFACE_DESCRIPTOR_TYPE);
    push16(buf, 0);
    push_unknown_descriptor(buf);

    UsbDeviceStatus status = Open;
    RR_SHARED_PTR<UsbDevice_Settings> settings;
    ASSERT_TRUE(parse_with_timeout(buf, status, settings));
    EXPECT_EQ(status, Error);
    EXPECT_TRUE(settings->supported_protocols.empty());
}

TEST(UsbDescriptors, ParseProtocolCountExceedsLength)
{
    // wNumProtocols claims more entries than wLength holds
    std::vector<uint8_t> buf = rr_descriptors_header();
    std::vector<uint16_t> protocols;
    protocols.push_back(0x0101);
    push_interface_descriptor(buf, 14, 4, protocols);
    push_unknown_descriptor(buf);

    UsbDeviceStatus status = Open;
    RR_SHARED_PTR<UsbDevice_Settings> settings;
    ASSERT_TRUE(parse_with_timeout(buf, status, settings));
    EXPECT_EQ(status, Error);
    EXPECT_TRUE(settings->supported_protocols.empty());
}
