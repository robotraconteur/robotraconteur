#include <RobotRaconteur.h>

#pragma once

using namespace RobotRaconteur;

namespace RobotRaconteurTest
{
	template <class T>
	void ca(RR_SHARED_PTR<RRArray<T> > v1, RR_SHARED_PTR<RRArray<T> > v2)
	{
		RR_NULL_CHECK(v1);
		RR_NULL_CHECK(v2);
		if (v1->Length() != v2->Length()) throw std::exception();
		for (size_t i = 0; i < v1->Length(); i++)
		{
			if ((*v1)[i] != (*v2)[i]) throw std::exception();
		}
	}

	template<>
	void ca<double>(RR_SHARED_PTR<RRArray<double> > v1, RR_SHARED_PTR<RRArray<double> > v2);
	
	template<>
	void ca<cdouble>(RR_SHARED_PTR<RRArray<cdouble> > v1, RR_SHARED_PTR<RRArray<cdouble> > v2);

	template<>
	void ca<cfloat>(RR_SHARED_PTR<RRArray<cfloat> > v1, RR_SHARED_PTR<RRArray<cfloat> > v2);


	template <class T>
	void ca(vector<T> v1, vector<T> v2)
	{
		if (v1.size() != v2.size()) throw std::exception();
		for (size_t i = 0; i < v1.size(); i++)
		{
			if ((v1)[i] != (v2)[i]) throw std::exception();
		}
	}

}