//NodeID
namespace RobotRaconteur
{ 

%typemap("cscode") NodeID
%{

public static bool operator == (NodeID a, NodeID b)
{
    if (Object.Equals(a,  null)  && Object.Equals(b,  null) ) return true;
    if (Object.Equals(a,  null)  || Object.Equals(b,  null) ) return false;
    return a.eq(b);
}

public override bool Equals(Object o)
{
    if (!(o is NodeID)) return false;
    return this == ((NodeID)o);
}

public override int GetHashCode()
{
	int o=0;
	byte[] b=ToByteArray();
	for (int i=0; i<b.Length; i++) o+=b[i];
    return o;
}

public static bool operator != (NodeID a, NodeID b)
{
    return !(a==b);
}

public byte[] ToByteArray()
{
	byte[] o=new byte[16];
	this.ToByteArrayC(o);
	return o;

}

public NodeID(byte[] bytes) : this(bytes,bytes.Length) {}

%}

class NodeID 
{
public:
	NodeID();
	%csmethodmodifiers ToString "public override";
	virtual std::string ToString() const;
	static NodeID NewUniqueID();
    
    %csmethodmodifiers operator== "public virtual";
    %rename(eq) operator==;
	bool operator== (const NodeID &id2) const;
    
    %rename(ne) operator!=;
	bool operator != (const NodeID &id2) const;
	%rename(lt) operator<;
	bool operator <(const NodeID& id2) const;
	
	bool IsAnyNode();
	static NodeID GetAny();

	
	//NodeID(std::boost<uint8_t,16> id);
	NodeID(const std::string& id);

	
	
	%rename(ToByteArrayC) ToByteArray;
	%csmethodmodifiers ToByteArray "private";
	//std::vector<uint8_t> ToByteArray();
	
	
	


};

%extend NodeID {
	%csmethodmodifiers NodeID (const uint8_t bytes[16], int32_t bytes_len) "private";
	%apply unsigned char INPUT[] {const uint8_t bytes[16]}
	NodeID (const uint8_t bytes[16], int32_t bytes_len)
	{
		if (bytes_len!=16) throw InvalidArgumentException("Invalid NodeID bytes length");
	
		boost::array<uint8_t,16> b = {};
		memcpy(&b[0],bytes,16);
	
		NodeID* n=new NodeID(b);
		return n;	
	}
	
	%apply unsigned char OUTPUT[] {uint8_t bytes[16]}
	void ToByteArray(uint8_t bytes[16])
	{
		boost::array<uint8_t,16> b=$self->ToByteArray();
		memcpy(bytes,&b[0],16);	
	}
	

}

}