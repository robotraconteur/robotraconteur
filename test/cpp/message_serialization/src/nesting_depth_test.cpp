#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <RobotRaconteur.h>
#include <RobotRaconteur/IOUtils.h>
#include <RobotRaconteur/AsyncMessageIO.h>
#include <gtest/gtest.h>

using namespace RobotRaconteur;

// Readers must reject messages whose element lists are nested deeper than 512 levels. Without a limit, a small
// message can nest deeply enough to exhaust the stack of the receiving thread.

// Build a message whose single element has `levels` nested structure lists
static RR_INTRUSIVE_PTR<Message> BuildNestedMessage(size_t levels)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > empty;
    RR_INTRUSIVE_PTR<MessageElement> el =
        CreateMessageElement("e", CreateMessageElementNestedElementList(DataTypes_structure_t, "", empty));
    for (size_t i = 1; i < levels; i++)
    {
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
        v.push_back(el);
        el = CreateMessageElement("e", CreateMessageElementNestedElementList(DataTypes_structure_t, "", v));
    }
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "f");
    e->elements.push_back(el);
    m->entries.push_back(e);
    return m;
}

static boost::shared_array<uint8_t> Serialize(const RR_INTRUSIVE_PTR<Message>& m, uint16_t version, size_t& size)
{
    size = version == 2 ? m->ComputeSize() : m->ComputeSize4();
    boost::shared_array<uint8_t> buf(new uint8_t[size]);
    ArrayBinaryWriter w(buf.get(), 0, size);
    if (version == 2)
        m->Write(w);
    else
        m->Write4(w);
    return buf;
}

// Build and serialize the nested message on a thread with a large stack. Building, writing and releasing a deeply
// nested message recurse once per level; in unoptimized Windows builds (~3 KB of stack per level) that alone
// overflows the default 1 MiB stack before 512 levels. The reads under test run on the calling thread, which has the
// platform's default stack size.
struct SerializeNestedTask
{
    size_t levels;
    uint16_t version;
    size_t* size;
    boost::shared_array<uint8_t>* buf;
    void operator()() const { *buf = Serialize(BuildNestedMessage(levels), version, *size); }
};

static boost::shared_array<uint8_t> SerializeNested(size_t levels, uint16_t version, size_t& size)
{
    boost::shared_array<uint8_t> buf;
    boost::thread::attributes attrs;
    attrs.set_stack_size(64 * 1024 * 1024);
    SerializeNestedTask task = {levels, version, &size, &buf};
    boost::thread t(attrs, task);
    t.join();
    return buf;
}

static void SyncRead(size_t levels, uint16_t version)
{
    size_t size = 0;
    boost::shared_array<uint8_t> buf = SerializeNested(levels, version, size);
    ArrayBinaryReader r(buf.get(), 0, size);
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    if (version == 2)
        m->Read(r);
    else
        m->Read4(r);
}

static bool AsyncRead(size_t levels, uint16_t version)
{
    size_t size = 0;
    boost::shared_array<uint8_t> buf = SerializeNested(levels, version, size);
    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    const_buffers b;
    b.push_back(boost::asio::buffer(buf.get(), size));
    mutable_buffers continue_bufs;
    size_t used = 0;
    if (version == 2)
        r->Read(b, used, 0, continue_bufs);
    else
        r->Read4(b, used, 0, continue_bufs);
    return r->MessageReady();
}

TEST(MessageNestingDepth, SyncRead2)
{
    EXPECT_NO_THROW(SyncRead(512, 2));
    EXPECT_THROW(SyncRead(513, 2), ProtocolException);
}

TEST(MessageNestingDepth, SyncRead4)
{
    EXPECT_NO_THROW(SyncRead(512, 4));
    EXPECT_THROW(SyncRead(513, 4), ProtocolException);
}

TEST(MessageNestingDepth, AsyncRead2)
{
    EXPECT_TRUE(AsyncRead(512, 2));
    EXPECT_THROW(AsyncRead(513, 2), ProtocolException);
}

TEST(MessageNestingDepth, AsyncRead4)
{
    EXPECT_TRUE(AsyncRead(512, 4));
    EXPECT_THROW(AsyncRead(513, 4), ProtocolException);
}

TEST(MessageNestingDepth, SyncReadAfterRejection)
{
    // The per-thread depth counter must be restored after a rejected message
    EXPECT_THROW(SyncRead(513, 4), ProtocolException);
    EXPECT_NO_THROW(SyncRead(512, 4));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
