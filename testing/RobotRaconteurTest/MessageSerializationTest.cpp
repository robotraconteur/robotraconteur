#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "MessageSerializationTest.h"

using namespace RobotRaconteur;

#include <RobotRaconteur.h>

#include <time.h>

#include <boost/foreach.hpp>
#include <boost/random.hpp>

using namespace boost::random;

namespace RobotRaconteurTest
{

RR_INTRUSIVE_PTR<Message> MessageSerializationTest::NewTestMessage()
{
    srand((uint32_t)time(NULL));

    RR_INTRUSIVE_PTR<MessageEntry> e1 = CreateMessageEntry(MessageEntryType_PropertyGetReq, "testprimitives");
    e1->ServicePath = "aservicepath";
    e1->RequestID = 134576;
    e1->MetaData = "md";

    // Test all primitive types
    double v1d[] = {1, 2, 3, 4, 5, 7.45, 8.9832};
    e1->AddElement("v1", AttachRRArrayCopy(v1d, 7));
    float v2d[] = {1, 2, 34};
    e1->AddElement("v2", AttachRRArrayCopy(v2d, 3));
    int8_t v3d[] = {1, -2, 3, 127};
    e1->AddElement("v3", AttachRRArrayCopy(v3d, 4));
    uint8_t v4d[] = {1, 2, 3, 4, 5};
    e1->AddElement("v4", AttachRRArrayCopy(v4d, 5));
    int16_t v5d[] = {1, 2, 3, -4, 5, 19746, 9870};
    e1->AddElement("v5", AttachRRArrayCopy(v5d, 7));
    uint16_t v6d[] = {1, 2, 3, 4, 5, 19746, 9870};
    e1->AddElement("v6", AttachRRArrayCopy(v6d, 7));
    int32_t v7d[] = {1, 2, 3, -4, 5, 19746, 9870, 2045323432};
    e1->AddElement("v7", AttachRRArrayCopy(v7d, 8));
    uint32_t v8d[] = {1, 2, 3, 4, 5, 19746, 9870, 345323432};
    e1->AddElement("v8", AttachRRArrayCopy(v8d, 8));
    int64_t v9d[] = {1, 2, 3, 4, 5, -19746, 9870, 9111111222345323432};
    e1->AddElement("v9", AttachRRArrayCopy(v9d, 8));
    uint64_t v10d[] = {1, 2, 3, 4, 5, 19746, 9870, 12111111222345323432u};
    e1->AddElement("v10", AttachRRArrayCopy(v10d, 8));
    e1->AddElement("v11", stringToRRArray("This is a test string"));
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > v12 = AllocateRRArray<uint32_t>(1024 * 1024);
    for (size_t i = 0; i < v12->size(); i++)
        (*v12)[i] = i;
    e1->AddElement("v12", v12);
    e1->AddElement("v13", RR_INTRUSIVE_PTR<RRBaseArray>());

    // Test vector

    RR_INTRUSIVE_PTR<MessageEntry> e2 = CreateMessageEntry(MessageEntryType_FunctionCallReq, "testavector");
    e2->RequestID = 4563;
    e2->ServicePath = "aservicepath.o2";

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > mv = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
    double mv_d1[] = {1, 2};
    double mv_d2[] = {1000, -2000.10};
    mv.push_back(CreateMessageElement("0", AttachRRArrayCopy(mv_d1, 2)));
    mv.push_back(CreateMessageElement("1", AttachRRArrayCopy(mv_d2, 2)));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> v =
        CreateMessageElementNestedElementList(DataTypes_vector_t, "", mv);
    e2->AddElement("testavector", v);

    // Test dictionary
    RR_INTRUSIVE_PTR<MessageEntry> e3 = CreateMessageEntry(MessageEntryType_FunctionCallRes, "testadictionary");
    e3->RequestID = 4567;
    e3->ServicePath = "aservicepath.o3";

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > md = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
    float md_d1[] = {1, 2};
    float md_d2[] = {1000, -2000.10f};
    md.push_back(CreateMessageElement("val1", AttachRRArrayCopy(md_d1, 2)));
    md.push_back(CreateMessageElement("val2", AttachRRArrayCopy(md_d2, 2)));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> d =
        CreateMessageElementNestedElementList(DataTypes_dictionary_t, "", md);
    e3->AddElement("testavector", d);

    // Test structure
    RR_INTRUSIVE_PTR<MessageEntry> e4 = CreateMessageEntry(MessageEntryType_EventReq, "testnamedarray");
    e4->RequestID = 4568;
    e4->ServicePath = "aservicepath.o4";

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > ms = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
    int64_t ms_d1[] = {1, 2, 3, 4, 5, -19746, 9870, 345323432};
    ms.push_back(CreateMessageElement("field1", AttachRRArrayCopy(ms_d1, 2)));
    ms.push_back(CreateMessageElement("field2", v));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> s =
        CreateMessageElementNestedElementList(DataTypes_structure_t, "RobotRaconteurTestService.TestStruct", ms);
    e4->AddElement("teststruct", s);

    // Test MultiDimArray

    RR_INTRUSIVE_PTR<MessageEntry> e5 = CreateMessageEntry(MessageEntryType_PipePacket, "testamultidimarray");
    e5->RequestID = 4569;
    e5->ServicePath = "aservicepath.o5";

    RR_INTRUSIVE_PTR<RRArray<double> > real = AllocateRRArray<double>(125);
    RR_INTRUSIVE_PTR<RRArray<double> > imag = AllocateRRArray<double>(125);

    for (int32_t i = 0; i < 125; i++)
        (*real)[i] = (((double)rand()) / ((double)RAND_MAX) - 0.5) * 1e5;
    for (int32_t i = 0; i < 125; i++)
        (*imag)[i] = (((double)rand()) / ((double)RAND_MAX) - 0.5) * 1e5;

    uint32_t dims1[] = {5, 5, 5};
    uint32_t dims2[] = {25, 5};

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a1 = AllocateRRMultiDimArray<double>(AttachRRArrayCopy(dims1, 3), real);
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a2 = AllocateRRMultiDimArray<double>(AttachRRArrayCopy(dims2, 2), real);
    e5->AddElement("ar1", RobotRaconteurNode::s()->PackMultiDimArray(a1));
    e5->AddElement("ar2", RobotRaconteurNode::s()->PackMultiDimArray(a2));

    // Test list

    RR_INTRUSIVE_PTR<MessageEntry> e6 = CreateMessageEntry(MessageEntryType_PipePacket, "testalist");
    e6->RequestID = 459;
    e6->ServicePath = "aservicepath.o6";
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > ml = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
    float md_l1[] = {1, 3};
    float md_l2[] = {1003, -2000.10f};
    ml.push_back(CreateMessageElement("val1", AttachRRArrayCopy(md_l1, 2)));
    ml.push_back(CreateMessageElement("val2", AttachRRArrayCopy(md_l2, 2)));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> l =
        CreateMessageElementNestedElementList(DataTypes_list_t, "", ml);
    e6->AddElement("testalist", l);

    // Create a new message
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
    h->ReceiverEndpoint = 1023;
    h->SenderEndpoint = 9876;
    h->SenderNodeID = NodeID::NewUniqueID();
    h->ReceiverNodeID = NodeID::NewUniqueID();
    h->SenderNodeName = "Sender";
    h->ReceiverNodeName = "Recv";
    h->MetaData = "meta";

    m->header = h;
    m->entries.push_back(e1);
    m->entries.push_back(e2);
    m->entries.push_back(e3);
    m->entries.push_back(e4);
    m->entries.push_back(e5);
    m->entries.push_back(e6);

    return m;
}

static std::string MessageSerializationTest_NewRandomString(mt19937& rng, size_t max_len)
{
    std::stringstream buf;
    std::string chars("abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "1234567890");

    uniform_int_distribution<uint32_t> size_t_dist(0, max_len);
    size_t l = size_t_dist(rng);

    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
    for (size_t i = 0; i < l; ++i)
    {
        buf << chars[index_dist(rng)];
    }

    return buf.str();
}

static RR_INTRUSIVE_PTR<MessageElement> MessageSerializationTest_NewRandomMessageElement(mt19937& rng, size_t depth)
{
    uniform_int_distribution<uint16_t> type_switch_dist(0, 1);
    uniform_int_distribution<uint16_t> type_dist(0, 11);
    uniform_int_distribution<uint16_t> n_type_dist(0, 4);
    uniform_int_distribution<uint8_t> uint8_dist;
    uniform_int_distribution<uint32_t> uint32_dist;
    uniform_int_distribution<int32_t> int32_dist;
    uniform_int_distribution<size_t> len_dist(0, 256);
    uniform_int_distribution<size_t> len_sub_dist(1, 8);
    RR_INTRUSIVE_PTR<MessageElement> e = CreateMessageElement();
    if (type_switch_dist(rng) == 0 || depth > 2)
    {
        e->ElementType = (DataTypes)type_dist(rng);
    }
    else
    {
        uint16_t t1 = n_type_dist(rng);
        switch (t1)
        {
        case 0:
            e->ElementType = DataTypes_structure_t;
            break;
        case 1:
            e->ElementType = DataTypes_vector_t;
            break;
        case 2:
            e->ElementType = DataTypes_dictionary_t;
            break;
        case 3:
            e->ElementType = DataTypes_multidimarray_t;
            break;
        case 4:
            e->ElementType = DataTypes_list_t;
            break;
        }
    }

    e->ElementName = MessageSerializationTest_NewRandomString(rng, 128);
    e->ElementTypeName = MessageSerializationTest_NewRandomString(rng, 128);
    e->MetaData = MessageSerializationTest_NewRandomString(rng, 128);

    switch (e->ElementType)
    {
    case DataTypes_void_t:
        return e;

    case DataTypes_double_t:
    case DataTypes_single_t:
    case DataTypes_int8_t:
    case DataTypes_uint8_t:
    case DataTypes_int16_t:
    case DataTypes_uint16_t:
    case DataTypes_int32_t:
    case DataTypes_uint32_t:
    case DataTypes_int64_t:
    case DataTypes_uint64_t:
    case DataTypes_string_t: {
        RR_INTRUSIVE_PTR<RRBaseArray> a = AllocateRRArrayByType(e->ElementType, len_dist(rng));
        uint8_t* a2 = (uint8_t*)a->void_ptr();
        for (size_t i = 0; i < a->ElementSize() * a->size(); i++)
        {
            a2[i] = uint8_dist(rng);
        }
        e->SetData(a);
        return e;
    }
    case DataTypes_structure_t: {
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
        size_t n = len_sub_dist(rng);
        for (size_t i = 0; i < n; i++)
        {
            v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
        }
        e->SetData(CreateMessageElementNestedElementList(DataTypes_structure_t,
                                                         MessageSerializationTest_NewRandomString(rng, 128), v));
        return e;
    }
    case DataTypes_vector_t: {
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
        size_t n = len_sub_dist(rng);
        for (size_t i = 0; i < n; i++)
        {
            v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
        }
        e->SetData(CreateMessageElementNestedElementList(DataTypes_vector_t, "", v));
        return e;
    }
    case DataTypes_dictionary_t: {
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
        size_t n = len_sub_dist(rng);
        for (size_t i = 0; i < n; i++)
        {
            v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
        }
        e->SetData(CreateMessageElementNestedElementList(DataTypes_dictionary_t, "", v));
        return e;
    }
    case DataTypes_multidimarray_t: {
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
        size_t n = len_sub_dist(rng);
        if (n > 4)
            n = 4;
        for (size_t i = 0; i < n; i++)
        {
            v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, 10));
        }
        e->SetData(CreateMessageElementNestedElementList(DataTypes_multidimarray_t, "", v));
        return e;
    }
    case DataTypes_list_t: {
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
        size_t n = len_sub_dist(rng);
        for (size_t i = 0; i < n; i++)
        {
            v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
        }
        e->SetData(CreateMessageElementNestedElementList(DataTypes_list_t, "", v));
        return e;
    }

    default:
        throw std::runtime_error("Invalid random type");
    }
}

RR_INTRUSIVE_PTR<Message> MessageSerializationTest::NewRandomTestMessage(boost::random::mt19937& rng)
{
    uniform_int_distribution<uint8_t> uint8_dist;
    uniform_int_distribution<uint16_t> uint16_dist;
    uniform_int_distribution<uint32_t> uint32_dist;
    uniform_int_distribution<size_t> small_dist(1, 4);
    uniform_int_distribution<int32_t> int32_dist;
    uniform_int_distribution<int64_t> int64_dist;

    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
    m->header = h;

    boost::array<uint8_t, 16> b;
    for (size_t i = 0; i < 16; i++)
        b[i] = uint8_dist(rng);
    h->SenderNodeID = NodeID(b);
    for (size_t i = 0; i < 16; i++)
        b[i] = uint8_dist(rng);
    h->ReceiverNodeID = NodeID(b);
    h->SenderNodeName = MessageSerializationTest_NewRandomString(rng, 64);
    h->ReceiverNodeName = MessageSerializationTest_NewRandomString(rng, 64);

    h->SenderEndpoint = uint32_dist(rng);
    h->ReceiverEndpoint = uint32_dist(rng);
    h->MetaData = MessageSerializationTest_NewRandomString(rng, 256);
    h->MessageID = uint16_dist(rng);
    h->MessageResID = uint16_dist(rng);
    h->EntryCount = (int16_t)small_dist(rng);

    // Add a large entry sometimes
    uint32_t add_large_entry = small_dist(rng);

    for (size_t i = 0; i < h->EntryCount; i++)
    {
        if (m->ComputeSize() > 10 * 1024 * 1024)
            break;

        RR_INTRUSIVE_PTR<MessageEntry> ee = CreateMessageEntry();

        uniform_int_distribution<uint16_t> entry_type_dist(101, 120);
        ee->EntryType = (MessageEntryType)entry_type_dist(rng);

        ee->ServicePath = MessageSerializationTest_NewRandomString(rng, 256);
        ee->ServicePath = MessageSerializationTest_NewRandomString(rng, 256);
        ee->RequestID = uint32_dist(rng);
        uniform_int_distribution<uint16_t> error_dist(1, 10);
        ee->Error = (MessageErrorType)error_dist(rng);
        ee->MetaData = MessageSerializationTest_NewRandomString(rng, 256);

        if (add_large_entry == 1 && i == 0)
        {
            uniform_int_distribution<size_t> large_len_dist(512 * 1024, 1024 * 1024);
            size_t l = large_len_dist(rng);
            RR_INTRUSIVE_PTR<RRArray<uint32_t> > a = AllocateRRArray<uint32_t>(l);
            uint32_t* a2 = a->data();
            for (size_t j = 0; j < l; j++)
            {
                a2[j] = uint32_dist(rng);
            }

            RR_INTRUSIVE_PTR<MessageElement> el = MessageSerializationTest_NewRandomMessageElement(rng, 10);
            el->SetData(a);
            ee->elements.push_back(el);
        }
        else
        {
            uniform_int_distribution<size_t> el_count_dist(0, 16);
            size_t n1 = el_count_dist(rng);
            for (size_t j = 0; j < n1; j++)
            {
                RR_INTRUSIVE_PTR<MessageElement> el = MessageSerializationTest_NewRandomMessageElement(rng, 0);
                ee->elements.push_back(el);
            }
        }

        m->entries.push_back(ee);
    }

    return m;
}

void MessageSerializationTest::RandomTest(size_t iterations)
{
    mt19937 rng;
    rng.seed((uint32_t)std::time(0));

    for (size_t i = 0; i < iterations; i++)
    {
        RR_INTRUSIVE_PTR<Message> m = NewRandomTestMessage(rng);

        // Write to stream and read back
        size_t message_size = m->ComputeSize();
        boost::shared_array<uint8_t> buf(new uint8_t[message_size]);
        ArrayBinaryWriter w(buf.get(), 0, message_size);
        m->Write(w);

        if (w.Position() != m->ComputeSize())
            throw std::runtime_error("");

        ArrayBinaryReader r(buf.get(), 0, message_size);

        RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
        m2->Read(r);

        MessageSerializationTest::CompareMessage(m, m2);
    }
}

void MessageSerializationTest::Test()
{
    RR_INTRUSIVE_PTR<Message> m = NewTestMessage();

    // Write to stream and read back
    size_t message_size = m->ComputeSize();
    boost::shared_array<uint8_t> buf(new uint8_t[message_size]);
    ArrayBinaryWriter w(buf.get(), 0, message_size);
    m->Write(w);

    if (w.Position() != m->ComputeSize())
        throw std::runtime_error("");

    ArrayBinaryReader r(buf.get(), 0, message_size);

    RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
    m2->Read(r);

    // Check to make sure the messages match
    CompareMessage(m, m2);
}

void MessageSerializationTest::CompareMessage(RR_INTRUSIVE_PTR<Message> m1, RR_INTRUSIVE_PTR<Message> m2)
{
    RR_INTRUSIVE_PTR<MessageHeader> h1 = m1->header;
    RR_INTRUSIVE_PTR<MessageHeader> h2 = m2->header;

    if (h1->MessageSize != h2->MessageSize)
        throw std::runtime_error("");
    if (h1->MessageFlags != h2->MessageFlags)
        throw std::runtime_error("");

    if (h1->MessageFlags & MessageFlags_ROUTING_INFO)
    {
        if (h1->SenderNodeID != h2->SenderNodeID)
            throw std::runtime_error("");
        if (h1->ReceiverNodeID != h2->ReceiverNodeID)
            throw std::runtime_error("");
        if (h1->SenderNodeName != h2->SenderNodeName)
            throw std::runtime_error("");
        if (h1->ReceiverNodeName != h2->ReceiverNodeName)
            throw std::runtime_error("");
    }

    if (h1->MessageFlags & MessageFlags_ENDPOINT_INFO)
    {
        if (h1->SenderEndpoint != h2->SenderEndpoint)
            throw std::runtime_error("");
        if (h1->ReceiverEndpoint != h2->ReceiverEndpoint)
            throw std::runtime_error("");
    }

    if (h1->MessageFlags & MessageFlags_PRIORITY)
    {
        if (h1->Priority != h2->Priority)
            throw std::runtime_error("");
    }

    if (h1->MessageFlags & MessageFlags_META_INFO)
    {
        if (h1->MetaData != h2->MetaData)
            throw std::runtime_error("");
        if (h1->MessageID != h2->MessageID)
            throw std::runtime_error("");
        if (h1->MessageResID != h2->MessageResID)
            throw std::runtime_error("");
    }

    if (h1->MessageFlags & MessageFlags_STRING_TABLE)
    {
        if (h1->StringTable.size() != h2->StringTable.size())
            throw std::runtime_error("");
        for (size_t i = 0; i < h1->StringTable.size(); i++)
        {
            if (h1->StringTable.at(i).get<0>() != h2->StringTable.at(i).get<0>())
                throw std::runtime_error("");
            if (h1->StringTable.at(i).get<1>() != h2->StringTable.at(i).get<1>())
                throw std::runtime_error("");
        }
    }

    if (h1->MessageFlags & MessageFlags_EXTENDED)
    {
        if (h1->Extended != h2->Extended)
            throw std::runtime_error("");
    }

    if (h1->EntryCount != h2->EntryCount)
        throw std::runtime_error("");
    if (m1->entries.size() != m2->entries.size())
        throw std::runtime_error("");
    for (size_t i = 0; i < m1->entries.size(); i++)
    {
        CompareMessageEntry(m1->entries.at(i), m2->entries.at(i));
    }
}

void MessageSerializationTest::CompareMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m1, RR_INTRUSIVE_PTR<MessageEntry> m2)
{
    if (m1->EntrySize != m2->EntrySize)
        throw std::runtime_error("");
    if (m1->EntryFlags != m2->EntryFlags)
        throw std::runtime_error("");
    if (m1->EntryType != m2->EntryType)
        throw std::runtime_error("");

    if (m1->EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
    {
        if (m1->ServicePath != m2->ServicePath)
            throw std::runtime_error("");
    }
    if (m1->EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
    {
        if (m1->ServicePathCode != m2->ServicePathCode)
            throw std::runtime_error("");
    }

    if (m1->EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
    {
        if (m1->MemberName != m2->MemberName)
            throw std::runtime_error("");
    }
    if (m1->EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
    {
        if (m1->MemberNameCode != m2->MemberNameCode)
            throw std::runtime_error("");
    }

    if (m1->EntryFlags & MessageEntryFlags_REQUEST_ID)
    {
        if (m1->RequestID != m2->RequestID)
            throw std::runtime_error("");
    }

    if (m1->EntryFlags & MessageEntryFlags_ERROR)
    {
        if (m1->Error != m2->Error)
            throw std::runtime_error("");
    }

    if (m1->EntryFlags & MessageEntryFlags_META_INFO)
    {
        if (m1->MetaData != m2->MetaData)
            throw std::runtime_error("");
    }

    if (m1->EntryFlags & MessageEntryFlags_EXTENDED)
    {
        if (m1->Extended != m2->Extended)
            throw std::runtime_error("");
    }

    if (m1->elements.size() != m2->elements.size())
        throw std::runtime_error("");

    for (size_t i = 0; i < m1->elements.size(); i++)
    {
        CompareMessageElement(m1->elements.at(i), m2->elements.at(i));
    }
}

static void MessageSerializationTest_CompareSubElements(RR_INTRUSIVE_PTR<MessageElement> m1,
                                                        RR_INTRUSIVE_PTR<MessageElement> m2)
{
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat1 = m1->CastDataToNestedList();
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat2 = m1->CastDataToNestedList();

    if (sdat1->Elements.size() != sdat2->Elements.size())
        throw std::runtime_error("");
    for (size_t i = 0; i < sdat1->Elements.size(); i++)
    {
        MessageSerializationTest::CompareMessageElement(sdat1->Elements.at(i), sdat2->Elements.at(i));
    }
}

void MessageSerializationTest::CompareMessageElement(RR_INTRUSIVE_PTR<MessageElement> m1,
                                                     RR_INTRUSIVE_PTR<MessageElement> m2)
{
    if (m1->ElementSize != m2->ElementSize)
        throw std::runtime_error("");
    if (m1->ElementFlags != m2->ElementFlags)
        throw std::runtime_error("");
    if (m1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
    {
        if (m1->ElementName != m2->ElementName)
            throw std::runtime_error("");
    }
    if (m1->ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
    {
        if (m1->ElementNameCode != m2->ElementNameCode)
            throw std::runtime_error("");
    }

    if (m1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
    {
        if (m1->ElementNumber != m2->ElementNumber)
            throw std::runtime_error("");
    }

    if (m1->ElementType != m2->ElementType)
        throw std::runtime_error("");

    if (m1->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
    {
        if (m1->ElementTypeName != m2->ElementTypeName)
            throw std::runtime_error("");
    }
    if (m1->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
    {
        if (m1->ElementTypeNameCode != m2->ElementTypeNameCode)
            throw std::runtime_error("");
    }

    if (m1->ElementFlags & MessageElementFlags_META_INFO)
    {
        if (m1->MetaData != m2->MetaData)
            throw std::runtime_error("");
    }

    if (m1->ElementFlags & 0x40)
        throw std::runtime_error("");

    if (m1->ElementFlags & MessageElementFlags_EXTENDED)
    {
        if (m1->Extended != m2->Extended)
            throw std::runtime_error("");
    }

    if (m1->DataCount != m2->DataCount)
        throw std::runtime_error("");

    if ((m1->ElementType == DataTypes_void_t) && m1->DataCount != 0)
        throw std::runtime_error("");

    switch (m1->ElementType)
    {
    case DataTypes_void_t:
        break;
    case DataTypes_double_t:
    case DataTypes_single_t:
    case DataTypes_int8_t:
    case DataTypes_uint8_t:
    case DataTypes_int16_t:
    case DataTypes_uint16_t:
    case DataTypes_int32_t:
    case DataTypes_uint32_t:
    case DataTypes_int64_t:
    case DataTypes_uint64_t:
    case DataTypes_string_t: {
        RR_INTRUSIVE_PTR<RRBaseArray> a1 = m1->CastData<RRBaseArray>();
        RR_INTRUSIVE_PTR<RRBaseArray> a2 = m2->CastData<RRBaseArray>();
        if (a1->size() != m1->DataCount)
            throw std::runtime_error("");
        if (a2->size() != m2->DataCount)
            throw std::runtime_error("");
        if (a1->GetTypeID() != m1->ElementType)
            throw std::runtime_error("");
        if (a2->GetTypeID() != m2->ElementType)
            throw std::runtime_error("");
        if (memcmp(a1->void_ptr(), a2->void_ptr(), a1->size() * a1->ElementSize()) != 0)
        {
            throw std::runtime_error("");
        }
        break;
    }
    case DataTypes_structure_t:
    case DataTypes_vector_t:
    case DataTypes_dictionary_t:
    case DataTypes_multidimarray_t:
    case DataTypes_list_t:
    case DataTypes_pod_t:
    case DataTypes_pod_array_t:
    case DataTypes_pod_multidimarray_t:
    case DataTypes_namedarray_array_t:
    case DataTypes_namedarray_multidimarray_t: {
        MessageSerializationTest_CompareSubElements(m1, m2);
        break;
    }
    default:
        throw std::runtime_error("");
    }
}
} // namespace RobotRaconteurTest
