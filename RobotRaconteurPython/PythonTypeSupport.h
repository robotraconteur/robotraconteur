// Copyright 2011-2020 Wason Technology, LLC
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

PyObject* NewStructure(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                       const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);
PyObject* GetStructureType(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                           boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node);
PyObject* GetNumPyDescrForType(const std::string& type,
                               const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                               const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);
PyObject* GetNumPyDescrForType(const RR_SHARED_PTR<ServiceEntryDefinition>& e,
                               const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                               const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);
PyObject* GetNumPyDescrForType(const RR_SHARED_PTR<TypeDefinition>& e,
                               const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                               const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);

boost::intrusive_ptr<MessageElement> PackMessageElement(PyObject* data, const boost::shared_ptr<TypeDefinition>& type1,
                                                        const boost::shared_ptr<WrappedServiceStub>& obj,
                                                        boost::shared_ptr<RobotRaconteurNode> node);
boost::intrusive_ptr<MessageElement> PackMessageElement(PyObject* data, const boost::shared_ptr<TypeDefinition>& type1,
                                                        const boost::shared_ptr<ServerContext>& obj,
                                                        boost::shared_ptr<RobotRaconteurNode> node);

PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element,
                               const boost::shared_ptr<TypeDefinition>& type1,
                               const boost::shared_ptr<WrappedServiceStub>& stub,
                               boost::shared_ptr<RobotRaconteurNode> node);
PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element,
                               const boost::shared_ptr<TypeDefinition>& type1,
                               const boost::shared_ptr<ServerContext>& obj, boost::shared_ptr<RobotRaconteurNode> node);

boost::intrusive_ptr<RRBaseArray> PackToRRArray(PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1,
                                                boost::intrusive_ptr<RRBaseArray> destrrarray);

PyObject* UnpackFromRRArray(const boost::intrusive_ptr<RRBaseArray>& rrarray,
                            const boost::shared_ptr<TypeDefinition>& type1);

boost::intrusive_ptr<RRBaseArray> PackToRRArray_numpy(PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1,
                                                      boost::intrusive_ptr<RRBaseArray> destrrarray);

PyObject* UnpackFromRRArray_numpy(const boost::intrusive_ptr<RRBaseArray>& rrarray,
                                  const boost::shared_ptr<TypeDefinition>& type1);

/*boost::intrusive_ptr<MessageElementNestedElementList> PackToRRMultiDimArray(PyObject* array_,
const boost::shared_ptr<TypeDefinition>& type1,const boost::intrusive_ptr<RRBaseArray>& destrrarray);

PyObject* UnpackFromRRMultiDimArray(const boost::intrusive_ptr<MessageElementNestedElementList>& rrarray,
const boost::shared_ptr<TypeDefinition>& type1);
*/

boost::intrusive_ptr<MessageElementNestedElementList> PackToRRMultiDimArray_numpy(
    PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1);

PyObject* UnpackFromRRMultiDimArray_numpy(const boost::intrusive_ptr<MessageElementNestedElementList>& rrarray,
                                          const boost::shared_ptr<TypeDefinition>& type1);

bool VerifyNumPyDataType(PyArray_Descr* np_type, DataTypes rr_type);

PyArray_Descr* RRTypeIdToNumPyDataType(DataTypes rr_type);

DataTypes GetNamedTypeEntryType(const boost::shared_ptr<RobotRaconteur::TypeDefinition>& type1,
                                const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                                const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node);

void PythonTypeSupport_Init();
} // namespace RobotRaconteur