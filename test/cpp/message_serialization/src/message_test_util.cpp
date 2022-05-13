#include "message_test_util.h"
#include "array_compare.h"

#include <RobotRaconteur.h>

#include <gtest/gtest.h>

using namespace RobotRaconteur;

namespace RobotRaconteur
{
namespace test
{
    RR_INTRUSIVE_PTR<Message> NewTestMessage()
    {
        srand((uint32_t)time(NULL));

		RR_INTRUSIVE_PTR<MessageEntry> e1 = CreateMessageEntry(MessageEntryType_PropertyGetReq, "testprimitives");
		e1->ServicePath = "aservicepath";
		e1->RequestID = 134576;
		e1->MetaData = "md";

		//Test all primitive types
		double v1d[] = { 1, 2, 3, 4, 5,7.45,8.9832 };
		e1->AddElement("v1", AttachRRArrayCopy(v1d, 7));
		float v2d[] = { 1, 2, 34 };
		e1->AddElement("v2", AttachRRArrayCopy(v2d, 3));
		int8_t v3d[] = { 1, -2, 3, 127 };
		e1->AddElement("v3", AttachRRArrayCopy(v3d, 4));
		uint8_t v4d[] = { 1, 2, 3, 4, 5 };
		e1->AddElement("v4", AttachRRArrayCopy(v4d, 5));
		int16_t v5d[] = { 1, 2, 3, -4, 5,19746,9870 };
		e1->AddElement("v5", AttachRRArrayCopy(v5d, 7));
		uint16_t v6d[] = { 1, 2, 3, 4, 5, 19746, 9870 };
		e1->AddElement("v6", AttachRRArrayCopy(v6d, 7));
		int32_t v7d[] = { 1, 2, 3, -4, 5, 19746, 9870,2045323432 };
		e1->AddElement("v7", AttachRRArrayCopy(v7d, 8));
		uint32_t v8d[] = { 1, 2, 3, 4, 5, 19746, 9870, 345323432 };
		e1->AddElement("v8", AttachRRArrayCopy(v8d, 8));
		int64_t v9d[] = { 1, 2, 3, 4, 5, -19746, 9870, 9111111222345323432 };
		e1->AddElement("v9", AttachRRArrayCopy(v9d, 8));
		uint64_t v10d[] = { 1, 2, 3, 4, 5, 19746, 9870, 12111111222345323432u };
		e1->AddElement("v10", AttachRRArrayCopy(v10d, 8));
		e1->AddElement("v11", stringToRRArray("This is a test string"));
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > v12 = AllocateRRArray<uint32_t>(1024 * 1024);
		for (size_t i = 0; i < v12->size(); i++) (*v12)[i] = i;
		e1->AddElement("v12", v12);
		e1->AddElement("v13", RR_INTRUSIVE_PTR<RRBaseArray>());

		//Test vector

		RR_INTRUSIVE_PTR<MessageEntry> e2 = CreateMessageEntry(MessageEntryType_FunctionCallReq, "testavector");
		e2->RequestID = 4563;
		e2->ServicePath = "aservicepath.o2";

		std::vector<RR_INTRUSIVE_PTR<MessageElement> > mv = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
		double mv_d1[] = { 1, 2 };
		double mv_d2[] = { 1000,-2000.10 };
		mv.push_back(CreateMessageElement("0", AttachRRArrayCopy(mv_d1, 2)));
		mv.push_back(CreateMessageElement("1", AttachRRArrayCopy(mv_d2, 2)));
		RR_INTRUSIVE_PTR<MessageElementNestedElementList > v = CreateMessageElementNestedElementList(DataTypes_vector_t,"",mv);
		e2->AddElement("testavector", v);

		//Test dictionary
		RR_INTRUSIVE_PTR<MessageEntry> e3 = CreateMessageEntry(MessageEntryType_FunctionCallRes, "testadictionary");
		e3->RequestID = 4567;
		e3->ServicePath = "aservicepath.o3";

		std::vector<RR_INTRUSIVE_PTR<MessageElement> > md = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
		float md_d1[] = { 1, 2 };
		float md_d2[] = { 1000, -2000.10f };
		md.push_back(CreateMessageElement("val1", AttachRRArrayCopy(md_d1, 2)));
		md.push_back(CreateMessageElement("val2", AttachRRArrayCopy(md_d2, 2)));
		RR_INTRUSIVE_PTR<MessageElementNestedElementList > d = CreateMessageElementNestedElementList(DataTypes_dictionary_t, "", md);
		e3->AddElement("testavector", d);

		//Test structure
		RR_INTRUSIVE_PTR<MessageEntry> e4 = CreateMessageEntry(MessageEntryType_EventReq, "testnamedarray");
		e4->RequestID = 4568;
		e4->ServicePath = "aservicepath.o4";

		std::vector<RR_INTRUSIVE_PTR<MessageElement> > ms = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
		int64_t ms_d1[] = { 1, 2, 3, 4, 5, -19746, 9870, 345323432 };
		ms.push_back(CreateMessageElement("field1", AttachRRArrayCopy(ms_d1, 2)));
		ms.push_back(CreateMessageElement("field2", v));
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> s = CreateMessageElementNestedElementList(DataTypes_structure_t,"RobotRaconteurTestService.TestStruct", ms);
		e4->AddElement("teststruct", s);

		//Test MultiDimArray

		RR_INTRUSIVE_PTR<MessageEntry> e5 = CreateMessageEntry(MessageEntryType_PipePacket, "testamultidimarray");
		e5->RequestID = 4569;
		e5->ServicePath = "aservicepath.o5";

		RR_INTRUSIVE_PTR<RRArray<double> > real = AllocateRRArray<double>(125);

		for (int32_t i = 0; i < 125; i++)
			(*real)[i] = (((double)rand()) / ((double)RAND_MAX) - 0.5)*1e5;

		uint32_t dims1[] = { 5, 5, 5 };
		uint32_t dims2[] = { 25, 5 };

		RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a1 = AllocateRRMultiDimArray<double>(AttachRRArrayCopy(dims1, 3), real);
		RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a2 = AllocateRRMultiDimArray<double>(AttachRRArrayCopy(dims2, 2), real);
		e5->AddElement("ar1", RobotRaconteurNode::s()->PackMultiDimArray(a1));
		e5->AddElement("ar2", RobotRaconteurNode::s()->PackMultiDimArray(a2));

		//Test list

		RR_INTRUSIVE_PTR<MessageEntry> e6 = CreateMessageEntry(MessageEntryType_PipePacket, "testalist");
		e6->RequestID = 459;
		e6->ServicePath = "aservicepath.o6";
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > ml = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
		float md_l1[] = { 1, 3 };
		float md_l2[] = { 1003, -2000.10f };
		ml.push_back(CreateMessageElement("val1", AttachRRArrayCopy(md_l1, 2)));
		ml.push_back(CreateMessageElement("val2", AttachRRArrayCopy(md_l2, 2)));
		RR_INTRUSIVE_PTR<MessageElementNestedElementList > l = CreateMessageElementNestedElementList(DataTypes_list_t,"",ml);
		e6->AddElement("testalist", l);


		//Create a new message
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

    static RR_INTRUSIVE_PTR<MessageElement> MessageSerializationTest_NewRandomMessageElement(LFSRSeqGen& rng, size_t depth)
	{
		RR_INTRUSIVE_PTR<MessageElement> e = CreateMessageElement();		
		if (rng.NextDist(0,1) == 0 || depth > 2)
		{
			e->ElementType = (DataTypes)(rng.NextDist(0,11));
		}
		else
		{
			uint16_t t1 = rng.NextDist(0,4);
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
		
		e->ElementName = rng.NextStringVarLen(128);
		e->ElementTypeName = rng.NextStringVarLen(128);
		e->MetaData = rng.NextStringVarLen(128);
		

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
		case DataTypes_string_t:
		{
			RR_INTRUSIVE_PTR<RRBaseArray> a = rng.NextArrayByTypeVarLen(e->ElementType, 256);
            e->SetData(a);
			return e;
		}
		case DataTypes_structure_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_structure_t,rng.NextStringVarLen(128), v));
			return e;
		}
		case DataTypes_vector_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_vector_t,"",v));
			return e;
		}
		case DataTypes_dictionary_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_dictionary_t,"",v));
			return e;
		}
		case DataTypes_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			if (n > 4) n = 4;
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, 10));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_multidimarray_t,"",v));
			return e;
		}
		case DataTypes_list_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_list_t,"",v));
			return e;
		}

		default:
			BOOST_ASSERT(false);
		}
	}


    RR_INTRUSIVE_PTR<Message> NewRandomTestMessage(LFSRSeqGen& rng)
    {
		RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
		m->header = h;		
				
		boost::array < uint8_t, 16> b;
		for (size_t i = 0; i < 16; i++) b[i] = rng.NextUInt8();
		h->SenderNodeID = NodeID(b);
		for (size_t i = 0; i < 16; i++) b[i] = rng.NextUInt8();
		h->ReceiverNodeID = NodeID(b);
		h->SenderNodeName = rng.NextStringVarLen(64);
		h->ReceiverNodeName = rng.NextStringVarLen(64);
		
		h->SenderEndpoint = rng.NextUInt32();
		h->ReceiverEndpoint = rng.NextUInt32();
		h->MetaData = rng.NextStringVarLen(256);
		h->MessageID = rng.NextUInt16();
		h->MessageResID = rng.NextUInt16();
		h->EntryCount = rng.NextDist(1,4);
		
		//Add a large entry sometimes
		uint32_t add_large_entry = rng.NextDist(1,4);

		for (size_t i = 0; i < h->EntryCount; i++)
		{
			if (m->ComputeSize() > 10 * 1024 * 1024) break;

			RR_INTRUSIVE_PTR<MessageEntry> ee = CreateMessageEntry();
						
			ee->EntryType = (MessageEntryType)rng.NextDist(101,120);
						
			ee->ServicePath = rng.NextStringVarLen(256);
			ee->ServicePath = rng.NextStringVarLen(256);
			ee->RequestID = rng.NextUInt32();
			ee->Error = (MessageErrorType)rng.NextDist(1,10);
			ee->MetaData = rng.NextStringVarLen(256);

			if (add_large_entry == 1 && i == 0)
			{				
				size_t l = rng.NextDist(512 * 1024, 1024 * 1024);
				RR_INTRUSIVE_PTR<RRArray<uint32_t> > a = rng.NextUInt32Array(l);
				RR_INTRUSIVE_PTR<MessageElement> el = MessageSerializationTest_NewRandomMessageElement(rng, 10);
				el->SetData(a);
				ee->elements.push_back(el);
			}
			else
			{
				size_t n1 = rng.NextDist(0, 16);
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

    static uint8_t MessageSerializationTest4_NewRandomMessageFlags(LFSRSeqGen& rng)
	{
		uint8_t o = 0;
		for (size_t i = 0; i < 8; i++)
		{
			o = (o << 1) | rng.NextDist(0,1);
		}
		return o;
	}

	static uint8_t MessageSerializationTest4_NewRandomFlags(LFSRSeqGen& rng)
	{
		uint8_t o = 0;
		for (size_t i = 0; i < 8; i++)
		{
			o = (o << 1) | rng.NextDist(0,1);
		}
		return o;
	}

    static std::vector<uint8_t> MessageSerializationTest4_NewRandomExtended(LFSRSeqGen& rng, size_t max_len)
	{
		size_t l = rng.NextDist(0,max_len);

		std::vector<uint8_t> buf(l);

		for (size_t i=0; i<l; i++)
		{
			buf[i] = rng.NextUInt8();
		}

		return buf;

	}

    static RR_INTRUSIVE_PTR<MessageElement> MessageSerializationTest4_NewRandomMessageElement(LFSRSeqGen& rng, size_t depth)
	{
		RR_INTRUSIVE_PTR<MessageElement> e = CreateMessageElement();
		e->ElementFlags = MessageSerializationTest4_NewRandomFlags(rng);
		e->ElementFlags &= ~0x40;		
		if (rng.NextDist(0,1) == 0 || depth > 2)
		{
			e->ElementType = (DataTypes)rng.NextDist(0,14);
		}
		else
		{
			uint16_t t1 = rng.NextDist(0,9);
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
			case 5:
				e->ElementType = DataTypes_pod_t;
				break;
			case 6:
				e->ElementType = DataTypes_pod_array_t;
				break;
			case 7:
				e->ElementType = DataTypes_pod_multidimarray_t;
				break;
			case 8:
				e->ElementType = DataTypes_namedarray_array_t;
				break;
			case 9:
				e->ElementType = DataTypes_namedarray_multidimarray_t;
				break;
			}
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
		{			
			e->ElementName = rng.NextStringVarLen(128);
			e->ElementFlags &= ~MessageElementFlags_ELEMENT_NUMBER;
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
		{
			e->ElementNameCode = rng.NextUInt32();
			e->ElementFlags &= ~MessageElementFlags_ELEMENT_NUMBER;
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
		{
			e->ElementNumber = rng.NextInt32();
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
		{			
			e->ElementTypeName = rng.NextStringVarLen(128);
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
		{
			e->ElementTypeNameCode = rng.NextUInt32();
		}

		if (e->ElementFlags & MessageElementFlags_META_INFO)
		{			
			e->MetaData = rng.NextStringVarLen(128);			
		}

		if (e->ElementFlags & MessageElementFlags_EXTENDED)
		{
			e->Extended = MessageSerializationTest4_NewRandomExtended(rng, 32);
		}

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
		case DataTypes_string_t:
        case DataTypes_cdouble_t:
		case DataTypes_csingle_t:
        case DataTypes_bool_t:
		{
			RR_INTRUSIVE_PTR<RRBaseArray> a = rng.NextArrayByTypeVarLen(e->ElementType, 256);
			e->SetData(a);
			return e;
		}
		case DataTypes_structure_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_structure_t,rng.NextStringVarLen(128),v));
			return e;
		}
		case DataTypes_vector_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_vector_t,"",v));
			return e;
		}
		case DataTypes_dictionary_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_dictionary_t,"",v));
			return e;
		}
		case DataTypes_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			if (n > 4) n = 4;
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, 10));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_multidimarray_t,"",v));
			return e;
		}
		case DataTypes_list_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_list_t,"",v));
			return e;
		}

		case DataTypes_pod_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_pod_t, "", v));
			return e;
		}
		case DataTypes_pod_array_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_pod_array_t,rng.NextStringVarLen(128),v));
			return e;
		}
		case DataTypes_pod_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_pod_multidimarray_t,rng.NextStringVarLen(128),v));
			return e;
		}
		case DataTypes_namedarray_array_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_namedarray_array_t,rng.NextStringVarLen(128), v));
			return e;
		}
		case DataTypes_namedarray_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = rng.NextDist(1,8);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest4_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNestedElementList(DataTypes_namedarray_multidimarray_t,rng.NextStringVarLen(128), v));
			return e;
		}
		default:
			BOOST_ASSERT(false);
		}
	}

    RR_INTRUSIVE_PTR<Message> NewRandomTestMessage4(LFSRSeqGen& rng)
    {
        RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
		m->header = h;
		h->MessageFlags = MessageSerializationTest4_NewRandomMessageFlags(rng);
		/*h->MessageFlags &= ~MessageFlags_ROUTING_INFO;
		h->MessageFlags &= ~MessageFlags_ENDPOINT_INFO;
		h->MessageFlags &= ~MessageFlags_PRIORITY;
		h->MessageFlags &= ~MessageFlags_META_INFO;
		h->MessageFlags &= ~MessageFlags_STRING_TABLE;
		h->MessageFlags &= ~MessageFlags_EXTENDED;*/
		if (h->MessageFlags & MessageFlags_PRIORITY)
		{
			h->Priority = rng.NextUInt16();
		}
		if (h->MessageFlags & MessageFlags_ROUTING_INFO)
		{
			boost::array < uint8_t, 16> b;
			for (size_t i = 0; i < 16; i++) b[i] = rng.NextUInt8();
			h->SenderNodeID = NodeID(b);
			for (size_t i = 0; i < 16; i++) b[i] = rng.NextUInt8();
			h->ReceiverNodeID = NodeID(b);
			h->SenderNodeName = rng.NextStringVarLen(64);
			h->ReceiverNodeName = rng.NextStringVarLen(64);
		}

		if (h->MessageFlags & MessageFlags_ENDPOINT_INFO)
		{
			h->SenderEndpoint = rng.NextUInt32();
			h->ReceiverEndpoint = rng.NextUInt32();
		}

		if (h->MessageFlags & MessageFlags_META_INFO)
		{
			h->MetaData = rng.NextStringVarLen(256);
			h->MessageID = rng.NextUInt16();
			h->MessageResID = rng.NextUInt16();
		}

		if (h->MessageFlags & MessageFlags_STRING_TABLE)
		{
			size_t n = rng.NextDist(1,4);
			for (size_t i = 0; i < n; i++)
			{
				h->StringTable.push_back(boost::make_tuple(rng.NextUInt32(), rng.NextStringVarLen(128)));
			}
		}

		if (h->MessageFlags & MessageFlags_MULTIPLE_ENTRIES)
		{
			h->EntryCount = (int16_t)rng.NextDist(1,4);
		}
		else
		{
			h->EntryCount = 1;
		}

		if (h->MessageFlags & MessageFlags_EXTENDED)
		{
			h->Extended = MessageSerializationTest4_NewRandomExtended(rng, 32);			
		}

		//Add a large entry sometimes
		uint32_t add_large_entry = rng.NextDist(1,4);

		for (size_t i = 0; i < h->EntryCount; i++)
		{
			if (m->ComputeSize4() > 10 * 1024 * 1024) break;

			RR_INTRUSIVE_PTR<MessageEntry> ee = CreateMessageEntry();

			ee->EntryFlags = MessageSerializationTest4_NewRandomFlags(rng);
			
		    ee->EntryType = (MessageEntryType)rng.NextDist(101,120);

			if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
			{				
				ee->ServicePath = rng.NextStringVarLen(256);
			}
			if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
			{				
				ee->ServicePathCode = rng.NextUInt32();
			}
			if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
			{				
				ee->MemberName = rng.NextStringVarLen(256);
			}
			if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
			{				
				ee->MemberNameCode = rng.NextUInt32();
			}
			
			if (ee->EntryFlags & MessageEntryFlags_REQUEST_ID)
			{
				ee->RequestID = rng.NextUInt32();
			}
			if (ee->EntryFlags & MessageEntryFlags_ERROR)
			{
				ee->Error = (MessageErrorType)rng.NextDist(1,10);
			}
			if (ee->EntryFlags & MessageEntryFlags_META_INFO)
			{				
				ee->MetaData = rng.NextStringVarLen(256);
			}
			
			if (ee->EntryFlags & MessageFlags_EXTENDED)
			{
				ee->Extended = MessageSerializationTest4_NewRandomExtended(rng, 32);
			}
			
			if (add_large_entry == 1 && i == 0)
			{
				size_t l = rng.NextDist(512 * 1024, 1024 * 1024);
				RR_INTRUSIVE_PTR<RRArray<uint32_t> > a = AllocateRRArray<uint32_t>(l);
				uint32_t* a2 = a->data();
				for (size_t j = 0; j < l; j++)
				{
					a2[j] = rng.NextUInt32();
				}

				RR_INTRUSIVE_PTR<MessageElement> el = MessageSerializationTest4_NewRandomMessageElement(rng, 10);
				el->SetData(a);
				ee->elements.push_back(el);
			}
			else
			{
				size_t n1 = rng.NextDist(0,16);
				for (size_t j = 0; j < n1; j++)
				{
					RR_INTRUSIVE_PTR<MessageElement> el= MessageSerializationTest4_NewRandomMessageElement(rng, 0);
					ee->elements.push_back(el);
				}
			}

			m->entries.push_back(ee);
		}
				
		return m;
    }

    void CompareMessage(RR_INTRUSIVE_PTR<Message> m1, RR_INTRUSIVE_PTR<Message> m2)
    {
        RR_INTRUSIVE_PTR<MessageHeader> h1 = m1->header;
		RR_INTRUSIVE_PTR<MessageHeader> h2 = m2->header;

		EXPECT_EQ(h1->MessageSize, h2->MessageSize);
		EXPECT_EQ(h1->MessageFlags, h2->MessageFlags);

		if (h1->MessageFlags & MessageFlags_ROUTING_INFO)
		{
			EXPECT_EQ(h1->SenderNodeID, h2->SenderNodeID);
			EXPECT_EQ(h1->ReceiverNodeID, h2->ReceiverNodeID);
			EXPECT_EQ(h1->SenderNodeName, h2->SenderNodeName);
			EXPECT_EQ(h1->ReceiverNodeName, h2->ReceiverNodeName);
		}

		if (h1->MessageFlags & MessageFlags_ENDPOINT_INFO)
		{
			EXPECT_EQ(h1->SenderEndpoint, h2->SenderEndpoint);
			EXPECT_EQ(h1->ReceiverEndpoint, h2->ReceiverEndpoint);
		}

		if (h1->MessageFlags & MessageFlags_PRIORITY)
		{
			EXPECT_EQ(h1->Priority, h2->Priority);
		}

		if (h1->MessageFlags & MessageFlags_META_INFO)
		{
			EXPECT_EQ(h1->MetaData, h2->MetaData);
			EXPECT_EQ(h1->MessageID, h2->MessageID);
			EXPECT_EQ(h1->MessageResID, h2->MessageResID);
		}

		if (h1->MessageFlags & MessageFlags_STRING_TABLE)
		{
			EXPECT_EQ(h1->StringTable.size(), h2->StringTable.size());
            if (h1->StringTable.size() == h2->StringTable.size())
            {
                for (size_t i = 0; i < h1->StringTable.size(); i++)
                {
                    EXPECT_EQ(h1->StringTable.at(i).get<0>(),h2->StringTable.at(i).get<0>());
                    EXPECT_EQ(h1->StringTable.at(i).get<1>(), h2->StringTable.at(i).get<1>());
                }
            }
		}

		if (h1->MessageFlags & MessageFlags_EXTENDED)
		{
			EXPECT_EQ(h1->Extended, h2->Extended);
		}

		EXPECT_EQ(h1->EntryCount, h2->EntryCount);
		EXPECT_EQ(m1->entries.size(), m2->entries.size());
		for (size_t i = 0; i < m1->entries.size() && i < m2->entries.size(); i++)
		{
			CompareMessageEntry(m1->entries.at(i), m2->entries.at(i));
		}
    }

    void CompareMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m1, RR_INTRUSIVE_PTR<MessageEntry> m2)
    {
        EXPECT_EQ(m1->EntrySize, m2->EntrySize);
		EXPECT_EQ(m1->EntryFlags, m2->EntryFlags);
		EXPECT_EQ(m1->EntryType, m2->EntryType);

		if (m1->EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
		{			
			EXPECT_EQ(m1->ServicePath, m2->ServicePath);
		}
		if (m1->EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
		{			
			EXPECT_EQ(m1->ServicePathCode, m2->ServicePathCode);
		}

		if (m1->EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
		{
			EXPECT_EQ(m1->MemberName, m2->MemberName);
		}
		if (m1->EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
		{
			EXPECT_EQ(m1->MemberNameCode, m2->MemberNameCode);
		}

		if (m1->EntryFlags & MessageEntryFlags_REQUEST_ID)
		{
			EXPECT_EQ(m1->RequestID, m2->RequestID);
		}

		if (m1->EntryFlags & MessageEntryFlags_ERROR)
		{			
			EXPECT_EQ(m1->Error, m2->Error);
		}

		if (m1->EntryFlags & MessageEntryFlags_META_INFO)
		{
			EXPECT_EQ(m1->MetaData, m2->MetaData);			
		}

		if (m1->EntryFlags & MessageEntryFlags_EXTENDED)
		{		
			EXPECT_EQ(m1->Extended, m2->Extended);
		}

		EXPECT_EQ(m1->elements.size(), m2->elements.size());

		for (size_t i = 0; i < m1->elements.size(); i++)
		{
			CompareMessageElement(m1->elements.at(i), m2->elements.at(i));
		}
    }

    static void MessageSerializationTest_CompareSubElements(RR_INTRUSIVE_PTR<MessageElement> m1, RR_INTRUSIVE_PTR<MessageElement> m2)
	{
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat1 = m1->CastDataToNestedList();
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat2 = m1->CastDataToNestedList();

		EXPECT_EQ(sdat1->Elements.size(), sdat2->Elements.size());
		for (size_t i = 0; i < sdat1->Elements.size() && i < sdat2->Elements.size(); i++)
		{
			CompareMessageElement(sdat1->Elements.at(i), sdat2->Elements.at(i));
		}
	}

    void CompareMessageElement(RR_INTRUSIVE_PTR<MessageElement> m1, RR_INTRUSIVE_PTR<MessageElement> m2)
    {
        EXPECT_EQ(m1->ElementSize, m2->ElementSize);
		EXPECT_EQ(m1->ElementFlags, m2->ElementFlags);
		if (m1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
		{
			EXPECT_EQ(m1->ElementName, m2->ElementName);
		}
		if (m1->ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
		{
			EXPECT_EQ(m1->ElementNameCode, m2->ElementNameCode);
		}

		if (m1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
		{
			EXPECT_EQ(m1->ElementNumber, m2->ElementNumber);
		}

		EXPECT_EQ(m1->ElementType, m2->ElementType);

		if (m1->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
		{
			EXPECT_EQ(m1->ElementTypeName, m2->ElementTypeName);
		}
		if (m1->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
		{
			EXPECT_EQ(m1->ElementTypeNameCode, m2->ElementTypeNameCode);
		}

		if (m1->ElementFlags & MessageElementFlags_META_INFO)
		{
			EXPECT_EQ(m1->MetaData, m2->MetaData);
		}

		EXPECT_EQ(m1->ElementFlags & 0x40, 0);

		if (m1->ElementFlags & MessageElementFlags_EXTENDED)
		{		
			EXPECT_EQ(m1->Extended, m2->Extended);
		}

		EXPECT_EQ(m1->DataCount, m2->DataCount);

		if(m1->ElementType == DataTypes_void_t)
			EXPECT_EQ(m1->DataCount,0);

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
		case DataTypes_string_t:
        case DataTypes_cdouble_t:
		case DataTypes_csingle_t:
        case DataTypes_bool_t:
		{
			RR_INTRUSIVE_PTR<RRBaseArray> a1 = m1->CastData<RRBaseArray>();
			RR_INTRUSIVE_PTR<RRBaseArray> a2 = m2->CastData<RRBaseArray>();
			EXPECT_EQ(a1->size(), m1->DataCount);
			EXPECT_EQ(a2->size(), m2->DataCount);
			EXPECT_EQ(a1->GetTypeID(), m1->ElementType);
			EXPECT_EQ(a2->GetTypeID(), m2->ElementType);
            EXPECT_RRBASEARRAY_EQ(a1,a2);
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
		case DataTypes_namedarray_multidimarray_t:
		{
			MessageSerializationTest_CompareSubElements(m1, m2);
			break;
		}
		default:
			FAIL();
		}
    }

}
}