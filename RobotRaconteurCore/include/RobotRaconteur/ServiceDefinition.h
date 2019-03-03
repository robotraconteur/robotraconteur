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

	class ROBOTRACONTEUR_CORE_API  RobotRaconteurParseException;
	
	class ROBOTRACONTEUR_CORE_API RobotRaconteurVersion
	{
	public:
		RobotRaconteurVersion();
		RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch=0, uint32_t tweak=0);
		RobotRaconteurVersion(const std::string& v);

		std::string ToString() const;
		void FromString(const std::string& v);

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

		virtual ~ServiceDefinition() {}

		virtual std::string ToString();

		void FromString(const std::string &s);
		void FromString(const std::string &s, std::vector<RobotRaconteurParseException>& warnings);

		virtual void ToStream(std::ostream& os);
		void FromStream(std::istream& is);
		void FromStream(std::istream& is, std::vector<RobotRaconteurParseException>& warnings);

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

		ServiceEntryDefinition(RR_SHARED_PTR<ServiceDefinition> def);

		virtual ~ServiceEntryDefinition() {}

		virtual std::string ToString();
		virtual void ToStream(std::ostream& os);

		void FromString(const std::string &s);
		void FromString(const std::string &s, size_t startline);
		void FromString(const std::string &s, size_t startline, std::vector<RobotRaconteurParseException>& warnings);

		void FromStream(std::istream &is);
		void FromStream(std::istream &is, size_t startline);
		void FromStream(std::istream &is, size_t startline, std::vector<RobotRaconteurParseException>& warnings);

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

		MemberDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

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

		void FromString(const std::string &s);

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

		void FromString(const std::string &s);

		virtual void Reset();

		bool IsGenerator();
		
	};

	class ROBOTRACONTEUR_CORE_API  EventDefinition : public MemberDefinition
	{
	public:
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		EventDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);
			
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


		void FromString(const std::string &s);

		virtual void Reset();
	};

	class ROBOTRACONTEUR_CORE_API  PipeDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		PipeDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

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

		void FromString(const std::string &s);

		virtual void Reset();

	};

	class ROBOTRACONTEUR_CORE_API  WireDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		WireDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

		virtual void Reset();

		MemberDefinition_Direction Direction();

	};

	class ROBOTRACONTEUR_CORE_API  MemoryDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		MemoryDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

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

		TypeDefinition();
		TypeDefinition(RR_SHARED_PTR<MemberDefinition> member);

		virtual std::string ToString();

		void FromString(const std::string &s);

		static DataTypes DataTypeFromString(const std::string &d);

		static std::string StringFromDataType(DataTypes d);
			
		void Reset();

		void CopyTo(TypeDefinition& def) const;
		RR_SHARED_PTR<TypeDefinition> Clone() const;

		void Rename(const std::string& name);
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

		UsingDefinition(RR_SHARED_PTR<ServiceDefinition> service);

		std::string ToString();
		void FromString(const std::string& s);

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

		ConstantDefinition(RR_SHARED_PTR<ServiceDefinition> service);		
		ConstantDefinition(RR_SHARED_PTR<ServiceEntryDefinition> service_entry);
		
		std::string ToString();
		void FromString(const std::string& s);

		void Reset();

		static bool VerifyTypeAndValue(TypeDefinition& t, boost::iterator_range<std::string::const_iterator> value);
		bool VerifyValue();

		template<typename T>
		T ValueToScalar()
		{
			return boost::lexical_cast<T>(boost::trim_copy(Value));
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

			//Count number of elements
			typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
			for (string_split_iterator e = boost::make_split_iterator(value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
				e != string_split_iterator(); e++)
			{
				(*o)[i] = boost::lexical_cast<T>(boost::trim_copy(*e));
				i++;
			}

			if (i != n) throw InternalErrorException("Internal error");
			return o;
		}

		std::string ValueToString();
		
		std::vector<ConstantDefinition_StructField> ValueToStructFields();

		static std::string UnescapeString(const std::string& in);
		static std::string EscapeString(const std::string& in);

	};

	class ROBOTRACONTEUR_CORE_API  EnumDefinition : public NamedTypeDefinition
	{
	public:
		virtual ~EnumDefinition();
		
		std::vector<EnumDefinitionValue> Values;

		RR_WEAK_PTR<ServiceDefinition> service;		

		EnumDefinition(RR_SHARED_PTR<ServiceDefinition> service);		

		std::string ToString();
		void FromString(const std::string& s);
		void FromString(const std::string& s, int32_t startline);

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

	class ROBOTRACONTEUR_CORE_API  RobotRaconteurParseException : public std::runtime_error
	{

	public:
		int32_t LineNumber;

		std::string Message;

		RobotRaconteurParseException(const std::string &e);
		RobotRaconteurParseException(const std::string &e, int32_t line);

		virtual std::string ToString();

		virtual const char* what() const throw ();

		~RobotRaconteurParseException() throw () {}

	private:
		std::string what_store;

	};

	ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def, std::vector<RobotRaconteurParseException>& warnings);
	ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def);

	ROBOTRACONTEUR_CORE_API bool CompareServiceDefinitions(RR_SHARED_PTR<ServiceDefinition> def1, RR_SHARED_PTR<ServiceDefinition> def2);

	ROBOTRACONTEUR_CORE_API boost::tuple<std::string, std::string> SplitQualifiedName(const std::string& name);
	
	template <typename T>
	RR_SHARED_PTR<T> TryFindByName(std::vector<RR_SHARED_PTR<T> >& v, const std::string& name)
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
	
}
