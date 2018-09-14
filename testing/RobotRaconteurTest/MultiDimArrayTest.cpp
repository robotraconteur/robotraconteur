#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "MultiDimArrayTest.h"
#include <boost/detail/endian.hpp>

namespace RobotRaconteurTest
{

	template <class T>
	void ca(RR_SHARED_PTR<RRArray<T> > v1, RR_SHARED_PTR<RRArray<T> > v2)
    {
        if (v1->Length() != v2->Length()) 
			throw std::exception();
        for (size_t i = 0; i < v1->Length(); i++)
        {
            if ((*v1)[i]!= (*v2)[i]) 
				throw std::exception();
        }
    }

	template<>
	void ca<double>(RR_SHARED_PTR<RRArray<double> > v1, RR_SHARED_PTR<RRArray<double> > v2);


	RR_SHARED_PTR<RRMultiDimArray<double> > MultiDimArrayTest::LoadDoubleArrayFromFile(const string& fname)
	{
		ifstream f;
		f.exceptions(ifstream::failbit | ifstream::badbit);
		f.open(fname.c_str(),fstream::in | fstream::binary);
		RR_SHARED_PTR<RRMultiDimArray<double> > a=LoadDoubleArray(f);
		
		f.close();
		return a;
	}

	RR_SHARED_PTR<RRMultiDimArray<double> > MultiDimArrayTest::LoadDoubleArray(istream& s)
	{
		int32_t dimcount;
		s.read((char*)&dimcount,4);
#ifdef BOOST_BIG_ENDIAN
		std::reverse((uint8_t*)&dimcount,((uint8_t*)&dimcount)+4);
#endif
		int32_t* dims=new int32_t[dimcount];
		int32_t count=1;
		for (int32_t i=0; i<dimcount; i++)
		{
			s.read((char*)&dims[i],4);
#ifdef BOOST_BIG_ENDIAN
			std::reverse((uint8_t*)&dims[i],((uint8_t*)&dims[i])+4);
#endif 
			count*=dims[i];
		}

		double* real=new double[count];
		s.read((char*)real,count*sizeof(double));
		 
#ifdef BOOST_BIG_ENDIAN
		uint8_t* pos=(uint8_t*)real;
		for (size_t i=0; i<count; i++)
		{
			std::reverse(pos,pos+sizeof(double));
			pos+=sizeof(double);
		}
#endif
	

		if (!s.eof())
		{
			double* imag=new double[count];
			s.read((char*)imag,count*sizeof(double));

#ifdef BOOST_BIG_ENDIAN
		uint8_t* pos2=(uint8_t*)imag;
		for (size_t i=0; i<count; i++)
		{
			std::reverse(pos2,pos2+sizeof(double));
			pos2+=sizeof(double);
		}
#endif

			return RR_MAKE_SHARED<RRMultiDimArray<double> >(AttachRRArray(dims,dimcount,true),AttachRRArray(real,count,true),AttachRRArray(imag,count,true));
		}

		else
		{
			return RR_MAKE_SHARED<RRMultiDimArray<double> >(AttachRRArray(dims,dimcount,true),AttachRRArray(real,count,true));
		}

	}

	RR_SHARED_PTR<RRMultiDimArray<uint8_t> > MultiDimArrayTest::LoadByteArrayFromFile(const string& fname)
	{
		ifstream f;
		f.exceptions(ifstream::failbit | ifstream::badbit);
		f.open(fname.c_str(),fstream::in | fstream::binary);
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > a=LoadByteArray(f);
		
		f.close();
		return a;
	}

	RR_SHARED_PTR<RRMultiDimArray<uint8_t> > MultiDimArrayTest::LoadByteArray(istream& s)
	{
		int32_t dimcount;
		s.read((char*)&dimcount,4);
#ifdef BOOST_BIG_ENDIAN
		std::reverse((uint8_t*)&dimcount,((uint8_t*)&dimcount)+4);
#endif
		int32_t* dims=new int32_t[dimcount];

		int32_t count=1;
		for (int32_t i=0; i<dimcount; i++)
		{
			s.read((char*)&dims[i],4);
#ifdef BOOST_BIG_ENDIAN
			std::reverse((uint8_t*)&dims[i],((uint8_t*)&dims[i])+4);
#endif 
			count*=dims[i];
		}

		uint8_t* real=new uint8_t[count];
		s.read((char*)real,count*sizeof(uint8_t));
		 
		if (false /*!s.eof()*/)
		{
			uint8_t* imag=new uint8_t[count];
			s.read((char*)imag,count*sizeof(uint8_t));

			return RR_MAKE_SHARED<RRMultiDimArray<uint8_t> >(AttachRRArray(dims,dimcount,true),AttachRRArray(real,count,true),AttachRRArray(imag,count,true));
		}

		else
		{
			return RR_MAKE_SHARED<RRMultiDimArray<uint8_t> >(AttachRRArray(dims,dimcount,true),AttachRRArray(real,count,true));
		}
	}

	void MultiDimArrayTest::TestDouble()
	{
		RR_SHARED_PTR<RRMultiDimArray<double> > m1=LoadDoubleArrayFromFile("../testdata/testmdarray1.bin");
		RR_SHARED_PTR<RRMultiDimArray<double> > m2=LoadDoubleArrayFromFile("../testdata/testmdarray2.bin");
		RR_SHARED_PTR<RRMultiDimArray<double> > m3=LoadDoubleArrayFromFile("../testdata/testmdarray3.bin");
		RR_SHARED_PTR<RRMultiDimArray<double> > m4=LoadDoubleArrayFromFile("../testdata/testmdarray4.bin");
		RR_SHARED_PTR<RRMultiDimArray<double> > m5=LoadDoubleArrayFromFile("../testdata/testmdarray5.bin");

		int32_t m1a[]={ 2, 2, 3, 3, 4 };
		int32_t m1b[]={ 0, 2, 0, 0, 0 };
		int32_t m1c[]={ 1, 5, 5, 2, 1 };
		m1->AssignSubArray(vector<int32_t>(m1a,m1a+5),m2,vector<int32_t>(m1b,m1b+5),vector<int32_t>(m1c,m1c+5));
		ca(m1->Real,m3->Real);
		ca(m1->Imag,m3->Imag);

		int32_t m6a[]={2,2,1,1,10};
		RR_SHARED_PTR<RRMultiDimArray<double> > m6=RR_MAKE_SHARED<RRMultiDimArray<double> >(AttachRRArray(m6a,5,false),AllocateRRArray<double>(40),AllocateRRArray<double>(40));
		int32_t m4a[]={ 4, 2, 2, 8, 0 };
		int32_t m4b[]={ 0, 0, 0, 0, 0 };
		int32_t m4c[]= { 2, 2, 1, 1, 10 };
		m1->RetrieveSubArray(vector<int32_t>(m4a,m4a+5),m6,vector<int32_t>(m4b,m4b+5),vector<int32_t>(m4c,m4c+5));
		ca(m4->Real,m6->Real);
		ca(m4->Imag,m6->Imag);

		int32_t m7a[]={ 4, 4, 4, 4, 10 };
		RR_SHARED_PTR<RRMultiDimArray<double> > m7=RR_MAKE_SHARED<RRMultiDimArray<double> >(AttachRRArray(m7a,5,false),AllocateRRArray<double>(2560),AllocateRRArray<double>(2560));
		memset(m7->Real->ptr(),0,2560*sizeof(double));
		memset(m7->Imag->ptr(),0,2560*sizeof(double));
		int32_t m5a[]={ 4, 2, 2, 8, 0 };
		int32_t m5b[]={ 2, 1,2, 1, 0 };
		int32_t m5c[]= { 2, 2, 1, 1, 10 };
		m1->RetrieveSubArray(vector<int32_t>(m5a,m5a+5),m7,vector<int32_t>(m5b,m5b+5),vector<int32_t>(m5c,m5c+5));
		ca(m5->Real,m7->Real);
		ca(m5->Imag,m7->Imag);

	}

	void MultiDimArrayTest::TestByte()
	{
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m1=LoadByteArrayFromFile("../testdata/testmdarray_b1.bin");
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m2=LoadByteArrayFromFile("../testdata/testmdarray_b2.bin");
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m3=LoadByteArrayFromFile("../testdata/testmdarray_b3.bin");
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m4=LoadByteArrayFromFile("../testdata/testmdarray_b4.bin");
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m5=LoadByteArrayFromFile("../testdata/testmdarray_b5.bin");

		int32_t m1a[]={ 50,100 };
		int32_t m1b[]={ 20,25 };
		int32_t m1c[]={ 200,200 };
		m1->AssignSubArray(vector<int32_t>(m1a,m1a+2),m2,vector<int32_t>(m1b,m1b+2),vector<int32_t>(m1c,m1c+2));
		ca(m1->Real,m3->Real);
		
		int32_t m6a[]={200,200};
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m6=RR_MAKE_SHARED<RRMultiDimArray<uint8_t> >(AttachRRArray(m6a,2,false),AllocateRRArray<uint8_t>(40000));
		int32_t m4a[]={ 65,800 };
		int32_t m4b[]={ 0,0 };
		int32_t m4c[]= { 200,200 };
		m1->RetrieveSubArray(vector<int32_t>(m4a,m4a+2),m6,vector<int32_t>(m4b,m4b+2),vector<int32_t>(m4c,m4c+2));
		ca(m4->Real,m6->Real);
		

		int32_t m7a[]={ 512,512 };
		RR_SHARED_PTR<RRMultiDimArray<uint8_t> > m7=RR_MAKE_SHARED<RRMultiDimArray<uint8_t> >(AttachRRArray(m7a,2,false),AllocateRRArray<uint8_t>(512*512));
		memset(m7->Real->ptr(),0,512*512);
		int32_t m5a[]={ 65,800 };
		int32_t m5b[]={ 100,230 };
		int32_t m5c[]= { 200,200 };
		m1->RetrieveSubArray(vector<int32_t>(m5a,m5a+2),m7,vector<int32_t>(m5b,m5b+2),vector<int32_t>(m5c,m5c+2));
		ca(m5->Real,m7->Real);
		

	}

}
