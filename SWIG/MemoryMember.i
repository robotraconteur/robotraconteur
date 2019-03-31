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

%shared_ptr(RobotRaconteur::WrappedPodArrayMemoryClient);
%shared_ptr(RobotRaconteur::WrappedPodMultiDimArrayMemoryClient);
%shared_ptr(RobotRaconteur::WrappedNamedArrayMemoryClient);
%shared_ptr(RobotRaconteur::WrappedNamedMultiDimArrayMemoryClient);

%feature("director") RobotRaconteur::WrappedPodArrayMemoryClientBuffer;
%feature("director") RobotRaconteur::WrappedPodMultiDimArrayMemoryClientBuffer;
%feature("director") RobotRaconteur::WrappedNamedArrayMemoryClientBuffer;
%feature("director") RobotRaconteur::WrappedNamedMultiDimArrayMemoryClientBuffer;

%feature("director") RobotRaconteur::WrappedArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedMultiDimArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedPodArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedPodMultiDimArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedNamedArrayMemoryDirector;
%feature("director") RobotRaconteur::WrappedNamedMultiDimArrayMemoryDirector;

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
	virtual DataTypes ElementTypeID()=0;
RR_KEEP_GIL()
};



class WrappedArrayMemoryClientUtil
{
RR_RELEASE_GIL()
public:
	static boost::intrusive_ptr<RRBaseArray> Read(boost::shared_ptr<ArrayMemoryBase> mem, uint64_t memorypos, uint64_t count);

	static void Write(boost::shared_ptr<ArrayMemoryBase> mem, uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);

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

class WrappedPodArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::intrusive_ptr<RobotRaconteur::MessageElementPodArray> res, uint64_t bufferpos, uint64_t count) = 0;
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElementPodArray> PackWriteRequest(uint64_t bufferpos, uint64_t count) = 0;
	virtual uint64_t GetBufferLength() = 0;
	virtual ~WrappedPodArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedPodArrayMemoryClient;
class WrappedPodArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual uint64_t Length();
	MemberDefinition_Direction Direction();
	virtual void Read(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, WrappedPodArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
RR_KEEP_GIL()
};

class WrappedPodMultiDimArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::intrusive_ptr<RobotRaconteur::MessageElementPodMultiDimArray> res, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElementPodMultiDimArray> PackWriteRequest(const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual ~WrappedPodMultiDimArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedPodMultiDimArrayMemoryClient;
class WrappedPodMultiDimArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual std::vector<uint64_t> Dimensions() ;
	virtual uint64_t DimCount();
	MemberDefinition_Direction Direction();
	virtual void Read(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, WrappedPodMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
RR_KEEP_GIL()
};

class WrappedArrayMemoryDirector
{
public:
	virtual ~WrappedArrayMemoryDirector() {}
	virtual uint64_t Length();
	virtual void Read(uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, boost::intrusive_ptr<RRBaseArray> buffer, uint64_t bufferpos, uint64_t count);
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
	virtual void Read(WrappedMultiDimArrayMemoryParams* p);
	virtual void Write(WrappedMultiDimArrayMemoryParams* p);
	int32_t objectheapid;
};

// pod

class WrappedPodArrayMemoryDirector
{
public:
	WrappedPodArrayMemoryDirector();
	virtual ~WrappedPodArrayMemoryDirector();
	virtual uint64_t Length();
	virtual boost::intrusive_ptr<MessageElementPodArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, boost::intrusive_ptr<MessageElementPodArray> buffer, uint64_t bufferpos, uint64_t count);
	int32_t objectheapid;
};

class WrappedPodMultiDimArrayMemoryDirector
{
public:		
	virtual ~WrappedPodMultiDimArrayMemoryDirector() {}
	virtual std::vector<uint64_t> Dimensions();
	virtual uint64_t DimCount();
	virtual boost::intrusive_ptr<MessageElementPodMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, boost::intrusive_ptr<MessageElementPodMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

	int32_t objectheapid;
};

// namedarray

class WrappedNamedArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::intrusive_ptr<RobotRaconteur::MessageElementNamedArray> res, uint64_t bufferpos, uint64_t count) = 0;
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElementNamedArray> PackWriteRequest(uint64_t bufferpos, uint64_t count) = 0;
	virtual uint64_t GetBufferLength() = 0;
	virtual ~WrappedNamedArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedNamedArrayMemoryClient;
class WrappedNamedArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual uint64_t Length();
	MemberDefinition_Direction Direction();
	virtual void Read(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, WrappedNamedArrayMemoryClientBuffer* buffer, uint64_t bufferpos, uint64_t count);
RR_KEEP_GIL()
};

class WrappedNamedMultiDimArrayMemoryClientBuffer
{
public:
	virtual void UnpackReadResult(boost::intrusive_ptr<RobotRaconteur::MessageElementNamedMultiDimArray> res, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual boost::intrusive_ptr<RobotRaconteur::MessageElementNamedMultiDimArray> PackWriteRequest(const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) = 0;
	virtual ~WrappedNamedMultiDimArrayMemoryClientBuffer() {}
};

%nodefaultctor WrappedNamedMultiDimArrayMemoryClient;
class WrappedNamedMultiDimArrayMemoryClient
{
RR_RELEASE_GIL()
public:
	virtual std::vector<uint64_t> Dimensions() ;
	virtual uint64_t DimCount();
	MemberDefinition_Direction Direction();
	virtual void Read(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, WrappedNamedMultiDimArrayMemoryClientBuffer* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
RR_KEEP_GIL()
};

class WrappedNamedArrayMemoryDirector
{
public:
	WrappedNamedArrayMemoryDirector();
	virtual ~WrappedNamedArrayMemoryDirector();
	virtual uint64_t Length();
	virtual boost::intrusive_ptr<MessageElementNamedArray> Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count);
	virtual void Write(uint64_t memorypos, boost::intrusive_ptr<MessageElementNamedArray> buffer, uint64_t bufferpos, uint64_t count);
	int32_t objectheapid;
};

class WrappedNamedMultiDimArrayMemoryDirector
{
public:		
	virtual ~WrappedNamedMultiDimArrayMemoryDirector() {}
	virtual std::vector<uint64_t> Dimensions();
	virtual uint64_t DimCount();
	virtual boost::intrusive_ptr<MessageElementNamedMultiDimArray> Read(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
	virtual void Write(const std::vector<uint64_t>& memorypos, boost::intrusive_ptr<MessageElementNamedMultiDimArray> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

	int32_t objectheapid;
};



}
