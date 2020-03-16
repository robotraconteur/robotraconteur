/** 
 * @file ServiceDefinition.h
 * 
 * @author Dr. John Wason
 * 
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"

#include <boost/tuple/tuple.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API  ServiceEntryDefinition;
	class ROBOTRACONTEUR_CORE_API  MemberDefinition;
	class ROBOTRACONTEUR_CORE_API  FunctionDefinition;
	class ROBOTRACONTEUR_CORE_API  PropertyDefinition;
	class ROBOTRACONTEUR_CORE_API  EventDefinition;
	class ROBOTRACONTEUR_CORE_API  ObjRefDefinition;
	class ROBOTRACONTEUR_CORE_API  PipeDefinition;
	class ROBOTRACONTEUR_CORE_API  CallbackDefinition;
	class ROBOTRACONTEUR_CORE_API  WireDefinition;
	class ROBOTRACONTEUR_CORE_API  MemoryDefinition;
	class ROBOTRACONTEUR_CORE_API  TypeDefinition;
	class ROBOTRACONTEUR_CORE_API  UsingDefinition;
	class ROBOTRACONTEUR_CORE_API  ConstantDefinition;
	class ROBOTRACONTEUR_CORE_API  EnumDefinition;
	class ROBOTRACONTEUR_CORE_API  EnumDefinitionValue;

	class ROBOTRACONTEUR_CORE_API  ServiceDefinitionParseException;
	class ROBOTRACONTEUR_CORE_API  ServiceDefinitionVerifyException;
	
	struct ROBOTRACONTEUR_CORE_API ServiceDefinitionParseInfo
	{
		std::string ServiceName;
		std::string RobDefFilePath;
		std::string Line;
		int32_t LineNumber;

		ServiceDefinitionParseInfo();
		void Reset();
	};

	class ROBOTRACONTEUR_CORE_API RobotRaconteurVersion
	{
	public:
		RobotRaconteurVersion();
		RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch=0, uint32_t tweak=0);
		RobotRaconteurVersion(boost::string_ref v);

		std::string ToString() const;
		void FromString(boost::string_ref v, const ServiceDefinitionParseInfo* parse_info = NULL);

		ROBOTRACONTEUR_CORE_API friend bool operator == (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2);
		ROBOTRACONTEUR_CORE_API friend bool operator != (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2);
		ROBOTRACONTEUR_CORE_API friend bool operator > (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2);
		ROBOTRACONTEUR_CORE_API friend bool operator >= (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2);
		ROBOTRACONTEUR_CORE_API friend bool operator < (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2);
		ROBOTRACONTEUR_CORE_API friend bool operator <= (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2);

		operator bool() const;

		uint32_t major;
		uint32_t minor;
		uint32_t patch;
		uint32_t tweak;

		ServiceDefinitionParseInfo ParseInfo;
	};

	class ROBOTRACONTEUR_CORE_API  ServiceDefinitionParseException : public ServiceDefinitionException
	{

	public:

		ServiceDefinitionParseInfo ParseInfo;

		std::string ShortMessage;

		ServiceDefinitionParseException(const std::string& e);
		ServiceDefinitionParseException(const std::string& e, const ServiceDefinitionParseInfo& info);

		virtual std::string ToString();

		virtual const char* what() const throw ();

		~ServiceDefinitionParseException() throw () {}

	private:
		std::string what_store;

	};

	class ROBOTRACONTEUR_CORE_API  ServiceDefinitionVerifyException : public ServiceDefinitionException
	{

	public:

		ServiceDefinitionParseInfo ParseInfo;

		std::string ShortMessage;

		ServiceDefinitionVerifyException(const std::string& e);
		ServiceDefinitionVerifyException(const std::string& e, const ServiceDefinitionParseInfo& info);

		virtual std::string ToString();

		virtual const char* what() const throw ();

		~ServiceDefinitionVerifyException() throw () {}

	private:
		std::string what_store;

	};

	class ROBOTRACONTEUR_CORE_API  ServiceDefinition : public RR_ENABLE_SHARED_FROM_THIS<ServiceDefinition>
	{
	public:
		std::string Name;

		std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Structures;
		std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Pods;
		std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > NamedArrays;
		std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Objects;

		std::vector<std::string> Options;

		std::vector<std::string> Imports;

		std::vector<RR_SHARED_PTR<UsingDefinition> > Using;

		std::vector<std::string> Exceptions;

		std::vector<RR_SHARED_PTR<ConstantDefinition> > Constants;
		std::vector<RR_SHARED_PTR<EnumDefinition> > Enums;

		RobotRaconteurVersion StdVer;

		ServiceDefinitionParseInfo ParseInfo;

		virtual ~ServiceDefinition() {}

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);
		void FromString(boost::string_ref s, std::vector<ServiceDefinitionParseException>& warnings, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void ToStream(std::ostream& os);
		void FromStream(std::istream& is, const ServiceDefinitionParseInfo* parse_info = NULL);
		void FromStream(std::istream& is, std::vector<ServiceDefinitionParseException>& warnings, const ServiceDefinitionParseInfo* parse_info = NULL);

		void CheckVersion(RobotRaconteurVersion ver=RobotRaconteurVersion(0,0));
			
		ServiceDefinition();

		void Reset();
	};

	class ROBOTRACONTEUR_CORE_API NamedTypeDefinition
	{
	public:
		std::string Name;
		virtual DataTypes RRDataType() = 0;
		virtual std::string ResolveQualifiedName() = 0;
	};

	class ROBOTRACONTEUR_CORE_API  ServiceEntryDefinition : public RR_ENABLE_SHARED_FROM_THIS<ServiceEntryDefinition>, public NamedTypeDefinition
	{
	public:		

		std::vector<RR_SHARED_PTR<MemberDefinition> > Members;

		DataTypes EntryType;

		std::vector<std::string> Implements;
		std::vector<std::string> Options;
		std::vector<RR_SHARED_PTR<ConstantDefinition> > Constants;		

		ServiceDefinitionParseInfo ParseInfo;

		ServiceEntryDefinition(RR_SHARED_PTR<ServiceDefinition> def);

		virtual ~ServiceEntryDefinition() {}

		virtual std::string ToString();
		virtual void ToStream(std::ostream& os);

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);		
		void FromString(boost::string_ref s, std::vector<ServiceDefinitionParseException>& warnings, const ServiceDefinitionParseInfo* parse_info = NULL);

		void FromStream(std::istream &is, const ServiceDefinitionParseInfo* parse_info = NULL);
		void FromStream(std::istream &is, std::vector<ServiceDefinitionParseException>& warnings, const ServiceDefinitionParseInfo* parse_info = NULL);

		RR_WEAK_PTR<ServiceDefinition> ServiceDefinition_;

		void Reset();

		virtual DataTypes RRDataType();
		virtual std::string ResolveQualifiedName();
			
	};


	class ROBOTRACONTEUR_CORE_API  MemberDefinition : public RR_ENABLE_SHARED_FROM_THIS<MemberDefinition>
	{
	public:
		std::string Name;
		RR_WEAK_PTR<ServiceEntryDefinition> ServiceEntry;

		std::vector<std::string> Modifiers;

		ServiceDefinitionParseInfo ParseInfo;

		MemberDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual MemberDefinition_NoLock NoLock();

		virtual ~MemberDefinition() {}

		virtual std::string ToString() { return ""; }
		virtual void Reset();

		
	};

	class ROBOTRACONTEUR_CORE_API  PropertyDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		PropertyDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual ~PropertyDefinition() {}

		virtual std::string ToString();

		std::string ToString(bool isstruct);

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();

		MemberDefinition_Direction Direction();

	};


	class ROBOTRACONTEUR_CORE_API  FunctionDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> ReturnType;
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		FunctionDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();

		bool IsGenerator();
		
	};

	class ROBOTRACONTEUR_CORE_API  EventDefinition : public MemberDefinition
	{
	public:
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		EventDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);
			
		virtual void Reset();
	};

	class ROBOTRACONTEUR_CORE_API  ObjRefDefinition : public MemberDefinition
	{
	public:
		std::string ObjectType;
		DataTypes_ArrayTypes ArrayType;
		DataTypes_ContainerTypes ContainerType;

		ObjRefDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();


		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();
	};

	class ROBOTRACONTEUR_CORE_API  PipeDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		PipeDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();

		MemberDefinition_Direction Direction();
		bool IsUnreliable();

	};

	class ROBOTRACONTEUR_CORE_API  CallbackDefinition : public MemberDefinition
	{
	public:

		virtual ~CallbackDefinition() {}

		RR_SHARED_PTR<TypeDefinition> ReturnType;
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		CallbackDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();

	};

	class ROBOTRACONTEUR_CORE_API  WireDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		WireDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();

		MemberDefinition_Direction Direction();

	};

	class ROBOTRACONTEUR_CORE_API  MemoryDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		MemoryDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		virtual void Reset();

		MemberDefinition_Direction Direction();

	};

	class ROBOTRACONTEUR_CORE_API  TypeDefinition
	{
	public:
		virtual ~TypeDefinition() {}

		std::string Name;

		DataTypes Type;
		std::string TypeString;

		DataTypes_ArrayTypes ArrayType;
		bool ArrayVarLength;
		std::vector<int32_t> ArrayLength;

		DataTypes_ContainerTypes ContainerType;				
		
		RR_WEAK_PTR<MemberDefinition> member;

		ServiceDefinitionParseInfo ParseInfo;

		TypeDefinition();
		TypeDefinition(RR_SHARED_PTR<MemberDefinition> member);

		virtual std::string ToString();

		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		static DataTypes DataTypeFromString(boost::string_ref d);

		static std::string StringFromDataType(DataTypes d);
			
		void Reset();

		void CopyTo(TypeDefinition& def) const;
		RR_SHARED_PTR<TypeDefinition> Clone() const;

		void Rename(boost::string_ref name);
		void RemoveContainers();
		void RemoveArray();
		void QualifyTypeStringWithUsing();
		void UnqualifyTypeStringWithUsing();
		RR_SHARED_PTR<NamedTypeDefinition> ResolveNamedType(std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs= std::vector<RR_SHARED_PTR<ServiceDefinition> >(), RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>(), RR_SHARED_PTR<RRObject> client=RR_SHARED_PTR<RRObject>());
			
		//Don't modify directly, use ResolveNamedType. Left public for specalized use.
		RR_WEAK_PTR<NamedTypeDefinition> ResolveNamedType_cache;

	};

	class ROBOTRACONTEUR_CORE_API  UsingDefinition
	{
	public:
		virtual ~UsingDefinition();

		std::string QualifiedName;
		std::string UnqualifiedName;

		RR_WEAK_PTR<ServiceDefinition> service;

		ServiceDefinitionParseInfo ParseInfo;

		UsingDefinition(RR_SHARED_PTR<ServiceDefinition> service);

		std::string ToString();
		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

	};

	struct ROBOTRACONTEUR_CORE_API ConstantDefinition_StructField
	{
		std::string Name;
		std::string ConstantRefName;
	};

	class ROBOTRACONTEUR_CORE_API  ConstantDefinition
	{
	public:
		virtual ~ConstantDefinition();

		std::string Name;

		RR_SHARED_PTR<TypeDefinition> Type;

		std::string Value;

		RR_WEAK_PTR<ServiceDefinition> service;
		RR_WEAK_PTR<ServiceEntryDefinition> service_entry;

		ServiceDefinitionParseInfo ParseInfo;

		ConstantDefinition(RR_SHARED_PTR<ServiceDefinition> service);		
		ConstantDefinition(RR_SHARED_PTR<ServiceEntryDefinition> service_entry);
		
		std::string ToString();
		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

		void Reset();

		static bool VerifyTypeAndValue(TypeDefinition& t, boost::string_ref value);
		bool VerifyValue();

		template<typename T>
		T ValueToScalar()
		{
			T v;
			if (!detail::try_convert_string_to_number(boost::trim_copy(Value), v))
			{
				throw ServiceDefinitionParseException("Invalid constant", ParseInfo);
			}
			return v;
		}

		template<typename T>
		RR_INTRUSIVE_PTR<RRArray<T> > ValueToArray()
		{
			boost::iterator_range<std::string::const_iterator> value1(Value);
			value1=boost::trim_copy_if(value1, boost::is_any_of(" \t{}"));
			value1=boost::trim_copy(value1);
			if (value1.empty()) 
				return AllocateRRArray<T>(0);

			size_t n = 0;

			//Count number of elements
			typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
			for (string_split_iterator e = boost::make_split_iterator(value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
				e != string_split_iterator(); e++)
			{
				n++;
			}

			RR_INTRUSIVE_PTR<RRArray<T> > o = AllocateRRArray<T>(n);

			size_t i = 0;

			//Read elements
			typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
			for (string_split_iterator e = boost::make_split_iterator(value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
				e != string_split_iterator(); e++)
			{
				if (!detail::try_convert_string_to_number(boost::trim_copy(*e), (*o)[i]))
				{
					throw ServiceDefinitionParseException("Invalid constant", ParseInfo);
				}				
				i++;
			}

			if (i != n) throw InternalErrorException("Internal error");
			return o;
		}

		std::string ValueToString();
		
		std::vector<ConstantDefinition_StructField> ValueToStructFields();

		static std::string UnescapeString(boost::string_ref in);
		static std::string EscapeString(boost::string_ref in);				
	};

	class ROBOTRACONTEUR_CORE_API  EnumDefinition : public NamedTypeDefinition
	{
	public:
		virtual ~EnumDefinition();
		
		std::vector<EnumDefinitionValue> Values;

		RR_WEAK_PTR<ServiceDefinition> service;		

		ServiceDefinitionParseInfo ParseInfo;

		EnumDefinition(RR_SHARED_PTR<ServiceDefinition> service);		

		std::string ToString();
		void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);
		
		bool VerifyValues();

		void Reset();

		virtual DataTypes RRDataType();
		virtual std::string ResolveQualifiedName();
	};

	class ROBOTRACONTEUR_CORE_API  EnumDefinitionValue
	{
	public:
		EnumDefinitionValue();

		std::string Name;
		int32_t Value;
		bool ImplicitValue;
		bool HexValue;
	};
		
	ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def, std::vector<ServiceDefinitionParseException>& warnings);
	ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def);

	ROBOTRACONTEUR_CORE_API bool CompareServiceDefinitions(RR_SHARED_PTR<ServiceDefinition> def1, RR_SHARED_PTR<ServiceDefinition> def2);

	ROBOTRACONTEUR_CORE_API boost::tuple<boost::string_ref, boost::string_ref> SplitQualifiedName(boost::string_ref name);
	
	template <typename T>
	RR_SHARED_PTR<T> TryFindByName(std::vector<RR_SHARED_PTR<T> >& v, boost::string_ref name)
	{
		for (typename std::vector<RR_SHARED_PTR<T> >::iterator e = v.begin(); e != v.end(); ++e)
		{
			if (*e)
			{
				if ((*e)->Name == name)
				{
					return *e;
				}
			}
		}

		return RR_SHARED_PTR<T>();
	}

	ROBOTRACONTEUR_CORE_API size_t EstimatePodPackedElementSize(RR_SHARED_PTR<ServiceEntryDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs = std::vector<RR_SHARED_PTR<ServiceDefinition> >(), RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>(), RR_SHARED_PTR<RRObject> client = RR_SHARED_PTR<RRObject>());

	ROBOTRACONTEUR_CORE_API boost::tuple<DataTypes, size_t> GetNamedArrayElementTypeAndCount(RR_SHARED_PTR<ServiceEntryDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs = std::vector<RR_SHARED_PTR<ServiceDefinition> >(), RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>(), RR_SHARED_PTR<RRObject> client = RR_SHARED_PTR<RRObject>());
		
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
	using ServiceDefinitionPtr = RR_SHARED_PTR<ServiceDefinition>;
	using NamedTypeDefinitionPtr = RR_SHARED_PTR<NamedTypeDefinition>;
	using ServiceEntryDefinitionPtr = RR_SHARED_PTR<ServiceEntryDefinition>;
	using MemberDefinitionPtr = RR_SHARED_PTR<MemberDefinition>;
	using FunctionDefinitionPtr = RR_SHARED_PTR<FunctionDefinition>;
	using PropertyDefinitionPtr = RR_SHARED_PTR<PropertyDefinition>;
	using EventDefinitionPtr = RR_SHARED_PTR<EventDefinition>;
	using ObjRefDefinitionPtr = RR_SHARED_PTR<ObjRefDefinition>;
	using PipeDefinitionPtr = RR_SHARED_PTR<PipeDefinition>;
	using CallbackDefinitionPtr = RR_SHARED_PTR<CallbackDefinition>;
	using WireDefinitionPtr = RR_SHARED_PTR<WireDefinition>;
	using MemoryDefinitionPtr = RR_SHARED_PTR<MemoryDefinition>;
	using TypeDefinitionPtr = RR_SHARED_PTR<TypeDefinition>;
	using UsingDefinitionPtr = RR_SHARED_PTR<UsingDefinition>;
	using EnumDefinitionPtr = RR_SHARED_PTR<EnumDefinition>;
	using ConstantDefinitionPtr = RR_SHARED_PTR<ConstantDefinition>;
#endif

}
