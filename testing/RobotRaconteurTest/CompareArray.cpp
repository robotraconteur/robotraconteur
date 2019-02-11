#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "CompareArray.h"

namespace RobotRaconteurTest
{
	template<>
	void ca<double>(RR_SHARED_PTR<RRArray<double> > v1, RR_SHARED_PTR<RRArray<double> > v2)
	{
		RR_NULL_CHECK(v1);
		RR_NULL_CHECK(v2);
		if (v1->Length() != v2->Length()) throw std::exception();
		for (size_t i = 0; i < v1->Length(); i++)
		{
			if (abs((*v1)[i] - (*v2)[i]) > 1e-15) throw std::exception();
		}
	}

	template<>
	void ca<cdouble>(RR_SHARED_PTR<RRArray<cdouble> > v1, RR_SHARED_PTR<RRArray<cdouble> > v2)
	{
		RR_NULL_CHECK(v1);
		RR_NULL_CHECK(v2);
		if (v1->Length() != v2->Length()) throw std::exception();
		for (size_t i = 0; i < v1->Length(); i++)
		{
			if (abs((*v1)[i].real - (*v2)[i].real) > 1e-15) throw std::exception();
			if (abs((*v1)[i].imag - (*v2)[i].imag) > 1e-15) throw std::exception();
		}
	}

	template<>
	void ca<cfloat>(RR_SHARED_PTR<RRArray<cfloat> > v1, RR_SHARED_PTR<RRArray<cfloat> > v2)
	{
		RR_NULL_CHECK(v1);
		RR_NULL_CHECK(v2);
		if (v1->Length() != v2->Length()) throw std::exception();
		for (size_t i = 0; i < v1->Length(); i++)
		{
			if (abs((*v1)[i].real - (*v2)[i].real) > 1e-15) throw std::exception();
			if (abs((*v1)[i].imag - (*v2)[i].imag) > 1e-15) throw std::exception();
		}
	}
}