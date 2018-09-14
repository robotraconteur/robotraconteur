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

#include <RobotRaconteur.h>
#include "RobotRaconteurWrapped.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <numpy/ndarraytypes.h>
#include <numpy/ndarrayobject.h>

#pragma once

namespace RobotRaconteur
{
	std::string PyObjectToUTF8(PyObject* obj);

	PyObject* NewStructure(const std::string& type, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);
	
	boost::shared_ptr<MessageElement>  PackMessageElement(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteurNode> node);
	boost::shared_ptr<MessageElement>  PackMessageElement(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<ServerContext> obj, boost::shared_ptr<RobotRaconteurNode> node);

	PyObject* UnpackMessageElement(boost::shared_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> stub, boost::shared_ptr<RobotRaconteurNode> node);
	PyObject* UnpackMessageElement(boost::shared_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<ServerContext> obj, boost::shared_ptr<RobotRaconteurNode> node);

	boost::shared_ptr<RRBaseArray> PackToRRArray(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<RRBaseArray> destrrarray);

	PyObject* UnpackFromRRArray(boost::shared_ptr<RRBaseArray> rrarray, boost::shared_ptr<TypeDefinition> type1);

	boost::shared_ptr<RRBaseArray> PackToRRArray_numpy(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<RRBaseArray> destrrarray);

	PyObject* UnpackFromRRArray_numpy(boost::shared_ptr<RRBaseArray> rrarray, boost::shared_ptr<TypeDefinition> type1);

	/*boost::shared_ptr<MessageElementMultiDimArray> PackToRRMultiDimArray(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<RRBaseArray> destrrarray);

	PyObject* UnpackFromRRMultiDimArray(boost::shared_ptr<MessageElementMultiDimArray> rrarray, boost::shared_ptr<TypeDefinition> type1);
	*/

	boost::shared_ptr<MessageElementMultiDimArray> PackToRRMultiDimArray_numpy(PyObject* array_, boost::shared_ptr<TypeDefinition> type1);

	PyObject* UnpackFromRRMultiDimArray_numpy(boost::shared_ptr<MessageElementMultiDimArray> rrarray, boost::shared_ptr<TypeDefinition> type1);

	DataTypes NumPyDataTypeToRRTypeId(int np_type);

	int RRTypeIdToNumPyDataType(DataTypes rr_type);

	void PythonTypeSupport_Init();
}