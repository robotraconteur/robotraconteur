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

//Memory

%shared_ptr(RobotRaconteur::ArrayMemoryBase);
%shared_ptr(RobotRaconteur::MultiDimArrayMemoryBase);

%shared_ptr(RobotRaconteur::WrappedCStructureArrayMemoryClient);
%shared_ptr(RobotRaconteur::WrappedCStructureMultiDimArrayMemoryClient);
%shared_ptr(RobotRaconteur::WrappedAStructureArrayMemoryClient);
%shared_ptr(RobotRaconteur::WrappedAStructureMultiDimArrayMemoryClient);

%feature("director") RobotRaconteur::WrappedCStructureArrayMemoryClientBuffer;
%feature("director") RobotRaconteur::WrappedCStructureMultiDimArrayMemoryClientBuffer;
%feature("director") RobotRaconteur::WrappedAStructureArrayMemoryClientBuffer;
%feature("director") RobotRaconteur::WrappedAStructureMultiDimArrayMemoryClientBuffer;

%feature("director") RobotRaconteur::WrappedArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedMultiDimArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedCStructureArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedCStructureMultiDimArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedAStructureArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedAStructureMultiDimArrayMemoryDirector;

namespace RobotRaconteur
{

class ArrayMemoryBase
{
RR_RELEASE_GIL()
public:
	virtual uint64_t Length()=0;
	virtual DataTypes ElementTypeID()=0;
RR_KEEP_GIL()
};

class MultiDimArrayMemoryBase
{
RR_RELEASE_GIL()
public:
	virtual std::vector<uint64_t> Dimensions()=0;
	virtual uint64_t DimCount()=0;
	virtual bool Complex()=0;
	virtual DataTypes ElementTypeID()=0;
RR_KEEP_GIL()
};



class WrappedArrayMemoryClientUtil
{
RR_RELEASE_GIL()
public:
	static boost::shared_ptr<RRBaseArray> Read(boost::shared_ptr<ArrayMemoryBase> mem, uint64_t memorypos, uint64_t count);

	static void Write(boost::shared_ptr<ArrayMemoryBase> mem, uint64_t memorypos, boost::shared_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);

	static MemberDefinition_Direction Direction(boost::shared_ptr<ArrayMemoryBase> mem);
RR_KEEP_GIL()
};

class WrappedMultiDimArrayMemoryClientUtil
{
RR_RELEASE_GIL()
public:
	static boost::shared_ptr<RobotRaconteur::RRMultiDimArrayUntyped> Read(boost::shared_ptr<MultiDimArrayMemoryBase> mem, std::vector<uint64_t> memorypos, std::vector<uint64_t> count);

	static void Write(boost::shared_ptr<MultiDimArrayMemoryBase> mem, std::vector<uint64_t> memorypos, boost::shared_ptr<RobotRaconteur::RRMultiDimArrayUntyped> buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count);

	static MemberDefinition_Direction Direction(boost::shared_ptr<MultiDimArrayMemoryBase> mem);

RR_KEEP_GIL()
};

class WrappedCStructureArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::shared_ptr<RobotRaconteur::MessageElementCStructureArray> res, uint64_t bufferpos, uint64_t count) = 0;
	virtual boost::shared_ptr<RobotRaconteur::MessageElementCStructureArray> PackWriteRequest(uint64_t bufferpos, uint64_t count) = 0;
	virtual uint64_t GetBufferLength() = 0;
	virtual ~WrappedCStructureArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedCStructureArrayMemoryClient;
class WrappedCStructureArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual uint64_t Length();
	MemberDefinition_Direction Direction();
	virtual void Read(uint64_t memorypos, WrappedCStructureArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, WrappedCStructureArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
RR_KEEP_GIL()
};

class WrappedCStructureMultiDimArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::shared_ptr<RobotRaconteur::MessageElementCStructureMultiDimArray> res, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual boost::shared_ptr<RobotRaconteur::MessageElementCStructureMultiDimArray> PackWriteRequest(const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual ~WrappedCStructureMultiDimArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedCStructureMultiDimArrayMemoryClient;
class WrappedCStructureMultiDimArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual std::vector<uint64_t> Dimensions() ;
	virtual uint64_t DimCount();
	MemberDefinition_Direction Direction();
	virtual void Read(const std::vector<uint64_t>& memorypos, WrappedCStructureMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, WrappedCStructureMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
RR_KEEP_GIL()
};

class WrappedArrayMemoryDirector
{
public:
	virtual ~WrappedArrayMemoryDirector() {}
	virtual uint64_t Length();
	virtual void Read(uint64_t memorypos, boost::shared_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, boost::shared_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);
	int32_t objectheapid;
};

class WrappedMultiDimArrayMemoryParams
{
public:
	std::vector<uint64_t> memorypos;
	boost::shared_ptr<RRMultiDimArrayUntyped> buffer;
	std::vector<uint64_t> bufferpos;
	std::vector<uint64_t> count;
};

class WrappedMultiDimArrayMemoryDirector
{
public:
	virtual ~WrappedMultiDimArrayMemoryDirector() {}
	virtual std::vector<uint64_t> Dimensions();
	virtual uint64_t DimCount();
	virtual bool Complex();
	virtual void Read(WrappedMultiDimArrayMemoryParams* p);
	virtual void Write(WrappedMultiDimArrayMemoryParams* p);
	int32_t objectheapid;
};

// cstruct

class WrappedCStructureArrayMemoryDirector
{
public:
	WrappedCStructureArrayMemoryDirector();
	virtual ~WrappedCStructureArrayMemoryDirector();
	virtual uint64_t Length();
	virtual boost::shared_ptr<MessageElementCStructureArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, boost::shared_ptr<MessageElementCStructureArray> buffer, uint64_t bufferpos, uint64_t count);
	int32_t objectheapid;
};

class WrappedCStructureMultiDimArrayMemoryDirector
{
public:		
	virtual ~WrappedCStructureMultiDimArrayMemoryDirector() {}
	virtual std::vector<uint64_t> Dimensions();
	virtual uint64_t DimCount();
	virtual boost::shared_ptr<MessageElementCStructureMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, boost::shared_ptr<MessageElementCStructureMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

	int32_t objectheapid;
};

// astruct

class WrappedAStructureArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::shared_ptr<RobotRaconteur::MessageElementAStructureArray> res, uint64_t bufferpos, uint64_t count) = 0;
	virtual boost::shared_ptr<RobotRaconteur::MessageElementAStructureArray> PackWriteRequest(uint64_t bufferpos, uint64_t count) = 0;
	virtual uint64_t GetBufferLength() = 0;
	virtual ~WrappedAStructureArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedAStructureArrayMemoryClient;
class WrappedAStructureArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual uint64_t Length();
	MemberDefinition_Direction Direction();
	virtual void Read(uint64_t memorypos, WrappedAStructureArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, WrappedAStructureArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
RR_KEEP_GIL()
};

class WrappedAStructureMultiDimArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::shared_ptr<RobotRaconteur::MessageElementAStructureMultiDimArray> res, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual boost::shared_ptr<RobotRaconteur::MessageElementAStructureMultiDimArray> PackWriteRequest(const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual ~WrappedAStructureMultiDimArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedAStructureMultiDimArrayMemoryClient;
class WrappedAStructureMultiDimArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual std::vector<uint64_t> Dimensions() ;
	virtual uint64_t DimCount();
	MemberDefinition_Direction Direction();
	virtual void Read(const std::vector<uint64_t>& memorypos, WrappedAStructureMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, WrappedAStructureMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
RR_KEEP_GIL()
};

class WrappedAStructureArrayMemoryDirector
{
public:
	WrappedAStructureArrayMemoryDirector();
	virtual ~WrappedAStructureArrayMemoryDirector();
	virtual uint64_t Length();
	virtual boost::shared_ptr<MessageElementAStructureArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, boost::shared_ptr<MessageElementAStructureArray> buffer, uint64_t bufferpos, uint64_t count);
	int32_t objectheapid;
};

class WrappedAStructureMultiDimArrayMemoryDirector
{
public:		
	virtual ~WrappedAStructureMultiDimArrayMemoryDirector() {}
	virtual std::vector<uint64_t> Dimensions();
	virtual uint64_t DimCount();
	virtual boost::shared_ptr<MessageElementAStructureMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, boost::shared_ptr<MessageElementAStructureMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

	int32_t objectheapid;
};



}
