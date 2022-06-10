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

#include "PythonTypeSupport.h"

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/numeric.hpp>

namespace RobotRaconteur
{
template <typename T>
class PyAutoPtr
{
  public:
    T* ptr;

    PyAutoPtr(T* ptr_) : ptr(ptr_) {}
    ~PyAutoPtr() { Py_XDECREF((PyObject*)ptr); }
    T* get() { return ptr; }
    T* detach()
    {
        T* ptr1 = ptr;
        ptr = NULL;
        return ptr1;
    }
};

class NpyIterAutoPtr
{
  public:
    NpyIter* iter;

    NpyIterAutoPtr(NpyIter* iter_) : iter(iter_) {}
    ~NpyIterAutoPtr() { NpyIter_Deallocate(iter); }
    NpyIter* get() { return iter; }
};

template <typename T>
T find_by_name(const std::vector<T>& t, const std::string& name)
{
    BOOST_FOREACH (T t1, t)
    {
        if (t1->Name == name)
        {
            return t1;
        }
    }

    throw ServiceException("Unknown structure type " + name);
}

template <typename T>
T find_by_name(const std::vector<T>& t, boost::string_ref name)
{
    BOOST_FOREACH (T t1, t)
    {
        if (t1->Name == name)
        {
            return t1;
        }
    }

    throw ServiceException("Unknown structure type " + name);
}

std::string PyObjectToUTF8(PyObject* obj)
{
#if (PY_MAJOR_VERSION == 2)

    if (PyUnicode_Check(obj))
    {
        PyAutoPtr<PyObject> str1(PyUnicode_AsUTF8String(obj));
        if (!str1.get())
        {
            throw DataTypeException("Invalid string");
        }
        return std::string(PyString_AsString(str1.get()));
    }
    else if (PyString_Check(obj))
    {
        return std::string(PyString_AsString(obj));
    }
    else
    {
        throw DataTypeException("Invalid string type specified");
    }
#else
    if (PyUnicode_Check(obj))
    {
        const char* str1 = PyUnicode_AsUTF8(obj);
        if (!str1)
        {
            throw DataTypeException("Invalid string type specified");
        }
        return std::string(str1);
    }
    else
    {
        throw DataTypeException("Invalid string type specified");
    }
#endif
}

PyAutoPtr<PyObject> stringToPyObject(const std::string& s)
{
#if (PY_MAJOR_VERSION == 2)
    PyObject* r = PyString_FromStringAndSize(s.c_str(), s.size());
    if (!r)
        throw DataTypeException("Invalid string specified");
    return r;
#else
    PyObject* r = PyUnicode_DecodeUTF8(s.c_str(), s.size(), "Invalid string specified");
    if (!r)
        throw DataTypeException("Invalid string specified");
    return r;
#endif
}

PyAutoPtr<PyObject> stringToPyObject(boost::string_ref s)
{
#if (PY_MAJOR_VERSION == 2)
    PyObject* r = PyString_FromStringAndSize(s.data(), s.size());
    if (!r)
        throw DataTypeException("Invalid string specified");
    return r;
#else
    PyObject* r = PyUnicode_DecodeUTF8(s.data(), s.size(), "Invalid string specified");
    if (!r)
        throw DataTypeException("Invalid string specified");
    return r;
#endif
}

PyAutoPtr<PyObject> stringToPyObject(const char* s) { return stringToPyObject(boost::string_ref(s, strlen(s))); }

void VerifyMultiDimArrayLength(const boost::intrusive_ptr<MessageElementNestedElementList>& data,
                               const boost::shared_ptr<TypeDefinition>& type1)
{
    boost::intrusive_ptr<RRArray<uint32_t> > data_dims =
        MessageElement::FindElement(data->Elements, "dims")->CastData<RRArray<uint32_t> >();
    if (!data_dims)
        throw DataTypeException("Invalid MultDimArray");
    if (type1->ArrayLength.size() != data_dims->size())
        throw DataTypeException("Array dimension mismatch");
    int32_t count = 1;

    for (int32_t i = 0; i < data_dims->size(); i++)
    {
        count *= data_dims->data()[i];
        if (data_dims->data()[i] != type1->ArrayLength[i])
            throw DataTypeException("Array dimension mismatch");
    }

    boost::intrusive_ptr<RRBaseArray> data_array =
        MessageElement::FindElement(data->Elements, "array")->CastData<RRBaseArray>();
    if (!data_array)
        throw DataTypeException("Invalid MultDimArray");
    if (data_array->size() != count)
        throw DataTypeException("Array dimension mismatch");
}

PyObject* GetStructureType(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                           boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node)
{
    PyObject* modules_dict = PyImport_GetModuleDict();
    if (modules_dict == NULL)
    {
        throw InternalErrorException("Could not load RobotRaconteurPython module");
    }

    /*PyObject* rrpy_module=PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPython");
    if (rrpy_module==NULL)
    {
    throw InternalErrorException("Could not load RobotRaconeturPython module");
    }*/

    PyObject* rrutil_module = PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonUtil");
    if (rrutil_module == NULL)
    {
        throw InternalErrorException("Could not load RobotRaconeturPythonUtil module");
    }

    if (!node)
    {
        node = RobotRaconteur::RobotRaconteurNode::sp();
    }

    boost::tuple<boost::string_ref, boost::string_ref> s1 = RobotRaconteur::SplitQualifiedName(type);

    RR_SHARED_PTR<RobotRaconteur::ServiceFactory> f;

    if (!obj)
    {
        f = node->GetServiceType(s1.get<0>());
    }
    else
    {
        f = node->GetPulledServiceType(obj, s1.get<0>());
    }

    RR_SHARED_PTR<RobotRaconteur::ServiceDefinition> d = f->ServiceDef();
    RR_SHARED_PTR<RobotRaconteur::ServiceEntryDefinition> s = TryFindByName(d->Structures, s1.get<1>());

    if (!s)
    {
        s = TryFindByName(d->Pods, s1.get<1>());
    }

    if (!s)
        throw ServiceException("Structure " + type + " not found");

    PyAutoPtr<PyObject> new_struct_type_func(PyObject_GetAttrString(rrutil_module, "CreateStructureType"));
    if (new_struct_type_func.get() == NULL)
    {
        throw InternalErrorException("Could not load RobotRaconeturPythonUtil.CreateStructureType function");
    }

    PyAutoPtr<PyObject> create_zero_array_func(PyObject_GetAttrString(rrutil_module, "CreateZeroArray"));
    if (create_zero_array_func.get() == NULL)
    {
        throw InternalErrorException("Could not load RobotRaconeturPythonUtil.CreateZeroArray function");
    }

    PyAutoPtr<PyObject> fields(PyDict_New());
    BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& e, s->Members)
    {

        RR_SHARED_PTR<PropertyDefinition> e2 = rr_cast<PropertyDefinition>(e);
        TypeDefinition& tdef = *e2->Type;
        if (tdef.ContainerType != DataTypes_ContainerTypes_none)
        {
            PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", Py_None, PyTuple_New(0)));
            PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
            continue;
        }

        switch (tdef.Type)
        {
        case DataTypes_namedtype_t: {
            std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
            RR_SHARED_PTR<NamedTypeDefinition> named_def = tdef.ResolveNamedType(empty_defs, node, obj);
            DataTypes named_def_type = named_def->RRDataType();
            PyObject* numpy_desc = NULL;

            switch (named_def_type)
            {
            case DataTypes_namedarray_t:
            case DataTypes_pod_t: {
                numpy_desc = GetNumPyDescrForType(rr_cast<ServiceEntryDefinition>(named_def), obj, node);
                break;
            }
            case DataTypes_enum_t: {
                PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", &PyLong_Type, PyTuple_New(0)));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
                continue;
            }
            default: {
                PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", Py_None, PyTuple_New(0)));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
                continue;
            }
            };

            if (!numpy_desc)
            {
                throw InternalErrorException("Could not get numpy descr for pod/namedarray for structure init");
            }

            if (tdef.ArrayType != DataTypes_ArrayTypes_none && (tdef.ArrayVarLength || tdef.ArrayLength.size() == 0))
            {
                PyAutoPtr<PyObject> args(Py_BuildValue("(O(OO))", create_zero_array_func.get(), numpy_desc, Py_None));
                Py_XDECREF(numpy_desc);
                PyDict_SetItemString(fields.get(), e->Name.c_str(), args.get());
            }
            else
            {
                std::vector<int32_t> dims_arg1;
                if (tdef.ArrayType == DataTypes_ArrayTypes_none)
                {
                    dims_arg1.push_back(1);
                }
                else
                {
                    dims_arg1 = tdef.ArrayLength;
                }
                PyAutoPtr<PyObject> dims_arg(PyTuple_New(dims_arg1.size()));
                for (size_t i = 0; i < dims_arg1.size(); i++)
                {
                    PyTuple_SetItem(dims_arg.get(), (Py_ssize_t)(i), PyLong_FromLong(dims_arg1.at(i)));
                }
                PyAutoPtr<PyObject> none_args(
                    Py_BuildValue("(O(OO))", create_zero_array_func.get(), numpy_desc, dims_arg.get()));
                Py_XDECREF(numpy_desc);
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
            }
            continue;
        }
        case DataTypes_string_t: {
            PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", &PyUnicode_Type, PyTuple_New(0)));
            PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
            continue;
        }
        case DataTypes_varvalue_t: {
            PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", Py_None, PyTuple_New(0)));
            PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
            continue;
        }
        default:
            break;
        };

        if (!IsTypeNumeric(tdef.Type))
        {
            throw DataTypeException("Invalid Robot Raconteur data type");
        }

        if (tdef.ArrayType == DataTypes_ArrayTypes_none)
        {
            switch (tdef.Type)
            {
            case DataTypes_double_t:
            case DataTypes_single_t: {
                PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", &PyFloat_Type, PyTuple_New(0)));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
                continue;
            }
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
            case DataTypes_int64_t:
            case DataTypes_uint64_t: {
                PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", &PyLong_Type, PyTuple_New(0)));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
                continue;
            }
            case DataTypes_cdouble_t:
            case DataTypes_csingle_t: {
                PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", &PyComplex_Type, PyTuple_New(0)));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
                continue;
            }
            case DataTypes_bool_t: {
                PyAutoPtr<PyObject> none_args(Py_BuildValue("(ON)", &PyBool_Type, PyTuple_New(0)));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
                continue;
            }

            default:
                throw DataTypeException("Invalid Robot Raconteur data type");
            }
        }
        else
        {
            if (tdef.ArrayVarLength)
            {
                PyAutoPtr<PyArray_Descr> numpy_desc = RRTypeIdToNumPyDataType(tdef.Type);
                PyAutoPtr<PyObject> none_args(
                    Py_BuildValue("(O(OO))", create_zero_array_func.get(), numpy_desc.get(), Py_None));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
            }
            else
            {
                PyAutoPtr<PyObject> dims_arg(PyTuple_New(tdef.ArrayLength.size()));
                for (size_t i = 0; i < tdef.ArrayLength.size(); i++)
                {
                    PyTuple_SetItem(dims_arg.get(), (Py_ssize_t)(i), PyLong_FromLong(tdef.ArrayLength.at(i)));
                }
                PyAutoPtr<PyArray_Descr> numpy_desc = RRTypeIdToNumPyDataType(tdef.Type);
                PyAutoPtr<PyObject> none_args(
                    Py_BuildValue("(O(OO))", create_zero_array_func.get(), numpy_desc.get(), dims_arg.get()));
                PyDict_SetItemString(fields.get(), e->Name.c_str(), none_args.get());
            }
            continue;
        }
    }

    std::string type2_str = boost::replace_all_copy(type, ".", "__");
    PyObject* new_struct_type =
        PyObject_CallFunction(new_struct_type_func.get(), "s#O", type2_str.c_str(), type2_str.size(), fields.get());
    if (!new_struct_type)
    {
        PyErr_Print();
        throw InternalErrorException("Could not create requested structure type");
    }
    return new_struct_type;
}

PyObject* NewStructure(const std::string& type, const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                       const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node)
{
    PyAutoPtr<PyObject> struct_type(GetStructureType(type, obj, node));
    return PyObject_CallObject(struct_type.get(), NULL);
}

PyObject* GetNumPyDescrForType(const std::string& type,
                               const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                               const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node)
{
    RR_SHARED_PTR<TypeDefinition> type1 = RR_MAKE_SHARED<TypeDefinition>();
    type1->Type = DataTypes_pod_t;
    type1->TypeString = type;
    std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
    RR_SHARED_PTR<NamedTypeDefinition> n = type1->ResolveNamedType(other_defs, node, obj);
    if (n->RRDataType() != DataTypes_pod_t && n->RRDataType() != DataTypes_namedarray_t)
        throw DataTypeException("Invalid pod or namedarray type");
    return GetNumPyDescrForType(rr_cast<ServiceEntryDefinition>(n), obj, node);
}

PyObject* GetNumPyDescrForType(const RR_SHARED_PTR<TypeDefinition>& type,
                               const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                               const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node)
{
    std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
    RR_SHARED_PTR<NamedTypeDefinition> n = type->ResolveNamedType(other_defs, node, obj);
    if (n->RRDataType() != DataTypes_pod_t && n->RRDataType() != DataTypes_namedarray_t)
        throw DataTypeException("Invalid pod or namedarray type");
    return GetNumPyDescrForType(rr_cast<ServiceEntryDefinition>(n), obj, node);
}

PyObject* GetNumPyDescrForType(const RR_SHARED_PTR<ServiceEntryDefinition>& e,
                               const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                               const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node)
{
    PyAutoPtr<PyObject> decr_list(PyList_New(e->Members.size()));
    for (size_t i = 0; i < e->Members.size(); i++)
    {
        PyAutoPtr<PyObject> f(PyTuple_New(3));
        std::string& f_name = e->Members[i]->Name;
        RR_SHARED_PTR<TypeDefinition> f_def = rr_cast<PropertyDefinition>(e->Members[i])->Type;
        if (IsTypeNumeric(f_def->Type))
        {
            PyArray_Descr* dtype = RRTypeIdToNumPyDataType(f_def->Type);
            PyTuple_SetItem(f.get(), 1, (PyObject*)(dtype));
        }
        else
        {
            std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
            RR_SHARED_PTR<ServiceEntryDefinition> f_def2 =
                rr_cast<ServiceEntryDefinition>(f_def->ResolveNamedType(other_defs, node, obj));
            PyAutoPtr<PyObject> f_desc(GetNumPyDescrForType(f_def2, obj, node));
            PyTuple_SetItem(f.get(), 1, f_desc.detach());
        }

        if (f_def->ArrayType == DataTypes_ArrayTypes_none)
        {
            PyAutoPtr<PyObject> f_dims(PyTuple_New(0));
            PyTuple_SetItem(f.get(), 2, f_dims.detach());
        }
        else
        {
            PyAutoPtr<PyObject> f_dims(PyTuple_New(f_def->ArrayLength.size()));
            for (size_t j = 0; j < f_def->ArrayLength.size(); j++)
            {
                PyTuple_SetItem(f_dims.get(), j, PyLong_FromLong(f_def->ArrayLength[j]));
            }
            PyTuple_SetItem(f.get(), 2, f_dims.detach());

            /*PyAutoPtr<PyObject> f_dims(PyTuple_New(1));
            int32_t n_elems = boost::accumulate(f_def->ArrayLength, 1, std::multiplies<int32_t>());
            PyTuple_SetItem(f_dims.get(), 0, PyLong_FromLong(n_elems));
            PyTuple_SetItem(f.get(), 2, f_dims.detach());*/
        }

        if (f_def->ArrayType == DataTypes_ArrayTypes_array && f_def->ArrayVarLength)
        {
            // Use "len" and "array" field for variable length arrays

            PyAutoPtr<PyObject> f_decr_list(PyList_New(2));
            PyList_SetItem(f_decr_list.get(), 0,
                           Py_BuildValue("(s,O,O)", "len", PyArray_DescrNewFromType(NPY_INT32), PyTuple_New(0)));
            PyTuple_SetItem(f.get(), 0, stringToPyObject("array").detach());
            PyList_SetItem(f_decr_list.get(), 1, f.detach());

            PyArray_Descr* f_descr;
            if (!PyArray_DescrConverter(f_decr_list.get(), &f_descr))
            {
                PyErr_Print();
                throw DataTypeException("Could not create numpy dtype for Pod");
            }

            PyAutoPtr<PyObject> f2(PyTuple_New(2));
            PyTuple_SetItem(f2.get(), 0, stringToPyObject(f_name).detach());
            PyTuple_SetItem(f2.get(), 1, (PyObject*)f_descr);

            PyList_SetItem(decr_list.get(), i, f2.detach());

            continue;
        }

        PyTuple_SetItem(f.get(), 0, stringToPyObject(f_name).detach());

        PyList_SetItem(decr_list.get(), i, f.detach());
    }

    PyArray_Descr* descr_ret;
    if (!PyArray_DescrConverter(decr_list.get(), &descr_ret))
    {
        throw DataTypeException("Could not create numpy dtype for Pod");
    }

    return (PyObject*)descr_ret;
}

class PackMessageElementImpl
{
  public:
    boost::intrusive_ptr<MessageElementNestedElementList> PackMessageElement_namedarray(
        PyObject* data, const boost::shared_ptr<TypeDefinition>& type1,
        const boost::shared_ptr<WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteurNode>& node)
    {
        if (!PyArray_Check(data))
        {
            throw DataTypeException("numpy.ndarray structure array expected");
        }

        PyArrayObject* data1 = (PyArrayObject*)data;
        /*if (type1->ArrayType != DataTypes_ArrayTypes_none)
        {
            if (PyArray_SIZE((PyArrayObject*)data) != 1)
            throw DataTypeException("numy.ndarray scalar structure array expected");
        }*/

        if (type1->ArrayType == DataTypes_ArrayTypes_none && PyArray_SIZE(data1) != 1)
        {
            throw DataTypeException("Scalar expected");
        }
        else
        {
            uint32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<uint32_t>());
            if (!type1->ArrayLength.empty() && c != 0)
            {
                if (type1->ArrayVarLength)
                {
                    if (PyArray_SIZE(data1) > c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
                else
                {
                    if (PyArray_SIZE(data1) != c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
            }
        }

        std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
        boost::shared_ptr<ServiceEntryDefinition> struct_def =
            RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(type1->ResolveNamedType(empty_defs, node, obj));
        if (!struct_def)
            throw DataTypeException("Invalid pod data type");
        if (struct_def->EntryType != DataTypes_namedarray_t)
            throw DataTypeException("Invalid pod data type");

        PyArray_Descr* data_desc = PyArray_DESCR(data1);
        PyAutoPtr<PyObject> type_desc(GetNumPyDescrForType(struct_def, obj, node));
        if (PyArray_EquivTypes(data_desc, (PyArray_Descr*)type_desc.get()) != NPY_TRUE)
        {
            throw DataTypeException("Invalid namedarray type");
        }

        boost::tuple<DataTypes, size_t> s = GetNamedArrayElementTypeAndCount(struct_def, empty_defs, node, obj);
        if (boost::numeric_cast<size_t>(PyArray_ITEMSIZE(data1)) != s.get<1>() * RRArrayElementSize(s.get<0>()))
        {
            throw DataTypeException("Invalid namedarray type");
        }

        std::string typestr2 = struct_def->ResolveQualifiedName();

        PyAutoPtr<PyObject> data4(PyArray_Flatten(data1, NPY_FORTRANORDER));
        PyAutoPtr<PyArrayObject> data2(PyArray_GETCONTIGUOUS((PyArrayObject*)data4.get()));
        if (data2.get() == NULL)
            throw DataTypeException("Internal error");

        RR_INTRUSIVE_PTR<RRBaseArray> data3 =
            AllocateRRArrayByType(s.get<0>(), (boost::numeric_cast<size_t>(PyArray_SIZE(data1)) * s.get<1>()));
        memcpy(data3->void_ptr(), PyArray_DATA(data2.get()), PyArray_NBYTES(data2.get()));

        std::vector<RR_INTRUSIVE_PTR<MessageElement> > ret1;
        ret1.push_back(CreateMessageElement("array", data3));
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> ret =
            CreateMessageElementNestedElementList(DataTypes_namedarray_array_t, typestr2, ret1);
        return ret;
    }

    boost::intrusive_ptr<MessageElementNestedElementList> PackMessageElement_pod(
        PyObject* data, const boost::shared_ptr<TypeDefinition>& type1,
        const boost::shared_ptr<WrappedServiceStub>& obj, const boost::shared_ptr<RobotRaconteurNode>& node)
    {
        if (!PyArray_Check(data))
        {
            throw DataTypeException("numpy.ndarray structure array expected");
        }

        PyArrayObject* data1 = (PyArrayObject*)data;
        /*if (type1->ArrayType != DataTypes_ArrayTypes_none)
        {
            if (PyArray_SIZE((PyArrayObject*)data) != 1)
            throw DataTypeException("numy.ndarray scalar structure array expected");
        }*/

        if (type1->Type != DataTypes_varvalue_t && type1->ArrayType == DataTypes_ArrayTypes_none &&
            PyArray_SIZE(data1) != 1)
        {
            throw DataTypeException("Scalar expected");
        }
        else
        {
            uint32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<uint32_t>());
            if (!type1->ArrayLength.empty() && c != 0)
            {
                if (type1->ArrayVarLength)
                {
                    if (PyArray_SIZE(data1) > c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
                else
                {
                    if (PyArray_SIZE(data1) != c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
            }
        }

        std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
        boost::shared_ptr<ServiceEntryDefinition> struct_def =
            RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(type1->ResolveNamedType(empty_defs, node, obj));
        if (!struct_def)
            throw DataTypeException("Invalid pod data type");
        if (struct_def->EntryType != DataTypes_pod_t)
            throw DataTypeException("Invalid pod data type");

        PyArray_Descr* data_desc = PyArray_DESCR(data1);
        PyAutoPtr<PyObject> type_desc(GetNumPyDescrForType(struct_def, obj, node));
        if (PyArray_EquivTypes(data_desc, (PyArray_Descr*)type_desc.get()) != NPY_TRUE)
        {
            throw DataTypeException("Invalid pod array type");
        }

        std::string typestr2 = struct_def->ResolveQualifiedName();

        std::vector<RR_INTRUSIVE_PTR<MessageElement> > ret;

        PyAutoPtr<PyObject> data2(PyArray_Flatten((PyArrayObject*)data, NPY_FORTRANORDER));

        for (Py_ssize_t i = 0; i < PySequence_Length(data2.get()); i++)
        {

            PyAutoPtr<PyObject> a(PySequence_GetItem(data2.get(), i));

            std::vector<boost::intrusive_ptr<MessageElement> > m_struct;
            BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& m_def, struct_def->Members)
            {
                boost::shared_ptr<PropertyDefinition> p_def = boost::dynamic_pointer_cast<PropertyDefinition>(m_def);
                if (!p_def)
                    throw ServiceException("Invalid structure definition: " + typestr2);

                PyAutoPtr<PyObject> p_def_name = stringToPyObject(p_def->Name);
                PyAutoPtr<PyObject> field_obj(PyObject_GetItem(a.get(), p_def_name.get()));
                if (field_obj.get() == NULL)
                {
                    throw DataTypeException("Field " + p_def->Name + " not found in structure of type " + typestr2);
                }

                RR_SHARED_PTR<TypeDefinition> p_def2_type = p_def->Type;
                if (p_def2_type->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    p_def2_type = RR_MAKE_SHARED<TypeDefinition>();
                    p_def->Type->CopyTo(*p_def2_type);
                    uint32_t c = boost::accumulate(p_def2_type->ArrayLength, 1, std::multiplies<uint32_t>());
                    p_def2_type->ArrayLength.clear();
                    p_def2_type->ArrayLength.push_back(c);
                    p_def2_type->ArrayType = DataTypes_ArrayTypes_array;
                    p_def2_type->ArrayVarLength = false;

                    if (IsTypeNumeric(p_def->Type->Type))
                    {
                        PyAutoPtr<PyObject> field_obj2(
                            PyArray_Flatten((PyArrayObject*)field_obj.get(), NPY_FORTRANORDER));

                        boost::intrusive_ptr<MessageElement> el1 =
                            PackMessageElement(field_obj2.get(), p_def2_type, obj, node);
                        el1->ElementName = p_def->Name;
                        m_struct.push_back(el1);
                        continue;
                    }
                }

                if (/*IsTypeNumeric(p_def->Type->Type) &&*/ p_def->Type->ArrayType == DataTypes_ArrayTypes_array &&
                    p_def->Type->ArrayVarLength)
                {
                    PyAutoPtr<PyObject> field_dim_str = stringToPyObject("len");
                    PyAutoPtr<PyObject> field_dim(PyObject_GetItem(field_obj.get(), field_dim_str.get()));
                    int64_t n;
                    PyArray_CastScalarToCtype(field_dim.get(), &n, PyArray_DescrFromType(NPY_INT64));
                    if (n > p_def->Type->ArrayLength.at(0))
                    {
                        throw DataTypeException("Invalid array length in pod");
                    }

                    PyAutoPtr<PyObject> field_array_str = stringToPyObject("array");
                    PyAutoPtr<PyObject> field_array(PyObject_GetItem(field_obj.get(), field_array_str.get()));
                    PyAutoPtr<PyObject> field_array2(
                        PyArray_NewCopy((PyArrayObject*)field_array.get(), NPY_FORTRANORDER));
                    PyArray_Dims field_array_np_dims;
                    field_array_np_dims.len = 1;
                    npy_intp field_array_np_dims1 = (npy_intp)n;
                    field_array_np_dims.ptr = &field_array_np_dims1;

                    if (!PyArray_Resize((PyArrayObject*)field_array2.get(), &field_array_np_dims, 1, NPY_FORTRANORDER))
                    {
                        throw InternalErrorException("Internal error");
                    }

                    boost::intrusive_ptr<MessageElement> el1 =
                        PackMessageElement(field_array2.get(), p_def2_type, obj, node);
                    el1->ElementName = p_def->Name;
                    m_struct.push_back(el1);
                    continue;
                }

                if (PyArray_CheckScalar(field_obj.get()) && p_def2_type->Type == DataTypes_namedtype_t)
                {
                    PyAutoPtr<PyObject> field_obj_array_desc(GetNumPyDescrForType(
                        rr_cast<ServiceEntryDefinition>(p_def->Type->ResolveNamedType(empty_defs, node, obj)), obj,
                        node));
                    npy_intp field_obj_array_dim = 1;
                    PyAutoPtr<PyObject> field_obj_array(PyArray_SimpleNewFromDescr(
                        1, &field_obj_array_dim, (PyArray_Descr*)field_obj_array_desc.get()));
                    Py_INCREF(field_obj_array.get());
                    PyAutoPtr<PyObject> zero_ind(PyLong_FromLong(0));
                    if (PyObject_SetItem(field_obj_array.get(), zero_ind.get(), field_obj.get()) != 0)
                    {
                        PyErr_Print();
                        throw DataTypeException("Internal error setting scalar pod value");
                    }
                    boost::intrusive_ptr<MessageElement> el2 =
                        PackMessageElement(field_obj_array.get(), p_def2_type, obj, node);
                    el2->ElementName = p_def->Name;
                    m_struct.push_back(el2);
                    continue;
                }

                boost::intrusive_ptr<MessageElement> el = PackMessageElement(field_obj.get(), p_def2_type, obj, node);
                el->ElementName = p_def->Name;
                m_struct.push_back(el);
            }

            boost::intrusive_ptr<MessageElement> el2 =
                CreateMessageElement("", CreateMessageElementNestedElementList(DataTypes_pod_t, "", m_struct));
            el2->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
            el2->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
            el2->ElementNumber = i;

            ret.push_back(el2);
        }

        return CreateMessageElementNestedElementList(DataTypes_pod_array_t, typestr2, ret);
    }

    boost::intrusive_ptr<MessageElement> PackMessageElement(PyObject* data,
                                                            const boost::shared_ptr<TypeDefinition>& type1,
                                                            const boost::shared_ptr<WrappedServiceStub>& obj,
                                                            boost::shared_ptr<RobotRaconteurNode> node)
    {
        if (!type1)
            throw InvalidArgumentException("Invalid parameters for PackMessageElement");

        if (!node)
        {
            if (obj)
            {
                node = obj->RRGetNode();
            }
            else
            {
                node = RobotRaconteurNode::sp();
            }
        }

        if (type1->Type == DataTypes_varvalue_t && type1->ContainerType == DataTypes_ContainerTypes_none &&
            data != NULL && data != Py_None)
        {

            PyAutoPtr<PyObject> dt(PyObject_GetAttrString(data, "datatype"));
            if (!dt.get())
                throw DataTypeException("Invalid VarValue object");
            std::string dt_str = PyObjectToUTF8(dt.get());
            boost::trim(dt_str);

            std::vector<std::string> dt_str1;
            boost::split(dt_str1, dt_str, boost::is_space());
            if (dt_str1.size() != 1)
            {
                dt_str += " value";
            }

            boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
            type2->FromString(dt_str);
            if (type2->Type == DataTypes_namedtype_t)
            {
                std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                RR_SHARED_PTR<NamedTypeDefinition> d = type2->ResolveNamedType(empty_defs, node, obj);
                if (d->RRDataType() == DataTypes_pod_t && type2->ArrayType == DataTypes_ArrayTypes_none)
                {
                    type2->ArrayType = DataTypes_ArrayTypes_array;
                }
            }

            PyAutoPtr<PyObject> data2(PyObject_GetAttrString(data, "data"));
            if (!data2.get())
                throw DataTypeException("Invalid VarValue object");

            return PackMessageElement(data2.get(), type2, obj, node);
        }

        boost::intrusive_ptr<MessageElement> element = CreateMessageElement();
        element->ElementName = type1->Name;

        if (data == NULL || data == Py_None)
        {
            if (type1->ContainerType == DataTypes_ContainerTypes_none)
            {
                std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                if (IsTypeNumeric(type1->Type))
                {
                    throw DataTypeException("Scalars and arrays must not be None");
                }
                if (type1->Type == DataTypes_string_t)
                {
                    throw DataTypeException("Strings must not be None");
                }
                if (type1->Type == DataTypes_namedtype_t)
                {
                    DataTypes rr_type = type1->ResolveNamedType(empty_defs, node, obj)->RRDataType();
                    if (rr_type == DataTypes_pod_t || rr_type == DataTypes_namedarray_t)
                        throw DataTypeException("Pods must not be None");
                }
            }

            element->ElementType = DataTypes_void_t;
            element->DataCount = 0;
            element->SetData(RR_INTRUSIVE_PTR<MessageElementData>());
            return element;
        }
        else if (type1->ContainerType == DataTypes_ContainerTypes_list)
        {
            if (!PySequence_Check(data))
                throw DataTypeException("Sequence type expected");

            boost::shared_ptr<TypeDefinition> type2 = type1->Clone();
            type2->RemoveContainers();

            element->ElementType = DataTypes_list_t;
            std::vector<boost::intrusive_ptr<MessageElement> > mret;

            for (int32_t i = 0; i < boost::numeric_cast<int32_t>(PySequence_Size(data)); i++)
            {
                push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);
                PyAutoPtr<PyObject> dat1(PySequence_GetItem(data, (Py_ssize_t)i));
                boost::intrusive_ptr<MessageElement> el = PackMessageElement(dat1.get(), type2, obj, node);
                el->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
                el->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
                el->ElementNumber = i;
                mret.push_back(el);
                pop_field_level();
            }
            element->DataCount = mret.size();
            element->SetData(CreateMessageElementNestedElementList(DataTypes_list_t, "", mret));
            return element;
        }

        if (type1->ContainerType == DataTypes_ContainerTypes_map_int32)
        {
            boost::shared_ptr<TypeDefinition> type2 = type1->Clone();
            type2->RemoveContainers();

            element->ElementType = DataTypes_vector_t;
            std::vector<boost::intrusive_ptr<MessageElement> > mret;

            if (PySequence_Check(data))
            {
                for (int32_t i = 0; i < boost::numeric_cast<int32_t>(PySequence_Size(data)); i++)
                {
                    push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);
                    PyAutoPtr<PyObject> dat1(PySequence_GetItem(data, (Py_ssize_t)i));
                    boost::intrusive_ptr<MessageElement> el = PackMessageElement(dat1.get(), type2, obj, node);
                    el->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
                    el->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
                    el->ElementNumber = i;
                    mret.push_back(el);
                    pop_field_level();
                }
                element->DataCount = mret.size();
                element->SetData(CreateMessageElementNestedElementList(DataTypes_vector_t, "", mret));
                return element;
            }
            else if (PyMapping_Check(data))
            {
                PyAutoPtr<PyObject> data_list(PyMapping_Items(data));
                if (!data_list.get())
                    throw DataTypeException("Invalid Map type");

                PyAutoPtr<PyObject> iter(PyObject_GetIter(data_list.get()));

                if (iter.get() == NULL)
                    throw DataTypeException("Invalid Map type");

                PyObject* item;

                while ((item = PyIter_Next(iter.get())))
                {
                    PyAutoPtr<PyObject> item1(item);

                    if (!PySequence_Check(item))
                        throw DataTypeException("Invalid Map");
                    if (PySequence_Size(item) != 2)
                        throw DataTypeException("Invalid Map");
                    PyAutoPtr<PyObject> key(PySequence_GetItem(item, 0));
                    PyAutoPtr<PyObject> val(PySequence_GetItem(item, 1));

                    long key_l = PyLong_AsLong(key.get());
                    push_field_level("[" + boost::lexical_cast<std::string>(key_l) + "]", type2);
                    if (PyErr_Occurred())
                    {
                        throw DataTypeException("Invalid Key in Map");
                    }

                    if (key_l < std::numeric_limits<int32_t>::min() || key_l > std::numeric_limits<int32_t>::max())
                    {
                        throw DataTypeException("Invalid Key in Map");
                    }

                    boost::intrusive_ptr<MessageElement> el = PackMessageElement(val.get(), type2, obj, node);
                    el->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
                    el->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
                    el->ElementNumber = boost::numeric_cast<int32_t>(key_l);
                    mret.push_back(el);
                    pop_field_level();
                }

                element->DataCount = mret.size();
                element->SetData(CreateMessageElementNestedElementList(DataTypes_vector_t, "", mret));
                return element;
            }
            else
            {
                throw DataTypeException("Sequence type or Map type expected");
            }
        }
        else if (type1->ContainerType == DataTypes_ContainerTypes_map_string)
        {
            boost::shared_ptr<TypeDefinition> type2 = type1->Clone();
            type2->RemoveContainers();

            if (!PyMapping_Check(data))
                throw DataTypeException("Map type expected");

            std::vector<boost::intrusive_ptr<MessageElement> > mret;
            PyAutoPtr<PyObject> data_list(PyMapping_Items(data));
            if (!data_list.get())
                throw DataTypeException("Invalid Map type");

            PyAutoPtr<PyObject> iter(PyObject_GetIter(data_list.get()));
            if (iter.get() == NULL)
                throw DataTypeException("Invalid Map type");

            PyObject* item;

            while (item = PyIter_Next(iter.get()))
            {
                PyAutoPtr<PyObject> item1(item);

                PyAutoPtr<PyObject> key(PySequence_GetItem(item, 0));
                PyAutoPtr<PyObject> val(PySequence_GetItem(item, 1));

                std::string key_s = PyObjectToUTF8(key.get());
                if (PyErr_Occurred())
                {
                    throw DataTypeException("Invalid Key in Map");
                }

                push_field_level("[\"" + key_s + "\"]", type2);
                boost::intrusive_ptr<MessageElement> el = PackMessageElement(val.get(), type2, obj, node);

                el->ElementName = key_s;
                mret.push_back(el);
                pop_field_level();
            }

            element->DataCount = mret.size();
            element->SetData(CreateMessageElementNestedElementList(DataTypes_dictionary_t, "", mret));
            return element;
        }

        if (IsTypeNumeric(type1->Type) && type1->ArrayType == DataTypes_ArrayTypes_multidimarray)
        {
            if (PyArray_Check(data) || PySequence_Check(data))
            {
                boost::intrusive_ptr<MessageElementNestedElementList> mm = PackToRRMultiDimArray_numpy(data, type1);
                element->SetData(mm);
                if (type1)
                {
                    if (!type1->ArrayVarLength)
                    {
                        VerifyMultiDimArrayLength(mm, type1);
                    }
                }

                return element;
            }
        }

        if (type1->Type == DataTypes_namedtype_t)
        {
            std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
            RR_SHARED_PTR<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, node, obj);
            switch (nt->RRDataType())
            {
            case DataTypes_structure_t: {

                RR_SHARED_PTR<ServiceEntryDefinition> struct_def = RR_STATIC_POINTER_CAST<ServiceEntryDefinition>(nt);
                std::string typestr = struct_def->ResolveQualifiedName();

                PyAutoPtr<PyObject> rrstructtype(PyObject_Type(data));
                if (!rrstructtype.get())
                    throw DataTypeException("Invalid structure, could not get Python type");
                PyAutoPtr<PyObject> rrstructtype_name(PyObject_GetAttrString(rrstructtype.get(), "__name__"));
                if (!rrstructtype_name.get())
                    throw DataTypeException("Invalid structure, could not get Python type name");
                std::string typestr2 = PyObjectToUTF8(rrstructtype_name.get());

                if (boost::replace_all_copy(typestr, ".", "__") != typestr2)
                {
                    throw DataTypeException("Structure type mismatch");
                }

                /*PyAutoPtr<PyObject> rrstructtype(PyObject_GetAttrString(data, "rrstructtype"));
                if (!rrstructtype.get()) throw DataTypeException("Invalid structure, missing rrstructtype");
                std::string typestr2 = PyObjectToUTF8(rrstructtype.get());
                if (PyErr_Occurred())
                {
                    throw DataTypeException("Invalid structure, invalid rrstructtype");
                }

                if (typestr != typestr2)
                {
                    throw DataTypeException("Structure type mismatch");
                }*/

                element->ElementType = DataTypes_structure_t;
                element->ElementTypeName = typestr;

                std::vector<boost::intrusive_ptr<MessageElement> > mret;
                BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& m_def, struct_def->Members)
                {
                    boost::shared_ptr<PropertyDefinition> p_def =
                        boost::dynamic_pointer_cast<PropertyDefinition>(m_def);
                    if (!p_def)
                        throw ServiceException("Invalid structure field definition " + typestr);

                    PyAutoPtr<PyObject> field_obj(PyObject_GetAttrString(data, p_def->Name.c_str()));
                    if (field_obj.get() == NULL)
                    {
                        throw DataTypeException("Field " + p_def->Name + " missing");
                    }

                    push_field_level(p_def->Name, p_def->Type);
                    boost::intrusive_ptr<MessageElement> el =
                        PackMessageElement(field_obj.get(), p_def->Type, obj, node);
                    el->ElementName = p_def->Name;
                    mret.push_back(el);
                    pop_field_level();
                }

                element->DataCount = mret.size();
                element->SetData(CreateMessageElementNestedElementList(DataTypes_structure_t, typestr, mret));
                return element;
            }
            case DataTypes_enum_t: {
                RR_SHARED_PTR<TypeDefinition> enum_type = RR_MAKE_SHARED<TypeDefinition>();
                enum_type->Type = DataTypes_int32_t;
                enum_type->Name = "value";
                element->SetData(PackToRRArray(data, enum_type, boost::intrusive_ptr<RRBaseArray>()));
                return element;
            }
            case DataTypes_pod_t:
            case DataTypes_namedarray_t: {

                if (type1->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    std::vector<boost::intrusive_ptr<MessageElement> > map_vec;
                    if (!PyArray_Check((PyArrayObject*)data))
                    {
                        throw new DataTypeException("Expected numpy.ndarray for pods and namedarrays");
                    }

                    int npy_dimcount = PyArray_NDIM((PyArrayObject*)data);
                    RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims =
                        AllocateRRArray<uint32_t>(boost::numeric_cast<size_t>(npy_dimcount));
                    for (size_t i = 0; i < boost::numeric_cast<size_t>(npy_dimcount); i++)
                    {
                        npy_intp s;
                        s = PyArray_DIM((PyArrayObject*)data, (int)i);
                        (*dims)[i] = boost::numeric_cast<uint32_t>(s);
                    }

                    boost::shared_ptr<TypeDefinition> dims_type = boost::make_shared<TypeDefinition>();
                    dims_type->Type = DataTypes_int32_t;
                    dims_type->ArrayType = DataTypes_ArrayTypes_array;
                    dims_type->ArrayVarLength = true;
                    dims_type->ArrayLength.push_back(0);

                    map_vec.push_back(CreateMessageElement("dims", dims));

                    boost::shared_ptr<TypeDefinition> array_type = boost::make_shared<TypeDefinition>();
                    type1->CopyTo(*array_type);
                    array_type->ArrayType = DataTypes_ArrayTypes_array;
                    array_type->ArrayLength.clear();
                    array_type->ArrayVarLength = true;
                    array_type->Name = "array";

                    map_vec.push_back(PackMessageElement(data, array_type, obj, node));

                    switch (nt->RRDataType())
                    {
                    case DataTypes_pod_t: {
                        boost::intrusive_ptr<MessageElementNestedElementList> mm =
                            CreateMessageElementNestedElementList(
                                DataTypes_pod_multidimarray_t,
                                type1->ResolveNamedType(empty_defs, node, obj)->ResolveQualifiedName(), map_vec);
                        element->SetData(mm);
                        return element;
                    }
                    case DataTypes_namedarray_t: {
                        boost::intrusive_ptr<MessageElementNestedElementList> mm =
                            CreateMessageElementNestedElementList(
                                DataTypes_namedarray_multidimarray_t,
                                type1->ResolveNamedType(empty_defs, node, obj)->ResolveQualifiedName(), map_vec);
                        element->SetData(mm);
                        return element;
                    }
                    default:
                        throw InternalErrorException("");
                    }
                }
                else
                {
                    switch (nt->RRDataType())
                    {
                    case DataTypes_pod_t:
                        element->SetData(PackMessageElement_pod(data, type1, obj, node));
                        return element;
                    case DataTypes_namedarray_t:
                        element->SetData(PackMessageElement_namedarray(data, type1, obj, node));
                        return element;
                    default:
                        throw InternalErrorException("");
                    }
                }
            }
            default:
                throw DataTypeException("Unknown named type id");
            }
        }

        if (type1->Type == DataTypes_string_t)
        {
#if (PY_MAJOR_VERSION == 2)

            if (PyUnicode_Check(data))
            {
                PyAutoPtr<PyObject> str1(PyUnicode_AsUTF8String(data));
                if (!str1.get())
                {
                    throw DataTypeException("Invalid string");
                }
                char* str_dat;
                Py_ssize_t str_len;
                if (PyString_AsStringAndSize(data, &str_dat, &str_len) < 0)
                {
                    throw DataTypeException("Invalid string");
                }
                element->SetData(AttachRRArrayCopy<char>(str_dat, str_len));
                return element;
            }
            else if (PyString_Check(data))
            {
                char* str_dat;
                Py_ssize_t str_len;
                if (PyString_AsStringAndSize(data, &str_dat, &str_len) < 0)
                {
                    throw DataTypeException("Invalid string");
                }
                element->SetData(AttachRRArrayCopy<char>(str_dat, str_len));
                return element;
            }
            else
            {
                throw DataTypeException("Invalid string");
            }
#else
            if (PyUnicode_Check(data))
            {
                Py_ssize_t str_len;
                const char* str_dat = PyUnicode_AsUTF8AndSize(data, &str_len);
                if (!str_dat)
                {
                    throw DataTypeException("Invalid string");
                }
                element->SetData(AttachRRArrayCopy<char>(str_dat, str_len));
                return element;
            }
            else
            {
                throw DataTypeException("Invalid string");
            }
#endif
        }

        if (IsTypeNumeric(type1->Type))
        {
            element->SetData(PackToRRArray(data, type1, boost::intrusive_ptr<RRBaseArray>()));
            return element;
        }

        throw DataTypeException("Could not pack message element due to unexpected type");
    }

    std::vector<std::string> field_name;
    std::vector<RR_SHARED_PTR<TypeDefinition> > field_type;

    void push_field_level(const std::string& new_var, const RR_SHARED_PTR<TypeDefinition>& new_type)
    {
        field_name.push_back(new_var);
        field_type.push_back(new_type);
    }

    void pop_field_level()
    {
        field_name.pop_back();
        field_type.pop_back();
    }

    boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > get_current_field()
    {
        if (field_name.empty())
        {
            return boost::make_tuple("**internal error**", RR_SHARED_PTR<TypeDefinition>());
        }

        std::string name = boost::join(field_name, ".");
        boost::replace_all(name, ".[", "[");
        RR_SHARED_PTR<TypeDefinition> type = field_type.back();

        return boost::make_tuple(name, type);
    }

    std::string get_exception_message(const std::string& msg)
    {
        boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > f = get_current_field();

        std::string msg2 = msg + " for field \"" + f.get<0>() + "\"";
        if (f.get<1>())
        {
            RR_SHARED_PTR<TypeDefinition> f2 = f.get<1>()->Clone();
            if (f2->Type == DataTypes_namedtype_t)
            {
                try
                {
                    f2->TypeString = f.get<1>()->ResolveNamedType()->ResolveQualifiedName();
                }
                catch (std::exception& e)
                {}
            }
            std::vector<std::string> f_split;
            std::string f2_str = f2->ToString();
            boost::split(f_split, f2_str, boost::is_any_of(" \t"), boost::token_compress_on);
            msg2 += " expected Robot Raconteur type \"" + f_split.at(0) + "\"";
        }

        return msg2;
    }
};

boost::intrusive_ptr<MessageElement> PackMessageElement(PyObject* data, const boost::shared_ptr<TypeDefinition>& type1,
                                                        const boost::shared_ptr<WrappedServiceStub>& obj,
                                                        boost::shared_ptr<RobotRaconteurNode> node)
{
    PackMessageElementImpl p;
    if (type1)
    {
        p.push_field_level(type1->Name, type1);
    }
    else
    {
        p.push_field_level("value", RR_SHARED_PTR<TypeDefinition>());
    }
    try
    {
        return p.PackMessageElement(data, type1, obj, node);
    }
    catch (DataTypeException& e)
    {
        e.Message = p.get_exception_message(e.Message);
        throw;
    }
    catch (InvalidArgumentException& e)
    {
        e.Message = p.get_exception_message(e.Message);
        throw;
    }
}

boost::intrusive_ptr<MessageElement> PackMessageElement(PyObject* data, const boost::shared_ptr<TypeDefinition>& type1,
                                                        const boost::shared_ptr<ServerContext>& obj,
                                                        boost::shared_ptr<RobotRaconteurNode> node)
{
    if (!node && obj)
    {
        node = obj->GetNode();
    }

    return PackMessageElement(data, type1, boost::shared_ptr<WrappedServiceStub>(), node);
}

class UnpackMessageElementImpl
{
  public:
    PyObject* UnpackMessageElement_namedarray(const boost::intrusive_ptr<MessageElement>& element,
                                              const boost::shared_ptr<TypeDefinition>& type1,
                                              const boost::shared_ptr<WrappedServiceStub>& stub,
                                              const boost::shared_ptr<RobotRaconteurNode>& node)
    {
        std::string typestr = element->ElementTypeName.str().to_string();

        boost::intrusive_ptr<MessageElementNestedElementList> l =
            element->CastDataToNestedList(DataTypes_namedarray_array_t);

        RR_SHARED_PTR<TypeDefinition> type2 = RR_MAKE_SHARED<TypeDefinition>();
        type2->Type = DataTypes_namedtype_t;
        type2->TypeString = typestr;
        std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
        RR_SHARED_PTR<ServiceEntryDefinition> n =
            rr_cast<ServiceEntryDefinition>(type2->ResolveNamedType(other_defs, node, stub));
        if (n->RRDataType() != DataTypes_namedarray_t)
            throw DataTypeException("Invalid pod or namedarray type");
        PyAutoPtr<PyObject> a_descr(GetNumPyDescrForType(typestr, stub, node));

        boost::tuple<DataTypes, size_t> s = GetNamedArrayElementTypeAndCount(n, other_defs, node, stub);

        RR_INTRUSIVE_PTR<RRBaseArray> a = MessageElement::FindElement(l->Elements, "array")->CastData<RRBaseArray>();
        if (!a)
            throw DataTypeException("NamedArray must not be null");
        if (a->size() % s.get<1>() != 0)
            throw DataTypeException("Invalid length for NamedArray");

        if (type1)
        {
            if (a->GetTypeID() != s.get<0>())
                throw DataTypeException("Invalid NamedArray");

            uint32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<uint32_t>());
            if (!type1->ArrayLength.empty() && c != 0)
            {

                if (type1->ArrayVarLength)
                {
                    if (a->size() / s.get<1>() > c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
                else
                {
                    if (a->size() / s.get<1>() != c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
            }
        }

        npy_intp a_dims = boost::lexical_cast<npy_intp>(a->size() / s.get<1>());
        PyAutoPtr<PyObject> array2(PyArray_NewFromDescr(&PyArray_Type, (PyArray_Descr*)a_descr.get(), 1, &a_dims, NULL,
                                                        a->void_ptr(), NPY_ARRAY_F_CONTIGUOUS | NPY_ARRAY_WRITEABLE,
                                                        NULL));

        Py_XINCREF(a_descr.get());

        PyAutoPtr<PyObject> ret(PyArray_NewCopy((PyArrayObject*)array2.get(), NPY_FORTRANORDER));
        if (ret.get() == NULL)
        {
            throw InternalErrorException("Could not allocate numpy array");
        }

        return ret.detach();
    }

    PyObject* UnpackMessageElement_pod(const boost::intrusive_ptr<MessageElement>& element,
                                       const boost::shared_ptr<TypeDefinition>& type1,
                                       const boost::shared_ptr<WrappedServiceStub>& stub,
                                       const boost::shared_ptr<RobotRaconteurNode>& node)
    {
        std::string typestr = element->ElementTypeName.str().to_string();
        boost::tuple<boost::string_ref, boost::string_ref> typestr_s = SplitQualifiedName(typestr);
        boost::shared_ptr<ServiceDefinition> def;
        if (!stub)
        {
            def = node->GetServiceType(typestr_s.get<0>())->ServiceDef();
            ;
        }
        else
        {
            def = node->GetPulledServiceType(stub, typestr_s.get<0>())->ServiceDef();
        }

        boost::shared_ptr<ServiceEntryDefinition> struct_def = find_by_name(def->Pods, typestr_s.get<1>());

        boost::intrusive_ptr<MessageElementNestedElementList> l = element->CastDataToNestedList(DataTypes_pod_array_t);

        PyAutoPtr<PyObject> a_descr(GetNumPyDescrForType(typestr, stub, node));

        npy_intp a_dims = boost::lexical_cast<npy_intp>(l->Elements.size());

        PyAutoPtr<PyObject> a(PyArray_SimpleNewFromDescr(1, &a_dims, (PyArray_Descr*)a_descr.get()));
        Py_XINCREF(a_descr.get());

        if (type1)
        {
            uint32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<uint32_t>());
            if (!type1->ArrayLength.empty() && c != 0)
            {

                if (type1->ArrayVarLength)
                {
                    if (l->Elements.size() > c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
                else
                {
                    if (l->Elements.size() != c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
            }
        }

        for (uint32_t i = 0; i < boost::numeric_cast<uint32_t>(l->Elements.size()); i++)
        {
            boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

            if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
            {
                if (i != el1->ElementNumber)
                    throw DataTypeException("Invalid pod array specified for " + element->ElementName.str());
            }
            else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
            {
                if (i != boost::lexical_cast<int32_t>(el1->ElementName.str()))
                    throw DataTypeException("Invalid list specified for " + element->ElementName.str());
            }
            else
            {
                throw DataTypeException("Invalid pod array specified for " + element->ElementName.str());
            }

            PyAutoPtr<PyObject> el2_ind(PyLong_FromLong(i));
            PyAutoPtr<PyObject> el2(PyObject_GetItem(a.get(), el2_ind.get()));

            boost::intrusive_ptr<MessageElementNestedElementList> s = el1->CastDataToNestedList(DataTypes_pod_t);

            BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& m, struct_def->Members)
            {
                boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
                boost::intrusive_ptr<MessageElement> el = MessageElement::FindElement(s->Elements, p->Name);

                RR_SHARED_PTR<TypeDefinition> p1 = p->Type;
                if (p->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    p1 = RR_MAKE_SHARED<TypeDefinition>();
                    p->Type->CopyTo(*p1);
                    int32_t c = boost::accumulate(p1->ArrayLength, 1, std::multiplies<int32_t>());
                    p1->ArrayLength.clear();
                    p1->ArrayLength.push_back(c);
                    p1->ArrayType = DataTypes_ArrayTypes_array;
                    p1->ArrayVarLength = false;
                }

                std::string& py_name_str = p->Name;
                PyAutoPtr<PyObject> py_name = stringToPyObject(py_name_str);

                PyObject* el1 = NULL;

                if (p1->Type == DataTypes_namedtype_t)
                {
                    std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                    if (p1->ResolveNamedType(empty_defs, node, stub)->RRDataType() == DataTypes_pod_t)
                    {
                        el1 = UnpackMessageElement_pod(el, p1, stub, node);
                    }
                    else
                    {
                        el1 = UnpackMessageElement_namedarray(el, p1, stub, node);
                    }
                }
                else
                {
                    el1 = (UnpackMessageElement(el, p1, stub, node));
                }

                if (p->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    std::vector<npy_intp> dims2(p->Type->ArrayLength.size());
                    for (size_t i = 0; i < p->Type->ArrayLength.size(); i++)
                    {
                        dims2[i] = boost::lexical_cast<npy_intp>(p->Type->ArrayLength[i]);
                    }

                    /*if (PyObject_SetItem(el2.get(), py_name.get(), el1) != 0)
                    {
                        PyErr_Print();
                        Py_XDECREF(el1);
                        throw DataTypeException("Could not set pod field");
                    }*/

                    PyArray_Dims dims3;
                    dims3.ptr = &dims2[0];
                    dims3.len = (int)dims2.size();

                    PyAutoPtr<PyObject> el3(PyArray_Newshape((PyArrayObject*)el1, &dims3, NPY_FORTRANORDER));

                    if (PyObject_SetItem(el2.get(), py_name.get(), el3.get()) != 0)
                    {
                        PyErr_Print();
                        Py_XDECREF(el1);
                        throw DataTypeException("Could not set pod field");
                    }
                }
                else if (p->Type->ArrayType == DataTypes_ArrayTypes_array && p->Type->ArrayVarLength)
                {
                    PyAutoPtr<PyObject> el3(PyObject_GetItem(el2.get(), py_name.get()));
                    PyAutoPtr<PyObject> py_len_str = stringToPyObject("len");
                    PyAutoPtr<PyObject> py_len(PyLong_FromLong(PyArray_SIZE((PyArrayObject*)el1)));
                    PyAutoPtr<PyObject> py_array_str = stringToPyObject("array");

                    int ret = PyObject_SetItem(el3.get(), py_len_str.get(), py_len.get());

                    if (ret != 0)
                    {
                        PyErr_Print();
                        Py_XDECREF(el1);
                        throw DataTypeException("Could not set pod field");
                    }

                    // if (el->DataCount > 0)
                    {

                        PyArray_Dims el4_dims;
                        el4_dims.len = 1;
                        npy_intp el4_dims_ptr = p->Type->ArrayLength.at(0);
                        el4_dims.ptr = &el4_dims_ptr;

                        PyAutoPtr<PyObject> el4(
                            (PyObject*)PyArray_Resize((PyArrayObject*)el1, &el4_dims, NPY_TRUE, NPY_FORTRANORDER));
                        if (el4.get() == NULL || PyObject_SetItem(el3.get(), py_array_str.get(), el1) != 0)
                        {
                            PyErr_Print();
                            Py_XDECREF(el1);
                            throw DataTypeException("Could not set pod field");
                        }
                    }
                    /*else
                    {
                        PyAutoPtr<PyObject> el5(PyObject_GetItem(el3.get(), py_array_str.get()));
                        PyAutoPtr<PyObject> py_zero = PyLong_FromLong(0);
                        PyArray_FillWithScalar((PyArrayObject*)el5.get(), py_zero.get());

                    }*/
                }
                else
                {
                    if (PyObject_SetItem(el2.get(), py_name.get(), el1) != 0)
                    {
                        PyErr_Print();
                        Py_XDECREF(el1);
                        throw DataTypeException("Could not set pod field");
                    }
                }
                Py_XDECREF(el1);
            }
        }

        if (type1)
        {
            if (type1->ArrayType == DataTypes_ArrayTypes_none)
            {
                if (PySequence_Length(a.get()) != 1)
                {
                    throw DataTypeException("Scalar array dimension mismatch");
                }
            }
        }

        return a.detach();
    }

    PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element,
                                   const boost::shared_ptr<TypeDefinition>& type1,
                                   const boost::shared_ptr<WrappedServiceStub>& stub,
                                   boost::shared_ptr<RobotRaconteurNode> node)
    {
        if (!element)
            throw NullValueException("element must not be null for UnpackMessageElement");

        if (!node)
        {
            if (stub)
            {
                node = stub->RRGetNode();
            }
            else
            {
                node = RobotRaconteurNode::sp();
            }
        }

        if (type1)
        {
            if (type1->Type == DataTypes_void_t)
            {
                Py_RETURN_NONE;
            }

            if (type1->Type == DataTypes_varvalue_t)
            {
                if (element->ElementType == DataTypes_void_t)
                {
                    Py_RETURN_NONE;
                }

                if (type1->ContainerType == DataTypes_ContainerTypes_none)
                {

                    boost::shared_ptr<TypeDefinition> type1_3t;
                    switch (element->ElementType)
                    {
                    case DataTypes_vector_t: {
                        type1_3t = boost::make_shared<TypeDefinition>();
                        type1_3t->Type = DataTypes_varvalue_t;
                        type1_3t->ContainerType = DataTypes_ContainerTypes_map_int32;
                        type1_3t->Name = "value";
                        break;
                    }
                    case DataTypes_dictionary_t: {
                        type1_3t = boost::make_shared<TypeDefinition>();
                        type1_3t->Type = DataTypes_varvalue_t;
                        type1_3t->ContainerType = DataTypes_ContainerTypes_map_string;
                        type1_3t->Name = "value";
                        break;
                    }
                    case DataTypes_list_t: {
                        type1_3t = boost::make_shared<TypeDefinition>();
                        type1_3t->Type = DataTypes_varvalue_t;
                        type1_3t->ContainerType = DataTypes_ContainerTypes_list;
                        type1_3t->Name = "value";
                        break;
                    }
                    default:
                        break;
                    }

                    PyAutoPtr<PyObject> vardata(UnpackMessageElement(element, type1_3t, stub, node));

                    boost::shared_ptr<TypeDefinition> type1_2t = boost::make_shared<TypeDefinition>();
                    if (IsTypeNumeric(element->ElementType))
                    {
                        type1_2t->ArrayType = DataTypes_ArrayTypes_array;
                        type1_2t->ArrayVarLength = true;
                        type1_2t->Type = element->ElementType;
                        type1_2t->ArrayLength.push_back(0);
                    }
                    else if (element->ElementType == DataTypes_string_t)
                    {
                        type1_2t->Type = DataTypes_string_t;
                    }
                    else if (element->ElementType == DataTypes_structure_t)
                    {
                        type1_2t->Type = DataTypes_namedtype_t;
                        type1_2t->TypeString = element->ElementTypeName.str().to_string();
                    }
                    else if (element->ElementType == DataTypes_pod_t)
                    {
                        type1_2t->Type = DataTypes_namedtype_t;
                        type1_2t->TypeString = element->ElementTypeName.str().to_string();
                    }
                    else if (element->ElementType == DataTypes_pod_array_t)
                    {
                        type1_2t->Type = DataTypes_namedtype_t;
                        type1_2t->TypeString = element->ElementTypeName.str().to_string();
                        type1_2t->ArrayVarLength = true;
                        type1_2t->ArrayLength.push_back(0);
                        type1_2t->ArrayType = DataTypes_ArrayTypes_array;
                    }
                    else if (element->ElementType == DataTypes_pod_multidimarray_t)
                    {
                        type1_2t->Type = DataTypes_namedtype_t;
                        type1_2t->TypeString = element->ElementTypeName.str().to_string();
                        type1_2t->ArrayVarLength = true;
                        type1_2t->ArrayType = DataTypes_ArrayTypes_multidimarray;
                    }
                    else if (element->ElementType == DataTypes_namedarray_array_t)
                    {
                        type1_2t->Type = DataTypes_namedtype_t;
                        type1_2t->TypeString = element->ElementTypeName.str().to_string();
                        type1_2t->ArrayVarLength = true;
                        type1_2t->ArrayLength.push_back(0);
                        type1_2t->ArrayType = DataTypes_ArrayTypes_array;
                    }
                    else if (element->ElementType == DataTypes_namedarray_multidimarray_t)
                    {
                        type1_2t->Type = DataTypes_namedtype_t;
                        type1_2t->TypeString = element->ElementTypeName.str().to_string();
                        type1_2t->ArrayVarLength = true;
                        type1_2t->ArrayType = DataTypes_ArrayTypes_multidimarray;
                    }
                    else if (element->ElementType == DataTypes_list_t)
                    {
                        type1_2t->Type = DataTypes_varvalue_t;
                        type1_2t->ContainerType = DataTypes_ContainerTypes_list;
                    }
                    else if (element->ElementType == DataTypes_vector_t)
                    {
                        type1_2t->Type = DataTypes_varvalue_t;
                        type1_2t->ContainerType = DataTypes_ContainerTypes_map_int32;
                    }
                    else if (element->ElementType == DataTypes_dictionary_t)
                    {
                        type1_2t->Type = DataTypes_varvalue_t;
                        type1_2t->ContainerType = DataTypes_ContainerTypes_map_string;
                    }
                    else if (element->ElementType == DataTypes_multidimarray_t)
                    {
                        type1_2t->ArrayType = DataTypes_ArrayTypes_multidimarray;
                        type1_2t->ArrayVarLength = true;
                        type1_2t->ArrayLength.push_back(0);
                        type1_2t->Type =
                            MessageElement::FindElement(
                                element->CastDataToNestedList(DataTypes_multidimarray_t)->Elements, "array")
                                ->ElementType;
                        if (!IsTypeNumeric(type1_2t->Type))
                            throw DataTypeException("Invalid MultiDimArray");
                    }
                    else
                    {
                        throw DataTypeException("Invalid data type for field " + type1->Name);
                    }

                    PyObject* modules = PyImport_GetModuleDict();
                    if (!modules)
                        throw InternalErrorException("Internal error");
                    PyObject* util = PyDict_GetItemString(modules, "RobotRaconteur.RobotRaconteurPythonUtil");
                    if (!util)
                        throw InvalidOperationException("RobotRaconteur.RobotRaconteurPythonUtil not loaded");
                    PyAutoPtr<PyObject> varvalue_type(PyObject_GetAttrString(util, "RobotRaconteurVarValue"));
                    if (!varvalue_type.get())
                        throw InvalidArgumentException("Could not find RobotRaconteurVarValue type");

                    std::string type1_2t_str = type1_2t->ToString();
                    return PyObject_CallFunction(varvalue_type.get(), "O,s#", vardata.get(), type1_2t_str.c_str(),
                                                 type1_2t_str.size());
                }
                else
                {
                    if (element->ElementType == DataTypes_list_t)
                    {
                        boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
                        type2->Type = DataTypes_varvalue_t;
                        type2->Name = "value";

                        boost::intrusive_ptr<MessageElementNestedElementList> l =
                            element->CastDataToNestedList(DataTypes_list_t);
                        PyAutoPtr<PyObject> ret(PyList_New(l->Elements.size()));

                        for (uint32_t i = 0; i < boost::numeric_cast<uint32_t>(l->Elements.size()); i++)
                        {
                            boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                            if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                            {
                                if (i != el1->ElementNumber)
                                    throw DataTypeException("Invalid list");
                            }
                            else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                            {
                                if (i != boost::lexical_cast<int32_t>(el1->ElementName.str()))
                                    throw DataTypeException("Invalid list");
                            }
                            else
                            {
                                throw DataTypeException("Invalid list");
                            }

                            push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);
                            PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
                            PyList_SetItem(ret.get(), i, el2.get());
                            Py_XINCREF(el2.get());
                            pop_field_level();
                        }

                        return ret.detach();
                    }

                    if (element->ElementType == DataTypes_vector_t)
                    {
                        boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
                        type2->Type = DataTypes_varvalue_t;
                        type2->Name = "value";

                        boost::intrusive_ptr<MessageElementNestedElementList> l =
                            element->CastDataToNestedList(DataTypes_vector_t);
                        PyAutoPtr<PyObject> ret(PyDict_New());

                        for (int32_t i = 0; i < boost::numeric_cast<int32_t>(l->Elements.size()); i++)
                        {
                            boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                            int32_t i2;
                            if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                            {
                                i2 = el1->ElementNumber;
                            }
                            else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                            {
                                i2 = boost::lexical_cast<int32_t>(el1->ElementName.str());
                            }
                            else
                            {
                                throw DataTypeException("Invalid int32 map specified");
                            }

                            push_field_level("[" + boost::lexical_cast<std::string>(i2) + "]", type2);
                            PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
                            PyDict_SetItem(ret.get(), PyLong_FromLong(i2), el2.get());
                            pop_field_level();
                        }

                        return ret.detach();
                    }

                    if (element->ElementType == DataTypes_dictionary_t)
                    {
                        boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
                        type2->Type = DataTypes_varvalue_t;
                        type2->Name = "value";

                        boost::intrusive_ptr<MessageElementNestedElementList> l =
                            element->CastDataToNestedList(DataTypes_dictionary_t);
                        PyAutoPtr<PyObject> ret(PyDict_New());

                        for (int32_t i = 0; i < boost::numeric_cast<int32_t>(l->Elements.size()); i++)
                        {
                            boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                            push_field_level("[\"" + el1->ElementName.str() + "\"]", type2);
                            PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
                            PyDict_SetItem(ret.get(), stringToPyObject(el1->ElementName.str()).get(), el2.get());
                            pop_field_level();
                        }

                        return ret.detach();
                    }
                }

                throw DataTypeException("Invalid message collection type");
            }
        }

        if (element->ElementType == DataTypes_void_t)
        {
            if (type1 && (type1->ContainerType == DataTypes_ContainerTypes_none))
            {
                std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                if (IsTypeNumeric(type1->Type))
                {
                    throw DataTypeException("Scalars and arrays must not be None");
                }
                if (type1->Type == DataTypes_string_t)
                {
                    throw DataTypeException("Strings must not be None");
                }
                if (type1->ResolveNamedType(empty_defs, node, stub)->RRDataType() == DataTypes_pod_t)
                {
                    throw DataTypeException("Pods must not be None");
                }
            }

            Py_RETURN_NONE;
        }

        if (IsTypeNumeric(element->ElementType))
        {
            if (type1 && type1->Type == DataTypes_namedtype_t)
            {
                std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                RR_SHARED_PTR<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, node, stub);
                switch (nt->RRDataType())
                {
                case DataTypes_enum_t: {
                    RR_SHARED_PTR<TypeDefinition> enum_type = RR_MAKE_SHARED<TypeDefinition>();
                    enum_type->Type = DataTypes_int32_t;
                    enum_type->Name = "value";
                    return UnpackFromRRArray(element->CastData<RRBaseArray>(), enum_type);
                }
                default:
                    break;
                }
            }

            return UnpackFromRRArray(element->CastData<RRBaseArray>(), type1);
        }

        if (element->ElementType == DataTypes_string_t)
        {
            boost::intrusive_ptr<RRArray<char> > s1 = element->CastData<RRArray<char> >();
            return PyUnicode_DecodeUTF8(s1->data(), s1->size(), "Invalid UTF-8 String");
        }

        if (element->ElementType == DataTypes_structure_t)
        {

            std::string typestr = element->ElementTypeName.str().to_string();
            boost::tuple<boost::string_ref, boost::string_ref> typestr_s = SplitQualifiedName(typestr);
            boost::shared_ptr<ServiceDefinition> def;
            if (!stub)
            {
                def = node->GetServiceType(typestr_s.get<0>())->ServiceDef();
                ;
            }
            else
            {
                def = node->GetPulledServiceType(stub, typestr_s.get<0>())->ServiceDef();
            }

            boost::shared_ptr<ServiceEntryDefinition> struct_def = find_by_name(def->Structures, typestr_s.get<1>());

            PyAutoPtr<PyObject> ret(NewStructure(typestr, stub, node));

            boost::intrusive_ptr<MessageElementNestedElementList> s =
                element->CastDataToNestedList(DataTypes_structure_t);

            BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& m, struct_def->Members)
            {
                boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
                push_field_level(p->Name, p->Type);
                boost::intrusive_ptr<MessageElement> el = MessageElement::FindElement(s->Elements, p->Name);
                PyAutoPtr<PyObject> el1 = UnpackMessageElement(el, p->Type, stub, node);
                PyObject_SetAttrString(ret.get(), p->Name.c_str(), el1.get());
                pop_field_level();
            }

            return ret.detach();
        }

        if (element->ElementType == DataTypes_list_t)
        {
            boost::shared_ptr<TypeDefinition> type2;

            if (type1)
            {
                type2 = type1->Clone();
                type2->RemoveContainers();
            }

            boost::intrusive_ptr<MessageElementNestedElementList> l = element->CastDataToNestedList(DataTypes_list_t);
            PyAutoPtr<PyObject> ret(PyList_New(l->Elements.size()));

            for (uint32_t i = 0; i < boost::numeric_cast<uint32_t>(l->Elements.size()); i++)
            {
                boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);
                if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                {
                    if (i != el1->ElementNumber)
                        throw DataTypeException("Invalid list specified");
                }
                else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                {
                    if (i != boost::lexical_cast<int32_t>(el1->ElementName.str()))
                        throw DataTypeException("Invalid list specified for " + type1->Name);
                }
                else
                {
                    throw DataTypeException("Invalid list specified");
                }

                PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
                PyList_SetItem(ret.get(), i, el2.get());
                Py_XINCREF(el2.get());
                pop_field_level();
            }

            return ret.detach();
        }

        if (element->ElementType == DataTypes_vector_t)
        {
            boost::shared_ptr<TypeDefinition> type2;
            if (type1)
            {
                type2 = type1->Clone();
                type2->RemoveContainers();
            }

            boost::intrusive_ptr<MessageElementNestedElementList> l = element->CastDataToNestedList(DataTypes_vector_t);
            PyAutoPtr<PyObject> ret(PyDict_New());

            for (int32_t i = 0; i < boost::numeric_cast<int32_t>(l->Elements.size()); i++)
            {
                boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                int32_t i2;
                if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                {
                    i2 = el1->ElementNumber;
                }
                else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                {
                    i2 = boost::lexical_cast<int32_t>(el1->ElementName.str());
                }
                else
                {
                    throw DataTypeException("Invalid list");
                }

                push_field_level("[" + boost::lexical_cast<std::string>(i2) + "]", type2);
                PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
#if (PY_MAJOR_VERSION == 2)
                PyDict_SetItem(ret.get(), PyInt_FromLong(i2), el2.get());
#else
                PyDict_SetItem(ret.get(), PyLong_FromLong(i2), el2.get());
#endif
                pop_field_level();
            }

            return ret.detach();
        }

        if (element->ElementType == DataTypes_dictionary_t)
        {
            boost::shared_ptr<TypeDefinition> type2;

            if (type1)
            {
                type2 = type1->Clone();
                type2->RemoveContainers();
            }

            boost::intrusive_ptr<MessageElementNestedElementList> l =
                element->CastDataToNestedList(DataTypes_dictionary_t);
            PyAutoPtr<PyObject> ret(PyDict_New());

            for (int32_t i = 0; i < boost::numeric_cast<int32_t>(l->Elements.size()); i++)
            {
                boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                push_field_level("[" + el1->ElementName.str() + "]", type2);
                PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
                PyDict_SetItem(ret.get(), stringToPyObject(el1->ElementName.str()).get(), el2.get());
                pop_field_level();
            }

            return ret.detach();
        }

        if (element->ElementType == DataTypes_multidimarray_t)
        {
            boost::intrusive_ptr<MessageElementNestedElementList> mm =
                element->CastDataToNestedList(DataTypes_multidimarray_t);

            if (type1)
            {
                if (type1->ArrayType != DataTypes_ArrayTypes_multidimarray)
                    throw DataTypeException("MultiDimArray mismatch");
                if (!type1->ArrayVarLength)
                {
                    VerifyMultiDimArrayLength(mm, type1);
                }
            }

            return UnpackFromRRMultiDimArray_numpy(element->CastDataToNestedList(DataTypes_multidimarray_t), type1);
        }

        if (element->ElementType == DataTypes_pod_multidimarray_t)
        {
            boost::intrusive_ptr<MessageElementNestedElementList> mm =
                element->CastDataToNestedList(DataTypes_pod_multidimarray_t);

            boost::intrusive_ptr<RRArray<uint32_t> > dims_rr =
                MessageElement::FindElement(mm->Elements, "dims")->CastData<RRArray<uint32_t> >();
            if (!dims_rr)
                throw DataTypeException("Invalid PodMultiDimArray");

            boost::intrusive_ptr<MessageElement> array = MessageElement::FindElement(mm->Elements, "array");
            if (!array)
                throw DataTypeException("Invalid PodMultiDimArray");
            boost::shared_ptr<TypeDefinition> type2;
            if (type1)
            {
                type2 = boost::make_shared<TypeDefinition>();
                type1->CopyTo(*type2);
                type2->ArrayType = DataTypes_ArrayTypes_array;
                type2->ArrayVarLength = true;
                type2->ArrayLength.clear();
            }

            PyAutoPtr<PyObject> ret(UnpackMessageElement_pod(array, type2, stub, node));

            std::vector<npy_intp> dims2(dims_rr->size());
            for (size_t i = 0; i < dims_rr->size(); i++)
            {
                dims2[i] = boost::lexical_cast<npy_intp>((*dims_rr)[i]);
            }

            PyArray_Dims dims3;
            dims3.ptr = &dims2[0];
            dims3.len = (int)dims2.size();

            PyAutoPtr<PyObject> ret2(PyArray_Newshape((PyArrayObject*)ret.get(), &dims3, NPY_FORTRANORDER));

            return ret2.detach();
        }

        if (element->ElementType == DataTypes_pod_array_t)
        {
            return UnpackMessageElement_pod(element, type1, stub, node);
        }

        if (element->ElementType == DataTypes_namedarray_multidimarray_t)
        {
            boost::intrusive_ptr<MessageElementNestedElementList> mm =
                element->CastDataToNestedList(DataTypes_namedarray_multidimarray_t);

            boost::intrusive_ptr<RRArray<uint32_t> > dims_rr =
                MessageElement::FindElement(mm->Elements, "dims")->CastData<RRArray<uint32_t> >();
            if (!dims_rr)
                throw DataTypeException("Invalid PodMultiDimArray");

            boost::intrusive_ptr<MessageElement> array = MessageElement::FindElement(mm->Elements, "array");
            if (!array)
                throw DataTypeException("Invalid PodMultiDimArray");
            boost::shared_ptr<TypeDefinition> type2;
            if (type1)
            {
                type2 = boost::make_shared<TypeDefinition>();
                type1->CopyTo(*type2);
                type2->ArrayType = DataTypes_ArrayTypes_array;
                type2->ArrayVarLength = true;
                type2->ArrayLength.clear();
            }

            PyAutoPtr<PyObject> ret(UnpackMessageElement_namedarray(array, type2, stub, node));

            std::vector<npy_intp> dims2(dims_rr->size());
            for (size_t i = 0; i < dims_rr->size(); i++)
            {
                dims2[i] = boost::lexical_cast<npy_intp>((*dims_rr)[i]);
            }

            PyArray_Dims dims3;
            dims3.ptr = &dims2[0];
            dims3.len = (int)dims2.size();

            PyAutoPtr<PyObject> ret2(PyArray_Newshape((PyArrayObject*)ret.get(), &dims3, NPY_FORTRANORDER));

            return ret2.detach();
        }

        if (element->ElementType == DataTypes_namedarray_array_t)
        {
            return UnpackMessageElement_namedarray(element, type1, stub, node);
        }

        throw DataTypeException("Invalid data type");
    }

    std::vector<std::string> field_name;
    std::vector<RR_SHARED_PTR<TypeDefinition> > field_type;

    void push_field_level(const std::string& new_var, const RR_SHARED_PTR<TypeDefinition>& new_type)
    {
        field_name.push_back(new_var);
        field_type.push_back(new_type);
    }

    void pop_field_level()
    {
        field_name.pop_back();
        field_type.pop_back();
    }

    boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > get_current_field()
    {
        if (field_name.empty())
        {
            return boost::make_tuple("**internal error**", RR_SHARED_PTR<TypeDefinition>());
        }

        std::string name = boost::join(field_name, ".");
        boost::replace_all(name, ".[", "[");
        RR_SHARED_PTR<TypeDefinition> type = field_type.back();

        return boost::make_tuple(name, type);
    }

    std::string get_exception_message(const std::string& msg)
    {
        boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > f = get_current_field();

        std::string msg2 = msg + " for field \"" + f.get<0>() + "\"";
        if (f.get<1>())
        {
            RR_SHARED_PTR<TypeDefinition> f2 = f.get<1>()->Clone();
            if (f2->Type == DataTypes_namedtype_t)
            {
                try
                {
                    f2->TypeString = f.get<1>()->ResolveNamedType()->ResolveQualifiedName();
                }
                catch (std::exception& e)
                {}
            }
            std::vector<std::string> f_split;
            std::string f2_str = f2->ToString();
            boost::split(f_split, f2_str, boost::is_any_of(" \t"), boost::token_compress_on);
            msg2 += " expected Robot Raconteur type \"" + f_split.at(0) + "\"";
        }

        return msg2;
    }
};

PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element,
                               const boost::shared_ptr<TypeDefinition>& type1,
                               const boost::shared_ptr<WrappedServiceStub>& stub,
                               boost::shared_ptr<RobotRaconteurNode> node)
{
    UnpackMessageElementImpl u;
    if (type1)
    {
        u.push_field_level(type1->Name, type1);
    }
    else
    {
        u.push_field_level("value", RR_SHARED_PTR<TypeDefinition>());
    }
    try
    {
        return u.UnpackMessageElement(element, type1, stub, node);
    }
    catch (DataTypeException& e)
    {
        e.Message = u.get_exception_message(e.Message);
        throw;
    }
    catch (InvalidArgumentException& e)
    {
        e.Message = u.get_exception_message(e.Message);
        throw;
    }
}

PyObject* UnpackMessageElement(const boost::intrusive_ptr<MessageElement>& element,
                               const boost::shared_ptr<TypeDefinition>& type1,
                               const boost::shared_ptr<ServerContext>& obj, boost::shared_ptr<RobotRaconteurNode> node)
{
    if (!node && obj)
    {
        node = obj->GetNode();
    }

    return UnpackMessageElement(element, type1, boost::shared_ptr<WrappedServiceStub>(), node);
}

template <typename type2>
boost::intrusive_ptr<RRBaseArray> PackToRRArray1_int(PyObject* array_,
                                                     const boost::intrusive_ptr<RRBaseArray>& destrrarray)
{
    PyAutoPtr<PyObject> array_fast(PySequence_Fast(array_, "Internal error"));
    if (array_fast.get() == NULL)
    {
        throw InternalErrorException("Internal error");
    }

    size_t seq_len = (size_t)PySequence_Fast_GET_SIZE(array_fast.get());

    RR_INTRUSIVE_PTR<RRArray<type2> > array2;
    if (destrrarray)
    {
        array2 = RR_DYNAMIC_POINTER_CAST<RRArray<type2> >(destrrarray);
        if (!array2)
        {
            throw DataTypeException("Invalid destination array provided for PackToRRArray");
        }
    }
    else
    {
        array2 = AllocateRRArray<type2>(seq_len);
    }

    type2* buf = array2->data();

    for (size_t i = 0; i < seq_len; i++)
    {
        PyObject* v = PySequence_Fast_GET_ITEM(array_fast.get(), i);
#if (PY_MAJOR_VERSION == 2)
        if (PyInt_Check(v))
        {
            long v2 = PyInt_AS_LONG(v);
            if (std::numeric_limits<type2>::is_signed)
            {
                if (std::numeric_limits<long>::min() < std::numeric_limits<type2>::min())
                {
                    if (v2 < (long)std::numeric_limits<type2>::min())
                        throw DataTypeException("Number outside of range limit for specified type");
                }
                if (std::numeric_limits<long>::max() > std::numeric_limits<type2>::max())
                {
                    if (v2 > (long)std::numeric_limits<type2>::max())
                        throw DataTypeException("Number outside of range limit for specified type");
                }
            }
            else
            {
                if (v2 < 0)
                    throw DataTypeException("Number outside of range limit for specified type");
                if (std::numeric_limits<long>::max() > std::numeric_limits<type2>::max())
                {
                    if (v2 > (long)std::numeric_limits<type2>::max())
                        throw DataTypeException("Number outside of range limit for specified type");
                }
            }

            buf[i] = (type2)v2;
        }
        else
#endif
            if (PyLong_Check(v))
        {

            if (std::numeric_limits<type2>::is_signed)
            {
                long long v2 = PyLong_AsLongLong(v);
                if (PyErr_Occurred())
                {
                    throw DataTypeException("Invalid value in list provided to PackRRArray");
                }
                if (v2 < (long long)std::numeric_limits<type2>::min() ||
                    v2 > (long long)std::numeric_limits<type2>::max())
                    throw DataTypeException("Number outside of range limit for specified type");
                buf[i] = (type2)v2;
            }
            else
            {
                unsigned long long v2 = PyLong_AsUnsignedLongLong(v);
                if (PyErr_Occurred())
                {
                    PyErr_Print();
                    throw DataTypeException("Invalid value in list provided to PackRRArray");
                }

                if (v2 < (unsigned long long)std::numeric_limits<type2>::min() ||
                    v2 > (unsigned long long)std::numeric_limits<type2>::max())
                    throw DataTypeException("Number outside of range limit for specified type");
                buf[i] = (type2)v2;
            }
        }
        else if (PyArray_CheckScalar(v))
        {
            type2 v2;
            PyAutoPtr<PyArray_Descr> descr(RRTypeIdToNumPyDataType(array2->GetTypeID()));
            PyArray_CastScalarToCtype(v, &v2, descr.get());
            buf[i] = v2;
        }
        else
        {
            throw DataTypeException("Invalid value in list provided to PackRRArray");
        }
    }

    return array2;
}

template <typename type2>
boost::intrusive_ptr<RRBaseArray> PackToRRArray1_float(PyObject* array_,
                                                       const boost::intrusive_ptr<RRBaseArray>& destrrarray)
{
    PyAutoPtr<PyObject> array_fast(PySequence_Fast(array_, "Internal error"));
    if (array_fast.get() == NULL)
    {
        throw InternalErrorException("Internal error");
    }

    size_t seq_len = (size_t)PySequence_Fast_GET_SIZE(array_fast.get());

    RR_INTRUSIVE_PTR<RRArray<type2> > array2;
    if (destrrarray)
    {
        array2 = RR_DYNAMIC_POINTER_CAST<RRArray<type2> >(destrrarray);
        if (!array2)
        {
            throw DataTypeException("Invalid destination array provided for PackToRRArray");
        }
    }
    else
    {
        array2 = AllocateRRArray<type2>(seq_len);
    }

    type2* buf = array2->data();

    for (size_t i = 0; i < seq_len; i++)
    {
        PyObject* v = PySequence_Fast_GET_ITEM(array_fast.get(), i);

        double v2 = 0;
        if (PyFloat_Check(v))
        {
            v2 = PyFloat_AS_DOUBLE(v);
        }
#if (PY_MAJOR_VERSION == 2)
        else if (PyInt_Check(v))
        {
            v2 = boost::lexical_cast<double>(PyInt_AsLong(v));
        }
#endif
        else if (PyLong_Check(v))
        {
            v2 = PyLong_AsDouble(v);
        }
        else if (PyArray_CheckScalar(v))
        {
            PyAutoPtr<PyArray_Descr> descr = PyArray_DescrFromType(NPY_DOUBLE);
            PyArray_CastScalarToCtype(v, &v2, descr.get());
        }
        else
        {
            throw DataTypeException("Invalid value in list provided to PackRRArray");
        }

        if (PyErr_Occurred())
        {
            PyErr_Print();
            throw DataTypeException("Invalid value in list provided to PackRRArray");
        }
        if (v2 < -std::numeric_limits<type2>::max() || v2 > std::numeric_limits<type2>::max())
            throw DataTypeException("Number outside of range limit for specified type");
        buf[i] = (type2)v2;
    }

    return array2;
}

template <typename T>
struct complex_scalar_type
{
};

template <>
struct complex_scalar_type<cdouble>
{
    typedef double type;
};
template <>
struct complex_scalar_type<cfloat>
{
    typedef float type;
};

template <typename type2>
boost::intrusive_ptr<RRBaseArray> PackToRRArray1_complex(PyObject* array_,
                                                         const boost::intrusive_ptr<RRBaseArray>& destrrarray)
{
    PyAutoPtr<PyObject> array_fast(PySequence_Fast(array_, "Internal error"));
    if (array_fast.get() == NULL)
    {
        throw InternalErrorException("Internal error");
    }

    size_t seq_len = (size_t)PySequence_Fast_GET_SIZE(array_fast.get());

    RR_INTRUSIVE_PTR<RRArray<type2> > array2;
    if (destrrarray)
    {
        array2 = RR_DYNAMIC_POINTER_CAST<RRArray<type2> >(destrrarray);
        if (!array2)
        {
            throw DataTypeException("Invalid destination array provided for PackToRRArray");
        }
    }
    else
    {
        array2 = AllocateRRArray<type2>(seq_len);
    }

    type2* buf = array2->data();

    for (size_t i = 0; i < seq_len; i++)
    {
        PyObject* v = PySequence_Fast_GET_ITEM(array_fast.get(), i);

        type2 v2;
        if (PyComplex_Check(v))
        {
            v2.real = (typename complex_scalar_type<type2>::type)PyComplex_RealAsDouble(v);
            v2.imag = (typename complex_scalar_type<type2>::type)PyComplex_ImagAsDouble(v);
        }
        else if (PyArray_CheckScalar(v))
        {
            if (sizeof(type2) == sizeof(cdouble))
            {
                PyAutoPtr<PyArray_Descr> descr = PyArray_DescrFromType(NPY_COMPLEX128);
                PyArray_CastScalarToCtype(v, &v2, descr.get());
            }
            else if (sizeof(type2) == sizeof(cfloat))
            {
                PyAutoPtr<PyArray_Descr> descr = PyArray_DescrFromType(NPY_COMPLEX64);
                PyArray_CastScalarToCtype(v, &v2, descr.get());
            }
            else
            {
                throw new InternalErrorException("");
            }
        }
        else
        {
            throw DataTypeException("Invalid value in list provided to PackRRArray");
        }

        if (PyErr_Occurred())
        {
            throw DataTypeException("Invalid value in list provided to PackRRArray");
        }
        buf[i] = (type2)v2;
    }

    return array2;
}

boost::intrusive_ptr<RRBaseArray> PackToRRArray1_bool(PyObject* array_,
                                                      const boost::intrusive_ptr<RRBaseArray>& destrrarray)
{
    PyAutoPtr<PyObject> array_fast(PySequence_Fast(array_, "Internal error"));
    if (array_fast.get() == NULL)
    {
        throw InternalErrorException("Internal error");
    }

    size_t seq_len = (size_t)PySequence_Fast_GET_SIZE(array_fast.get());

    RR_INTRUSIVE_PTR<RRArray<rr_bool> > array2;
    if (destrrarray)
    {
        array2 = RR_DYNAMIC_POINTER_CAST<RRArray<rr_bool> >(destrrarray);
        if (!array2)
        {
            throw DataTypeException("Invalid destination array provided for PackToRRArray");
        }
    }
    else
    {
        array2 = AllocateRRArray<rr_bool>(seq_len);
    }

    rr_bool* buf = array2->data();

    for (size_t i = 0; i < seq_len; i++)
    {
        PyObject* v = PySequence_Fast_GET_ITEM(array_fast.get(), i);
        if (PyBool_Check(v))
        {
            rr_bool v2 = (rr_bool)PyObject_IsTrue(v);

            if (PyErr_Occurred())
            {
                throw DataTypeException("Invalid value in list provided to PackRRArray");
            }
            buf[i] = v2;
        }
        else
        {
            throw DataTypeException("Invalid value in list provided to PackRRArray");
        }
    }

    return array2;
}

boost::intrusive_ptr<RRBaseArray> PackToRRArray(PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1,
                                                boost::intrusive_ptr<RRBaseArray> destrrarray)
{
    if (!type1)
        throw NullValueException("PackToRRArray type must not be None");

    if (array_ == Py_None)
    {
        if (type1->ArrayType == DataTypes_ArrayTypes_none)
            throw DataTypeException("Scalar values must not be none");
        return boost::intrusive_ptr<RRBaseArray>();
    }

    if (PyArray_Check(array_))
    {
        return PackToRRArray_numpy(array_, type1, destrrarray);
    }

    if (PyNumber_Check(array_))
    {
        PyAutoPtr<PyObject> array1 = PyList_New(1);
        Py_XINCREF(array_);
        PyList_SetItem(array1.get(), 0, array_);
        return PackToRRArray(array1.get(), type1, destrrarray);
    }

#if (PY_MAJOR_VERSION == 2)
    if (type1->Type == DataTypes_uint8_t && (PyByteArray_Check(array_) || PyString_CheckExact(array_)))
    {
        Py_ssize_t bytearray_ssize;
        char* bytearray_buf;

        if (PyString_CheckExact(array_))
        {
            bytearray_ssize = PyString_Size(array_);
            bytearray_buf = PyString_AsString(array_);
        }
        else
        {
            bytearray_ssize = PyByteArray_Size(array_);
            bytearray_buf = PyByteArray_AsString(array_);
        }
#else
    if (type1->Type == DataTypes_uint8_t && PyByteArray_Check(array_))
    {
        Py_ssize_t bytearray_ssize;
        char* bytearray_buf;

        bytearray_ssize = PyByteArray_Size(array_);
        bytearray_buf = PyByteArray_AsString(array_);

#endif

        if (!bytearray_buf)
            throw DataTypeException("Invalid data argument for PackToRRArray");

        if (bytearray_ssize < 0)
            throw InternalErrorException("PackToRRArray internal error");
        size_t bytearray_size = (size_t)(bytearray_ssize);

        if (type1->ArrayType == DataTypes_ArrayTypes_none && (bytearray_size != 1))
            throw DataTypeException("Invalid array length for PackToRRArray");
        if (type1->ArrayType == DataTypes_ArrayTypes_array)
        {
            if (!type1->ArrayVarLength)
            {
                if (type1->ArrayLength.at(0) != bytearray_size)
                    throw DataTypeException("Invalid array length for PackToRRArray");
            }
            else if (type1->ArrayLength.at(0) != 0)
            {
                if (type1->ArrayLength.at(0) < bytearray_size)
                    throw DataTypeException("Array to long for PackToRRArray");
            }
        }

        if (destrrarray)
        {
            if (destrrarray->GetTypeID() != DataTypes_uint8_t || bytearray_size == destrrarray->size())
            {
                throw DataTypeException("Invalid destination array provided for PackToRRArray");
            }
        }
        else
        {
            destrrarray = AllocateRRArrayByType(DataTypes_uint8_t, bytearray_size);
        }

        memcpy(destrrarray->void_ptr(), bytearray_buf, bytearray_size);
        return destrrarray;
    }

    /*if (_NumPyAvailable)
    {
        if (PyArray_Check(array_))
        {
            return PackToRRArray_numpy(array_, type1, destrrarray);
        }
    }*/

    if (!PySequence_Check(array_))
    {
        throw DataTypeException("Invalid data argument for PackToRRArray");
    }

    Py_ssize_t seq_ssize = PySequence_Length(array_);
    if (seq_ssize < 0)
        throw DataTypeException("Invalid array length for PackToRRArray");
    size_t seq_size = (size_t)seq_ssize;

    if (type1->ArrayType == DataTypes_ArrayTypes_none && (seq_size != 1))
        throw DataTypeException("Invalid array length for PackToRRArray");
    if (type1->ArrayType == DataTypes_ArrayTypes_array)
    {
        if (!type1->ArrayVarLength)
        {
            if (type1->ArrayLength.at(0) != seq_size)
                throw DataTypeException("Invalid array length for PackToRRArray");
        }
        else if (type1->ArrayLength.at(0) != 0)
        {
            if (type1->ArrayLength.at(0) < seq_size)
                throw DataTypeException("Array too long for PackToRRArray");
        }
    }

    switch (type1->Type)
    {
    case DataTypes_double_t:
        return PackToRRArray1_float<double>(array_, destrrarray);
    case DataTypes_single_t:
        return PackToRRArray1_float<float>(array_, destrrarray);
    case DataTypes_int8_t:
        return PackToRRArray1_int<int8_t>(array_, destrrarray);
    case DataTypes_uint8_t:
        return PackToRRArray1_int<uint8_t>(array_, destrrarray);
    case DataTypes_int16_t:
        return PackToRRArray1_int<int16_t>(array_, destrrarray);
    case DataTypes_uint16_t:
        return PackToRRArray1_int<uint16_t>(array_, destrrarray);
    case DataTypes_int32_t:
        return PackToRRArray1_int<int32_t>(array_, destrrarray);
    case DataTypes_uint32_t:
        return PackToRRArray1_int<uint32_t>(array_, destrrarray);
    case DataTypes_int64_t:
        return PackToRRArray1_int<int64_t>(array_, destrrarray);
    case DataTypes_uint64_t:
        return PackToRRArray1_int<uint64_t>(array_, destrrarray);
    case DataTypes_cdouble_t:
        return PackToRRArray1_complex<cdouble>(array_, destrrarray);
    case DataTypes_csingle_t:
        return PackToRRArray1_complex<cfloat>(array_, destrrarray);
    case DataTypes_bool_t:
        return PackToRRArray1_bool(array_, destrrarray);
    default:
        throw DataTypeException("Unknown numeric data type");
    }
}

static PyObject* RR_PyInt_FromLong(long ival)
{
#if (PY_MAJOR_VERSION == 2)
    return PyInt_FromLong(ival);
#else
    return PyLong_FromLong(ival);
#endif
}

PyObject* UnpackFromRRArray(const boost::intrusive_ptr<RRBaseArray>& rrarray,
                            const boost::shared_ptr<TypeDefinition>& type1)
{

    if (type1)
    {
        if (!rrarray)
        {
            if (type1->ArrayType == DataTypes_ArrayTypes_none)
                throw DataTypeException("Scalars must not be null");
        }
        else
        {
            if (rrarray->GetTypeID() != type1->Type)
                throw DataTypeException("Data type mismatch");
            if (type1->ArrayType == DataTypes_ArrayTypes_multidimarray)
                throw DataTypeException("Data type mismatch");
            if (type1->ArrayType == DataTypes_ArrayTypes_array)
            {
                if (!type1->ArrayVarLength)
                {
                    if (rrarray->size() != type1->ArrayLength.at(0))
                        throw DataTypeException("Array length mismatch");
                }
                else if (type1->ArrayLength.at(0) != 0)
                {
                    if (rrarray->size() > type1->ArrayLength.at(0))
                        throw DataTypeException("Array length to long");
                }
            }
            else
            {
                if (rrarray->size() != 1)
                    throw DataTypeException("Array length mismatch");
            }
        }
    }

    if (!rrarray)
    {
        Py_RETURN_NONE;
    }

    return UnpackFromRRArray_numpy(rrarray, type1);
}

boost::intrusive_ptr<RRBaseArray> PackToRRArray_numpy(PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1,
                                                      boost::intrusive_ptr<RRBaseArray> destrrarray)
{
    if (!PyArray_Check(array_))
        throw DataTypeException("numpy array expected");

    PyArrayObject* array1 = (PyArrayObject*)array_;

    npy_intp len = PyArray_SIZE(array1);

    if (destrrarray)
    {
        if (destrrarray->size() != len)
            throw DataTypeException("Invalid destrrarray specified for PackRRArray");
    }
    else
    {
        destrrarray = AllocateRRArrayByType(type1->Type, len);
    }

    if (!VerifyNumPyDataType(PyArray_DESCR(array1), destrrarray->GetTypeID()))
        throw DataTypeException("Invalid destrrarray specified for PackRRArray");

    int ndim = PyArray_NDIM(array1);
    npy_intp* dims = PyArray_DIMS(array1);
    npy_intp* shape = PyArray_SHAPE(array1);

    if (ndim > 1)
    {
        npy_intp nonzero_dims = 0;
        npy_intp zero_dims = 0;
        for (int i = 0; i < ndim; i++)
        {
            if (dims[i] == 0)
            {
                zero_dims++;
            }
            else if (dims[i] != 1)
            {
                nonzero_dims++;
            }
        }

        if (zero_dims > 0)
        {
            if (len != 0)
                throw DataTypeException("Invalid numpy array");
        }
        else if (nonzero_dims != 1)
        {
            throw DataTypeException("Invalid numpy array");
        }
    }

    PyAutoPtr<PyArrayObject> array2(PyArray_GETCONTIGUOUS(array1));

    if (array2.get() == NULL)
        throw DataTypeException("Internal error");

    if (PyArray_NBYTES(array2.get()) != destrrarray->size() * destrrarray->ElementSize())
        throw DataTypeException("numpy data size error in PackToRRArray");

    memcpy(destrrarray->void_ptr(), PyArray_DATA(array2.get()), PyArray_NBYTES(array2.get()));

    return destrrarray;
}

PyObject* UnpackFromRRArray_numpy(const boost::intrusive_ptr<RRBaseArray>& rrarray,
                                  const boost::shared_ptr<TypeDefinition>& type1)
{
    if (!rrarray)
    {
        Py_RETURN_NONE;
    }

    if (type1)
    {
        if (rrarray->GetTypeID() != type1->Type)
            throw DataTypeException("Invalid array specified for UnpackFromRRArray");
        if (type1->ArrayType == DataTypes_ArrayTypes_multidimarray)
            throw DataTypeException("Invalid parameter for UnpackFromRRArray");
        if (type1->ArrayType == DataTypes_ArrayTypes_none)
        {
            if (rrarray->size() != 1)
                throw DataTypeException("Invalid length for scalar in UnpackFromRRArray");
        }
        else
        {
            if (!type1->ArrayVarLength)
            {
                if (rrarray->size() != type1->ArrayLength.at(0))
                    throw DataTypeException("Invalid length for fixed length array in UnpackFromRRArray");
            }
        }
    }

    npy_intp dims = rrarray->size();
    PyArray_Descr* a_descr = RRTypeIdToNumPyDataType(rrarray->GetTypeID());
    PyAutoPtr<PyObject> ret1(PyArray_NewFromDescr(&PyArray_Type, (PyArray_Descr*)a_descr, 1, &dims, NULL,
                                                  rrarray->void_ptr(), NPY_ARRAY_F_CONTIGUOUS | NPY_ARRAY_WRITEABLE,
                                                  NULL));
    if (ret1.get() == NULL)
    {
        throw InternalErrorException("Could not allocate numpy array");
    }
    PyAutoPtr<PyObject> ret(PyArray_NewCopy((PyArrayObject*)ret1.get(), NPY_FORTRANORDER));
    if (ret.get() == NULL)
    {
        throw InternalErrorException("Could not allocate numpy array");
    }

    if (type1 && type1->ArrayType == DataTypes_ArrayTypes_none)
    {
        PyAutoPtr<PyObject> ret_list = PyArray_ToList((PyArrayObject*)ret.get());
        PyAutoPtr<PyObject> ret_scalar = PyList_GetItem(ret_list.get(), 0);
        Py_XINCREF(ret_scalar.get());
        return ret_scalar.detach();
    }

    return ret.detach();
}

boost::tuple<boost::intrusive_ptr<RRBaseArray>, boost::intrusive_ptr<RRBaseArray> > PackToMultiDimArray_numpy1(
    PyArrayObject* array1, const RR_SHARED_PTR<TypeDefinition>& type1)
{
    int npy_type = PyArray_TYPE(array1);
    switch (npy_type)
    {
    case NPY_FLOAT32:
    case NPY_FLOAT64:
    case NPY_INT8:
    case NPY_UINT8:
    case NPY_INT16:
    case NPY_UINT16:
    case NPY_INT32:
    case NPY_UINT32:
    case NPY_INT64:
    case NPY_UINT64:
    case NPY_COMPLEX128:
    case NPY_COMPLEX64:
    case NPY_BOOL: {
        DataTypes rrtype = type1->Type;
        size_t count = (size_t)PyArray_SIZE(array1);
        boost::intrusive_ptr<RRBaseArray> ret = AllocateRRArrayByType(rrtype, count);

        if (!VerifyNumPyDataType(PyArray_DESCR(array1), ret->GetTypeID()))
            throw DataTypeException("Invalid destrrarray specified for PackRRArray");

        if (count == 0)
            return boost::make_tuple(ret, boost::intrusive_ptr<RRBaseArray>());

        if (PyArray_IS_F_CONTIGUOUS(array1))
        {
            if (PyArray_NBYTES(array1) != ret->ElementSize() * count)
                throw DataTypeException("MultiDimArray type mismatch");
            memcpy(ret->void_ptr(), PyArray_BYTES(array1), PyArray_NBYTES(array1));
            return boost::make_tuple(ret, boost::intrusive_ptr<RRBaseArray>());
        }
        else
        {
            npy_intp* array2_dims = PyArray_DIMS(array1);
            int array2_ndims = PyArray_NDIM(array1);
            PyAutoPtr<PyObject> array2(PyArray_New(&PyArray_Type, array2_ndims, array2_dims, npy_type, NULL,
                                                   ret->void_ptr(), 0, NPY_ARRAY_F_CONTIGUOUS | NPY_ARRAY_WRITEABLE,
                                                   NULL));
            if (!array2.get())
                throw InternalErrorException(
                    "internal error: Could not allocate array2 for PackToMultiDimArray_numpy1");

            if (PyArray_CopyInto((PyArrayObject*)array2.get(), array1) < 0)
            {
                PyErr_Print();
                throw InternalErrorException(
                    "internal error: Could not copy array1 to array2 in PackToMultiDimArray_numpy1");
            }
            return boost::make_tuple(ret, boost::intrusive_ptr<RRBaseArray>());
        }
    }
    default:
        throw DataTypeException("Unsupported numpy matrix type");
    }
}

boost::intrusive_ptr<MessageElementNestedElementList> PackToRRMultiDimArray_numpy(
    PyObject* array_, const boost::shared_ptr<TypeDefinition>& type1)
{
    // if (!_NumPyAvailable) throw InvalidOperationException("NumPy is not available");

    if (PyArray_Check(array_))
    {
        PyArrayObject* array1 = (PyArrayObject*)array_;
        int ndim = PyArray_NDIM(array1);
        npy_intp* dims1 = PyArray_SHAPE(array1);
        boost::intrusive_ptr<RRArray<uint32_t> > dims = AllocateRRArray<uint32_t>(ndim);
        for (int i = 0; i < ndim; i++)
        {
            (*dims)[i] = boost::numeric_cast<uint32_t>(dims1[i]);
        }

        std::vector<boost::intrusive_ptr<MessageElement> > ret_vec;
        ret_vec.push_back(CreateMessageElement("dims", dims));

        int npy_type = PyArray_TYPE(array1);

        switch (npy_type)
        {
        case NPY_FLOAT32:
        case NPY_FLOAT64:
        case NPY_INT8:
        case NPY_UINT8:
        case NPY_INT16:
        case NPY_UINT16:
        case NPY_INT32:
        case NPY_UINT32:
        case NPY_INT64:
        case NPY_UINT64:
        case NPY_COMPLEX64:
        case NPY_COMPLEX128:
        case NPY_BOOL: {
            ret_vec.push_back(CreateMessageElement("array", PackToMultiDimArray_numpy1(array1, type1).get<0>()));
            return CreateMessageElementNestedElementList(DataTypes_multidimarray_t, "", ret_vec);
        }

        default:
            throw DataTypeException("Unsupported numpy matrix type");
        }
    }
    else
    {
        throw DataTypeException("Invalid numpy array format");
    }
}

// NO ERROR CHECKING! USE WITH CAUTION!
static PyObject* UnpackFromRRMultiDimArray_numpy1(boost::intrusive_ptr<RRBaseArray>& rrarray, PyArray_Descr* npy_type,
                                                  int npy_dimcount, npy_intp* npy_dims)
{
    PyAutoPtr<PyObject> array_a(PyArray_NewFromDescr(&PyArray_Type, npy_type, npy_dimcount, npy_dims, NULL,
                                                     rrarray->void_ptr(), NPY_ARRAY_F_CONTIGUOUS, NULL));
    if (!array_a.get())
        throw InternalErrorException("internal error: Could not create array_a in UnpackFromRRMultiDimArray_numpy");
    PyAutoPtr<PyObject> array_b(PyArray_SimpleNewFromDescr(npy_dimcount, npy_dims, npy_type));
    if (!array_b.get())
        throw InternalErrorException("internal error: Could not create array_b in UnpackFromRRMultiDimArray_numpy");
    Py_XINCREF(npy_type);
    if (PyArray_CopyInto((PyArrayObject*)array_b.get(), (PyArrayObject*)array_a.get()) < 0)
    {
        throw InternalErrorException(
            "internal error: Could not copy array_a to array_b in UnpackFromRRMultiDimArray_numpy");
    }

    return array_b.detach();
}

PyObject* UnpackFromRRMultiDimArray_numpy(const boost::intrusive_ptr<MessageElementNestedElementList>& rrarray,
                                          const boost::shared_ptr<TypeDefinition>& type1)
{
    if (!rrarray)
    {
        Py_RETURN_NONE;
    }

    boost::intrusive_ptr<RRArray<uint32_t> > dims =
        MessageElement::FindElement(rrarray->Elements, "dims")->CastData<RRArray<uint32_t> >();
    if (!dims)
        throw DataTypeException("Invalid MultiDimArray");
    boost::intrusive_ptr<RRBaseArray> array =
        MessageElement::FindElement(rrarray->Elements, "array")->CastData<RRBaseArray>();
    if (!array)
        throw DataTypeException("Invalid MultiDimArray");

    DataTypes rr_type = array->GetTypeID();
    std::vector<npy_intp> npy_dims(dims->size());
    for (size_t i = 0; i < dims->size(); i++)
    {
        if ((*dims)[i] < 0)
            throw DataTypeException("Invalid MultiDimArray");
        npy_dims[i] = (npy_intp)(*dims)[i];
    }

    switch (rr_type)
    {
    case DataTypes_double_t:
    case DataTypes_single_t:
    case DataTypes_int8_t:
    case DataTypes_uint8_t:
    case DataTypes_int16_t:
    case DataTypes_uint16_t:
    case DataTypes_int32_t:
    case DataTypes_uint32_t:
    case DataTypes_int64_t:
    case DataTypes_uint64_t:
    case DataTypes_cdouble_t:
    case DataTypes_csingle_t:
    case DataTypes_bool_t: {
        PyArray_Descr* npy_type = RRTypeIdToNumPyDataType(rr_type);
        return UnpackFromRRMultiDimArray_numpy1(array, npy_type, npy_dims.size(), &npy_dims[0]);
    }
    // TODO: bool
    default:
        throw DataTypeException("Invalid MultiDimArray data type");
    }
}

bool VerifyNumPyDataType(PyArray_Descr* np_type, DataTypes rr_type)
{
    switch (np_type->type_num)
    {
    case NPY_FLOAT64:
        return rr_type == DataTypes_double_t;
    case NPY_FLOAT32:
        return rr_type == DataTypes_single_t;
    case NPY_INT8:
        return rr_type == DataTypes_int8_t;
    case NPY_UINT8:
        return rr_type == DataTypes_uint8_t;
    case NPY_INT16:
        return rr_type == DataTypes_int16_t;
    case NPY_UINT16:
        return rr_type == DataTypes_uint16_t;
    case NPY_INT32:
        return rr_type == DataTypes_int32_t;
    case NPY_UINT32:
        return rr_type == DataTypes_uint32_t;
    case NPY_INT64:
        return rr_type == DataTypes_int64_t;
    case NPY_UINT64:
        return rr_type == DataTypes_uint64_t;
    case NPY_COMPLEX128:
        return rr_type == DataTypes_cdouble_t;
    case NPY_COMPLEX64:
        return rr_type == DataTypes_csingle_t;
    case NPY_BOOL:
        return rr_type == DataTypes_bool_t;
    default: {
        PyAutoPtr<PyArray_Descr> datetime_descr(RRTypeIdToNumPyDataType(rr_type));
        if (PyArray_EquivTypes(np_type, datetime_descr.get()) == NPY_TRUE)
        {
            return true;
        }
        return false;
    }
    }
}

PyArray_Descr* RRTypeIdToNumPyDataType(DataTypes rr_type)
{
    switch (rr_type)
    {
    case DataTypes_double_t:
        return PyArray_DescrNewFromType(NPY_FLOAT64);
    case DataTypes_single_t:
        return PyArray_DescrNewFromType(NPY_FLOAT32);
    case DataTypes_int8_t:
        return PyArray_DescrNewFromType(NPY_INT8);
    case DataTypes_uint8_t:
        return PyArray_DescrNewFromType(NPY_UINT8);
    case DataTypes_int16_t:
        return PyArray_DescrNewFromType(NPY_INT16);
    case DataTypes_uint16_t:
        return PyArray_DescrNewFromType(NPY_UINT16);
    case DataTypes_int32_t:
        return PyArray_DescrNewFromType(NPY_INT32);
    case DataTypes_uint32_t:
        return PyArray_DescrNewFromType(NPY_UINT32);
    case DataTypes_int64_t:
        return PyArray_DescrNewFromType(NPY_INT64);
    case DataTypes_uint64_t:
        return PyArray_DescrNewFromType(NPY_UINT64);
    case DataTypes_cdouble_t:
        return PyArray_DescrNewFromType(NPY_COMPLEX128);
    case DataTypes_csingle_t:
        return PyArray_DescrNewFromType(NPY_COMPLEX64);
    case DataTypes_bool_t:
        return PyArray_DescrNewFromType(NPY_BOOL);
    default:
        throw DataTypeException("Unknown numpy data type");
    }
}

/*void RRInitPrimitiveTypes()
{
    PyAutoPtr<PyArray_Descr> bool_descr(PyArray_DescrFromType(NPY_UINT8));
    int bool_code = PyArray_RegisterDataType(bool_descr.get());
    if (PyErr_Occurred() != NULL)
    {
        return;
    }

    bool_numpy_dtype_code = bool_code;
    bool_numpy_dtype = bool_descr.detach();

    PyAutoPtr<PyObject> datetime_descr_list(PyList_New(2));
    PyList_SetItem(datetime_descr_list.get(), 0, Py_BuildValue("(s,s)", "secs", "i8"));
    PyList_SetItem(datetime_descr_list.get(), 1, Py_BuildValue("(s,s)", "nsecs", "i8"));
    if (PyErr_Occurred() != NULL)
    {
        bool_numpy_dtype_code = -1;
        bool_numpy_dtype = NULL;
    }

    PyAutoPtr<PyArray_Descr> datetime_descr(PyArray_DescrNew();


}*/

DataTypes GetNamedTypeEntryType(const boost::shared_ptr<RobotRaconteur::TypeDefinition>& type1,
                                const boost::shared_ptr<RobotRaconteur::WrappedServiceStub>& obj,
                                const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node)
{
    if (!type1)
        throw InvalidArgumentException("Type must not be null");
    std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
    return type1->ResolveNamedType(other_defs, node, obj)->RRDataType();
}

void PythonTypeSupport_Init() { _import_array(); }

} // namespace RobotRaconteur
