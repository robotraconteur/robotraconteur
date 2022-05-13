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
        //Write to stream and read back
        message_size = m->ComputeSize();
        buf.reset(new uint8_t[message_size]);
        ArrayBinaryWriter w(buf.get(), 0, message_size);
        m->Write(w);

        EXPECT_EQ(w.Position(),m->ComputeSize());
    }
    else if (version == 4)
    {
        //Write to stream and read back
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
        size_t new_size = rng.NextDist(0,n1);
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
        //std::cout << "Read " << buf_used << " continue_bufs: " << boost::asio::buffer_size(continue_bufs) << std::endl;
    }

    ASSERT_TRUE(r->MessageReady());
    RR_INTRUSIVE_PTR<Message> m2 = r->GetNextMessage();

    CompareMessage(m, m2);
}

TEST(AsyncMessageReaderTest,Test1)
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
        m4->entries.at(0)->elements.at(1)->ElementFlags = MessageElementFlags_ELEMENT_NAME_CODE | MessageElementFlags_ELEMENT_TYPE_NAME_CODE;
        m4->entries.at(0)->elements.at(1)->ElementNameCode = 5;
        m4->entries.at(0)->elements.at(1)->ElementTypeNameCode = 11;
        m4->entries.at(0)->elements.at(2)->ElementNumber = -113;
        DoTest(m4, 4, rng);
    }
}

TEST(AsyncMessageReaderTest,RandomTest)
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

TEST(AsyncMessageReaderTest,RandomTest4)
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
                    
        size_t quota = rng.NextDist(0, 16*1024);

        if (version == 2)
        {
            ret = wr->Write(quota, work_bufs, work_bufs_used, write_bufs);
        }
        else if (version == 4)
        {
            ret=wr->Write4(quota, work_bufs, work_bufs_used, write_bufs);
        }
        else
        {
            FAIL();
        }			

        size_t n = boost::asio::buffer_copy(buf2, write_bufs);
        buf2 = buf2 + n;
    } while (ret != AsyncMessageWriter::WriteReturn_done);

    EXPECT_EQ(boost::asio::buffer_size(buf2), 0);


    RR_INTRUSIVE_PTR<Message> m2=CreateMessage();
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

TEST(AsyncMessageWriterTest,Test1)
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
        m4->entries.at(0)->elements.at(1)->ElementFlags = MessageElementFlags_ELEMENT_NAME_CODE | MessageElementFlags_ELEMENT_TYPE_NAME_CODE;
        m4->entries.at(0)->elements.at(1)->ElementNameCode = 5;
        m4->entries.at(0)->elements.at(1)->ElementTypeNameCode = 11;
        m4->entries.at(0)->elements.at(2)->ElementNumber = -113;
        DoTestW(m4, 4, rng);
    }
}

TEST(AsyncMessageWriterTest,RandomTest)
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

TEST(AsyncMessageWriterTest,RandomTest4)
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

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}