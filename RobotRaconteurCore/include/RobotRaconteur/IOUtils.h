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

#pragma once



#include "RobotRaconteur/DataTypes.h"
#include <boost/detail/endian.hpp>
#include <stack>


namespace RobotRaconteur

{
	class ROBOTRACONTEUR_CORE_API ArrayBinaryReader : private boost::noncopyable
	{
	public:
		ArrayBinaryReader(uint8_t* buffer, size_t start_position, size_t length, bool nativeorder=false);

		size_t Length();

		virtual size_t Position();
		virtual void Seek(size_t position);

		virtual size_t Read(void* buffer, size_t index, size_t length);
		
		template <typename T> 
		T ReadNumber()
		{
			T out;
			Read((uint8_t*)((void*)&out),0,sizeof(T));
#ifdef BOOST_BIG_ENDIAN
			if (!nativeorder) std::reverse((uint8_t*)((void*)&out),((uint8_t*)((void*)&out))+sizeof(T));
#endif
			return out;

		}
		
		
		void ReadArray(RR_INTRUSIVE_PTR<RRBaseArray>& arr);

		std::string ReadString8(size_t length);

		uint32_t ReadUintX();		
		uint64_t ReadUintX2();

		int32_t ReadIntX();
		int64_t ReadIntX2();

		//A stack to set local limits on the
		//length of reads.  This will detect errors
		//in binary messages.

		virtual size_t CurrentLimit();

		virtual void PushRelativeLimit(size_t limit);
		
		virtual void PushAbsoluteLimit(size_t limit);

		virtual void PopLimit();

		virtual int32_t DistanceFromLimit();

	private:
		uint8_t* buffer;
		size_t position;
		size_t length;
		bool nativeorder;

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
		boost::container::small_vector<size_t, 8> limits;
#else		
		std::vector<size_t> limits;
#endif


	};

	class ROBOTRACONTEUR_CORE_API ArrayBinaryWriter : private boost::noncopyable
	{
	public:
		ArrayBinaryWriter(uint8_t* buffer, size_t start_position, size_t length, bool nativeorder=false);

		virtual size_t Length();

		virtual size_t Position();
		virtual void Seek(size_t position);

		virtual size_t Write(const void* buffer, size_t index, size_t length);
		
		template <typename T> 
		void WriteNumber(T number)
		{
			void* n1=(void*)&number;
#ifdef BOOST_BIG_ENDIAN
			if (!nativeorder) std::reverse((uint8_t*)n1,((uint8_t*)n1)+sizeof(T));
#endif
			Write((uint8_t*)n1,0,sizeof(T));
			

		}

		
		void WriteArray(RR_INTRUSIVE_PTR<RRBaseArray>& arr);

		void WriteString8(const std::string& str);
		void WriteString8WithXLen(const std::string& str);

		void WriteUintX(uint32_t v);
		void WriteUintX2(uint64_t v);

		void WriteIntX(int32_t v);
		void WriteIntX2(int64_t v);

		static size_t GetStringByteCount8(const std::string& str);
		static size_t GetStringByteCount8WithXLen(const std::string& str);
		
		static size_t GetUintXByteCount(uint32_t v);
		static size_t GetUintX2ByteCount(uint64_t v);
		
		static size_t GetSizePlusUintX(size_t s);

		static size_t GetIntXByteCount(int32_t v);
		static size_t GetIntX2ByteCount(int64_t v);

		//A stack to set local limits on the
		//length of reads.  This will detect errors
		//in binary messages.

		virtual size_t CurrentLimit();

		virtual void PushRelativeLimit(size_t limit);
		
		virtual void PushAbsoluteLimit(size_t limit);

		virtual void PopLimit();

		virtual int32_t DistanceFromLimit();

	private:
		uint8_t* buffer;
		size_t position;
		size_t length;
		bool nativeorder;

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
		boost::container::small_vector<size_t, 8> limits;
#else		
		std::vector<size_t> limits;
#endif


	};

}
