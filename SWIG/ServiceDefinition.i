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


%template(vectorptr_typedefinition) std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> >;
%template(vectorptr_memberdefinition) std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >;
%template(vectorptr_serviceentrydefinition) std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> >;
%template(vectorptr_constantdefinition) std::vector<boost::shared_ptr<RobotRaconteur::ConstantDefinition> >;
%template(vectorptr_enumdefinition) std::vector<boost::shared_ptr<RobotRaconteur::EnumDefinition> >;
%template(vector_constantdefinition_structfield) std::vector<RobotRaconteur::ConstantDefinition_StructField>;
%template(vector_enumdefinitionvalues) std::vector<RobotRaconteur::EnumDefinitionValue>;
%template(vector_usingdefinition) std::vector<boost::shared_ptr<RobotRaconteur::UsingDefinition> >;

namespace RobotRaconteur
{

class RobotRaconteurVersion
{
public:
	RobotRaconteurVersion();
	RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch=0, uint32_t tweak=0);
	RobotRaconteurVersion(const std::string& v);

	std::string ToString() const;
	void FromString(const std::string& v);
		
	operator bool() const;

	uint32_t major;
	uint32_t minor;
	uint32_t patch;
	uint32_t tweak;
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
	std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> > Structures;
	std::vector<boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> > Objects;
	std::vector<std::string> Options;
	std::vector<std::string> Imports;
	std::vector<std::string> Exceptions;
	std::vector<boost::shared_ptr<RobotRaconteur::UsingDefinition> > Using;
	std::vector<boost::shared_ptr<RobotRaconteur::ConstantDefinition> > Constants;
	std::vector<boost::shared_ptr<RobotRaconteur::EnumDefinition> > Enums;
	RobotRaconteurVersion StdVer;
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
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
	ServiceEntryDefinition(boost::shared_ptr<RobotRaconteur::ServiceDefinition> def);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
	void FromString(const std::string &s, int32_t startline);
	
	
	
	%extend	{
	boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetServiceDefinition()
	{
		return $self->ServiceDefinition_.lock();
	}
	
	void SetServiceDefinition(boost::shared_ptr<RobotRaconteur::ServiceDefinition> value)
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
	
	MemberDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString() { return ""; }
	
	%extend	{
	boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> GetServiceEntry()
	{
		return $self->ServiceEntry.lock();
	}
	
	void SetServiceEntry(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> value)
	{
		$self->ServiceEntry=value;
	}
	
	}
	
	std::vector<std::string> Modifiers;
	
	void Reset();

};



class PropertyDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	PropertyDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
    RR_PUBLIC_VIRTUAL_METHOD(ToString)
	std::string ToString(bool isstruct);
	void FromString(const std::string &s);
};

class FunctionDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> ReturnType;
	std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> > Parameters;
	FunctionDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
	bool IsGenerator();
};

class EventDefinition : public MemberDefinition
{
public:
	std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> > Parameters;
	EventDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
};

class ObjRefDefinition : public MemberDefinition
{
public:
	std::string ObjectType;
	
	DataTypes_ArrayTypes ArrayType;
	DataTypes_ContainerTypes ContainerType;
	
	ObjRefDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
};

class PipeDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	PipeDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
};

class CallbackDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> ReturnType;
	std::vector<boost::shared_ptr<RobotRaconteur::TypeDefinition> > Parameters;
	CallbackDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
};

class WireDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	WireDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
};

class MemoryDefinition : public MemberDefinition
{
public:
	boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;
	MemoryDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> ServiceEntry);
    RR_PUBLIC_OVERRIDE_METHOD(ToString)
	virtual std::string ToString();
	void FromString(const std::string &s);
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
	void FromString(const std::string &s);
	static DataTypes DataTypeFromString(const std::string &d);
	static std::string StringFromDataType(DataTypes d);
	    
    RR_PROPERTY(Member)
	%extend	{
	boost::shared_ptr<RobotRaconteur::MemberDefinition> GetMember()
	{
		return $self->member.lock();
	}
	
	void SetMember(boost::shared_ptr<RobotRaconteur::MemberDefinition> value)
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

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(boost::shared_ptr<RobotRaconteur::ServiceDefinition> value)
		{
			$self->service=value;
		}	
		}

		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		UsingDefinition(boost::shared_ptr<RobotRaconteur::ServiceDefinition> service);

		std::string ToString();
		void FromString(const std::string& s);
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

		boost::shared_ptr<RobotRaconteur::TypeDefinition> Type;

		std::string Value;

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(boost::shared_ptr<RobotRaconteur::ServiceDefinition> value)
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
	
		void SetServiceEntry(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> value)
		{
			$self->service_entry=value;
		}	
		}

		ConstantDefinition(boost::shared_ptr<RobotRaconteur::ServiceDefinition> service);		
		ConstantDefinition(boost::shared_ptr<RobotRaconteur::ServiceEntryDefinition> service_entry);
		
		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		std::string ToString();
		void FromString(const std::string& s);

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
		std::vector<RobotRaconteur::EnumDefinitionValue> Values;

		RR_PROPERTY(Service)
		%extend	{
		boost::shared_ptr<RobotRaconteur::ServiceDefinition> GetService()
		{
			return $self->service.lock();
		}
	
		void SetService(boost::shared_ptr<RobotRaconteur::ServiceDefinition> value)
		{
			$self->service=value;
		}	
		}		

		EnumDefinition(boost::shared_ptr<RobotRaconteur::ServiceDefinition> service);		

		RR_PUBLIC_OVERRIDE_METHOD(ToString)
		std::string ToString();
		void FromString(const std::string& s);
		void FromString(const std::string& s, int32_t startline);

		bool VerifyValues();

		void Reset();
	};

	class EnumDefinitionValue
	{
	public:
		EnumDefinitionValue();

		std::string Name;
		int32_t Value;
		bool ImplicitValue;
		bool HexValue;
	};
}

%inline %{

class MemberDefinitionUtil
{
public:
static boost::shared_ptr<RobotRaconteur::PropertyDefinition> ToProperty(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::PropertyDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::FunctionDefinition> ToFunction(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::FunctionDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::ObjRefDefinition> ToObjRef(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::ObjRefDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::EventDefinition> ToEvent(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::EventDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::PipeDefinition> ToPipe(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::PipeDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::CallbackDefinition> ToCallback(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::CallbackDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::WireDefinition> ToWire(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::WireDefinition>(t);
}

static boost::shared_ptr<RobotRaconteur::MemoryDefinition> ToMemory(boost::shared_ptr<RobotRaconteur::MemberDefinition> t) 
{ 
return boost::dynamic_pointer_cast<RobotRaconteur::MemoryDefinition>(t);
}

};
%}