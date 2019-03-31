#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "MessageSerializationTest3.h"

using namespace RobotRaconteur;


#include <RobotRaconteur.h>

#include <time.h>

#include <RobotRaconteur/StringTable.h>

#include "MessageSerializationTest.h"

#include <boost/random.hpp>

using namespace boost::random;

namespace RobotRaconteurTest
{
	static uint16_t MessageSerializationTest3_NewRandomMessageFlags(mt19937& rng)
	{
		uniform_int_distribution<uint16_t> bit_dist(0, 1);
		uint16_t o = 0;
		for (size_t i = 0; i < 14; i++)
		{
			o = (o << 1) | bit_dist(rng);
		}
		return o;
	}

	static uint8_t MessageSerializationTest3_NewRandomFlags(mt19937& rng)
	{
		uniform_int_distribution<uint16_t> bit_dist(0, 1);
		uint8_t o = 0;
		for (size_t i = 0; i < 8; i++)
		{
			o = (o << 1) | bit_dist(rng);
		}
		return o;
	}

	static std::string MessageSerializationTest3_NewRandomString(mt19937& rng, size_t max_len)
	{
		std::stringstream buf;
		std::string chars(
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"1234567890");

		uniform_int_distribution<uint32_t> size_t_dist(0, max_len);
		size_t l = size_t_dist(rng);

		boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
		for (size_t i = 0; i < l; ++i) {
			buf << chars[index_dist(rng)];
		}

		return buf.str();

	}

	static RR_INTRUSIVE_PTR<MessageElement> MessageSerializationTest3_NewRandomMessageElement(mt19937& rng, size_t depth)
	{
		uniform_int_distribution<uint16_t> type_switch_dist(0, 1);
		uniform_int_distribution<uint16_t> type_dist(0, 11);
		uniform_int_distribution<uint16_t> n_type_dist(0, 9);
		uniform_int_distribution<uint8_t>  uint8_dist;
		uniform_int_distribution<uint32_t> uint32_dist;
		uniform_int_distribution<int32_t> int32_dist;
		uniform_int_distribution<size_t> len_dist(0,256);
		uniform_int_distribution<size_t> len_sub_dist(1, 8);
		RR_INTRUSIVE_PTR<MessageElement> e = CreateMessageElement();
		e->ElementFlags = MessageSerializationTest3_NewRandomFlags(rng);
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
			e->ElementName = MessageSerializationTest3_NewRandomString(rng, 128);
			e->ElementFlags &= ~MessageElementFlags_ELEMENT_NUMBER;
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
		{
			e->ElementNameCode = uint32_dist(rng);
			e->ElementFlags &= ~MessageElementFlags_ELEMENT_NUMBER;
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
		{
			e->ElementNumber = int32_dist(rng);
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
		{			
			e->ElementTypeName = MessageSerializationTest3_NewRandomString(rng, 128);
		}

		if (e->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
		{
			e->ElementTypeNameCode = uint32_dist(rng);
		}

		if (e->ElementFlags & MessageElementFlags_SEQUENCE_NUMBER)
		{
			e->SequenceNumber = uint32_dist(rng);
		}

		if (e->ElementFlags & MessageElementFlags_META_INFO)
		{			
			e->MetaData = MessageSerializationTest3_NewRandomString(rng, 128);
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
		{
			RR_INTRUSIVE_PTR<RRBaseArray> a = AllocateRRArrayByType(e->ElementType, len_dist(rng));
			uint8_t* a2 = (uint8_t*)a->void_ptr();
			for (size_t i = 0; i < a->ElementSize() * a->size(); i++)
			{
				a2[i] = uint8_dist(rng);
			}
			e->SetData(a);
			return e;
		}
		case DataTypes_structure_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementStructure(MessageSerializationTest3_NewRandomString(rng, 128),v));
			return e;
		}
		case DataTypes_vector_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementMap<int32_t>(v));
			return e;
		}
		case DataTypes_dictionary_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementMap<std::string>(v));
			return e;
		}
		case DataTypes_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			if (n > 4) n = 4;
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, 10));
			}
			e->SetData(CreateMessageElementMultiDimArray(v));
			return e;
		}
		case DataTypes_list_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementList(v));
			return e;
		}

		case DataTypes_pod_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementPod(v));
			return e;
		}
		case DataTypes_pod_array_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementPodArray(MessageSerializationTest3_NewRandomString(rng, 128),v));
			return e;
		}
		case DataTypes_pod_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementPodMultiDimArray(MessageSerializationTest3_NewRandomString(rng, 128),v));
			return e;
		}
		case DataTypes_namedarray_array_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNamedArray(MessageSerializationTest3_NewRandomString(rng, 128), v));
			return e;
		}
		case DataTypes_namedarray_multidimarray_t:
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
			size_t n = len_sub_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				v.push_back(MessageSerializationTest3_NewRandomMessageElement(rng, depth + 1));
			}
			e->SetData(CreateMessageElementNamedMultiDimArray(MessageSerializationTest3_NewRandomString(rng, 128), v));
			return e;
		}
		default:
			throw std::runtime_error("Invalid random type");
		}
	}

	RR_INTRUSIVE_PTR<Message> MessageSerializationTest3::NewRandomTestMessage3(boost::random::mt19937& rng)
	{
		uniform_int_distribution<uint8_t>  uint8_dist;
		uniform_int_distribution<uint16_t> uint16_dist;
		uniform_int_distribution<uint32_t> uint32_dist;
		uniform_int_distribution<size_t> small_dist(1, 4);
		uniform_int_distribution<int32_t> int32_dist;
		uniform_int_distribution<int64_t> int64_dist;

		RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
		m->header = h;
		h->MessageFlags = MessageSerializationTest3_NewRandomMessageFlags(rng);
		if (h->MessageFlags & MessageFlags_SUBSTREAM_ID)
		{
			h->SubstreamID = uint16_dist(rng);
		}
		if (h->MessageFlags & MessageFlags_SUBSTREAM_SEQUENCE_NUMBER)
		{
			h->SubstreamSequenceNumber.SequenceNumber = uint32_dist(rng);
			h->SubstreamSequenceNumber.RecvSequenceNumber = uint32_dist(rng);
		}
		if (h->MessageFlags & MessageFlags_FRAGMENT)
		{
			h->FragmentHeader.FragmentMessageNumber = uint32_dist(rng);
			h->FragmentHeader.FragmentMessageSize = uint32_dist(rng);
			h->FragmentHeader.FragmentOffset = uint32_dist(rng);
		}
		if (h->MessageFlags & MessageFlags_UNRELIABLE_EXPIRATION)
		{
			h->UnreliableExpiration.seconds = int64_dist(rng);
			h->UnreliableExpiration.nanoseconds = int32_dist(rng);
		}
		if (h->MessageFlags & MessageFlags_PRIORITY)
		{
			h->Priority = uint16_dist(rng);			
		}
		if (h->MessageFlags & MessageFlags_ROUTING_INFO)
		{
			boost::array < uint8_t, 16> b;
			for (size_t i = 0; i < 16; i++) b[i] = uint8_dist(rng);
			h->SenderNodeID = NodeID(b);
			for (size_t i = 0; i < 16; i++) b[i] = uint8_dist(rng);
			h->ReceiverNodeID = NodeID(b);
			h->SenderNodeName = MessageSerializationTest3_NewRandomString(rng, 64);
			h->ReceiverNodeName = MessageSerializationTest3_NewRandomString(rng, 64);
		}

		if (h->MessageFlags & MessageFlags_ENDPOINT_INFO)
		{
			h->SenderEndpoint = uint32_dist(rng);
			h->ReceiverEndpoint = uint32_dist(rng);
		}

		if (h->MessageFlags & MessageFlags_META_INFO)
		{
			h->MetaData = MessageSerializationTest3_NewRandomString(rng, 256);
		}

		if (h->MessageFlags & MessageFlags_MESSAGE_ID)
		{
			h->MessageID = uint16_dist(rng);
			h->MessageResID = uint16_dist(rng);
		}

		if (h->MessageFlags & MessageFlags_STRING_TABLE)
		{
			size_t n = small_dist(rng);
			for (size_t i = 0; i < n; i++)
			{
				h->StringTable.push_back(boost::make_tuple(uint32_dist(rng), MessageSerializationTest3_NewRandomString(rng, 128)));
			}
		}

		if (h->MessageFlags & MessageFlags_MULTIPLE_ENTRIES)
		{
			h->EntryCount = (int16_t)small_dist(rng);
		}
		else
		{
			h->EntryCount = 1;
		}

		if (h->MessageFlags & MessageFlags_TRANSPORT_SPECIFIC)
		{
			std::string dat = MessageSerializationTest3_NewRandomString(rng, 32);
			h->TransportSpecific.resize(dat.size());
			for (size_t i = 0; i < dat.size(); i++)
			{
				h->TransportSpecific[i] = (uint8_t)(dat[i]);
			}
		}

		//Add a large entry sometimes
		uint32_t add_large_entry = small_dist(rng);

		for (size_t i = 0; i < h->EntryCount; i++)
		{
			if (m->ComputeSize3() > 10 * 1024 * 1024) break;

			RR_INTRUSIVE_PTR<MessageEntry> ee = CreateMessageEntry();

			ee->EntryFlags = MessageSerializationTest3_NewRandomFlags(rng);
			uniform_int_distribution<uint16_t> entry_type_dist(101, 120);
			ee->EntryType = (MessageEntryType)entry_type_dist(rng);

			bool read_streamid = true;
			if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
			{
				read_streamid = false;
				ee->ServicePath = MessageSerializationTest3_NewRandomString(rng, 256);
			}
			if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
			{
				read_streamid = false;
				ee->ServicePathCode = uint32_dist(rng);
			}
			if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
			{
				read_streamid = false;
				ee->MemberName = MessageSerializationTest3_NewRandomString(rng, 256);
			}
			if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
			{
				read_streamid = false;
				ee->MemberNameCode = uint32_dist(rng);
			}
			if (read_streamid)
			{
				ee->EntryStreamID = uint32_dist(rng);
			}
			if (ee->EntryFlags & MessageEntryFlags_REQUEST_ID)
			{
				ee->RequestID = uint32_dist(rng);
			}
			if (ee->EntryFlags & MessageEntryFlags_ERROR)
			{
				uniform_int_distribution<uint16_t> error_dist(1, 10);
				ee->Error = (MessageErrorType)error_dist(rng);
			}
			if (ee->EntryFlags & MessageEntryFlags_META_INFO)
			{				
				ee->MetaData = MessageSerializationTest3_NewRandomString(rng, 256);
			}
			if (ee->EntryFlags & MessageEntryFlags_TIMESPEC)
			{
				ee->EntryTimeSpec.seconds = int64_dist(rng);
				ee->EntryTimeSpec.nanoseconds = int32_dist(rng);
			}
			
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

				RR_INTRUSIVE_PTR<MessageElement> el = MessageSerializationTest3_NewRandomMessageElement(rng, 10);
				el->SetData(a);
				ee->elements.push_back(el);
			}
			else
			{
				uniform_int_distribution<size_t> el_count_dist(0, 16);
				size_t n1 = el_count_dist(rng);
				for (size_t j = 0; j < n1; j++)
				{
					RR_INTRUSIVE_PTR<MessageElement> el= MessageSerializationTest3_NewRandomMessageElement(rng, 0);
					ee->elements.push_back(el);
				}
			}

			m->entries.push_back(ee);
		}

		return m;
	}

	void MessageSerializationTest3::Test()
	{
		Test1();
		Test2(false);
		Test2(true);
		Test3();
	}

	void MessageSerializationTest3::Test1()
	{
		RR_INTRUSIVE_PTR<Message> m = MessageSerializationTest::NewTestMessage();

		//Write to stream and read back
		size_t message_size=m->ComputeSize3();
		boost::shared_array<uint8_t> buf(new uint8_t[message_size]);
		ArrayBinaryWriter w(buf.get(),0,message_size); 
		m->Write3(w, 0);

		if (w.Position() != m->ComputeSize3())
			throw std::runtime_error("");

		ArrayBinaryReader r(buf.get(),0,message_size);

		RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
		uint16_t version_minor;
		m2->Read3(r, version_minor);

		MessageSerializationTest::CompareMessage(m, m2);
	}

	void MessageSerializationTest3::Test2(bool use_string_table)
	{
		RR_INTRUSIVE_PTR<Message> m= MessageSerializationTest::NewTestMessage();

		RR_SHARED_PTR<RobotRaconteur::detail::StringTable> string_table1 = RR_MAKE_SHARED<RobotRaconteur::detail::StringTable>(false);
		if (use_string_table)
		{
			
			string_table1->MessageReplaceStringsWithCodes(m);
		}
		
		//Write to stream and read back
		size_t message_size = m->ComputeSize3();
		boost::shared_array<uint8_t> buf(new uint8_t[message_size]);
		ArrayBinaryWriter w(buf.get(), 0, message_size);
		m->Write3(w, 0);

		if (w.Position() != m->ComputeSize3())
			throw std::runtime_error("");

		ArrayBinaryReader r(buf.get(), 0, message_size);

		RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
		uint16_t version_minor;
		m2->Read3(r, version_minor);		

		if (use_string_table)
		{
			RR_SHARED_PTR<RobotRaconteur::detail::StringTable> string_table2 = RR_MAKE_SHARED<RobotRaconteur::detail::StringTable>(false);
			string_table2->MessageReplaceCodesWithStrings(m2);
			string_table1->MessageReplaceCodesWithStrings(m);
		}

		MessageSerializationTest::CompareMessage(m, m2);
	}

	void MessageSerializationTest3::Test3()
	{
		srand((uint32_t)time(NULL));
		
		RR_INTRUSIVE_PTR<MessageElement> e2 = CreateMessageElement();
		e2->ElementFlags &= MessageElementFlags_ELEMENT_NAME_STR;
		e2->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER | MessageElementFlags_SEQUENCE_NUMBER | MessageElementFlags_REQUEST_ACK;
		e2->ElementNumber = 134576569;
		e2->SequenceNumber = 9998934;

		RR_INTRUSIVE_PTR<MessageEntry> e1 = CreateMessageEntry(MessageEntryType_PropertyGetReq, "testprimitives");
		e1->EntryFlags = MessageEntryFlags_TIMESPEC;

		e1->EntryTimeSpec.seconds = 100;
		e1->EntryTimeSpec.nanoseconds = 12323;
		e1->EntryStreamID = 557475;

		RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
		h->MessageFlags = 0;

		m->header = h;
		h->FragmentHeader.FragmentMessageNumber = 1000;
		h->FragmentHeader.FragmentMessageSize = 1001;
		h->FragmentHeader.FragmentOffset = 42;

		h->SubstreamID = 34434;

		h->MessageFlags = MessageFlags_FRAGMENT | MessageFlags_SUBSTREAM_ID | MessageFlags_UNRELIABLE;
		
		m->entries.push_back(e1);

		//Write to stream and read back
		size_t message_size = m->ComputeSize3();
		boost::shared_array<uint8_t> buf(new uint8_t[message_size]);
		ArrayBinaryWriter w(buf.get(), 0, message_size);
		m->Write3(w, 0);

		if (w.Position() != m->ComputeSize3())
			throw std::runtime_error("");

		ArrayBinaryReader r(buf.get(), 0, message_size);

		RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
		uint16_t version_minor;
		m2->Read3(r, version_minor);

		MessageSerializationTest::CompareMessage(m, m2);
	}

	void MessageSerializationTest3::RandomTest(size_t iterations)
	{
		mt19937 rng;
		rng.seed((uint32_t)std::time(0));

		for (size_t i = 0; i < iterations; i++)
		{
			RR_INTRUSIVE_PTR<Message> m = NewRandomTestMessage3(rng);

			//Write to stream and read back
			size_t message_size = m->ComputeSize3();
			boost::shared_array<uint8_t> buf(new uint8_t[message_size]);
			ArrayBinaryWriter w(buf.get(), 0, message_size);
			m->Write3(w, 0);

			if (w.Position() != m->ComputeSize3())
				throw std::runtime_error("");

			ArrayBinaryReader r(buf.get(), 0, message_size);

			RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
			uint16_t version_minor;
			m2->Read3(r, version_minor);

			MessageSerializationTest::CompareMessage(m, m2);
		}


	}

}
