//NodeID
namespace RobotRaconteur
{ 

%typemap("javacode") NodeID
%{

/*public byte[] toByteArray()
{
vector_uint8_t o1=ToByteArrayC();
byte[] o2=new byte[(int)o1.size()];
for (int i=0; i<o1.size(); i++)
{
	ByteBuffer b=ByteBuffer.allocate(2);
	b.putShort(o1.get(i));
	
	o2[i]=b.array()[1];
}
return o2;
}

public NodeID(byte[] id)
{
	vector_uint8_t id2=new vector_uint8_t(16);
	for (int i=0; i<id.length; i++)
	{
		ByteBuffer bb=ByteBuffer.allocate(2);
		bb.put((byte)0);
		bb.put(id[i]);
		bb.rewind();
		id2.set(i, bb.getShort());
	}
	
	swigCMemOwn=true;
	swigCPtr=RobotRaconteurJavaJNI.new_NodeID__SWIG_1(vector_uint8_t.getCPtr(id2),id2);
		
}*/


%}

%typemap(javaimports) NodeID 
%{
import java.nio.*;
%}

class NodeID 
{
public:
	NodeID();
	%javamethodmodifiers ToString "public ";
	virtual std::string ToString() const;
	static NodeID NewUniqueID();
    
    %rename(equals) operator==;
	bool operator == (const NodeID &id2) const;
    %rename(ne) operator!=;
	bool operator != (const NodeID &id2) const;
	%rename(lt) operator<;
	bool operator <(const NodeID& id2) const;
	
	bool IsAnyNode();
	static NodeID GetAny();

	%javamethodmodifiers NodeID(std::vector<uint8_t> id) "private";
	//NodeID(std::vector<uint8_t> id);
	NodeID(const std::string& id);

	
	
	%rename(ToByteArrayC) ToByteArray;
	%javamethodmodifiers ToByteArray "private";
	//std::vector<uint8_t> ToByteArray();
	
};

}