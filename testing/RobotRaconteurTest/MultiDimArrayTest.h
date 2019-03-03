#include <fstream>
#include <RobotRaconteur/DataTypes.h>

#pragma once

using namespace RobotRaconteur;
using namespace std;



namespace RobotRaconteurTest
{
	class MultiDimArrayTest
	{
	public:

		static RR_INTRUSIVE_PTR<RRMultiDimArray<double> > LoadDoubleArrayFromFile(const string& fname);

		static RR_INTRUSIVE_PTR<RRMultiDimArray<double> > LoadDoubleArray(istream& s);

		static RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > LoadByteArrayFromFile(const string& fname);

		static RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > LoadByteArray(istream& s);
		
		static void TestDouble();

		static void TestByte();


		
		


	};



}