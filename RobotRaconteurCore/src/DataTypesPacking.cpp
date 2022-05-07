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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/RobotRaconteurNode.h"

namespace RobotRaconteur
{
namespace detail
{
namespace packing
{

static RobotRaconteurNode* check_node_null(RobotRaconteurNode* node)
{
    if (!node)
    {
        throw DataTypeException("Could not pack/unpack named type because node is null");
    }
    return node;
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackStructure(const RR_INTRUSIVE_PTR<RRStructure>& structure,
                                                                RobotRaconteurNode* node)
{

    if (!structure)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    std::string type = structure->RRType();

    boost::string_ref servicetype = SplitQualifiedName(type).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return factory->PackStructure(structure);
}

RR_INTRUSIVE_PTR<RRStructure> UnpackStructure(const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& structure,
                                              RobotRaconteurNode* node)
{
    if (!structure)
        return RR_INTRUSIVE_PTR<RRStructure>();

    if (structure->GetTypeID() != DataTypes_structure_t)
        throw DataTypeMismatchException("Expected structure");

    const MessageStringPtr& type = structure->TypeName;

    boost::string_ref servicetype = SplitQualifiedName(type.str()).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return rr_cast<RRStructure>(factory->UnpackStructure(structure));
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodArray(const RR_INTRUSIVE_PTR<RRPodBaseArray>& a,
                                                               RobotRaconteurNode* node)
{

    if (!a)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    boost::string_ref type = a->RRElementTypeString();

    boost::string_ref servicetype = SplitQualifiedName(type).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return factory->PackPodArray(a);
}

RR_INTRUSIVE_PTR<RRPodBaseArray> UnpackPodArray(const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a,
                                                RobotRaconteurNode* node)
{
    if (!a)
        return RR_INTRUSIVE_PTR<RRPodBaseArray>();
    if (a->GetTypeID() != DataTypes_pod_array_t)
        throw DataTypeMismatchException("Expected pod array");
    const MessageStringPtr& type = a->TypeName;

    boost::string_ref servicetype = SplitQualifiedName(type.str()).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return rr_cast<RRPodBaseArray>(factory->UnpackPodArray(a));
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray>& a, RobotRaconteurNode* node)
{

    if (!a)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    boost::string_ref type = a->RRElementTypeString();

    boost::string_ref servicetype = SplitQualifiedName(type).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return factory->PackPodMultiDimArray(a);
}

RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> UnpackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a, RobotRaconteurNode* node)
{
    if (!a)
        return RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray>();
    if (a->GetTypeID() != DataTypes_pod_multidimarray_t)
        throw DataTypeMismatchException("Expected pod multidimarray");
    const MessageStringPtr& type = a->TypeName;

    boost::string_ref servicetype = SplitQualifiedName(type.str()).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return rr_cast<RRPodBaseMultiDimArray>(factory->UnpackPodMultiDimArray(a));
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedArray(const RR_INTRUSIVE_PTR<RRNamedBaseArray>& a,
                                                                 RobotRaconteurNode* node)
{

    if (!a)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    boost::string_ref type = a->RRElementTypeString();

    boost::string_ref servicetype = SplitQualifiedName(type).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return factory->PackNamedArray(a);
}

RR_INTRUSIVE_PTR<RRNamedBaseArray> UnpackNamedArray(const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a,
                                                    RobotRaconteurNode* node)
{
    if (!a)
        return RR_INTRUSIVE_PTR<RRNamedBaseArray>();
    if (a->GetTypeID() != DataTypes_namedarray_array_t)
        throw DataTypeMismatchException("Expected namedarray");

    const MessageStringPtr& type = a->TypeName;

    boost::string_ref servicetype = SplitQualifiedName(type.str()).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return rr_cast<RRNamedBaseArray>(factory->UnpackNamedArray(a));
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray>& a, RobotRaconteurNode* node)
{

    if (!a)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    boost::string_ref type = a->RRElementTypeString();

    boost::string_ref servicetype = SplitQualifiedName(type).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return factory->PackNamedMultiDimArray(a);
}

RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a, RobotRaconteurNode* node)
{
    if (!a)
        return RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray>();
    if (a->GetTypeID() != DataTypes_namedarray_multidimarray_t)
        throw DataTypeMismatchException("Expected namedarray multidimarray");

    const MessageStringPtr& type = a->TypeName;

    boost::string_ref servicetype = SplitQualifiedName(type.str()).get<0>();
    // std::string structuretype=res[1];

    RR_SHARED_PTR<ServiceFactory> factory = check_node_null(node)->GetServiceType(servicetype);

    return rr_cast<RRNamedBaseMultiDimArray>(factory->UnpackNamedMultiDimArray(a));
}

RR_INTRUSIVE_PTR<MessageElementData> PackVarType(const RR_INTRUSIVE_PTR<RRValue>& vardata, RobotRaconteurNode* node)
{

    if (!vardata)
        return RR_INTRUSIVE_PTR<MessageElementData>();

    std::string type = vardata->RRType();

    if (boost::starts_with(type, "RobotRaconteur.RRArray"))
    {
        return rr_cast<MessageElementData>(vardata);
    }

    if (type == "RobotRaconteur.RRMap<int32_t>")
    {
        return PackMapType<int32_t, RRValue>(vardata, node);
    }

    if (type == "RobotRaconteur.RRMap<string>")
    {
        return PackMapType<std::string, RRValue>(vardata, node);
    }

    if (boost::starts_with(type, "RobotRaconteur.RRMap"))
    {
        // Unknown keytype type for map
        throw DataTypeException("Invalid map keytype");
    }

    if (boost::starts_with(type, "RobotRaconteur.RRMultiDimArray"))
    {
        RR_INTRUSIVE_PTR<RRMultiDimBaseArray> multi_array = rr_cast<RRMultiDimBaseArray>(vardata);
        switch (multi_array->GetElementTypeID())
        {
        case DataTypes_double_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<double> >(vardata));
        case DataTypes_single_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<float> >(vardata));
        case DataTypes_int8_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<int8_t> >(vardata));
        case DataTypes_uint8_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<uint8_t> >(vardata));
        case DataTypes_int16_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<int16_t> >(vardata));
        case DataTypes_uint16_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<uint16_t> >(vardata));
        case DataTypes_int32_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<int32_t> >(vardata));
        case DataTypes_uint32_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<uint32_t> >(vardata));
        case DataTypes_int64_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<int64_t> >(vardata));
        case DataTypes_uint64_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<uint64_t> >(vardata));
        case DataTypes_cdouble_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<cdouble> >(vardata));
        case DataTypes_csingle_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<cfloat> >(vardata));
        case DataTypes_bool_t:
            return PackMultiDimArray(rr_cast<RRMultiDimArray<rr_bool> >(vardata));
        default:
            throw DataTypeException("Invalid MultiDimArray type");
        }
    }

    if (type == "RobotRaconteur.RRList")
    {
        return PackListType<RRValue>(vardata, node);
    }

    if (type == "RobotRaconteur.RRPodArray")
    {
        return PackPodArray(rr_cast<RRPodBaseArray>(vardata), node);
    }

    if (type == "RobotRaconteur.RRPodMultiDimArray")
    {
        return PackPodMultiDimArray(rr_cast<RRPodBaseMultiDimArray>(vardata), node);
    }

    if (type == "RobotRaconteur.RRNamedArray")
    {
        return PackNamedArray(rr_cast<RRNamedBaseArray>(vardata), node);
    }

    if (type == "RobotRaconteur.RRNamedMultiDimArray")
    {
        return PackNamedMultiDimArray(rr_cast<RRNamedBaseMultiDimArray>(vardata), node);
    }

    return PackStructure(rr_cast<RRStructure>(vardata), node);
}

RR_INTRUSIVE_PTR<RRValue> UnpackVarType(const RR_INTRUSIVE_PTR<MessageElement>& mvardata1, RobotRaconteurNode* node)
{
    if (!mvardata1)
        return RR_INTRUSIVE_PTR<RRValue>();
    if (mvardata1->ElementType == DataTypes_void_t)
        return RR_INTRUSIVE_PTR<RRValue>();

    RR_INTRUSIVE_PTR<MessageElementData> mvardata = mvardata1->GetData();

    DataTypes type = mvardata->GetTypeID();

    if (IsTypeRRArray(type))
    {
        return rr_cast<RRValue>(mvardata);
    }

    if (type == DataTypes_structure_t)
    {
        return UnpackStructure(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_vector_t)
    {
        return UnpackMapType<int32_t, RRValue>(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_dictionary_t)
    {
        return UnpackMapType<std::string, RRValue>(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_multidimarray_t)
    {
        DataTypes type1 =
            MessageElement::FindElement(mvardata1->CastDataToNestedList(DataTypes_multidimarray_t)->Elements, "array")
                ->ElementType;

        switch (type1)
        {

        case DataTypes_double_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<double>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_single_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<float>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_int8_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<int8_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_uint8_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<uint8_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_int16_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<int16_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_uint16_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<uint16_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_int32_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<int32_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_uint32_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<uint32_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_int64_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<int64_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_uint64_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<uint64_t>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_cdouble_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<cdouble>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_csingle_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<cfloat>(rr_cast<MessageElementNestedElementList>(mvardata)));
        case DataTypes_bool_t:
            return rr_cast<RRValue>(UnpackMultiDimArray<rr_bool>(rr_cast<MessageElementNestedElementList>(mvardata)));
        default:
            throw DataTypeException("Invalid data type");
        }

        throw DataTypeException("Invalid MultiDimArray type");
    }

    if (type == DataTypes_list_t)
    {
        return UnpackListType<RRValue>(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_pod_array_t)
    {
        return UnpackPodArray(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_pod_multidimarray_t)
    {
        return UnpackPodMultiDimArray(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_namedarray_array_t)
    {
        return UnpackNamedArray(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    if (type == DataTypes_namedarray_multidimarray_t)
    {
        return UnpackNamedMultiDimArray(rr_cast<MessageElementNestedElementList>(mvardata), node);
    }

    throw DataTypeException("Unknown data type");
}

} // namespace packing
} // namespace detail
} // namespace RobotRaconteur