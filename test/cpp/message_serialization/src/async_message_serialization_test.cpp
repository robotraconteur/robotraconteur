#include "message_test_util.h"

#include <boost/shared_array.hpp>
#include <RobotRaconteur/IOUtils.h>
#include <RobotRaconteur/AsyncMessageIO.h>

#include <gtest/gtest.h>

#include <boost/range.hpp>
#include <algorithm>

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;

#undef min
#undef max

static void DoTest(RR_INTRUSIVE_PTR<Message> m, uint16_t version, LFSRSeqGen& rng)
{
    boost::shared_array<uint8_t> buf;
    size_t message_size = 0;

    if (version == 2)
    {
        // Write to stream and read back
        message_size = m->ComputeSize();
        buf.reset(new uint8_t[message_size]);
        ArrayBinaryWriter w(buf.get(), 0, message_size);
        m->Write(w);

        EXPECT_EQ(w.Position(), m->ComputeSize());
    }
    else if (version == 4)
    {
        // Write to stream and read back
        message_size = m->ComputeSize4();
        buf.reset(new uint8_t[message_size]);
        ArrayBinaryWriter w(buf.get(), 0, message_size);
        m->Write4(w);

        /*std::ofstream f("message.dat", std::ios::binary);
        f.unsetf(std::ios::skipws);
        std::copy(buf.get(), buf.get() + message_size, std::ostream_iterator<uint8_t>(f));
        f.close();*/

        ASSERT_EQ(w.Position(), m->ComputeSize4());
    }
    else
    {
        FAIL();
    }

    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    const_buffers buf2;
    buf2.push_back(boost::asio::buffer(buf.get(), message_size));
    mutable_buffers continue_bufs;
    while (boost::asio::buffer_size(buf2) > 0)
    {
        size_t n1 = boost::asio::buffer_size(buf2);
        n1 = std::min(n1, (size_t)16193);
        size_t new_size = rng.NextDist(0, n1);
        new_size = std::max(new_size, (size_t)16);
        new_size = std::min(new_size, n1);

        const_buffers buf3 = buffers_truncate(buf2, new_size);
        size_t buf_used;
        if (boost::asio::buffer_size(continue_bufs) == 0)
        {
            continue_bufs.clear();
            if (version == 2)
            {
                r->Read(buf3, buf_used, 0, continue_bufs);
            }
            else
            {
                r->Read4(buf3, buf_used, 0, continue_bufs);
            }
            buffers_consume(buf2, buf_used);
        }
        else
        {
            size_t n = boost::asio::buffer_copy(continue_bufs, buf3);
            const_buffers empty;
            continue_bufs.clear();
            if (version == 2)
            {
                r->Read(empty, buf_used, n, continue_bufs);
            }
            else
            {
                r->Read4(empty, buf_used, n, continue_bufs);
            }
            ASSERT_EQ(buf_used, 0);
            buffers_consume(buf2, n);
        }
        // std::cout << "Read " << buf_used << " continue_bufs: " << boost::asio::buffer_size(continue_bufs) <<
        // std::endl;
    }

    ASSERT_TRUE(r->MessageReady());
    RR_INTRUSIVE_PTR<Message> m2 = r->GetNextMessage();

    CompareMessage(m, m2);
}

TEST(AsyncMessageReaderTest, Test1)
{
    LFSRSeqGen rng((uint32_t)std::time(0), "async_message_reader_test_test1");

    {
        RR_INTRUSIVE_PTR<Message> m1 = NewTestMessage();
        m1->header->MessageID = 10322;
        m1->header->MessageResID = 8364;
        DoTest(m1, 2, rng);
    }
    {
        RR_INTRUSIVE_PTR<Message> m3 = NewTestMessage();
        m3->entries.resize(1);
        DoTest(m3, 4, rng);
    }
    {
        RR_INTRUSIVE_PTR<Message> m4 = NewTestMessage();

        m4->entries.at(1)->EntryFlags = MessageEntryFlags_ERROR;
        m4->entries.at(1)->Error = MessageErrorType_DataSerializationError;
        m4->entries.at(1)->RequestID = 0;
        m4->entries.at(2)->EntryFlags = MessageEntryFlags_SERVICE_PATH_CODE | MessageEntryFlags_MEMBER_NAME_CODE;
        m4->entries.at(2)->ServicePathCode = 7;
        m4->entries.at(2)->MemberNameCode = 13;
        m4->entries.at(0)->elements.at(0)->ElementFlags = MessageElementFlags_ELEMENT_NUMBER;
        m4->entries.at(0)->elements.at(0)->ElementNumber = 7483948;
        m4->entries.at(0)->elements.at(1)->ElementFlags =
            MessageElementFlags_ELEMENT_NAME_CODE | MessageElementFlags_ELEMENT_TYPE_NAME_CODE;
        m4->entries.at(0)->elements.at(1)->ElementNameCode = 5;
        m4->entries.at(0)->elements.at(1)->ElementTypeNameCode = 11;
        m4->entries.at(0)->elements.at(2)->ElementNumber = -113;
        DoTest(m4, 4, rng);
    }
}

TEST(AsyncMessageReaderTest, RandomTest)
{
    size_t iterations = 100;
    LFSRSeqGen rng((uint32_t)std::time(0), "async_message_reader_test_random_test");

    for (size_t i = 0; i < iterations; i++)
    {
        RR_INTRUSIVE_PTR<Message> m = NewRandomTestMessage(rng);
        DoTest(m, 2, rng);
        if (i % 10 == 0)
        {
            std::cout << "Message read: " << i << " size: " << m->ComputeSize() << std::endl;
        }
    }
}

TEST(AsyncMessageReaderTest, RandomTest4)
{
    size_t iterations = 100;
    LFSRSeqGen rng((uint32_t)std::time(0), "async_message_reader_test_random_test4");

    for (size_t i = 0; i < iterations; i++)
    {
        RR_INTRUSIVE_PTR<Message> m = NewRandomTestMessage4(rng);
        DoTest(m, 4, rng);
        if (i % 10 == 0)
        {
            std::cout << "Message4 read: " << i << " size: " << m->ComputeSize4() << std::endl;
        }
    }
}

// Feed the whole buffer to the async reader, then keep supplying the continue buffers it asks for. Returns true
// if the reader accepted a complete message. Exceptions propagate
static bool AsyncReadVoidElementTest(const std::vector<uint8_t>& buf, uint16_t version)
{
    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    size_t pos = 0;
    mutable_buffers continue_bufs;
    for (size_t iter = 0; iter < 1000; iter++)
    {
        AsyncMessageReader::return_type ret;
        mutable_buffers next;
        size_t used = 0;
        if (continue_bufs.empty())
        {
            const_buffers bufs;
            if (pos < buf.size())
                bufs.push_back(boost::asio::buffer(&buf[pos], buf.size() - pos));
            ret = (version == 2) ? r->Read(bufs, used, 0, next) : r->Read4(bufs, used, 0, next);
            pos += used;
        }
        else
        {
            size_t n = 0;
            for (mutable_buffers::iterator b = continue_bufs.begin(); b != continue_bufs.end(); ++b)
            {
                size_t c = std::min(boost::asio::buffer_size(*b), buf.size() - pos);
                if (c == 0)
                    break;
                memcpy(RR_BOOST_ASIO_BUFFER_CAST(uint8_t*, *b), &buf[pos], c);
                pos += c;
                n += c;
            }
            if (n == 0)
                return false;
            const_buffers empty;
            ret = (version == 2) ? r->Read(empty, used, n, next) : r->Read4(empty, used, n, next);
        }
        if (ret == AsyncMessageReader::ReadReturn_done)
            return r->MessageReady();
        continue_bufs.clear();
        if (ret == AsyncMessageReader::ReadReturn_continue_buffers)
            continue_bufs = next;
        else if (used == 0 && pos >= buf.size())
            return false;
    }
    return false;
}

// A void element whose element size is larger than its contents must be rejected, as Message::Read and
// Message::Read4 do. The async reader finished void elements without checking that the element was fully
// consumed, so it accepted an element size that overlaps the following element
TEST(AsyncMessageReaderTest, VoidElementSizeSlackRejected)
{
    const std::string name = "zz_void_size_test";
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "func");
    e->ServicePath = "svc";
    e->AddElement(name, RR_INTRUSIVE_PTR<MessageElementData>());
    e->AddElement("d", ScalarToRRArray<double>(1.5));
    m->entries.push_back(e);
    ASSERT_EQ(e->elements.at(0)->ElementType, DataTypes_void_t);

    for (uint16_t version = 2; version <= 4; version += 2)
    {
        std::vector<uint8_t> buf(version == 2 ? m->ComputeSize() : m->ComputeSize4());
        ArrayBinaryWriter w(&buf[0], 0, buf.size());
        if (version == 2)
            m->Write(w);
        else
            m->Write4(w);

        EXPECT_TRUE(AsyncReadVoidElementTest(buf, version)) << "version " << version;

        std::vector<uint8_t>::iterator it = std::search(buf.begin(), buf.end(), name.begin(), name.end());
        ASSERT_TRUE(it != buf.end());
        size_t name_pos = it - buf.begin();

        // Grow the void element's size by 4 bytes, into the following element (the entry size is unchanged).
        // Format 2: uint32 element size, uint16 name length, name. Format 4: uint_x element size, uint8 flags,
        // uint_x name length, name
        if (version == 2)
        {
            size_t el_pos = name_pos - 6;
            ASSERT_EQ(buf[name_pos - 2], name.size());
            ASSERT_LT(buf[el_pos], 250);
            ASSERT_EQ(buf[el_pos + 1], 0);
            buf[el_pos] += 4;
        }
        else
        {
            size_t el_pos = name_pos - 3;
            ASSERT_EQ(buf[name_pos - 1], name.size());
            ASSERT_LT(buf[el_pos], 249);
            buf[el_pos] += 4;
        }

        {
            ArrayBinaryReader sr(&buf[0], 0, buf.size());
            RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
            if (version == 2)
            {
                EXPECT_ANY_THROW(m2->Read(sr)) << "version " << version;
            }
            else
            {
                EXPECT_ANY_THROW(m2->Read4(sr)) << "version " << version;
            }
        }

        EXPECT_THROW(AsyncReadVoidElementTest(buf, version), ProtocolException) << "version " << version;
    }
}

static void DoTestW(RR_INTRUSIVE_PTR<RobotRaconteur::Message> m, uint16_t version, LFSRSeqGen& rng)
{

    boost::shared_array<uint8_t> buf;
    size_t message_size = 0;

    if (version == 2)
    {
        message_size = m->ComputeSize();
    }
    else if (version == 4)
    {
        message_size = m->ComputeSize4();
        boost::shared_array<uint8_t> sync_buf(new uint8_t[message_size]);
        ArrayBinaryWriter sync_writer(sync_buf.get(), 0, message_size);
        m->Write4(sync_writer);
    }
    else
    {
        FAIL();
    }

    buf.reset(new uint8_t[message_size]);
    boost::asio::mutable_buffer buf2(buf.get(), message_size);

    AsyncMessageWriter::return_type ret = AsyncMessageWriter::WriteReturn_continue;

    RR_SHARED_PTR<AsyncMessageWriter> wr = AsyncMessageWriter::CreateInstance();
    wr->BeginWrite(m, version);

    do
    {
        boost::shared_array<uint8_t> work_buf1(new uint8_t[4096]);
        mutable_buffers work_bufs;
        work_bufs.push_back(boost::asio::buffer(work_buf1.get(), 4096));
        const_buffers write_bufs;
        size_t work_bufs_used;

        size_t quota = rng.NextDist(0, 16 * 1024);

        if (version == 2)
        {
            ret = wr->Write(quota, work_bufs, work_bufs_used, write_bufs);
        }
        else if (version == 4)
        {
            ret = wr->Write4(quota, work_bufs, work_bufs_used, write_bufs);
        }
        else
        {
            FAIL();
        }

        size_t n = boost::asio::buffer_copy(buf2, write_bufs);
        buf2 = buf2 + n;
    } while (ret != AsyncMessageWriter::WriteReturn_done);

    EXPECT_EQ(boost::asio::buffer_size(buf2), 0);

    RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
    ArrayBinaryReader r(buf.get(), 0, message_size);
    if (version == 2)
    {
        m2->Read(r);
        EXPECT_EQ(r.Position(), message_size);
    }
    else if (version == 4)
    {
        m2->Read4(r);
        EXPECT_EQ(r.Position(), message_size);
    }
    else
    {
        FAIL();
    }

    CompareMessage(m, m2);
}

// Returns true if the async reader accepts buf as a complete message
static bool AsyncReadAccepts(const std::vector<uint8_t>& buf, uint16_t version)
{
    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    const_buffers buf2;
    buf2.push_back(boost::asio::buffer(&buf[0], buf.size()));
    mutable_buffers continue_bufs;
    size_t buf_used = 0;
    try
    {
        AsyncMessageReader::return_type ret =
            (version == 2) ? r->Read(buf2, buf_used, 0, continue_bufs) : r->Read4(buf2, buf_used, 0, continue_bufs);
        return ret == AsyncMessageReader::ReadReturn_done && r->MessageReady();
    }
    catch (std::exception&)
    {
        return false;
    }
}

// The header size field declares more bytes than the header fields use. The sync reader rejects this
// ("Error in message format"); the async reader must not accept it and start reading entries inside
// the declared header
TEST(AsyncMessageReaderTest, HeaderSizeSlackRejected)
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->header->SenderNodeName = "sender";
    m->header->ReceiverNodeName = "receiver";
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "func");
    e->ServicePath = "svc.obj";
    e->AddElement("d", ScalarToRRArray<double>(1.5));
    m->entries.push_back(e);

    for (uint16_t version = 2; version <= 4; version += 2)
    {
        std::vector<uint8_t> buf;
        if (version == 2)
        {
            buf.resize(m->ComputeSize());
            ArrayBinaryWriter w(&buf[0], 0, buf.size());
            m->Write(w);
        }
        else
        {
            buf.resize(m->ComputeSize4());
            ArrayBinaryWriter w(&buf[0], 0, buf.size());
            m->Write4(w);
        }

        EXPECT_TRUE(AsyncReadAccepts(buf, version)) << "version " << version;

        // Increase the declared header size by 2 bytes, leaving the message size and contents unchanged
        if (version == 2)
        {
            uint16_t hs = static_cast<uint16_t>(buf[10] | (buf[11] << 8));
            hs += 2;
            buf[10] = static_cast<uint8_t>(hs & 0xFF);
            buf[11] = static_cast<uint8_t>(hs >> 8);
        }
        else
        {
            ASSERT_LT(buf[10], 250);
            buf[10] += 2;
        }

        {
            ArrayBinaryReader sr(&buf[0], 0, buf.size());
            RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
            if (version == 2)
            {
                EXPECT_ANY_THROW(m2->Read(sr)) << "version " << version;
            }
            else
            {
                EXPECT_ANY_THROW(m2->Read4(sr)) << "version " << version;
            }
        }

        EXPECT_FALSE(AsyncReadAccepts(buf, version)) << "version " << version;
    }
}

TEST(AsyncMessageWriterTest, Test1)
{
    LFSRSeqGen rng((uint32_t)std::time(0), "async_message_writer_test_test1");
    {
        RR_INTRUSIVE_PTR<Message> m1 = NewTestMessage();
        m1->header->MessageID = 10322;
        m1->header->MessageResID = 8364;
        DoTest(m1, 2, rng);
    }
    {
        RR_INTRUSIVE_PTR<Message> m2 = NewTestMessage();
        m2->entries.push_back(CreateMessageEntry());
        DoTest(m2, 4, rng);
    }
    {
        RR_INTRUSIVE_PTR<Message> m3 = NewTestMessage();
        m3->entries.resize(1);
        DoTest(m3, 4, rng);
    }
    {
        RR_INTRUSIVE_PTR<Message> m4 = NewTestMessage();

        m4->entries.at(1)->EntryFlags = MessageEntryFlags_ERROR;
        m4->entries.at(1)->Error = MessageErrorType_DataSerializationError;
        m4->entries.at(1)->RequestID = 0;
        m4->entries.at(2)->EntryFlags = MessageEntryFlags_SERVICE_PATH_CODE | MessageEntryFlags_MEMBER_NAME_CODE;
        m4->entries.at(2)->ServicePathCode = 7;
        m4->entries.at(2)->MemberNameCode = 13;
        m4->entries.at(0)->elements.at(0)->ElementFlags = MessageElementFlags_ELEMENT_NUMBER;
        m4->entries.at(0)->elements.at(0)->ElementNumber = 7483948;
        m4->entries.at(0)->elements.at(1)->ElementFlags =
            MessageElementFlags_ELEMENT_NAME_CODE | MessageElementFlags_ELEMENT_TYPE_NAME_CODE;
        m4->entries.at(0)->elements.at(1)->ElementNameCode = 5;
        m4->entries.at(0)->elements.at(1)->ElementTypeNameCode = 11;
        m4->entries.at(0)->elements.at(2)->ElementNumber = -113;
        DoTestW(m4, 4, rng);
    }
}

TEST(AsyncMessageWriterTest, RandomTest)
{
    size_t iterations = 100;
    LFSRSeqGen rng((uint32_t)std::time(0), "async_message_writer_test_random_test");

    for (size_t i = 0; i < iterations; i++)
    {
        RR_INTRUSIVE_PTR<Message> m = NewRandomTestMessage(rng);
        DoTestW(m, 2, rng);
        if (i % 10 == 0)
        {
            std::cout << "Message write: " << i << " size: " << m->ComputeSize() << std::endl;
        }
    }
}

TEST(AsyncMessageWriterTest, RandomTest4)
{
    size_t iterations = 100;
    LFSRSeqGen rng((uint32_t)std::time(0), "async_message_writer_test_random_test4");

    for (size_t i = 0; i < iterations; i++)
    {
        RR_INTRUSIVE_PTR<Message> m = NewRandomTestMessage4(rng);

        DoTestW(m, 4, rng);
        if (i % 10 == 0)
        {
            std::cout << "Message4 write: " << i << " size: " << m->ComputeSize4() << std::endl;
        }
    }
}

// Serialize m in format 2 or 4
static std::vector<uint8_t> WriteMessageElementTypeTest(const RR_INTRUSIVE_PTR<Message>& m, uint16_t version)
{
    std::vector<uint8_t> buf(version == 2 ? m->ComputeSize() : m->ComputeSize4());
    ArrayBinaryWriter w(&buf[0], 0, buf.size());
    if (version == 2)
        m->Write(w);
    else
        m->Write4(w);
    return buf;
}

// Element types from the wire above 127 are outside the range of the DataTypes enum. They must be rejected
// before they are stored in the enum (undefined behavior, reported by UBSan -fsanitize=enum)
TEST(AsyncMessageReaderTest, ElementTypeOutOfRangeRejected)
{
    const std::string name = "zz_element_type_test";
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "func");
    e->ServicePath = "svc";
    e->AddElement(name, ScalarToRRArray<double>(1.5));
    m->entries.push_back(e);

    const uint16_t bad_types[] = {128, 200, 0xFFFF};
    for (uint16_t version = 2; version <= 4; version += 2)
    {
        std::vector<uint8_t> buf = WriteMessageElementTypeTest(m, version);
        // The element type follows the element name in both formats
        std::vector<uint8_t>::iterator it = std::search(buf.begin(), buf.end(), name.begin(), name.end());
        ASSERT_TRUE(it != buf.end());
        size_t type_pos = (it - buf.begin()) + name.size();
        ASSERT_EQ(buf[type_pos] | (buf[type_pos + 1] << 8), DataTypes_double_t);

        for (size_t i = 0; i < sizeof(bad_types) / sizeof(bad_types[0]); i++)
        {
            std::vector<uint8_t> buf2 = buf;
            buf2[type_pos] = static_cast<uint8_t>(bad_types[i] & 0xFF);
            buf2[type_pos + 1] = static_cast<uint8_t>(bad_types[i] >> 8);

            ArrayBinaryReader sr(&buf2[0], 0, buf2.size());
            RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
            if (version == 2)
            {
                EXPECT_ANY_THROW(m2->Read(sr)) << "version " << version << " type " << bad_types[i];
            }
            else
            {
                EXPECT_ANY_THROW(m2->Read4(sr)) << "version " << version << " type " << bad_types[i];
            }

            RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
            r->Reset();
            const_buffers bufs;
            bufs.push_back(boost::asio::buffer(&buf2[0], buf2.size()));
            mutable_buffers continue_bufs;
            size_t buf_used = 0;
            if (version == 2)
            {
                EXPECT_ANY_THROW(r->Read(bufs, buf_used, 0, continue_bufs))
                    << "version " << version << " type " << bad_types[i];
            }
            else
            {
                EXPECT_ANY_THROW(r->Read4(bufs, buf_used, 0, continue_bufs))
                    << "version " << version << " type " << bad_types[i];
            }
        }
    }
}

// Feed buf to the async reader in one call. Returns true if it accepted the complete message, false if it
// is waiting for more data. Exceptions propagate
static bool AsyncRead4HeaderSizeTest(std::vector<uint8_t>& buf)
{
    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    const_buffers bufs;
    bufs.push_back(boost::asio::buffer(&buf[0], buf.size()));
    mutable_buffers continue_bufs;
    size_t buf_used = 0;
    AsyncMessageReader::return_type ret = r->Read4(bufs, buf_used, 0, continue_bufs);
    return ret == AsyncMessageReader::ReadReturn_done && r->MessageReady();
}

// The v4 header size is an absolute position from the wire. A value past the end of the message, or before
// the data already read, must be rejected instead of becoming the reader's limit
TEST(AsyncMessageReaderTest, HeaderSize4OutOfRangeRejected)
{
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "func");
    e->ServicePath = "svc";
    e->AddElement("d", ScalarToRRArray<double>(1.5));
    m->entries.push_back(e);

    std::vector<uint8_t> buf(m->ComputeSize4());
    ArrayBinaryWriter w(&buf[0], 0, buf.size());
    m->Write4(w);
    // The header size at offset 10 must fit in one byte, and the message must be shorter than 252 bytes
    ASSERT_LT(buf[10], 252);
    ASSERT_LT(buf.size(), 252u);

    {
        std::vector<uint8_t> buf2 = buf;
        EXPECT_TRUE(AsyncRead4HeaderSizeTest(buf2));
    }

    // Past the end of the message, and before the current position (just after the header size field)
    const uint8_t bad_sizes[] = {251, 5};
    for (size_t i = 0; i < sizeof(bad_sizes) / sizeof(bad_sizes[0]); i++)
    {
        std::vector<uint8_t> buf2 = buf;
        buf2[10] = bad_sizes[i];
        EXPECT_THROW(AsyncRead4HeaderSizeTest(buf2), ProtocolException) << "header size " << (int)bad_sizes[i];
    }
}

// Feed buf to the async reader as the stream transports do, offering the rest of the data after each call.
// Once all of buf has been offered, keep calling with no new data so the reader processes the bytes it has
// buffered. Returns true if the reader accepted the complete message. Exceptions propagate
static bool AsyncReadSectionLimitTest(const std::vector<uint8_t>& buf, uint16_t version)
{
    RR_SHARED_PTR<AsyncMessageReader> r = AsyncMessageReader::CreateInstance();
    r->Reset();
    size_t pos = 0;
    size_t idle_calls = 0;
    mutable_buffers continue_bufs;
    for (size_t iter = 0; iter < 1000; iter++)
    {
        AsyncMessageReader::return_type ret;
        mutable_buffers next;
        size_t used = 0;
        if (continue_bufs.empty())
        {
            const_buffers bufs;
            if (pos < buf.size())
                bufs.push_back(boost::asio::buffer(&buf[pos], buf.size() - pos));
            else if (++idle_calls > 100)
                return false;
            ret = (version == 2) ? r->Read(bufs, used, 0, next) : r->Read4(bufs, used, 0, next);
            pos += used;
        }
        else
        {
            size_t n = 0;
            for (mutable_buffers::iterator b = continue_bufs.begin(); b != continue_bufs.end(); ++b)
            {
                size_t c = std::min(boost::asio::buffer_size(*b), buf.size() - pos);
                if (c == 0)
                    break;
                memcpy(RR_BOOST_ASIO_BUFFER_CAST(uint8_t*, *b), &buf[pos], c);
                pos += c;
                n += c;
            }
            if (n == 0)
                return false;
            const_buffers empty;
            ret = (version == 2) ? r->Read(empty, used, n, next) : r->Read4(empty, used, n, next);
        }
        if (ret == AsyncMessageReader::ReadReturn_done)
            return r->MessageReady();
        continue_bufs.clear();
        if (ret == AsyncMessageReader::ReadReturn_continue_buffers)
            continue_bufs = next;
    }
    return false;
}

// A read that runs past the end of its section (here a string that continues past the end of its element)
// must be rejected. distance_from_limit() wrapped to a huge value instead, so the following reads, and the
// length and allocation checks that use it, were no longer limited
TEST(AsyncMessageReaderTest, ReadPastSectionLimitRejected)
{
    const std::string name = "zz_section_limit_test";
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_FunctionCallReq, "func");
    e->ServicePath = "svc";
    e->AddElement(name, RR_INTRUSIVE_PTR<MessageElementData>());
    e->AddElement("d", ScalarToRRArray<double>(1.5));
    m->entries.push_back(e);
    ASSERT_EQ(e->elements.at(0)->ElementType, DataTypes_void_t);

    for (uint16_t version = 2; version <= 4; version += 2)
    {
        std::vector<uint8_t> buf(version == 2 ? m->ComputeSize() : m->ComputeSize4());
        ArrayBinaryWriter w(&buf[0], 0, buf.size());
        if (version == 2)
            m->Write(w);
        else
            m->Write4(w);

        EXPECT_TRUE(AsyncReadSectionLimitTest(buf, version)) << "version " << version;

        std::vector<uint8_t>::iterator it = std::search(buf.begin(), buf.end(), name.begin(), name.end());
        ASSERT_TRUE(it != buf.end());
        size_t name_pos = it - buf.begin();

        // Shrink the element size of the void element so the element ends 4 bytes into its name.
        // Format 2: uint32 element size, uint16 name length, name. Format 4: uint_x element size, uint8 flags,
        // uint_x name length, name
        if (version == 2)
        {
            size_t el_pos = name_pos - 6;
            ASSERT_EQ(buf[name_pos - 2], name.size());
            uint32_t el_size = static_cast<uint32_t>(name_pos + 4 - el_pos);
            buf[el_pos] = static_cast<uint8_t>(el_size);
            buf[el_pos + 1] = 0;
            buf[el_pos + 2] = 0;
            buf[el_pos + 3] = 0;
        }
        else
        {
            size_t el_pos = name_pos - 3;
            ASSERT_EQ(buf[name_pos - 1], name.size());
            ASSERT_LT(buf[el_pos], 253);
            buf[el_pos] = static_cast<uint8_t>(name_pos + 4 - el_pos);
        }

        {
            ArrayBinaryReader sr(&buf[0], 0, buf.size());
            RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
            if (version == 2)
            {
                EXPECT_ANY_THROW(m2->Read(sr)) << "version " << version;
            }
            else
            {
                EXPECT_ANY_THROW(m2->Read4(sr)) << "version " << version;
            }
        }

        EXPECT_THROW(AsyncReadSectionLimitTest(buf, version), ProtocolException) << "version " << version;
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
