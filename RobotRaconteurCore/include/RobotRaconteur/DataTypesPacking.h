/**
 * @file DataTypesPacking.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

namespace detail
{
namespace packing
{
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackStructure(
    RR_INTRUSIVE_PTR<RRStructure> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRStructure> UnpackStructure(
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodArray(
    RR_INTRUSIVE_PTR<RRPodBaseArray> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRPodBaseArray> UnpackPodArray(
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodMultiDimArray(
    RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> UnpackPodMultiDimArray(
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedArray(
    RR_INTRUSIVE_PTR<RRNamedBaseArray> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRNamedBaseArray> UnpackNamedArray(
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedMultiDimArray(
    RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> structure, RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> structure, RobotRaconteurNode* node);

template <typename T>
RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMultiDimArray(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > arr)
{
    if (!arr)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > ar;
    ar.reserve(2);
    ar.push_back(CreateMessageElement("dims", arr->Dims));
    ar.push_back(CreateMessageElement("array", arr->Array));
    return CreateMessageElementNestedElementList(DataTypes_multidimarray_t, "", RR_MOVE(ar));
}

template <typename T>
RR_INTRUSIVE_PTR<RRMultiDimArray<T> > UnpackMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNestedElementList> ar)
{
    if (!ar)
        return RR_INTRUSIVE_PTR<RRMultiDimArray<T> >();
    if (ar->GetTypeID() != DataTypes_multidimarray_t)
        throw DataTypeMismatchException("Expected a multidimarray");

    RR_INTRUSIVE_PTR<RRMultiDimArray<T> > arr = AllocateEmptyRRMultiDimArray<T>();
    arr->Dims = MessageElement::FindElement(ar->Elements, "dims")->CastData<RRArray<uint32_t> >();
    arr->Array = MessageElement::FindElement(ar->Elements, "array")->CastData<RRArray<T> >();
    return arr;
}

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementData> PackVarType(RR_INTRUSIVE_PTR<RRValue> vardata,
                                                                         RobotRaconteurNode* node);

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRValue> UnpackVarType(RR_INTRUSIVE_PTR<MessageElement> mvardata,
                                                                RobotRaconteurNode* node);

template <typename T, typename U>
RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const RR_INTRUSIVE_PTR<U>& data, RobotRaconteurNode* node);

template <typename T>
T UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, RobotRaconteurNode* node);

template <typename K, typename T>
class PackMapTypeSupport
{
  public:
    static RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMapType(RobotRaconteurNode* node,
                                                                         const RR_INTRUSIVE_PTR<RRValue> set)
    {
        BOOST_STATIC_ASSERT(sizeof(T) == 0);
    }

    static RR_INTRUSIVE_PTR<RRValue> UnpackMapType(RobotRaconteurNode* node,
                                                   const RR_INTRUSIVE_PTR<MessageElementNestedElementList> mset)
    {
        BOOST_STATIC_ASSERT(sizeof(T) == 0);
    }
};

template <typename T>
class PackMapTypeSupport<int32_t, T>
{
  public:
    template <typename U>
    static RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMapType(RobotRaconteurNode* node, const U& set)
    {
        if (!set)
            return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

        RR_INTRUSIVE_PTR<RRMap<int32_t, T> > set2 = rr_cast<RRMap<int32_t, T> >(set);

        std::vector<RR_INTRUSIVE_PTR<MessageElement> > mret;
        mret.reserve(set2->size());

        for (typename std::map<int32_t, RR_INTRUSIVE_PTR<T> >::iterator e = set2->begin(); e != set2->end(); e++)
        {
            int32_t key = e->first;

            RR_INTRUSIVE_PTR<MessageElementData> dat = PackAnyType<RR_INTRUSIVE_PTR<T> >(e->second, node);

            RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement(key, dat);
            mret.push_back(m);
        }

        return CreateMessageElementNestedElementList(DataTypes_vector_t, "", RR_MOVE(mret));
    }

    static RR_INTRUSIVE_PTR<RRMap<int32_t, T> > UnpackMapType(
        RobotRaconteurNode* node, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& mset)
    {
        if (!mset)
            return RR_INTRUSIVE_PTR<RRMap<int32_t, T> >();
        if (mset->GetTypeID() != DataTypes_vector_t)
            throw DataTypeMismatchException("Expected an int32 map");

        RR_INTRUSIVE_PTR<RRMap<int32_t, T> > ret = AllocateEmptyRRMap<int32_t, T>();

        for (std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator e = mset->Elements.begin();
             e != mset->Elements.end(); e++)
        {
            RR_INTRUSIVE_PTR<MessageElement> m = *e;
            int32_t key;
            if (!MessageElement_GetElementNumber(m, key))
            {
                throw DataTypeException("Invalid map format");
            }

            RR_INTRUSIVE_PTR<T> dat = UnpackAnyType<RR_INTRUSIVE_PTR<T> >(m, node);
            ret->insert(std::make_pair(key, dat));
        }

        return ret;
    }
};

template <typename T>
class PackMapTypeSupport<std::string, T>
{
  public:
    template <typename U>
    static RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMapType(RobotRaconteurNode* node, const U& set)
    {
        if (!set)
            return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

        RR_INTRUSIVE_PTR<RRMap<std::string, T> > set2 = rr_cast<RRMap<std::string, T> >(set);

        std::vector<RR_INTRUSIVE_PTR<MessageElement> > mret;
        mret.reserve(set2->size());

        for (typename std::map<std::string, RR_INTRUSIVE_PTR<T> >::iterator e = set2->begin(); e != set2->end(); e++)
        {
            RR_INTRUSIVE_PTR<MessageElementData> dat = PackAnyType<RR_INTRUSIVE_PTR<T> >(e->second, node);

            RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement("", dat);
            m->ElementName = e->first;
            mret.push_back(m);
        }

        return CreateMessageElementNestedElementList(DataTypes_dictionary_t, "", RR_MOVE(mret));
    }

    static RR_INTRUSIVE_PTR<RRMap<std::string, T> > UnpackMapType(
        RobotRaconteurNode* node, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& mset)
    {
        if (!mset)
            return RR_INTRUSIVE_PTR<RRMap<std::string, T> >();
        if (mset->GetTypeID() != DataTypes_dictionary_t)
            throw DataTypeMismatchException("Expected a string map");

        RR_INTRUSIVE_PTR<RRMap<std::string, T> > ret = AllocateEmptyRRMap<std::string, T>();

        for (std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator e = mset->Elements.begin();
             e != mset->Elements.end(); e++)
        {
            RR_INTRUSIVE_PTR<MessageElement> m = *e;

            MessageStringPtr key;

            if (!MessageElement_GetElementName(m, key))
            {
                throw DataTypeException("Invalid map format");
            }

            RR_INTRUSIVE_PTR<T> dat = UnpackAnyType<RR_INTRUSIVE_PTR<T> >(m, node);
            ret->insert(std::make_pair(RR_MOVE(key.str().to_string()), dat));
        }

        return ret;
    }
};

template <typename K, typename T, typename U>
RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMapType(const U& set, RobotRaconteurNode* node)
{
    return detail::packing::PackMapTypeSupport<K, T>::PackMapType(node, set);
}

template <typename K, typename T>
RR_INTRUSIVE_PTR<RRMap<K, T> > UnpackMapType(const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& mset,
                                             RobotRaconteurNode* node)
{
    return detail::packing::PackMapTypeSupport<K, T>::UnpackMapType(node, mset);
}

template <typename T, typename U>
RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackListType(U& set, RobotRaconteurNode* node)
{
    if (!set)
        return RR_INTRUSIVE_PTR<MessageElementNestedElementList>();

    RR_INTRUSIVE_PTR<RRList<T> > set2 = rr_cast<RRList<T> >(set);

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > mret;
    mret.reserve(set2->size());

    typename RRList<T>::iterator set2_iter = set2->begin();
    for (int32_t i = 0; i < boost::numeric_cast<int32_t>(set2->size()); i++)
    {
        int32_t key = i;

        RR_INTRUSIVE_PTR<MessageElementData> dat = PackAnyType<RR_INTRUSIVE_PTR<T> >(*set2_iter, node);

        RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement(key, dat);
        mret.push_back(m);
        ++set2_iter;
    }

    return CreateMessageElementNestedElementList(DataTypes_list_t, "", RR_MOVE(mret));
}

template <typename T>
RR_INTRUSIVE_PTR<RRList<T> > UnpackListType(const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& mset,
                                            RobotRaconteurNode* node)
{
    if (!mset)
        return RR_INTRUSIVE_PTR<RRList<T> >();
    if (mset->GetTypeID() != DataTypes_list_t)
        throw DataTypeMismatchException("Expected message element list");

    RR_INTRUSIVE_PTR<RRList<T> > ret = AllocateEmptyRRList<T>();

    for (int32_t i = 0; i < boost::numeric_cast<int32_t>(mset->Elements.size()); i++)
    {
        RR_INTRUSIVE_PTR<MessageElement> m = mset->Elements.at(i);
        int32_t key;
        if (!MessageElement_GetElementNumber(m, key))
        {
            throw DataTypeException("Invalid list format");
        }

        if (key != i)
            throw DataTypeException("Invalid list format");

        RR_INTRUSIVE_PTR<T> dat = UnpackAnyType<RR_INTRUSIVE_PTR<T> >(m, node);
        ret->push_back(dat);
    }

    return ret;
}

template <typename T>
class PackAnyTypeSupport
{
  public:
    template <typename NodeType>
    static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const RR_INTRUSIVE_PTR<RRValue>& data, NodeType node)
    {
        return PackVarType(data, node);
    }

    template <typename NodeType>
    static RR_INTRUSIVE_PTR<RRValue> UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
    {
        return UnpackVarType(mdata, node);
    }
};

template <typename T>
class PackAnyTypeSupport<RR_INTRUSIVE_PTR<T> >
{
  public:
    template <typename U, typename NodeType>
    static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
    {
        if (boost::is_base_of<RRStructure, T>::value)
        {
            return PackStructure(rr_cast<RRStructure>(data), node);
        }
        return PackVarType(data, node);
    }

    template <typename NodeType>
    static RR_INTRUSIVE_PTR<T> UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
    {
        if (boost::is_base_of<RRStructure, T>::value)
        {
            return rr_cast<T>(UnpackStructure(mdata->CastDataToNestedList(DataTypes_structure_t), node));
        }

        return rr_cast<T>(UnpackVarType(mdata, node));
    }
};

template <typename T>
class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRArray<T> > >
{
  public:
    template <typename U, typename NodeType>
    static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
    {
        return RR_STATIC_POINTER_CAST<MessageElementData>(data);
    }

    template <typename NodeType>
    static RR_INTRUSIVE_PTR<RRArray<T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
    {
        return mdata->CastData<RRArray<T> >();
    }
};

template <typename K, typename T>
class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRMap<K, T> > >
{
  public:
    template <typename U, typename NodeType>
    static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
    {
        return PackMapType<K, T>(data, node);
    }

    template <typename NodeType>
    static RR_INTRUSIVE_PTR<RRMap<K, T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
    {
        return UnpackMapType<K, T>(mdata->CastDataToNestedList(), node);
    }
};

template <typename T>
class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRList<T> > >
{
  public:
    template <typename U, typename NodeType>
    static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
    {
        return PackListType<T>(data, node);
    }

    template <typename NodeType>
    static RR_INTRUSIVE_PTR<RRList<T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
    {
        return UnpackListType<T>(mdata->CastDataToNestedList(), node);
    }
};

template <typename T>
class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRMultiDimArray<T> > >
{
  public:
    template <typename U, typename NodeType>
    static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
    {
        return PackMultiDimArray<T>(rr_cast<RRMultiDimArray<T> >(data));
    }

    template <typename NodeType>
    static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata,
                                                               NodeType node)
    {
        return UnpackMultiDimArray<T>(mdata->CastDataToNestedList(DataTypes_multidimarray_t));
    }
};

template <typename T, typename U>
RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const RR_INTRUSIVE_PTR<U>& data, RobotRaconteurNode* node)
{
    return PackAnyTypeSupport<T>::PackAnyType(data, node);
}

template <typename T>
T UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, RobotRaconteurNode* node)
{
    return PackAnyTypeSupport<T>::UnpackAnyType(mdata, node);
}

} // namespace packing
} // namespace detail
} // namespace RobotRaconteur
