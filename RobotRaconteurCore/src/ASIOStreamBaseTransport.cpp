// Copyright 2011-2018 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "ASIOStreamBaseTransport.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/IOUtils.h"
#include "RobotRaconteur/StringTable.h"

#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

using namespace boost::asio;
using namespace RobotRaconteur;

using namespace std;
using namespace boost::asio::ip;
using namespace boost;


namespace RobotRaconteur
{
	namespace detail
	{
	RR_SHARED_PTR<RobotRaconteurNode> ASIOStreamBaseTransport::GetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
		if (!n) throw InvalidOperationException("Node has been released");
		return n;
	}



	ASIOStreamBaseTransport::ASIOStreamBaseTransport(RR_SHARED_PTR<RobotRaconteurNode> node) : _io_service(node->GetThreadPool()->get_io_service())
{
	connected.store(true);

	sendbuf_len=4*1024;
	sendbuf=boost::shared_array<uint8_t>(new uint8_t[sendbuf_len]);
	sending=false;

	recvbuf_len=4*1024;
	recvbuf_pos=0;
	recvbuf_end=0;
	recvbuf=boost::shared_array<uint8_t>(new uint8_t[recvbuf_len]);

	tlastrecv.store(node->NowUTC());
	tlastsend.store(node->NowUTC());

	this->ReceiveTimeout=15000;
	this->SendHeartbeat=true;
	this->HeartbeatPeriod=1000;

	streamop_waiting=false;
	streamop_closed = false;
	CheckStreamCapability_closed = false;
	CheckStreamCapability_waiting=false;
	this->node=node;
	recv_paused=false;
	send_paused=false;
	recv_pause_request=false;
	send_pause_request=false;

	receiving=false;
	max_message_size = 12 * 1024 * 1024;

	send_large_transfer_authorized = false;
	recv_large_transfer_authorized = false;

	send_version3.store(false);
	use_string_table3.store(false);

	
	//cout << "New stream" << endl;
	string_table_3_requestid = 0;

	server = false;
	string_table_3_pause_updates = false;
	string_table_3_closed = false;

	disable_message3 = false;
	disable_string_table = false;
	disable_async_io = false;

	async_reader = AsyncMessageReader::CreateInstance();

	async_recv_size = 0;
	async_recv_version = 0;
	async_recv_pos = 0;
	async_recv_continue_buf_count = 0;

	async_writer = AsyncMessageWriter::CreateInstance();
	async_send_version = 0;

	active_capabilities_message2_basic = 0;
	active_capabilities_message3_basic = 0;
	active_capabilities_message3_stringtable = 0;
}



void ASIOStreamBaseTransport::AsyncAttachStream(bool server, const NodeID& target_nodeid, const std::string& target_nodename, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	string_table3 = RR_MAKE_SHARED<detail::StringTable>(server);

	try
	{		
		heartbeat_timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service));
		{
			boost::mutex::scoped_lock lock(recv_lock);
			BeginReceiveMessage1();
		}

		//std::cout << "AsyncAttachStream" << std::endl;

		this->server = server;

		if (!server)
		{
			RR_SHARED_PTR<AsyncAttachStream_args> args=RR_MAKE_SHARED<AsyncAttachStream_args>(target_nodeid, target_nodename);

			boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> cb = boost::bind(&ASIOStreamBaseTransport::AsyncAttachStream1, RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1, _2, callback);
			AsyncStreamOp("CreateConnection", args, cb);

		}
		else
		{
			try
			{				
				{
					boost::mutex::scoped_lock lock(heartbeat_timer_lock);
					if (heartbeat_timer)
					{
						heartbeat_timer->expires_from_now(boost::posix_time::milliseconds(400));
						heartbeat_timer->async_wait(boost::bind(&ASIOStreamBaseTransport::heartbeat_timer_func, RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),boost::asio::placeholders::error));  
					}

				}
				detail::PostHandler(node, callback, true);				
			}
			catch (std::exception& e)
			{				
				detail::PostHandlerWithException(node, callback, e, MessageErrorType_ConnectionError, true);
			}
		}

		
	}
	catch(std::exception& e)
	{
		detail::InvokeHandlerWithException(node, callback, e, MessageErrorType_ConnectionError);
	}
}


void ASIOStreamBaseTransport::AsyncAttachStream1(RR_SHARED_PTR<RRObject> parameter, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	//std::cout << "AsyncAttachStream1" << std::endl;
	if (err)
	{
		
		RobotRaconteurNode::TryPostToThreadPool(node,boost::bind(&ASIOStreamBaseTransport::Close,shared_from_this()));
		
		detail::PostHandlerWithException(node, (callback), err, true);		
		return;
	}
	try
	{
		if (!parameter)
		{
			detail::PostHandlerWithException(node, (callback), RR_MAKE_SHARED<ConnectionException>("IO error"), true);
			return;
		}

		{	
			boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
			NodeID RemoteNodeID1=*rr_cast<NodeID>(parameter);
			if (RemoteNodeID.IsAnyNode())
			{
				this->RemoteNodeID = RemoteNodeID1;
			}
			else
			{
				if (RemoteNodeID != RemoteNodeID1)
				{
					detail::PostHandlerWithException(node, (callback), RR_MAKE_SHARED<ConnectionException>("Invalid server NodeID"), true);
					return;
				}
			}

		}

		{
			boost::mutex::scoped_lock lock(heartbeat_timer_lock);
			if (heartbeat_timer)
			{
				heartbeat_timer->expires_from_now(boost::posix_time::milliseconds(400));
				heartbeat_timer->async_wait(boost::bind(&ASIOStreamBaseTransport::heartbeat_timer_func, RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),boost::asio::placeholders::error));  
			}

		}

		detail::PostHandler(node, callback, true);
	}
	catch (std::exception& e)
	{
		detail::InvokeHandlerWithException(node, callback, e, MessageErrorType_ConnectionError);
	}

}



void ASIOStreamBaseTransport::AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{

	if (!connected.load())
	{
		throw ConnectionException("Transport not connected");
	}

	// Clear flags that should not be sent from the node layer
	if (m->header->MessageFlags & MessageFlags_TRANSPORT_SPECIFIC) m->header->TransportSpecific.clear();
	m->header->MessageFlags &= ~(MessageFlags_SUBSTREAM_SEQUENCE_NUMBER | MessageFlags_TRANSPORT_SPECIFIC);
	// End clear flags

	bool send_3 = send_version3.load();
	
	//TODO: find more elegant solution for this
	if (m->entries.size() == 1)
	{
		uint16_t t = m->entries[0]->EntryType;
		if (t == MessageEntryType_StreamOp || t == MessageEntryType_StreamOpRet)
		{
			if (m->entries[0]->MemberName == "CreateConnection")
			{
				send_3 = false;
			}
		}
	}

	boost::mutex::scoped_lock lock(send_lock);
	
	size_t message_size; 
	if (!send_3)
	{
		message_size = m->ComputeSize();
	}
	else
	{
		message_size = m->ComputeSize3();
	}
		
	if ((int32_t)message_size > (max_message_size-100)) throw ProtocolException("Message larger than maximum message size");

	if (!send_large_transfer_authorized)
	{
		if (IsLargeTransferAuthorized())
		{
			send_large_transfer_authorized = true;
		}
		else
		{
			if (message_size > 16 * 1024) throw ProtocolException("");
		}
	}

	if (sending || send_paused)
	{

		message_queue_entry e;
		e.message=m;
		e.callback=callback;

		//Look for older wire and check connection packets and delete
		//if in queue to prevent packet flooding.

		bool replaced=false;

		for (std::list<message_queue_entry >::iterator ee=send_queue.begin(); ee!=send_queue.end();)
		{
			bool remove=false;

			RR_INTRUSIVE_PTR<MessageHeader> h1=m->header;
			RR_INTRUSIVE_PTR<MessageHeader> h2=ee->message->header;

			if (h1->ReceiverNodeName==h2->ReceiverNodeName 
				&& h1->SenderNodeName==h2->SenderNodeName
				&& h1->ReceiverNodeID==h2->ReceiverNodeID
				&& h1->SenderNodeID==h2->SenderNodeID
				&& h1->SenderEndpoint==h2->SenderEndpoint
				&& h1->ReceiverEndpoint==h2->ReceiverEndpoint)
			{
				if (ee->message->entries.size() == m->entries.size() && ee->message->entries.size() == 1)
				{
					if (ee->message->entries.at(0)->EntryType==MessageEntryType_ConnectionTest && m->entries.at(0)->EntryType==MessageEntryType_ConnectionTest) return;

					if (ee->message->entries.at(0)->EntryType==MessageEntryType_WirePacket
						&& m->entries.at(0)->EntryType==MessageEntryType_WirePacket)
					{
						if (ee->message->entries.at(0)->ServicePath==m->entries.at(0)->ServicePath
							&& ee->message->entries.at(0)->MemberName==m->entries.at(0)->MemberName)
						{
							remove=true;
						}
					}

				}
			}

			if (remove)
			{
				detail::PostHandler(node, ee->callback, true, false);
				
				if (replaced)
				{
					ee=send_queue.erase(ee);
				}
				else
				{
					*ee=e;
					replaced=true;
					++ee;
				}
				
			}
			else
			{
				++ee;
			}
			
		}

		if (!replaced)
		{
			send_queue.push_back(e);
		}
	}
	else
	{
		BeginSendMessage(m,callback);
	}
}

void ASIOStreamBaseTransport::SimpleAsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	if (!connected.load()) return;
	try
	{
		AsyncSendMessage(m, callback);
	}
	catch (std::exception&)
	{
		Close();
	}

}

void ASIOStreamBaseTransport::SimpleAsyncEndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err)
{
	if (err)
	{
		Close();
	}
}



void ASIOStreamBaseTransport::BeginSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	size_t message_size = 0;
	bool send_3=send_version3.load();
	bool string_table3 = use_string_table3.load();
	
	//Don't use version 3 for special requests

	//TODO: find more elegant solution for this
	if (m->entries.size() == 1)
	{
		uint16_t t = m->entries[0]->EntryType;
		if (t == MessageEntryType_StreamOp || t == MessageEntryType_StreamOpRet)
		{
			if (m->entries[0]->MemberName == "CreateConnection")
			{
				send_3 = false;
			}
		}
	}

	if (!send_3)
	{
		message_size = m->ComputeSize();

		if (!disable_async_io)
		{
			async_send_version = 2;
			sending = true;
			send_message_size = message_size;
			BeginSendMessage1(m, callback);
			return;
		}

		if (message_size > sendbuf_len)
		{

			sendbuf = shared_array<uint8_t>(new uint8_t[message_size]);
			sendbuf_len = message_size;
		}		

		ArrayBinaryWriter w(sendbuf.get(), 0, message_size);
		m->Write(w);
	}
	else
	{
		if (!GetRemoteNodeID().IsAnyNode() && GetRemoteEndpoint() != 0)
		{
			if (m->header->SenderNodeID == GetNode()->NodeID()
				&& m->header->ReceiverNodeID == GetRemoteNodeID()
				&& m->header->SenderEndpoint == GetLocalEndpoint()
				&& m->header->ReceiverEndpoint == GetRemoteEndpoint())
			{
				if (!(m->entries.size() == 1 && m->entries[0]->EntryType < 500))
				{
					m->header->MessageFlags &= ~(MessageFlags_ROUTING_INFO | MessageFlags_ENDPOINT_INFO | MessageFlags_MESSAGE_ID);
				}
			}
		}		

		message_size = m->ComputeSize3();

		if (string_table3)
		{
			this->string_table3->MessageReplaceStringsWithCodes(m);
			message_size = m->ComputeSize3();
			UpdateStringTable();
		}

		if (!disable_async_io)
		{
			sending = true;
			send_message_size = message_size;
			async_send_version = 3;
			BeginSendMessage1(m, callback);
			return;
		}

		if (message_size > sendbuf_len)
		{

			sendbuf = shared_array<uint8_t>(new uint8_t[message_size]);
			sendbuf_len = message_size;
		}		

		ArrayBinaryWriter w(sendbuf.get(), 0, message_size);
		m->Write3(w, 0);

	}


	boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)> f=boost::bind(&ASIOStreamBaseTransport::EndSendMessage,
			shared_from_this(),
			0,
			_1,
			_2, m, message_size, callback, sendbuf);

	const_buffers buf;
	buf.push_back(const_buffer(sendbuf.get(), message_size));

	this->async_write_some(buf, f);
	

	sending=true;
	send_message_size=message_size;
}

void ASIOStreamBaseTransport::BeginSendMessage1(RR_INTRUSIVE_PTR<Message> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	
	async_writer->Reset();
	async_writer->BeginWrite(m, async_send_version);
	mutable_buffers work_bufs;	

	work_bufs.push_back(boost::asio::buffer(sendbuf.get(), sendbuf_len));
	size_t work_bufs_used;

	async_send_bufs.clear();
	switch (async_send_version)
	{
	case 2:
		async_writer->Write(send_message_size, work_bufs, work_bufs_used, async_send_bufs);
		break;
	case 3:
		async_writer->Write3(send_message_size, work_bufs, work_bufs_used, async_send_bufs);
		break;
	default:
		throw InvalidOperationException("Invalid message version");
	}
	
	boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndSendMessage2,
		shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred, callback);
	async_write_some(async_send_bufs, h);

}

void ASIOStreamBaseTransport::EndSendMessage2(const boost::system::error_code& error, size_t bytes_transferred, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	try
	{
		if (error)
		{
			Close();
			sending = false;
			detail::InvokeHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>(error.message()));
			return;
		}

		buffers_consume(async_send_bufs, bytes_transferred);

		if (boost::asio::buffer_size(async_send_bufs) > 0)
		{
			boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndSendMessage2,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, callback);

			async_write_some(async_send_bufs, h);
			return;
		}

		async_send_bufs.clear();

		size_t remaining = async_writer->WriteRemaining();
		if (remaining > 0)
		{
			mutable_buffers work_bufs;

			work_bufs.push_back(boost::asio::buffer(sendbuf.get(), sendbuf_len));
			size_t work_bufs_used;

			async_send_bufs.clear();
			switch (async_send_version)
			{
			case 2:
				async_writer->Write(remaining, work_bufs, work_bufs_used, async_send_bufs);
				break;
			case 3:
				async_writer->Write3(remaining, work_bufs, work_bufs_used, async_send_bufs);
				break;
			default:
				throw InvalidOperationException("Invalid message version");
			}


			boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndSendMessage2,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, callback);

			async_write_some(async_send_bufs, h);

			return;
		}

	}
	
	catch (std::exception& exp)
	{
		Close();
		detail::InvokeHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>("Send message error: " + std::string(exp.what())));
	}

	try
	{
		EndSendMessage1();
	}
	catch (std::exception&)
	{
		Close();
	}

	detail::InvokeHandler(node, callback);
}

void ASIOStreamBaseTransport::EndSendMessage(size_t startpos, const boost::system::error_code& error, size_t bytes_transferred, RR_INTRUSIVE_PTR<Message> m, size_t m_len, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback, boost::shared_array<uint8_t> buf)
{
	
	try
	{
		if (error)
		{
			Close();
			sending = false;
			detail::InvokeHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>(error.message()));
			return;

		}

		if (bytes_transferred < (m_len - startpos))
		{
			size_t new_startpos = startpos + bytes_transferred;

			boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> f = boost::bind(&ASIOStreamBaseTransport::EndSendMessage,
				shared_from_this(),
				new_startpos,
				_1,
				_2, m, m_len, callback, sendbuf);

			const_buffers buf1;
			buf1.push_back(const_buffer(sendbuf.get() + new_startpos, m_len - new_startpos));

			this->async_write_some(buf1, f);
			return;
		}

		EndSendMessage1();
	}

	catch (std::exception&)
	{
		Close();		
	}

	detail::InvokeHandler(node, callback);
	
}

void ASIOStreamBaseTransport::EndSendMessage1()
{

	boost::mutex::scoped_lock lock(send_lock);


	tlastsend.store(GetNode()->NowUTC());

	bool c = connected.load();

	if (send_queue.size() != 0 && c && !send_pause_request)
	{
		message_queue_entry m = send_queue.front();
		send_queue.pop_front();
		try
		{
			BeginSendMessage(m.message, m.callback);
		}
		catch (std::exception& e)
		{			
			detail::InvokeHandlerWithException(node, m.callback,RR_MAKE_SHARED<ConnectionException>(e.what()));			
		}
	}
	else
	{
		sending = false;
		send_event.notify_all();
		if (send_pause_request && !send_paused)
		{
			send_paused = true;
			boost::system::error_code ec;
			boost::function<void(const boost::system::error_code&)> f = send_pause_request_handler;
			send_pause_request_handler.clear();
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f, ec), true);

		}
	}	
}

//void ASIOStreamBaseTransport::
void ASIOStreamBaseTransport::AsyncPauseSend(boost::function<void (const boost::system::error_code&)>& handler)
{
	boost::mutex::scoped_lock lock(send_lock);

	if (send_pause_request || send_paused) throw InvalidOperationException("Already pausing");

	if (!sending)
	{
		send_pause_request=true;
		send_paused=true;
		
		boost::system::error_code ec;
		RobotRaconteurNode::TryPostToThreadPool(node,boost::bind(handler,ec));

	}
	else
	{
		send_pause_request=true;
		send_pause_request_handler=handler;
	}

}


void ASIOStreamBaseTransport::AsyncResumeSend()
{
	boost::mutex::scoped_lock lock(send_lock);
	if (!send_pause_request) return;
	if (send_pause_request && !send_paused)
	{
		throw InvalidOperationException("Invalid operation");
	}

	send_pause_request=false;
	send_paused=false;

	bool c=connected.load();

	if (send_queue.size()!=0 && c && !send_pause_request && !sending)
	{
		message_queue_entry m=send_queue.front();
		send_queue.pop_front();
		try
		{
			BeginSendMessage(m.message,m.callback);
		}
		catch (std::exception& e)
		{			
			detail::InvokeHandlerWithException(node, m.callback, RR_MAKE_SHARED<ConnectionException>(e.what()));			
		}
	}

}

void ASIOStreamBaseTransport::BeginReceiveMessage1()
{
	
		receiving=true;
		if (disable_async_io)
		{
			mutable_buffers buf;
			buf.push_back(boost::asio::buffer(streamseed, 8));
			boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage1,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
				0,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred);
			async_read_some(buf, h);
		}
		else
		{
			active_recv_bufs.clear();
			active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get(), 16));

			recvbuf_pos = 0;
			recvbuf_end = 0;
			async_recv_size = 0;
			async_recv_pos = 0;
			async_recv_version = 2;
			async_recv_continue_buf_count = 0;

			boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred);

			async_read_some(active_recv_bufs, h);

		}
			
}



void ASIOStreamBaseTransport::EndReceiveMessage1(size_t startpos, const boost::system::error_code& error,
      size_t bytes_transferred)
{
	try
	{
		if (!error)
		{
			if (bytes_transferred < (8-startpos))
			{
				size_t new_startpos=startpos+bytes_transferred;

				mutable_buffers buf1;
				buf1.push_back(boost::asio::buffer(streamseed + new_startpos, 8 - new_startpos));
				boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage1,
					RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
					new_startpos,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred);
				async_read_some(buf1, h);
				return;

			}


			char seed[4];
			uint32_t size;

			memcpy(seed,streamseed,4);
			memcpy(&size,streamseed+4,4);

#ifdef BOOST_BIG_ENDIAN
			std::reverse((uint8_t*)&size,((uint8_t*)&size)+4);
#endif

			if (std::string(seed,4) != "RRAC") throw ProtocolException("");

			if (size<8) throw ProtocolException("");

			if ((int32_t)size > (max_message_size)) throw ProtocolException("");

			if (!recv_large_transfer_authorized)
			{
				if (IsLargeTransferAuthorized())
				{
					recv_large_transfer_authorized = true;
				}
				else
				{
					if (size > 16 * 1024) throw ProtocolException("");
				}

			}

			/*
			if (size > recvbuf_len)
			{
				delete recvbuf;
				recvbuf=new uint8_t[size];
				recvbuf_len=size;
			}

			memcpy(recvbuf,streamseed,8);

			recv_message_size=size;

			async_read(*stream,buffer(recvbuf+8, recv_message_size-8),
				boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage3, 
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));*/

			if (recvbuf_len < size)
			{
				
				recvbuf_len=size*12/10;
				recvbuf=boost::shared_array<uint8_t>(new uint8_t[recvbuf_len]);
			}
			

			memcpy(recvbuf.get(),streamseed,8);

			recv_message_size=size;

			//std::cout << this->available() << std::endl;

			mutable_buffers buf2;
			buf2.push_back(buffer(recvbuf.get() + 8, size - 8));
			
			boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage2,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
				8,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, size, recvbuf);

			async_read_some(buf2, h);
			


		}
		else
		{
			Close();
		}
	}
	catch (std::exception&)
	{
		Close();
	}
}


void ASIOStreamBaseTransport::EndReceiveMessage2(size_t startpos, const boost::system::error_code& error,
    size_t bytes_transferred,  size_t message_size, boost::shared_array<uint8_t> buf)
{

	try
	{
		if (!error)
		{
			if (bytes_transferred<(message_size-startpos))
			{
				size_t new_startpos=startpos+bytes_transferred;

				mutable_buffers buf2;
				buf2.push_back(buffer(recvbuf.get() + new_startpos, message_size - new_startpos));
			
				boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage2,
					RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
					new_startpos,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred, message_size, recvbuf);

				async_read_some(buf2, h);
				return;

			}

			ArrayBinaryReader r(recvbuf.get(),0,message_size);
			r.ReadString8(4);
			r.ReadNumber<uint32_t>();
			uint16_t message_version = r.ReadNumber<uint16_t>();
			r.Seek(0);


			RR_INTRUSIVE_PTR<Message> message=CreateMessage();

			if (message_version == 3)
			{
				uint16_t message_version_minor;
				message->Read3(r, message_version_minor);

				bool string_table3 = use_string_table3.load();

				if (string_table3)
				{
					this->string_table3->MessageReplaceCodesWithStrings(message);
					UpdateStringTable();
				}

				uint16_t flags = message->header->MessageFlags;

				if (!(flags & MessageFlags_ROUTING_INFO))
				{
					message->header->SenderNodeID = GetRemoteNodeID();
					message->header->ReceiverNodeID = GetNode()->NodeID();					
				}

				if (!(flags & MessageFlags_ENDPOINT_INFO))
				{
					message->header->SenderEndpoint = GetRemoteEndpoint();
					message->header->ReceiverEndpoint = GetLocalEndpoint();
				}

			}
			else
			{
				message->Read(r);
			}
						
			EndReceiveMessage3(message);
		}
		else
		{
			Close();
		}
	}
	catch (std::exception&) {
	
		Close();
	}

}

void ASIOStreamBaseTransport::EndReceiveMessage3(RR_INTRUSIVE_PTR<Message> message)
{
	
	try
	{

		if (message->entries.size() > 0)
		{
			if (message->entries.at(0)->EntryType == MessageEntryType_ConnectionTest)
			{

				EndReceiveMessage4();	
				RR_INTRUSIVE_PTR<Message> &m=message;
						

				RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
				ret->header = CreateMessageHeader();
				ret->header->ReceiverNodeName = m->header->SenderNodeName;
				ret->header->SenderNodeName = GetNode()->NodeName();
				ret->header->ReceiverNodeID = m->header->SenderNodeID;
				ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
				ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
				ret->header->SenderNodeID = GetNode()->NodeID();

				RR_INTRUSIVE_PTR<MessageEntry> eret = ret->AddEntry(MessageEntryType_ConnectionTestRet,m->entries.at(0)->MemberName);
				eret->RequestID = m->entries.at(0)->RequestID;
				eret->ServicePath = m->entries.at(0)->ServicePath;

				bool send_3 = send_version3.load();

				if (send_3)
				{
					ret->header->MessageFlags &= ~(MessageFlags_ROUTING_INFO | MessageFlags_ENDPOINT_INFO | MessageFlags_MESSAGE_ID);
				}						

				RR_SHARED_PTR<ASIOStreamBaseTransport > p=RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());

				
				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind((&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage),
					RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1
				);
				AsyncSendMessage(ret, h);

							
			}
			else if (message->entries.at(0)->EntryType== MessageEntryType_ConnectionTestRet)
			{
				EndReceiveMessage4();
			}
			else if (message->entries.at(0)->EntryType ==MessageEntryType_StreamOp || message->entries.at(0)->EntryType ==MessageEntryType_StreamOpRet)
			{						
				StreamOpMessageReceived(message);
				EndReceiveMessage4();
			}
			else if (message->entries.at(0)->EntryType ==MessageEntryType_StreamCheckCapability ||message->entries.at(0)->EntryType ==MessageEntryType_StreamCheckCapabilityRet)
			{						
				CheckStreamCapability_MessageReceived(message);
				EndReceiveMessage4();
			}
			else if (message->entries.at(0)->EntryType == MessageEntryType_StringTableOp || message->entries.at(0)->EntryType == MessageEntryType_StringTableOpRet)
			{
				UpdateStringTable2(message);
				EndReceiveMessage4();
			}
			else
			{
				BeginReceiveMessage1();
				try
				{
				MessageReceived(message);
				}
				catch (...) {
					Close();
				}
			}

			tlastrecv.store(GetNode()->NowUTC());

		}
	}
	catch (std::exception&) {}
		
		

				

		
	
}

void ASIOStreamBaseTransport::EndReceiveMessage4()
{
	{
		boost::mutex::scoped_lock recv_lock; 

		

		if (recv_pause_request)
		{

			receiving=false;
			recv_paused=true;
			boost::system::error_code ec;

			boost::function<void (const boost::system::error_code&)> f=recv_pause_request_handler;
			recv_pause_request_handler.clear();
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f,ec), true);
			return;
		}

		if (!recv_pause_request && recv_paused)
		{
			recv_paused=false;
		}
		
		BeginReceiveMessage1();

	}
}

void ASIOStreamBaseTransport::EndReceiveMessage5(const boost::system::error_code& error,
	size_t bytes_transferred)
{
	try
	{
		if (error)
		{
			Close();
			return;
		}

		if (bytes_transferred == 0)
		{
			Close();
			return;
		}
		
		{
			//boost::mutex::scoped_lock lock(recv_lock);

			if (async_recv_size == 0)
			{
				if (active_recv_bufs.size() != 1) throw ProtocolException("");

				recvbuf_end += bytes_transferred;

				if ((recvbuf_end - recvbuf_pos) < 16)
				{


					buffers_consume(active_recv_bufs, bytes_transferred);
					boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
						RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred);
					async_read_some(active_recv_bufs, h);
					return;
				}

				char magic[4];
				uint32_t size;
				uint16_t message_version;

				uint8_t* recvbuf1 = recvbuf.get() + recvbuf_pos;

				memcpy(magic, recvbuf1, 4);
				memcpy(&size, recvbuf1 + 4, 4);
				memcpy(&message_version, recvbuf1 + 8, 2);

#ifdef BOOST_BIG_ENDIAN
				std::reverse((uint8_t*)&size, ((uint8_t*)&size) + 4);
				std::reverse((uint8_t*)&message_version, ((uint8_t*)&message_version) + 2);
#endif

				if (std::string(magic, 4) != "RRAC") throw ProtocolException("");
				if (size < 16) throw ProtocolException("");

				async_recv_size = size;
				async_recv_version = message_version;

				async_reader->Reset();
			}
			else if (async_recv_continue_buf_count == 0)
			{
				recvbuf_end += bytes_transferred;
			}

			size_t bufs_read = 0;
			mutable_buffers continue_bufs;

			AsyncMessageReader::return_type ret;

			if (async_recv_continue_buf_count == 0)
			{
				const_buffers bufs;
				bufs.push_back(boost::asio::buffer(recvbuf.get(), recvbuf_end) + recvbuf_pos);

				switch (async_recv_version)
				{
				case 2:
					ret = async_reader->Read(bufs, bufs_read, 0, continue_bufs);
					break;
				case 3:
					ret = async_reader->Read3(bufs, bufs_read, 0, continue_bufs);
					break;
				default:
					throw InvalidOperationException("");
				}

				if (bufs_read > recvbuf_end - recvbuf_pos) throw ProtocolException("");
				recvbuf_pos += bufs_read;
				async_recv_pos += bufs_read;
				if (recvbuf_pos == recvbuf_end)
				{
					recvbuf_pos = 0;
					recvbuf_end = 0;
				}


				if (ret == AsyncMessageReader::ReadReturn_continue_buffers && recvbuf_end != 0)
				{
					throw ProtocolException("");
				}
			}
			else
			{
				size_t continue_buffer_bytes = 0;
				mutable_buffers::iterator e = active_recv_bufs.begin();
				{
					for (size_t i=0; i<async_recv_continue_buf_count; i++)
					{
						if (e == active_recv_bufs.end()) throw ProtocolException("");
						continue_buffer_bytes += boost::asio::buffer_size(*e);
						e++;
					}
				}

				size_t b1 = bytes_transferred;

				if (bytes_transferred > continue_buffer_bytes)
				{
					b1 = continue_buffer_bytes;
					recvbuf_end += bytes_transferred - continue_buffer_bytes;
					if (recvbuf_end > recvbuf_len) throw ProtocolException("");
				}

				const_buffers bufs;

				switch (async_recv_version)
				{
				case 2:
					ret = async_reader->Read(bufs, bufs_read, b1, continue_bufs);
					break;
				case 3:
					ret = async_reader->Read3(bufs, bufs_read, b1, continue_bufs);
					break;
				default:
					throw ProtocolException("");
				}

				if (bufs_read != 0) throw ProtocolException("");

				async_recv_pos += b1;

				if (ret != AsyncMessageReader::ReadReturn_done && recvbuf_pos != recvbuf_end)
				{
					//Read data in extra buffer if not finished
					bufs.push_back(boost::asio::buffer(recvbuf.get(), recvbuf_end) + recvbuf_pos);

					switch (async_recv_version)
					{
					case 2:
						ret = async_reader->Read(bufs, bufs_read, 0, continue_bufs);
						break;
					case 3:
						ret = async_reader->Read3(bufs, bufs_read, 0, continue_bufs);
						break;
					default:
						throw InvalidOperationException("");
					}

					if (bufs_read > recvbuf_end - recvbuf_pos) throw ProtocolException("");
					recvbuf_pos += bufs_read;
					async_recv_pos += bufs_read;
					if (recvbuf_pos == recvbuf_end)
					{
						recvbuf_pos = 0;
						recvbuf_end = 0;
					}
				}
			}

			//Sanity checks
			if (recvbuf_pos > recvbuf_len) throw ProtocolException("");
			if (async_recv_pos > async_recv_size) throw ProtocolException("");

			if (ret == AsyncMessageReader::ReadReturn_done)
			{
				if (async_recv_pos != async_recv_size)
				{
					throw ProtocolException("Message did not read all data");
				}

				async_recv_size = 0;
				async_recv_pos = 0;
				async_recv_continue_buf_count = 0;
				active_recv_bufs.clear();

				if (async_reader->MessageReady())
				{
					RR_INTRUSIVE_PTR<Message> m = async_reader->GetNextMessage();
					bool string_table3 = use_string_table3.load();

					if (string_table3)
					{
						this->string_table3->MessageReplaceCodesWithStrings(m);
						UpdateStringTable();
					}

					uint16_t flags = m->header->MessageFlags;

					if (!(flags & MessageFlags_ROUTING_INFO))
					{
						m->header->SenderNodeID = GetRemoteNodeID();
						m->header->ReceiverNodeID = GetNode()->NodeID();
					}

					if (!(flags & MessageFlags_ENDPOINT_INFO))
					{
						m->header->SenderEndpoint = GetRemoteEndpoint();
						m->header->ReceiverEndpoint = GetLocalEndpoint();
					}

					if (recvbuf_pos != recvbuf_end)
					{
						throw ProtocolException("Still data in buffer");
					}

					//lock.unlock();

					EndReceiveMessage3(m);

					return;					
				}
			}

			if (recvbuf_pos != recvbuf_end)
			{
				//Still some data in the buffer, process it
				size_t l1 = recvbuf_end - recvbuf_pos;
				size_t l2 = recvbuf_len - recvbuf_pos;
				if (l2 < 128)
				{
					std::memmove(recvbuf.get(), recvbuf.get() + recvbuf_pos, l1);
					recvbuf_pos = 0;
				}

				recvbuf_end = recvbuf_pos;				

				size_t l3 = std::min(l2, async_recv_size - async_recv_pos);
				l3 = std::min(l3, (size_t)1024);

				active_recv_bufs.clear();
				active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get() + async_recv_pos, l3));

				//TODO: This post may not always be necessary
				boost::system::error_code ec1;
				RobotRaconteurNode::TryPostToThreadPool(node,boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
					RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
					ec1, l1));
			}
			else
			{
				if (ret != AsyncMessageReader::ReadReturn_continue_buffers)
				{
					recvbuf_pos = 0;
					recvbuf_end = 0;
					async_recv_continue_buf_count = 0;

					size_t l1 = std::min(recvbuf_len, async_recv_size - async_recv_pos);
					l1 = std::min(l1, (size_t)1024);

					active_recv_bufs.clear();
					active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get(), l1));
					boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
						RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred);
					async_read_some(active_recv_bufs, h);
				}
				else
				{
					recvbuf_pos = 0;
					recvbuf_end = 0;
					async_recv_continue_buf_count = continue_bufs.size();
					active_recv_bufs.clear();
					size_t l3 = async_recv_size - async_recv_pos;
					size_t l2 = boost::asio::buffer_size(continue_bufs);
					if (l2 > l3) throw ProtocolException("");
					size_t l1 = std::min (recvbuf_len, l3 - l2);
					l1 = std::min(l1, (size_t)1024);
					
					boost::range::copy(continue_bufs, std::back_inserter(active_recv_bufs));
					if (l1 > 0)
					{
						active_recv_bufs.push_back(boost::asio::buffer(recvbuf.get(), l1));
					}
					boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)> h = boost::bind(&ASIOStreamBaseTransport::EndReceiveMessage5,
						RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred);
					async_read_some(active_recv_bufs, h);
				}
			}
		}
	}
	catch (std::exception&)
	{
		Close();
	}
}



void ASIOStreamBaseTransport::AsyncPauseReceive(boost::function<void (const boost::system::error_code&)>& handler)
{
	boost::mutex::scoped_lock lock(recv_lock);
	if (recv_pause_request || recv_paused) throw InvalidOperationException("Already pausing");
	
	
	if (!receiving)
	{
		recv_pause_request=true;
		recv_paused=true;

		boost::system::error_code ec;
		RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler,ec));

	}
	else
	{
		recv_pause_request=true;
		recv_pause_request_handler=handler;
	}

}

void ASIOStreamBaseTransport::AsyncResumeReceive()
{
	boost::mutex::scoped_lock lock(recv_lock);
	if (!recv_pause_request) return;
	if (recv_pause_request && !recv_paused)
	{
		throw InvalidOperationException("Invalid operation");
	}

	recv_pause_request=false;
	recv_paused=false;

	bool c=connected.load();

	if (!c) return;

	if (receiving) return;

	BeginReceiveMessage1();

}

void ASIOStreamBaseTransport::Close()
{
	{
		bool c=connected.exchange(false);
		if (!c) return;
	}

	//std::cout << "ASIO Close" << std::endl;

	
	if (heartbeat_timer)
	{
		boost::mutex::scoped_lock lock(heartbeat_timer_lock);
		heartbeat_timer->cancel();
		heartbeat_timer.reset();
	}

	try
	{
		//stream->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		//stream->close();

		boost::mutex::scoped_lock lock2 (send_lock);

		if (send_pause_request_handler)
		{
			boost::system::error_code ec(boost::system::errc::broken_pipe,boost::system::generic_category());
			boost::function<void (const boost::system::error_code&)> f=send_pause_request_handler;
			send_pause_request_handler.clear();
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f,ec));
		}
		
		for (std::list<message_queue_entry >::iterator e=send_queue.begin(); e!=send_queue.end();)
		{
			boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> f=e->callback;
			e=send_queue.erase(e);
			detail::PostHandlerWithException(node, f, RR_MAKE_SHARED<ConnectionException>("Transport Closed"), true, false);
		}

		send_queue.clear();
	}
	catch (std::exception&)
	{
	}

	
	try
	{
		boost::mutex::scoped_lock lock(recv_lock);
		if (recv_pause_request_handler)
		{
			boost::system::error_code ec(boost::system::errc::broken_pipe,boost::system::generic_category());
			boost::function<void (const boost::system::error_code&)> f=recv_pause_request_handler;
			recv_pause_request_handler.clear();
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(f,ec));			

		}

	}
	catch (std::exception&)
	{

	}

		

		boost::mutex::scoped_lock lock (streamop_lock);
		boost::mutex::scoped_lock lock2(CheckStreamCapability_lock);

		CheckStreamCapability_closed = true;
		streamop_closed = true;

		if (CheckStreamCapability_timer) CheckStreamCapability_timer->cancel();
		if (streamop_timer) streamop_timer->cancel();

		streamop_timer.reset();
		CheckStreamCapability_timer.reset();

		if (streamop_waiting && streamop_callback) 
			detail::PostHandlerWithException(node, streamop_callback,RR_MAKE_SHARED<ConnectionException>("Connection closed"),false,false);
		if (CheckStreamCapability_waiting && CheckStreamCapability_callback)
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(CheckStreamCapability_callback,0,RR_MAKE_SHARED<ConnectionException>("Connection closed")));
		
		streamop_waiting=false;
		CheckStreamCapability_waiting=false;

		while (!CheckStreamCapability_queue.empty())
		{
			boost::tuple<std::string,boost::function<void (uint32_t, RR_SHARED_PTR<RobotRaconteurException>)> > d=CheckStreamCapability_queue.front();
			CheckStreamCapability_queue.pop();			
			detail::PostHandlerWithException(node, d.get<1>(), RR_MAKE_SHARED<ConnectionException>("Connection closed"), false, false);
		}

		while (!streamop_queue.empty())
		{
			boost::tuple<std::string,RR_SHARED_PTR<RRObject>,boost::function<void (RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > d=streamop_queue.front();
			streamop_queue.pop();
			detail::PostHandlerWithException(node, d.get<2>(),RR_MAKE_SHARED<ConnectionException>("Connection closed"), false,false);
		}
		//boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		streamop_callback.clear();
		CheckStreamCapability_callback.clear();

		{
			boost::mutex::scoped_lock lock(string_table3_lock);
			string_table_3_closed = true;
			BOOST_FOREACH(RR_SHARED_PTR<boost::asio::deadline_timer>& e, string_table_3_timers)
			{
				boost::system::error_code ec;
				e->cancel(ec);
			}
			string_table_3_timers.clear();
		}

	
}

void ASIOStreamBaseTransport::heartbeat_timer_func(const boost::system::error_code& e)
{

	if (!connected.load()) return;

	if (e != boost::asio::error::operation_aborted)
	{
		uint32_t heartbeat_period2 = HeartbeatPeriod;

		if (server)
		{
			heartbeat_period2 += (HeartbeatPeriod * 2) / 10;
		}

		try
		{
		boost::posix_time::ptime t=tlastsend.load();

		if ((GetNode()->NowUTC() - t).total_milliseconds() > heartbeat_period2 && SendHeartbeat)
		{
			RR_INTRUSIVE_PTR<Message> m = CreateMessage();
			m->header = CreateMessageHeader();
			m->header->SenderNodeID = GetNode()->NodeID();
			RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_ConnectionTest, "");
			m->entries.push_back(mm);

			
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
				_1);
			AsyncSendMessage(m, h);
		}

		boost::posix_time::ptime tr=tlastrecv.load();

		if ((t - tr).total_milliseconds()> ReceiveTimeout)
		{
			Close();
		}
		else
		{
			boost::mutex::scoped_lock lock(heartbeat_timer_lock);
			if (!heartbeat_timer) return;

			boost::posix_time::time_duration t = boost::posix_time::milliseconds(heartbeat_period2+10) - (GetNode()->NowUTC() - tlastsend);

			heartbeat_timer->expires_from_now(t);
			heartbeat_timer->async_wait(boost::bind(&ASIOStreamBaseTransport::heartbeat_timer_func, RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),boost::asio::placeholders::error));  

		}
		}
		catch (std::exception&)
		{
			Close();
		}
	}

}

bool ASIOStreamBaseTransport::IsConnected()
{
	return connected.load();
}


uint32_t ASIOStreamBaseTransport::StreamCapabilities(const std::string &name)
{
	if (name == "com.robotraconteur.message.v_max")
	{
		return 3;
	}

	if (name == "com.robotraconteur.v2")
	{
		return 1;
	}

	if (name == "com.robotraconteur.v2.minor")
	{
		return 0;
	}

	if (name == "com.robotraconteur.v2.0")
	{
		return 1;
	}

	if (name == "com.robotraconteur.message.v3")
	{
		return 1;
	}

	if (name == "com.robotraconteur.message.v3.minor")
	{
		return 0;
	}

	if (name == "com.robotraconteur.message.v3.0")
	{
		return 1;
	}

	if (name == "com.robotraconteur.stringtable")
	{
		return 3;
	}

	if (name == "com.robotraconteur.stringtable.v3")
	{
		return 1;
	}

	if (name == "com.robotraconteur.stringtable.v3.minor")
	{
		return 0;
	}

	if (name == "com.robotraconteur.stringtable.v3.0")
	{
		return 1;
	}

	return 0;
}


void ASIOStreamBaseTransport::AsyncCheckStreamCapability(const std::string &name, boost::function<void (uint32_t, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	boost::mutex::scoped_lock lock (CheckStreamCapability_lock);
	if (CheckStreamCapability_closed)
	{
		RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback, 0, RR_MAKE_SHARED<ConnectionException>("Connection closed")), true);
		return;
	}
	if (!CheckStreamCapability_waiting)
	{
		BeginCheckStreamCapability(name,callback);
	}
	else
	{
		CheckStreamCapability_queue.push(boost::make_tuple(name,callback));
	}
}

void ASIOStreamBaseTransport::BeginCheckStreamCapability(const std::string &name, boost::function<void (uint32_t, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
		
	{
		

		if (CheckStreamCapability_waiting) throw InvalidOperationException("Already checking capability");
		CheckStreamCapability_waiting=true;
		RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		m->header->SenderNodeID = GetNode()->NodeID();
		{
			boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
			m->header->ReceiverNodeID = RemoteNodeID;
		}
		RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamCheckCapability, name);
		m->entries.push_back(mm);
			
		if (CheckStreamCapability_timer)
		{
			try
			{
				CheckStreamCapability_timer->cancel();
			}
			catch (std::exception&) {}
		}

		CheckStreamCapability_callback=callback;

		CheckStreamCapability_timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service),boost::posix_time::milliseconds(10000));
		RR_WEAK_PTR<ASIOStreamBaseTransport> t=RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());
		RobotRaconteurNode::asio_async_wait(node, CheckStreamCapability_timer,boost::bind(&ASIOStreamBaseTransport::CheckStreamCapability_timercallback, t,boost::asio::placeholders::error));  
			
		
		CheckStreamCapability_waiting=true;

		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
			RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
			_1);
		AsyncSendMessage(m, h);

			
		return ;

	}

}


void ASIOStreamBaseTransport::CheckStreamCapability_EndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err)
{
	if (err)
	{
		boost::mutex::scoped_lock lock (CheckStreamCapability_lock);
		if (CheckStreamCapability_waiting)
		{
			if (CheckStreamCapability_callback)
			{
				RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(CheckStreamCapability_callback,0, err));
			}
			CheckStreamCapability_waiting=false;
			CheckStreamCapability_callback=NULL;
			if (CheckStreamCapability_timer)
			{
				CheckStreamCapability_timer->cancel();
				CheckStreamCapability_timer.reset();
			}
			
			if (!CheckStreamCapability_queue.empty())
			{
				boost::tuple<std::string,boost::function<void (uint32_t, RR_SHARED_PTR<RobotRaconteurException>)> > d=CheckStreamCapability_queue.front();
				CheckStreamCapability_queue.pop();
				try
				{
					BeginCheckStreamCapability(d.get<0>(),d.get<1>());
				}				
				catch (std::exception& exp)
				{
					detail::InvokeHandlerWithException(node, d.get<1>(), exp);
				}
			}
		}
			
	}

}


void ASIOStreamBaseTransport::CheckStreamCapability_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t,const boost::system::error_code& e)
{
	if (e != boost::asio::error::operation_aborted)
	{
		RR_SHARED_PTR<ASIOStreamBaseTransport> t2=t.lock();
		if (!t2) return;

		boost::mutex::scoped_lock lock (t2->CheckStreamCapability_lock);
		if (t2->CheckStreamCapability_waiting)
		{
			if (t2->CheckStreamCapability_callback)
			{
				RobotRaconteurNode::TryPostToThreadPool(t2->node, boost::bind(t2->CheckStreamCapability_callback,0,RR_MAKE_SHARED<RequestTimeoutException>("Timed out")));
			}
			t2->CheckStreamCapability_waiting=false;
			t2->CheckStreamCapability_callback=NULL;
			t2->CheckStreamCapability_timer.reset();

			while (!t2->CheckStreamCapability_queue.empty())
			{
				boost::tuple<std::string,boost::function<void (uint32_t, RR_SHARED_PTR<RobotRaconteurException>)> > d=t2->CheckStreamCapability_queue.front();
				t2->CheckStreamCapability_queue.pop();
				//t2->BeginCheckStreamCapability(d.get<0>(),d.get<1>());
				detail::PostHandlerWithException(t2->node, d.get<1>(), RR_MAKE_SHARED<RequestTimeoutException>("Timed out"));
			}
		}
	}
}


void ASIOStreamBaseTransport::CheckStreamCapability_MessageReceived( RR_INTRUSIVE_PTR<Message> m)
{
	try
	{
		if (m->entries.at(0)->EntryType == MessageEntryType_StreamCheckCapability)
		{
			RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
			ret->header = CreateMessageHeader();
			ret->header->SenderNodeID = GetNode()->NodeID();
			ret->header->ReceiverNodeID = m->header->SenderNodeID;
			RR_INTRUSIVE_PTR<MessageEntry> mret = CreateMessageEntry(MessageEntryType_StreamCheckCapabilityRet, m->entries.at(0)->MemberName);
			mret->ServicePath = m->entries.at(0)->ServicePath;
			mret->AddElement("return", ScalarToRRArray(StreamCapabilities(m->entries.at(0)->MemberName)));
			ret->entries.push_back(mret);

			

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
				_1);
			AsyncSendMessage(ret, h);
		}
		else if (m->entries.at(0)->EntryType == MessageEntryType_StreamCheckCapabilityRet)
		{
			boost::mutex::scoped_lock lock (CheckStreamCapability_lock);
			if (CheckStreamCapability_waiting)
			{
				if (CheckStreamCapability_callback)
				{
					RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(CheckStreamCapability_callback,RRArrayToScalar(m->entries.at(0)->FindElement("return")->CastData<RRArray<uint32_t> >()),RR_SHARED_PTR<RobotRaconteurException>()));
				}
				CheckStreamCapability_waiting=false;
				CheckStreamCapability_callback=NULL;
				if (CheckStreamCapability_timer)
				{
				CheckStreamCapability_timer->cancel();
				CheckStreamCapability_timer.reset();
				}

				if (!CheckStreamCapability_queue.empty())
				{
					boost::tuple<std::string,boost::function<void (uint32_t, RR_SHARED_PTR<RobotRaconteurException>)> > d=CheckStreamCapability_queue.front();
					CheckStreamCapability_queue.pop();
					try
					{
						BeginCheckStreamCapability(d.get<0>(),d.get<1>());
					}					
					catch (std::exception& exp)
					{
						detail::PostHandlerWithException(node, d.get<1>(), exp);
					}
				}
			}
		}
	}
	catch (std::exception&)
	{
	};
}


void ASIOStreamBaseTransport::AsyncStreamOp(const std::string &command, RR_SHARED_PTR<RRObject> args,  boost::function<void (RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	boost::mutex::scoped_lock lock(streamop_lock);

	if (streamop_closed)
	{
		detail::PostHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>("Connection closed"),true, false);
		return;
	}

	if (!streamop_waiting)
	{
		BeginStreamOp(command, args, callback);
	}
	else
	{
		streamop_queue.push(boost::make_tuple(command,args,callback));
	}
}

void ASIOStreamBaseTransport::BeginStreamOp(const std::string &command, RR_SHARED_PTR<RRObject> args,  boost::function<void (RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	{		
		
		RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		m->header = CreateMessageHeader();
		m->header->ReceiverNodeName = "";
		m->header->SenderNodeName = GetNode()->NodeName();
		m->header->SenderNodeID = GetNode()->NodeID();
		{
			boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
			m->header->ReceiverNodeID = RemoteNodeID;
		}
		
		if (command=="CreateConnection")
		{
			RR_SHARED_PTR<AsyncAttachStream_args> a=RR_STATIC_POINTER_CAST<AsyncAttachStream_args>(args);
			m->header->ReceiverNodeID=a->nodeid;
			m->header->ReceiverNodeName=a->nodename;
			RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamOp, command);

			std::vector<uint32_t> caps;
			caps.push_back(TransportCapabilityCode_MESSAGE2_BASIC_PAGE | TransportCapabilityCode_MESSAGE2_BASIC_ENABLE 
				| TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED);

			if (!disable_message3)
			{
				caps.push_back(TransportCapabilityCode_MESSAGE3_BASIC_PAGE | TransportCapabilityCode_MESSAGE3_BASIC_ENABLE 
					| TransportCapabilityCode_MESSAGE3_BASIC_CONNECTCOMBINED);
				if (!disable_string_table)
				{
					caps.push_back(TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE | TransportCapabilityCode_MESSAGE3_STRINGTABLE_ENABLE
						| TransportCapabilityCode_MESSAGE3_STRINGTABLE_MESSAGE_LOCAL | TransportCapabilityCode_MESSAGE3_STRINGTABLE_DYNAMIC_TABLE
						| TransportCapabilityCode_MESSAGE3_STRINGTABLE_STANDARD_TABLE
					);
				}
			}
			mm->AddElement("capabilities", VectorToRRArray<uint32_t>(caps));
			m->entries.push_back(mm);
		}
		else
		{
			RR_INTRUSIVE_PTR<MessageEntry> mm=PackStreamOpRequest(command,args);
			m->entries.push_back(mm);
		}
		if (streamop_timer)
		{
			try
			{
				streamop_timer->cancel();
			}
			catch (std::exception&) {}
		}

		streamop_callback=callback;

		streamop_timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service),boost::posix_time::milliseconds(10000));
		RR_WEAK_PTR<ASIOStreamBaseTransport> t=RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());
		RobotRaconteurNode::asio_async_wait(node, streamop_timer, boost::bind(&ASIOStreamBaseTransport::StreamOp_timercallback, t,boost::asio::placeholders::error));
			
		
		
		streamop_waiting=true;
		
		
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
			RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()),
			_1);

		AsyncSendMessage(m, h);

			


	}
}


RR_INTRUSIVE_PTR<MessageEntry> ASIOStreamBaseTransport::PackStreamOpRequest(const std::string &command, RR_SHARED_PTR<RRObject> args)
{
	RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamOp, command);

	if (command == "GetRemoteNodeID")
	{
	}
	else
	{
			throw InvalidOperationException("Unknown StreamOp command");

	}

	return mm;
}

void ASIOStreamBaseTransport::StreamOp_EndSendMessage(RR_SHARED_PTR<RobotRaconteurException> err)
{
	if (err)
	{
		//std::cout << "Err sending streamop message" << endl;
		boost::mutex::scoped_lock lock (streamop_lock);
		if (streamop_waiting)
		{
			if (streamop_callback)
			{
				detail::PostHandlerWithException(node, streamop_callback,err);
			}
			streamop_waiting=false;
			
			streamop_callback=NULL;
			if (streamop_timer)
			{
			streamop_timer->cancel();
			streamop_timer.reset();
			}
			if (!streamop_queue.empty())
			{
				boost::tuple<std::string,RR_SHARED_PTR<RRObject>,boost::function<void (RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > d=streamop_queue.front();
				streamop_queue.pop();
				try
				{
					BeginStreamOp(d.get<0>(),d.get<1>(),d.get<2>());
				}				
				catch (std::exception& exp)
				{
					detail::InvokeHandlerWithException(node, d.get<2>(), exp, MessageErrorType_ConnectionError);
				}
			}
				
		}
			
	}

}


void ASIOStreamBaseTransport::StreamOp_timercallback(RR_WEAK_PTR<ASIOStreamBaseTransport> t,const boost::system::error_code& e)
{
	if (e != boost::asio::error::operation_aborted)
	{
		RR_SHARED_PTR<ASIOStreamBaseTransport> t2=t.lock();
		if (!t2) return;
		boost::mutex::scoped_lock lock (t2->streamop_lock);
		if (t2->streamop_waiting)
		{
			if (t2->streamop_callback)
			{
				detail::PostHandlerWithException(t2->node, t2->streamop_callback,RR_MAKE_SHARED<RequestTimeoutException>("Timed out"), true, false);
			}
			t2->streamop_waiting=false;
			
			t2->streamop_callback=NULL;
			t2->streamop_timer.reset();

			while (!t2->streamop_queue.empty())
			{
				boost::tuple<std::string,RR_SHARED_PTR<RRObject>,boost::function<void (RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > d=t2->streamop_queue.front();
				t2->streamop_queue.pop();
				//t2->BeginStreamOp(d.get<0>(),d.get<1>(),d.get<2>());
				detail::PostHandlerWithException(t2->node, d.get<2>(), RR_MAKE_SHARED<RequestTimeoutException>("Timed out"), true, false);
			}
		}
	}
}


RR_INTRUSIVE_PTR<MessageEntry> ASIOStreamBaseTransport::ProcessStreamOpRequest(RR_INTRUSIVE_PTR<MessageEntry> request, RR_INTRUSIVE_PTR<MessageHeader> header)
{
	std::string command=request->MemberName;
	RR_INTRUSIVE_PTR<MessageEntry> mmret = CreateMessageEntry(MessageEntryType_StreamOpRet, command);

	try
	{
		if (command == "GetRemoteNodeID")
		{
		}
		else if (command == "CreateConnection")
		{
			RR_INTRUSIVE_PTR<MessageElement> elem_caps;
			if (request->TryFindElement("capabilities", elem_caps))
			{
				uint32_t message2_basic_caps = TransportCapabilityCode_MESSAGE2_BASIC_ENABLE;
				uint32_t message3_basic_caps = 0;
				uint32_t message3_string_caps = 0;

				std::vector<uint32_t> ret_caps;

				RR_INTRUSIVE_PTR<RRArray<uint32_t> > caps_array = elem_caps->CastData<RRArray<uint32_t> >();
				for (size_t i = 0; i < caps_array->size(); i++)
				{
					uint32_t cap = (*caps_array)[i];
					uint32_t cap_page = cap & TranspartCapabilityCode_PAGE_MASK;
					uint32_t cap_value = cap & ~TranspartCapabilityCode_PAGE_MASK;
					if (cap_page == TransportCapabilityCode_MESSAGE2_BASIC_PAGE)
					{
						message2_basic_caps = (cap_value & (TransportCapabilityCode_MESSAGE2_BASIC_ENABLE 
							| TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED));
					}

					if (cap_page == TransportCapabilityCode_MESSAGE3_BASIC_PAGE)
					{
						message3_basic_caps = (cap_value & (TransportCapabilityCode_MESSAGE3_BASIC_ENABLE 
							| TransportCapabilityCode_MESSAGE3_BASIC_CONNECTCOMBINED));
					}

					if (cap_page == TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE)
					{
						message3_string_caps = (cap_value & (TransportCapabilityCode_MESSAGE3_STRINGTABLE_ENABLE 
							| TransportCapabilityCode_MESSAGE3_STRINGTABLE_MESSAGE_LOCAL
							| TransportCapabilityCode_MESSAGE3_STRINGTABLE_DYNAMIC_TABLE 
							| TransportCapabilityCode_MESSAGE3_STRINGTABLE_STANDARD_TABLE));
					}

				}

				if (!(message2_basic_caps & TransportCapabilityCode_MESSAGE2_BASIC_ENABLE))
				{
					throw ProtocolException("Transport must support Message Version 2");
				}
				else
				{
					message2_basic_caps |= TransportCapabilityCode_MESSAGE2_BASIC_PAGE;
					ret_caps.push_back(message2_basic_caps);
					active_capabilities_message2_basic = message2_basic_caps;
				}

				if ((message3_basic_caps & TransportCapabilityCode_MESSAGE3_BASIC_ENABLE) && !disable_message3)
				{
					send_version3.store(true);
					message3_basic_caps |= TransportCapabilityCode_MESSAGE3_BASIC_PAGE;
					ret_caps.push_back(message3_basic_caps);
					active_capabilities_message3_basic = message3_basic_caps;
					if ((message3_string_caps & TransportCapabilityCode_MESSAGE3_STRINGTABLE_ENABLE) && !disable_string_table)
					{						
						message3_string_caps |= TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE;
						std::vector<uint32_t> string_table_flags;
						string_table_flags.push_back(message3_string_caps);
						string_table3->SetTableFlags(string_table_flags);						
						ret_caps.push_back(message3_string_caps);
						active_capabilities_message3_stringtable = message3_string_caps;
						use_string_table3.store(true);
					}
				}				

				mmret->AddElement("capabilities", VectorToRRArray<uint32_t>(ret_caps));
			}
			
			boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
			if (RemoteNodeID.IsAnyNode() || RemoteNodeID==header->SenderNodeID)
			{
				if (header->ReceiverNodeID.IsAnyNode() && (header->ReceiverNodeName=="" || header->ReceiverNodeName==GetNode()->NodeName()))
				{
					RemoteNodeID=header->SenderNodeID;
					return mmret;
				}

				if (header->ReceiverNodeID==GetNode()->NodeID())
				{
					if (header->ReceiverNodeName=="" || header->ReceiverNodeName ==GetNode()->NodeName())
					{
						RemoteNodeID=header->SenderNodeID;
						return mmret;
					}
				}
			}
			
			mmret->Error = MessageErrorType_NodeNotFound;
			mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.NodeNotFound"));
			mmret->AddElement("errorstring", stringToRRArray("Node not found"));
		}
		else
		{
				throw ProtocolException("Unknown StreamOp command");

		}
	}
	catch (std::exception&)
	{
		mmret->Error = MessageErrorType_ProtocolError;
		mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.ProtocolError"));
		mmret->AddElement("errorstring", stringToRRArray("Invalid Stream Operation"));
	}

	return mmret;

}

void ASIOStreamBaseTransport::StreamOpMessageReceived(RR_INTRUSIVE_PTR<Message> m)
{

	//std::cout << "streamop got message" << endl;

	RR_INTRUSIVE_PTR<MessageEntry> mm;

	try
	{
		mm = m->entries.at(0);



	}
	catch (std::exception&)
	{
		return;
	}
	if (mm->EntryType == MessageEntryType_StreamOp)
	{
		std::string command = mm->MemberName;
		RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
		mret->header = CreateMessageHeader();
		mret->header->SenderNodeName = GetNode()->NodeName();
		mret->header->ReceiverNodeName = m->header->SenderNodeName;
		mret->header->SenderNodeID = GetNode()->NodeID();
		mret->header->ReceiverNodeID = m->header->SenderNodeID;
		RR_INTRUSIVE_PTR<MessageEntry> mmret = ProcessStreamOpRequest(mm,m->header);
		
		if (mmret)
		{
			mret->entries.push_back(mmret);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::StreamOp_EndSendMessage, RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1);
			AsyncSendMessage(mret, h);
		}
	}
	else
	{
		boost::mutex::scoped_lock lock (streamop_lock);
		if (streamop_waiting)
		{
			streamop_waiting = false;

			string command=mm->MemberName;

			RR_SHARED_PTR<RobotRaconteurException> rrexp;
			RR_SHARED_PTR<RRObject> ret;

			try
			{

				ret=UnpackStreamOpResponse(mm,m->header);
			}			
			catch (std::exception& exp)
			{
				rrexp=RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp);
			}

			if (streamop_callback)
			{
				try
				{
					streamop_callback(rr_cast<RRObject>(ret),rrexp);
				}
				catch (std::exception&)
				{
					Close();
				}
			}
			streamop_callback=NULL;
			if (streamop_timer)
			{
				streamop_timer->cancel();
				streamop_timer.reset();
			}
			while (!streamop_queue.empty())
			{
				boost::tuple<std::string,RR_SHARED_PTR<RRObject>,boost::function<void (RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> > d=streamop_queue.front();
				streamop_queue.pop();
				try
				{
					BeginStreamOp(d.get<0>(),d.get<1>(),d.get<2>());
					break;
				}				
				catch (std::exception& exp)
				{					
					detail::InvokeHandlerWithException(node, d.get<2>(),exp);					
				}
			}

			

			
		}


	}

}

RR_SHARED_PTR<RRObject> ASIOStreamBaseTransport::UnpackStreamOpResponse(RR_INTRUSIVE_PTR<MessageEntry> response, RR_INTRUSIVE_PTR<MessageHeader> header)
{
	std::string command=response->MemberName;
	if (command == "GetRemoteNodeID")
	{
		//std::cout << "Got node id" << std::endl;

		NodeID* n=new NodeID(header->SenderNodeID);
		RR_SHARED_PTR<NodeID> ret=RR_SHARED_PTR<NodeID>(n);
		return ret;
	}
	else if (command == "CreateConnection")
	{
		boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
		if (response->Error!=0 && response->Error!=MessageErrorType_ProtocolError)
		{
			RobotRaconteurExceptionUtil::ThrowMessageEntryException(response);
		}

		if (!RemoteNodeID.IsAnyNode())
		{
			if (header->SenderNodeID!=RemoteNodeID) throw ConnectionException("Invalid node connection");
		}
		else
		{
			if (target_nodename!="")
			{
				if (target_nodename!=header->SenderNodeName)
					throw ConnectionException("Invalid node connection");
			}

			if (!target_nodeid.IsAnyNode())
			{
				if (target_nodeid!=header->SenderNodeID)
					throw ConnectionException("Invalid node connection");
			}
		}
		
		RR_INTRUSIVE_PTR<MessageElement> elem_caps;
		if (response->TryFindElement("capabilities", elem_caps))
		{
			uint32_t message2_basic_caps = TransportCapabilityCode_MESSAGE2_BASIC_ENABLE;
			uint32_t message3_basic_caps = 0;
			uint32_t message3_string_caps = 0;

			RR_INTRUSIVE_PTR<RRArray<uint32_t> > caps_array = elem_caps->CastData<RRArray<uint32_t> >();
			for (size_t i = 0; i < caps_array->size(); i++)
			{
				uint32_t cap = (*caps_array)[i];
				uint32_t cap_page = cap & TranspartCapabilityCode_PAGE_MASK;
				uint32_t cap_value = cap & ~TranspartCapabilityCode_PAGE_MASK;
				if (cap_page == TransportCapabilityCode_MESSAGE2_BASIC_PAGE)
				{
					if (!(cap_value & TransportCapabilityCode_MESSAGE2_BASIC_ENABLE))
					{
						throw ProtocolException("Transport must support Message Version 2");
					}

					if ((cap_value & ~(TransportCapabilityCode_MESSAGE2_BASIC_ENABLE
						| TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED)) != 0)
					{
						throw ProtocolException("Invalid Message Version 2 capabilities");
					}

					message2_basic_caps = cap_value;
				}

				if (cap_page == TransportCapabilityCode_MESSAGE3_BASIC_PAGE)
				{
					if (disable_message3)
					{
						if (cap_value != 0)
						{
							throw ProtocolException("Invalid Message Version 3 capabilities");
						}
					}
					else
					{
						if (!(cap_value & TransportCapabilityCode_MESSAGE3_BASIC_ENABLE))
						{
							if (cap_value != 0)
							{
								throw ProtocolException("Invalid Message Version 3 capabilities");
							}
						}
						else
						{
							if ((cap_value & ~(TransportCapabilityCode_MESSAGE3_BASIC_ENABLE
								| TransportCapabilityCode_MESSAGE3_BASIC_CONNECTCOMBINED)) != 0)
							{
								throw ProtocolException("Invalid Message Version 2 capabilities");
							}

							message3_basic_caps = cap_value;
						}
					}


				}

				if (cap_page == TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE)
				{
					if (disable_message3 || disable_string_table)
					{
						if (cap_value != 0)
						{
							throw ProtocolException("Invalid Message Version 3 String Table capabilities");
						}
					}
					else
					{
						if (!(cap_value & TransportCapabilityCode_MESSAGE3_STRINGTABLE_ENABLE))
						{
							if (cap_value != 0)
							{
								throw ProtocolException("Invalid Message Version 3 String Table capabilities");
							}
						}
						else
						{
							if ((cap_value & ~(TransportCapabilityCode_MESSAGE3_STRINGTABLE_ENABLE
								| TransportCapabilityCode_MESSAGE3_STRINGTABLE_MESSAGE_LOCAL
								| TransportCapabilityCode_MESSAGE3_STRINGTABLE_DYNAMIC_TABLE
								| TransportCapabilityCode_MESSAGE3_STRINGTABLE_STANDARD_TABLE)) != 0)
							{
								throw ProtocolException("Invalid Message Version 3 String Table capabilities");
							}

							message3_string_caps = cap_value;							
						}
					}
				}
			}

			active_capabilities_message2_basic = message2_basic_caps | TransportCapabilityCode_MESSAGE2_BASIC_PAGE;

			if (message3_basic_caps)
			{	
				active_capabilities_message3_basic = message3_basic_caps | TransportCapabilityCode_MESSAGE2_BASIC_PAGE;
				send_version3.store(true);				
				if (message3_string_caps)
				{
					active_capabilities_message3_stringtable = message3_string_caps | TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE;
					std::vector<uint32_t> string_table_flags;
					string_table_flags.push_back(active_capabilities_message3_stringtable);
					string_table3->SetTableFlags(string_table_flags);					
					use_string_table3.store(true);
				}
			}
			else
			{
				if (message3_string_caps != 0)
				{
					throw ProtocolException("Message 3 must be enabled for String Table 3");
				}
			}

		}
		
		NodeID* n=new NodeID(header->SenderNodeID);
		RR_SHARED_PTR<NodeID> ret=RR_SHARED_PTR<NodeID>(n);
		return ret;
	}
	else
	{
				
		throw MemberNotFoundException("Unknown command");

	}

}

void ASIOStreamBaseTransport::PeriodicCleanupTask()
{

}

NodeID ASIOStreamBaseTransport::GetRemoteNodeID()
{
	boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
	return RemoteNodeID;
}

void ASIOStreamBaseTransport::SendMessage(RR_INTRUSIVE_PTR<Message> m)
{
	RR_SHARED_PTR<detail::sync_async_handler<void> > s=RR_MAKE_SHARED<detail::sync_async_handler<void> >(RR_MAKE_SHARED<ConnectionException>("Send timeout"));
	
	boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&detail::sync_async_handler<void>::operator(), s, _1);
	AsyncSendMessage(m, h);

	s->end_void();
	
}

bool ASIOStreamBaseTransport::IsLargeTransferAuthorized()
{
	return true;
}

void ASIOStreamBaseTransport::UpdateStringTable()
{
	if (this->server) return;
	if (this->string_table_3_pause_updates) return;

	if (string_table3->GetUnconfirmedCodeCount() > 0)
	{
		boost::mutex::scoped_lock lock(string_table3_lock);
		if (string_table_3_closed) return;

		if (this->string_table_3_pause_updates) return;
		if (string_table_3_requests.size() > 2) return;

		std::vector<RR_SHARED_PTR<const detail::StringTableEntry> > a = string_table3->GetUnconfirmedCodes(512);
		for (std::vector<RR_SHARED_PTR<const detail::StringTableEntry> >::iterator e = a.begin(); e != a.end();)
		{
			uint32_t e1 = (*e)->code;
			if (std::find(string_table_3_confirming.begin(), string_table_3_confirming.end(), e1) != string_table_3_confirming.end())
			{
				e = a.erase(e);
			}
			else
			{
				e++;
			}
		}

		if (a.size() > 0)
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > el;
			std::vector<uint32_t> codes;
			BOOST_FOREACH (RR_SHARED_PTR<const detail::StringTableEntry>& e, a)
			{
				if (el.size() >= 32) break;
				codes.push_back(e->code);
				string_table_3_confirming.push_back(e->code);
				RR_INTRUSIVE_PTR<MessageElement> el1 = CreateMessageElement("", stringToRRArray(e->value));
				el1->ElementNumber = (int32_t)e->code;
				el1->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
				el1->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
				el.push_back(el1);
			}

			RR_INTRUSIVE_PTR<MessageEntry> me = CreateMessageEntry(MessageEntryType_StringTableOp, "confirmcodes");
			me->AddElement("value", CreateMessageElementMap<int32_t>(el));
			string_table_3_requestid++;
			me->RequestID = string_table_3_requestid;

			boost::posix_time::ptime n = boost::posix_time::microsec_clock::universal_time();

			string_table_3_requests.insert(std::make_pair(string_table_3_requestid, boost::make_tuple(codes,n)));

			RR_INTRUSIVE_PTR<Message> m = CreateMessage();
			m->header = CreateMessageHeader();
			m->header->MessageFlags = 0;
			m->entries.push_back(me);

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::UpdateStringTable1,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1, m);
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ASIOStreamBaseTransport::SimpleAsyncSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), m,h	));

			RR_SHARED_PTR<boost::asio::deadline_timer> t = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service));
			t->expires_from_now(boost::posix_time::seconds(15));
			RobotRaconteurNode::asio_async_wait(node, t, boost::bind(&ASIOStreamBaseTransport::UpdateStringTable3,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1, t, me->RequestID));
			string_table_3_timers.push_back(t);

		}

	}
}

void ASIOStreamBaseTransport::UpdateStringTable1(RR_SHARED_PTR<RobotRaconteurException> ret, RR_INTRUSIVE_PTR<Message> m)
{
	if (ret)
	{
		if (this->server) return;

		boost::mutex::scoped_lock lock(string_table3_lock);
		RR_UNORDERED_MAP<uint32_t, boost::tuple<std::vector<uint32_t>, boost::posix_time::ptime> >::iterator e = string_table_3_requests.find(m->entries.at(0)->RequestID);
		if (e == string_table_3_requests.end()) return;
		BOOST_FOREACH (uint32_t& e2, e->second.get<0>())
		{
			string_table_3_confirming.remove(e2);
		}
		string_table_3_requests.erase(e);
	}
}

void ASIOStreamBaseTransport::UpdateStringTable2(RR_INTRUSIVE_PTR<Message> m)
{		
	boost::mutex::scoped_lock lock(string_table3_lock);

	if (string_table_3_closed) return;

	if (m->entries.size() != 1) return;
	RR_INTRUSIVE_PTR<MessageEntry>& me = m->entries.at(0);

	if (me->MemberName == "pause" || me->MemberName == "resume")
	{
		if (me->EntryType == MessageEntryType_StringTableOp)
		{
			if (me->MemberName == "pause")
			{
				this->string_table_3_pause_updates = true;
			}
			else
			{
				this->string_table_3_pause_updates = false;
			}

			RR_INTRUSIVE_PTR<MessageEntry> me4 = CreateMessageEntry(MessageEntryType_StringTableOpRet, me->MemberName);

			RR_INTRUSIVE_PTR<Message> m4 = CreateMessage();
			m4->header = CreateMessageHeader();
			m4->header->MessageFlags = 0;
			m4->entries.push_back(me4);

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1);

			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ASIOStreamBaseTransport::SimpleAsyncSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), m4, h));
		}
	}

	if (server)
	{
		std::vector<uint32_t> o;		
		if (me->EntryType != MessageEntryType_StringTableOp) return;
		if (me->MemberName != "confirmcodes") return;
		if (me->Error != MessageErrorType_None) return;
		if (me->elements.size() != 1) return;
		RR_INTRUSIVE_PTR<MessageElement> mee = me->elements.at(0);
		if (mee->ElementName != "value") return;
		if (mee->ElementType != DataTypes_vector_t) return;
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > v = mee->CastData<MessageElementMap<int32_t> >()->Elements;
		BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, v)
		{
			if (!(e->ElementFlags & MessageElementFlags_ELEMENT_NUMBER))
				continue;

			RR_INTRUSIVE_PTR<RRArray<char> > c = RR_DYNAMIC_POINTER_CAST<RRArray<char> >(e->GetData());
			if (!c)
				continue;

			std::string c1 = RRArrayToString(c);
			if (e->ElementNumber < 2) continue;
			uint32_t c2 = (uint32_t)e->ElementNumber;
			if (this->string_table3->AddCode(c2, c1))
			{
				o.push_back(c2);
			}
		}
		
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > o1;
		if (o.size() > 0)
		{
			o1 = AttachRRArrayCopy(&o[0], o.size());
		}
		else
		{ 
			o1 = AllocateRRArray<uint32_t>(0);
		}

		RR_INTRUSIVE_PTR<MessageEntry> me2 = CreateMessageEntry(MessageEntryType_StringTableOpRet, "confirmcodes");
		me2->AddElement("value", o1);
		
		me2->RequestID = me->RequestID;		

		RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
		m2->header = CreateMessageHeader();
		m2->header->MessageFlags = 0;
		m2->entries.push_back(me2);

		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::UpdateStringTable1,
			RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1, m2);

		RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ASIOStreamBaseTransport::SimpleAsyncSendMessage,
			RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), m2, h));

		if (this->string_table3->IsTableFull())
		{
			RR_INTRUSIVE_PTR<MessageEntry> me3 = CreateMessageEntry(MessageEntryType_StringTableOp, "pause");			

			RR_INTRUSIVE_PTR<Message> m3 = CreateMessage();
			m3->header = CreateMessageHeader();
			m3->header->MessageFlags = 0;
			m3->entries.push_back(me3);

			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&ASIOStreamBaseTransport::SimpleAsyncEndSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), _1);
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ASIOStreamBaseTransport::SimpleAsyncSendMessage,
				RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this()), m3, h));
		}
	}
	else
	{
		std::vector<uint32_t> o;
		if (m->entries.size() != 1) return;
		RR_INTRUSIVE_PTR<MessageEntry>& me = m->entries.at(0);
		if (me->EntryType != MessageEntryType_StringTableOpRet) return;
		if (me->MemberName != "confirmcodes") return;
		if (me->Error != MessageErrorType_None) return;
		if (me->elements.size() != 1) return;
		RR_INTRUSIVE_PTR<MessageElement> mee = me->elements.at(0);
		if (mee->ElementName != "value") return;
		if (mee->ElementType != DataTypes_uint32_t) return;
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > codes = RR_DYNAMIC_POINTER_CAST<RRArray<uint32_t> >(mee->GetData());
		if (!codes) return;
		
		std::vector<uint32_t> o1;
		o1.reserve(codes->size());
		for (size_t i = 0; i < codes->size(); i++)
		{
			o1.push_back((*codes)[i]);
		}

		this->string_table3->ConfirmCodes(o1);
		std::vector<uint32_t> drop_codes;

		RR_UNORDERED_MAP<uint32_t, boost::tuple<std::vector<uint32_t>, boost::posix_time::ptime> >::iterator e1 = string_table_3_requests.find(me->RequestID);
		if (e1 != string_table_3_requests.end())
		{
			for (std::vector<uint32_t>::iterator e2 = e1->second.get<0>().begin(); e2 != e1->second.get<0>().end(); e2++)
			{
				if (std::find(o1.begin(), o1.end(), *e2) == o1.end())
				{
					drop_codes.push_back(*e2);					
				}
			}

			for (std::vector<uint32_t>::iterator e2 = e1->second.get<0>().begin(); e2 != e1->second.get<0>().end(); e2++)
			{
				string_table_3_confirming.remove(*e2);
			}

			string_table_3_requests.erase(e1);
		}

		string_table3->DropUnconfirmedCodes(drop_codes);		
	}
}

void ASIOStreamBaseTransport::UpdateStringTable3(const boost::system::error_code& ec, RR_SHARED_PTR<boost::asio::deadline_timer> t, uint32_t request_id)
{
	boost::mutex::scoped_lock lock(string_table3_lock);

	if (string_table_3_closed) return;

	string_table_3_timers.remove(t);

	if (ec) return;

	RR_UNORDERED_MAP<uint32_t, boost::tuple<std::vector<uint32_t>, boost::posix_time::ptime> >::iterator e1 = string_table_3_requests.find(request_id);
	if (e1 == string_table_3_requests.end()) return;
		
	for (std::vector<uint32_t>::iterator e2 = e1->second.get<0>().begin(); e2 != e1->second.get<0>().end(); e2++)
	{
		string_table_3_confirming.remove(*e2);		
	}

	string_table_3_requests.erase(e1);
	
}

bool ASIOStreamBaseTransport::GetDisableMessage3()
{
	return disable_message3;
}
void ASIOStreamBaseTransport::SetDisableMessage3(bool d)
{
	disable_message3 = d;
}

bool ASIOStreamBaseTransport::GetDisableStringTable()
{
	return disable_string_table;
}
void ASIOStreamBaseTransport::SetDisableStringTable(bool d)
{
	disable_string_table = d;
}

bool ASIOStreamBaseTransport::CheckCapabilityActive(uint32_t cap)
{
	uint32_t cap_page = cap & TranspartCapabilityCode_PAGE_MASK;
	uint32_t cap_value = cap & (~TranspartCapabilityCode_PAGE_MASK);

	if (cap_page == TransportCapabilityCode_MESSAGE2_BASIC_PAGE)
	{
		return (cap_value & (active_capabilities_message2_basic & (~TranspartCapabilityCode_PAGE_MASK))) != 0;
	}

	if (cap_page == TransportCapabilityCode_MESSAGE3_BASIC_PAGE)
	{
		return (cap_value & (active_capabilities_message3_basic & (~TranspartCapabilityCode_PAGE_MASK))) != 0;
	}

	if (cap_page == TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE)
	{
		return (cap_value & (active_capabilities_message3_stringtable & (~TranspartCapabilityCode_PAGE_MASK))) != 0;
	}

	return false;
}

}
}

