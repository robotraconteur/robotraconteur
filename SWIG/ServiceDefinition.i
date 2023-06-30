// Copyright 2011-2020 Wason Technology, LLC
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

%shared_ptr(RobotRaconteur::ServiceDefinition)
%shared_ptr(RobotRaconteur::ServiceEntryDefinition)
%shared_ptr(RobotRaconteur::MemberDefinition)
%shared_ptr(RobotRaconteur::PropertyDefinition)
%shared_ptr(RobotRaconteur::FunctionDefinition)
%shared_ptr(RobotRaconteur::EventDefinition)
%shared_ptr(RobotRaconteur::ObjRefDefinition)
%shared_ptr(RobotRaconteur::PipeDefinition)
%shared_ptr(RobotRaconteur::CallbackDefinition)
%shared_ptr(RobotRaconteur::WireDefinition)
%shared_ptr(RobotRaconteur::MemoryDefinition)
%shared_ptr(RobotRaconteur::TypeDefinition)
%shared_ptr(RobotRaconteur::UsingDefinition)
%shared_ptr(RobotRaconteur::ConstantDefinition)
%shared_ptr(RobotRaconteur::EnumDefinition)
%shared_ptr(RobotRaconteur::ExceptionDefinition)


%template(vectorptr_typedefinition) std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> >;
%template(vectorptr_memberdefinition) std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >;
%template(vectorptr_serviceentrydefinition) std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> >;
%template(vectorptr_constantdefinition) std::vector<boost::shared_ptr<RobotRaconteur::ConstantDefinition> >;
%template(vectorptr_enumdefinition) std::vector<boost::shared_ptr<RobotRaconteur::EnumDefinition> >;
%template(vector_constantdefinition_structfield) std::vector<RobotRaconteur::ConstantDefinition_StructField>;
%template(vector_enumdefinitionvalues) std::vector<RobotRaconteur::EnumDefinitionValue>;
%template(vector_usingdefinition) std::vector<boost::shared_ptr<RobotRaconteur::UsingDefinition> >;
%template(vectorptr_exceptiondefinition) std::vector<boost::shared_ptr<RobotRaconteur::ExceptionDefinition> >;
%template(vectorptr_servicedefinition) std::vector<boost::shared_ptr<RobotRaconteur::ServiceDefinition> >;

namespace RobotRaconteur
{

struct ServiceDefinitionParseInfo
{
	std::string ServiceName;
	std::string RobDefFilePath;
	std::string Line;
	int32_t LineNumber;

	ServiceDefinitionParseInfo();
	void Reset();
};

class RobotRaconteurVersion
{
public:
	RobotRaconteurVersion();
	RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch=0, uint32_t tweak=0);
	RobotRaconteurVersion(const std::string& v);

	std::string ToString() const;
	void FromString(const std::string& v, const ServiceDefinitionParseInfo* parse_info = NULL);
		
	operator bool() const;

	uint32_t major;
	uint32_t minor;
	uint32_t patch;
	uint32_t tweak;

	ServiceDefinitionParseInfo ParseInfo;
};
}
%rename(RobotRaconteurVersion_eq) operator == (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
bool operator== (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
%rename(RobotRaconteurVersion_ne)  operator != (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
bool operator!= (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
%rename(RobotRaconteurVersion_gt)  operator > (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
bool operator> (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
%rename(RobotRaconteurVersion_ge)  operator >= (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
bool operator>= (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
%rename(RobotRaconteurVersion_lt) operator < (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
bool operator< (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
%rename(RobotRaconteurVersion_le)  operator <= (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);
bool operator<= (const RobotRaconteur::RobotRaconteurVersion &v1, const RobotRaconteur::RobotRaconteurVersion &v2);

namespace RobotRaconteur
{
class ServiceDefinition
{
public:
	std::string Name;
	std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> > NamedArrays;
	std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> > Pods;
	std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> > Structures;
	std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> > Objects;
	std::vector<std::string> Options;
	std::vector<std::string> Imports;
	std::vector<boost::shared_ptr<RobotRaconteur::ExceptionDefinition> > Exceptions;
	std::vector<boost::shared_ptr<RobotRaconteur::UsingDefinition> > Using;
	std::vector<boost::shared_ptr<RobotRaconteur::ConstantDefinition> > Constants;
	std::vector<boost::shared_ptr<RobotRaconteur::EnumDefinition> > Enums;
	RobotRaconteurVersion StdVer;
	ServiceDefinitionParseInfo ParseInfo;
	std::string DocString;
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
	ServiceDefinition();
	
	void Reset();
};



class ServiceEntryDefinition
{
public:
	std::string Name;
	std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> > Members;
	DataTypes EntryType;
	std::vector<std::string> Implements;
	std::vector<std::string> Options;
	std::vector<boost::shared_ptr<RobotRaconteur::ConstantDefinition> > Constants;
	ServiceDefinitionParseInfo ParseInfo;
	std::string DocString;
	ServiceEntryDefinition(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
	
	
	
	%extend	{
	boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetServiceDefinition()
	{
		return $self->ServiceDefinition_.lock();
	}
	
	void SetServiceDefinition(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& value)
	{
		$self->ServiceDefinition_=value;
	}
	
	}
	
	void Reset();
	
	
};




class MemberDefinition
{
public:
	std::string Name;
	std::string DocString;
	
	MemberDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);

	virtual MemberDefinition_NoLock NoLock();

    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString() { return ""; }
	
	%extend	{
	boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> GetServiceEntry()
	{
		return $self->ServiceEntry.lock();
	}
	
	void SetServiceEntry(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& value)
	{
		$self->ServiceEntry=value;
	}
	
	}
	
	std::vector<std::string> Modifiers;
	
	ServiceDefinitionParseInfo ParseInfo;

	void Reset();

};



class PropertyDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	PropertyDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
    RR_PUBLIC_VIRTUAL_METHOD(ToString)
	std::string ToString(bool isstruct);
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

class FunctionDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> ReturnType;
	std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> > Parameters;
	FunctionDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
	bool IsGenerator();
};

class EventDefinition : public MemberDefinition
{
public:
	std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> > Parameters;
	EventDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

class ObjRefDefinition : public MemberDefinition
{
public:
	std::string ObjectType;
	
	DataTypes_ArrayTypes ArrayType;
	DataTypes_ContainerTypes ContainerType;
	
	ObjRefDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

class PipeDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	PipeDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

class CallbackDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> ReturnType;
	std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> > Parameters;
	CallbackDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

class WireDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	WireDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

class MemoryDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	MemoryDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

%copyctor TypeDefinition;
class TypeDefinition
{
public:
	std::string Name;
	DataTypes Type;
	std::string TypeString;
	DataTypes_ArrayTypes ArrayType;
	bool ArrayVarLength;
	std::vector<int32_t> ArrayLength;
	DataTypes_ContainerTypes ContainerType;	
	
	TypeDefinition();
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s, const ServiceDefinitionParseInfo* parse_info = NULL);
	static DataTypes DataTypeFromString(const std::string &d);
	static std::string StringFromDataType(DataTypes d);
	    
    RR_PROPERTY(Member)
	%extend	{
	boost::shared_ptr<RobotRaconteur::MemberDefinition> GetMember()
	{
		return $self->member.lock();
	}
	
	void SetMember(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& value)
	{
		$self->member=value;
	}	
	
	}
	
	void CopyTo(TypeDefinition& def) const;
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Clone() const;

	void Rename(const std::string& name);
	void RemoveContainers();
	void RemoveArray();
	void QualifyTypeStringWithUsing();
	void UnqualifyTypeStringWithUsing();

	void Reset();
};

class  UsingDefinition
	{
	public:
		virtual ~UsingDefinition();

		std::string QualifiedName;
		std::string UnqualifiedName;

		ServiceDefinitionParseInfo ParseInfo;

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& value)
		{
			$self->service=value;
		}	
		}

		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		UsingDefinition(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& service);

		std::string ToString();
		void FromString(const std::string& s, const ServiceDefinitionParseInfo* parse_info = NULL);
	};

	struct ConstantDefinition_StructField
	{
		std::string Name;
		std::string ConstantRefName;
	};

	class ConstantDefinition
	{
	public:
		virtual ~ConstantDefinition();

		std::string Name;
		std::string DocString;

		boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;

		std::string Value;

		ServiceDefinitionParseInfo ParseInfo;

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& value)
		{
			$self->service=value;
		}	
		}

		RR_PROPERTY(ServiceEntry)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> GetServiceEntry()
		{
			return $self->service_entry.lock();
		}
	
		void SetServiceEntry(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& value)
		{
			$self->service_entry=value;
		}	
		}

		ConstantDefinition(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& service);		
		ConstantDefinition(const boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition>& service_entry);
		
		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		std::string ToString();
		void FromString(const std::string& s, const ServiceDefinitionParseInfo* parse_info = NULL);

		void Reset();
		
		bool VerifyValue();

		std::string ValueToString();
				
		std::vector<ConstantDefinition_StructField> ValueToStructFields();

		static std::string UnescapeString(const std::string& in);
		static std::string EscapeString(const std::string& in);

	};
		
	class  EnumDefinition
	{
	public:
		virtual ~EnumDefinition();

		std::string Name;
		std::string DocString;
		std::vector<RobotRaconteur::EnumDefinitionValue> Values;

		ServiceDefinitionParseInfo ParseInfo;

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& value)
		{
			$self->service=value;
		}	
		}		

		EnumDefinition(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& service);		

		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		std::string ToString();		
		void FromString(const std::string& s, const ServiceDefinitionParseInfo* parse_info = NULL);

		bool VerifyValues();

		void Reset();
	};

	class EnumDefinitionValue
	{
	public:
		EnumDefinitionValue();

		std::string Name;
		std::string DocString;
		int32_t Value;
		bool ImplicitValue;
		bool HexValue;
	};

	class  ExceptionDefinition
	{
	public:
		virtual ~ExceptionDefinition();

		std::string Name;
		std::string DocString;		

		ServiceDefinitionParseInfo ParseInfo;

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& value)
		{
			$self->service=value;
		}	
		}		

		ExceptionDefinition(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& service);		

		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		std::string ToString();		
		void FromString(const std::string& s, const ServiceDefinitionParseInfo* parse_info = NULL);

		void Reset();
	};
}

%inline %{

class MemberDefinitionUtil
{
public:
static boost::shared_ptr<RobotRaconteur::PropertyDefinition> ToProperty(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::PropertyDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::FunctionDefinition> ToFunction(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::FunctionDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::ObjRefDefinition> ToObjRef(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::ObjRefDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::EventDefinition> ToEvent(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::EventDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::PipeDefinition> ToPipe(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::PipeDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::CallbackDefinition> ToCallback(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::CallbackDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::WireDefinition> ToWire(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::WireDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::MemoryDefinition> ToMemory(const boost::shared_ptr<RobotRaconteur::MemberDefinition>& t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::MemoryDefinition>(t);
}

};
%}