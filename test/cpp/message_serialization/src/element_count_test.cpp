#include <boost/shared_array.hpp>
#include <RobotRaconteur.h>
#include <RobotRaconteur/IOUtils.h>
#include <RobotRaconteur/AsyncMessageIO.h>
#include <gtest/gtest.h>
#include <cstring>
#include <fstream>
#include <string>

using namespace RobotRaconteur;

// Readers must check element counts received from the network against the remaining message bytes before
// allocating. Otherwise a few bytes can make the receiver allocate (and for complex and bool arrays,
// zero-fill) gigabytes of memory.

static const uint32_t kOriginalCount = 70000;

static RR_INTRUSIVE_PTR<Message> BuildArrayMessage()
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "f");
    e->AddElement("a", AllocateRRArray<cdouble>(kOriginalCount));
    m->entries.push_back(e);
    return m;
}

static RR_INTRUSIVE_PTR<Message> BuildListMessage()
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
    for (uint32_t i = 0; i < kOriginalCount; i++)
    {
        v.push_back(CreateMessageElement("a", RR_INTRUSIVE_PTR<MessageElementData>()));
    }
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "f");
    e->AddElement("l", CreateMessageElementNestedElementList(DataTypes_list_t, "", v));
    m->entries.push_back(e);
    return m;
}

// Serialize, then overwrite the element count kOriginalCount in place with new_count
static std::vector<uint8_t> SerializePatched(const RR_INTRUSIVE_PTR<Message>& m, uint16_t version, uint32_t new_count)
{
    size_t size = version == 2 ? m->ComputeSize() : m->ComputeSize4();
    std::vector<uint8_t> buf(size);
    ArrayBinaryWriter w(&buf[0], 0, size);
    if (version == 2)
        m->Write(w);
    else
        m->Write4(w);

    // v2: count is a little-endian uint32; v4: count is a uint_x, 0xFE followed by a little-endian uint32
    uint8_t pattern[5] = {0xFE, 0x70, 0x11, 0x01, 0x00};
    const uint8_t* p = version == 2 ? pattern + 1 : pattern;
    size_t plen = version == 2 ? 4 : 5;
    std::vector<uint8_t>::iterator it = std::search(buf.begin(), buf.end(), p, p + plen);
    EXPECT_TRUE(it != buf.end());
    if (it != buf.end())
    {
        uint8_t* c = &(*it) + (plen - 4);
        c[0] = static_cast<uint8_t>(new_count);
        c[1] = static_cast<uint8_t>(new_count >> 8);
        c[2] = static_cast<uint8_t>(new_count >> 16);
        c[3] = static_cast<uint8_t>(new_count >> 24);
    }
    return buf;
}

static void SyncRead(std::vector<uint8_t>& buf, uint16_t version)
{
    ArrayBinaryReader r(&buf[0], 0, buf.size());
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    if (version == 2)
        m->Read(r);
    else
        m->Read4(r);
}

static void AsyncRead(std::vector<uint8_t>& buf, uint16_t version)
{
    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    const_buffers b;
    b.push_back(boost::asio::buffer(&buf[0], buf.size()));
    mutable_buffers continue_bufs;
    size_t used = 0;
    if (version == 2)
        r->Read(b, used, 0, continue_bufs);
    else
        r->Read4(b, used, 0, continue_bufs);
}

// Peak resident memory of this process in KiB (Linux), or 0 if unknown
static size_t PeakRssKiB()
{
#ifdef __linux__
    std::ifstream f("/proc/self/status");
    std::string line;
    while (std::getline(f, line))
    {
        if (line.compare(0, 6, "VmHWM:") == 0)
            return boost::lexical_cast<size_t>(boost::trim_copy(line.substr(6, line.size() - 6 - 3)));
    }
#endif
    return 0;
}

static void CheckAsyncArrayNoLargeAllocation(uint16_t version)
{
    // 0x01000000 cdouble elements claim 256 MiB, but only ~1.1 MiB of data is present
    std::vector<uint8_t> buf = SerializePatched(BuildArrayMessage(), version, 0x01000000);
    size_t before = PeakRssKiB();
    EXPECT_THROW(AsyncRead(buf, version), ProtocolException);
    size_t after = PeakRssKiB();
    if (before != 0)
    {
        EXPECT_LT(after - before, 64u * 1024u) << "Reader allocated memory for data that was not in the message";
    }
}

TEST(MessageElementCount, AsyncRead2ArrayCount) { CheckAsyncArrayNoLargeAllocation(2); }

TEST(MessageElementCount, AsyncRead4ArrayCount) { CheckAsyncArrayNoLargeAllocation(4); }

TEST(MessageElementCount, NestedListCount)
{
    // A list claiming 0xFFFFFFFF children must be rejected before reserving space for them
    for (uint16_t version = 2; version <= 4; version += 2)
    {
        std::vector<uint8_t> buf = SerializePatched(BuildListMessage(), version, 0xFFFFFFFF);
        EXPECT_THROW(SyncRead(buf, version), ProtocolException) << "sync v" << version;
        EXPECT_THROW(AsyncRead(buf, version), ProtocolException) << "async v" << version;
    }
}

TEST(MessageElementCount, ValidMessagesStillRead)
{
    for (uint16_t version = 2; version <= 4; version += 2)
    {
        std::vector<uint8_t> a = SerializePatched(BuildArrayMessage(), version, kOriginalCount);
        EXPECT_NO_THROW(SyncRead(a, version));
        EXPECT_NO_THROW(AsyncRead(a, version));
        std::vector<uint8_t> l = SerializePatched(BuildListMessage(), version, kOriginalCount);
        EXPECT_NO_THROW(SyncRead(l, version));
        EXPECT_NO_THROW(AsyncRead(l, version));
    }
}

// Message whose single element has a 70000 character name (v4 encodes the length as uint_x 0xFE + uint32)
static RR_INTRUSIVE_PTR<Message> BuildLongNameMessage()
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "f");
    e->AddElement(std::string(kOriginalCount, 'a'), RR_INTRUSIVE_PTR<MessageElementData>());
    m->entries.push_back(e);
    return m;
}

// Message whose single element carries 70000 bytes of Extended data
static RR_INTRUSIVE_PTR<Message> BuildExtendedMessage()
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "f");
    RR_INTRUSIVE_PTR<MessageElement> el = CreateMessageElement("a", RR_INTRUSIVE_PTR<MessageElementData>());
    el->ElementFlags |= MessageElementFlags_EXTENDED;
    el->Extended.resize(kOriginalCount);
    e->elements.push_back(el);
    m->entries.push_back(e);
    return m;
}

// Message whose entry has 70000 elements
static RR_INTRUSIVE_PTR<Message> BuildManyElementsMessage()
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "f");
    for (uint32_t i = 0; i < kOriginalCount; i++)
    {
        e->elements.push_back(CreateMessageElement("a", RR_INTRUSIVE_PTR<MessageElementData>()));
    }
    m->entries.push_back(e);
    return m;
}

typedef void (*ReadFunc)(std::vector<uint8_t>&, uint16_t);

// Patch a v4 length to 0x10000000 (256 MiB) and check the reader rejects it without allocating it
static void CheckV4LengthNoLargeAllocation(const RR_INTRUSIVE_PTR<Message>& m, ReadFunc read)
{
    std::vector<uint8_t> buf = SerializePatched(m, 4, 0x10000000);
    size_t before = PeakRssKiB();
    EXPECT_ANY_THROW(read(buf, 4));
    size_t after = PeakRssKiB();
    if (before != 0)
    {
        EXPECT_LT(after - before, 64u * 1024u) << "Reader allocated memory for data that was not in the message";
    }
}

TEST(MessageElementCount, SyncRead4StringLength) { CheckV4LengthNoLargeAllocation(BuildLongNameMessage(), &SyncRead); }

TEST(MessageElementCount, AsyncRead4StringLength)
{
    CheckV4LengthNoLargeAllocation(BuildLongNameMessage(), &AsyncRead);
}

TEST(MessageElementCount, SyncRead4ExtendedLength)
{
    CheckV4LengthNoLargeAllocation(BuildExtendedMessage(), &SyncRead);
}

TEST(MessageElementCount, AsyncRead4ExtendedLength)
{
    CheckV4LengthNoLargeAllocation(BuildExtendedMessage(), &AsyncRead);
}

TEST(MessageElementCount, EntryElementCount)
{
    // An entry claiming 0xFFFFFFFF elements must be rejected before reserving space for them
    std::vector<uint8_t> buf = SerializePatched(BuildManyElementsMessage(), 4, 0xFFFFFFFF);
    EXPECT_THROW(SyncRead(buf, 4), ProtocolException);
    EXPECT_THROW(AsyncRead(buf, 4), ProtocolException);
}

TEST(MessageElementCount, ValidLongFieldsStillRead)
{
    std::vector<uint8_t> a = SerializePatched(BuildLongNameMessage(), 4, kOriginalCount);
    EXPECT_NO_THROW(SyncRead(a, 4));
    EXPECT_NO_THROW(AsyncRead(a, 4));
    std::vector<uint8_t> b = SerializePatched(BuildExtendedMessage(), 4, kOriginalCount);
    EXPECT_NO_THROW(SyncRead(b, 4));
    EXPECT_NO_THROW(AsyncRead(b, 4));
    std::vector<uint8_t> c = SerializePatched(BuildManyElementsMessage(), 4, kOriginalCount);
    EXPECT_NO_THROW(SyncRead(c, 4));
    EXPECT_NO_THROW(AsyncRead(c, 4));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
