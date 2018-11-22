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

#include "PythonTypeSupport.h"

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/numeric.hpp>

namespace RobotRaconteur
{
	static bool _NumPyAvailable = false;

	template <typename T>
	class PyAutoPtr
	{
	public:
		T* ptr;

		PyAutoPtr(T* ptr_) : ptr(ptr_) {}
		~PyAutoPtr() { Py_XDECREF((PyObject*)ptr); }
		T* get() { return ptr; }
		T* detach() { T* ptr1 = ptr; ptr = NULL; return ptr1; }
	};

	
	template <typename T>
	T find_by_name(std::vector<T> t, std::string& name)
	{
		BOOST_FOREACH(T t1, t)
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
			const char *str1 = PyUnicode_AsUTF8(obj);
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
		PyObject* r= PyString_FromStringAndSize(s.c_str(), s.size());
		if (!r) throw DataTypeException("Invalid string specified");
		return r;
#else
		PyObject* r = PyUnicode_DecodeUTF8(s.c_str(), s.size(), "Invalid string specified");
		if (!r) throw DataTypeException("Invalid string specified");
		return r;
#endif
	}

	void VerifyMultiDimArrayLength(boost::shared_ptr<MessageElementMultiDimArray> data, boost::shared_ptr<TypeDefinition> type1)
	{
		int32_t data_dimcount = RRArrayToScalar(MessageElement::FindElement(data->Elements, "dimcount")->CastData<RRArray<int32_t> >());
		boost::shared_ptr<RRArray<int32_t> > data_dims = MessageElement::FindElement(data->Elements, "dims")->CastData<RRArray<int32_t> >();
		if (!data_dims) throw DataTypeException("Invalid MultDimArray");
		if (type1->ArrayLength.size() != data_dimcount || data_dimcount != data_dims->Length())
			throw DataTypeException("Array dimension mismatch");
		int32_t count = 1;

		for (int32_t i = 0; i < data_dimcount; i++)
		{
			count *= data_dims->ptr()[i];
			if (data_dims->ptr()[i] != type1->ArrayLength[i])
				throw DataTypeException("Array dimension mismatch");
		}

		boost::shared_ptr<RRBaseArray> data_real = MessageElement::FindElement(data->Elements, "real")->CastData<RRBaseArray>();
		if (!data_real) throw DataTypeException("Invalid MultDimArray");
		if (data_real->Length() != count) throw DataTypeException("Array dimension mismatch");
		
		boost::shared_ptr<MessageElement> el_imag;
		if (MessageElement::TryFindElement(data->Elements, "imag", el_imag))
		{
			throw DataTypeException("Fixed dimension arrays must not be complex");
		}

	}


	PyObject* NewStructure(const std::string& type, boost::shared_ptr<RobotRaconteur::WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node)
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

		PyObject* rrutil_module = PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonDataTypes");
		if (rrutil_module == NULL)
		{
			throw InternalErrorException("Could not load RobotRaconeturPythonDataTypes module");
		}

		if (!node)
		{
			node = RobotRaconteur::RobotRaconteurNode::sp();
		}

		boost::tuple<std::string, std::string> s1 = RobotRaconteur::SplitQualifiedName(type);

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
		RR_SHARED_PTR<RobotRaconteur::ServiceEntryDefinition> s=TryFindByName(d->Structures, s1.get<1>());
		
		if (!s)
		{
			s = TryFindByName(d->CStructures, s1.get<1>());
		}

		if (!s) throw ServiceException("Structure " + type + " not found");

		PyAutoPtr<PyObject> struct_class(PyObject_GetAttrString(rrutil_module, "RobotRaconteurStructure"));
		if (struct_class.get() == NULL)
		{
			throw InternalErrorException("Could not load RobotRaconeturPythonUtil.RobotRaconteurStructure class");
		}

		PyObject* new_struct = PyObject_CallFunction(struct_class.get(), NULL);
		if (!new_struct)
		{
			throw InternalErrorException("Could not create RobotRaconeturPythonUtil.RobotRaconteurStructure class");
		}

		PyAutoPtr<PyObject> py_type(stringToPyObject(type));
		PyObject_SetAttrString(new_struct, "rrstructtype", py_type.get());
		
		BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition>& e, s->Members)
		{
			PyObject_SetAttrString(new_struct, e->Name.c_str(), Py_None);
		}

		return new_struct;
	}
	
	boost::shared_ptr<MessageElementCStructureArray>  PackMessageElement_cstruct(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteurNode> node)
	{		
		if (!PySequence_Check(data))
		{
			if (type1->ArrayType != DataTypes_ArrayTypes_none)
			{
				throw DataTypeException("Scalar expected");
			}
			PyAutoPtr<PyObject> data1(PyList_New(1));
			PyList_SetItem(data1.get(), 0, data);
			Py_INCREF(data);
			return PackMessageElement_cstruct(data1.get(), type1, obj, node);
		}
				
		if (type1->ArrayType != DataTypes_varvalue_t 
			&& type1->ArrayType == DataTypes_ArrayTypes_none 
			&& PySequence_Length(data) !=1 )
		{
			throw DataTypeException("Scalar expected");
		}
		else
		{
			int32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<int32_t>());
			if (!type1->ArrayLength.empty() && c!=0 )
			{				
				if (type1->ArrayVarLength)
				{
					if (PySequence_Length(data) > c)
					{
						throw DataTypeException("Array dimension mismatch");
					}
				}
				else
				{
					if (PySequence_Length(data) != c)
					{
						throw DataTypeException("Array dimension mismatch");
					}
				}
			}
		}
				
		std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
		boost::shared_ptr<ServiceEntryDefinition> struct_def = RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(type1->ResolveNamedType(empty_defs, node, obj));
		if (!struct_def) throw DataTypeException("Invalid cstruct data type");
		if (struct_def->EntryType != DataTypes_cstructure_t) throw DataTypeException("Invalid cstruct data type");
		std::string typestr2 = struct_def->ResolveQualifiedName();
		
		std::vector<RR_SHARED_PTR<MessageElement> > ret;

		for (ssize_t i = 0; i < PySequence_Length(data); i++)
		{

			PyAutoPtr<PyObject> a(PySequence_GetItem(data, i));

			PyAutoPtr<PyObject> rrstructtype(PyObject_GetAttrString(a.get(), "rrstructtype"));
			if (!rrstructtype.get()) throw DataTypeException("Invalid structure for field " + type1->Name);
			std::string typestr = PyObjectToUTF8(rrstructtype.get());
			if (PyErr_Occurred())
			{
				throw DataTypeException("Invalid structure for field " + type1->Name);
			}

			std::vector<boost::shared_ptr<MessageElement> > m_struct;
			BOOST_FOREACH(boost::shared_ptr<MemberDefinition> m_def, struct_def->Members)
			{
				boost::shared_ptr<PropertyDefinition> p_def = boost::dynamic_pointer_cast<PropertyDefinition>(m_def);
				if (!p_def) throw ServiceException("Invalid structure definition: " + typestr2);

				PyAutoPtr<PyObject> field_obj(PyObject_GetAttrString(a.get(), p_def->Name.c_str()));
				if (field_obj.get() == NULL)
				{
					throw DataTypeException("Field " + p_def->Name + " not found in structure of type " + typestr2);
				}
				
				RR_SHARED_PTR<TypeDefinition> p_def2_type = p_def->Type;
				if (p_def2_type->ArrayType == DataTypes_ArrayTypes_multidimarray)
				{
					p_def2_type = RR_MAKE_SHARED<TypeDefinition>();
					p_def->Type->CopyTo(*p_def2_type);
					int32_t c = boost::accumulate(p_def2_type->ArrayLength, 1, std::multiplies<int32_t>());
					p_def2_type->ArrayLength.clear();
					p_def2_type->ArrayLength.push_back(c);
					p_def2_type->ArrayType = DataTypes_ArrayTypes_array;
					p_def2_type->ArrayVarLength = false;
				}
				boost::shared_ptr<MessageElement> el = PackMessageElement(field_obj.get(), p_def2_type, obj, node);
				el->ElementName = p_def->Name;
				m_struct.push_back(el);
			}

			boost::shared_ptr<MessageElement> el2 = boost::make_shared<MessageElement>("", boost::make_shared<MessageElementCStructure>(m_struct));
			el2->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
			el2->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
			el2->ElementNumber = i;

			ret.push_back(el2);
		}

		return boost::make_shared<MessageElementCStructureArray>(typestr2, ret);
	}

	boost::shared_ptr<MessageElement>  PackMessageElement(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> obj, boost::shared_ptr<RobotRaconteurNode> node)
	{
		if (!type1) throw InvalidArgumentException("Invalid parameters for PackMessageElement");

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
			
		if (type1->Type == DataTypes_varvalue_t && type1->ContainerType == DataTypes_ContainerTypes_none)
		{
			PyAutoPtr<PyObject> dt(PyObject_GetAttrString(data, "datatype"));
			if (!dt.get()) throw DataTypeException("Invalid VarValue object");
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
				if (d->RRDataType() == DataTypes_cstructure_t && type2->ArrayType == DataTypes_ArrayTypes_none)
				{
					type2->ArrayType == DataTypes_ArrayTypes_array;
				}
			}

			PyAutoPtr<PyObject> data2(PyObject_GetAttrString(data, "data"));
			if (!data2.get()) throw DataTypeException("Invalid VarValue object");

			return PackMessageElement(data2.get(), type2, obj, node);
		}

		boost::shared_ptr<MessageElement> element = boost::make_shared<MessageElement>();
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
				if (type1->ResolveNamedType(empty_defs, node, obj)->RRDataType() == DataTypes_cstructure_t)
				{
					throw DataTypeException("CStructures must not be None");
				}
			}
			
			element->ElementType = DataTypes_void_t;
			element->DataCount = 0;
			element->SetData(RR_SHARED_PTR<MessageElementData>());
			return element;
		}
		else if (type1->ContainerType == DataTypes_ContainerTypes_list)
		{
			if (!PySequence_Check(data)) throw DataTypeException("Sequence type expected for field " + type1->Name);

			boost::shared_ptr<TypeDefinition> type2 = type1->Clone();
			type2->RemoveContainers();
			
			element->ElementType = DataTypes_list_t;
			std::vector<boost::shared_ptr<MessageElement> > mret;
						
			for (int32_t i = 0; i < (int32_t)PySequence_Size(data); i++)
			{
				PyAutoPtr<PyObject> dat1( PySequence_GetItem(data, (Py_ssize_t)i));
				boost::shared_ptr<MessageElement> el = PackMessageElement(dat1.get(), type2, obj, node);
				el->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
				el->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
				el->ElementNumber = i;
				mret.push_back(el);
			}
			element->DataCount = mret.size();
			element->SetData(boost::make_shared<MessageElementList>(mret));
			return element;
		}
					
		if (type1->ContainerType == DataTypes_ContainerTypes_map_int32)
		{
			boost::shared_ptr<TypeDefinition> type2 = type1->Clone();
			type2->RemoveContainers();

			element->ElementType = DataTypes_vector_t;
			std::vector<boost::shared_ptr<MessageElement> > mret;

			if (PySequence_Check(data))
			{
				for (int32_t i = 0; i < (int32_t)PySequence_Size(data); i++)
				{
					PyAutoPtr<PyObject> dat1(PySequence_GetItem(data, (Py_ssize_t)i));
					boost::shared_ptr<MessageElement> el = PackMessageElement(dat1.get(), type2, obj, node);
					el->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
					el->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
					el->ElementNumber = i;
					mret.push_back(el);
				}
				element->DataCount = mret.size();
				element->SetData(boost::make_shared<MessageElementMap<int32_t> >(mret));
				return element;
			}
			else if (PyMapping_Check(data))
			{
				PyAutoPtr<PyObject> data_list(PyMapping_Items(data));
				if (!data_list.get()) throw DataTypeException("Invalid Map type for field " + type1->Name);

				PyAutoPtr<PyObject> iter(PyObject_GetIter(data_list.get()));
										
				if (iter.get() == NULL) throw DataTypeException("Invalid Map type for field " + type1->Name);
					
				PyObject* item;

				while(item = PyIter_Next(iter.get()))
				{
					PyAutoPtr<PyObject> item1(item);

					if (!PySequence_Check(item)) throw DataTypeException("Invalid Map for field " + type1->Name);
					if (PySequence_Size(item) != 2) throw DataTypeException("Invalid Map for field " + type1->Name);
					PyAutoPtr<PyObject> key(PySequence_GetItem(item, 0));
					PyAutoPtr<PyObject> val(PySequence_GetItem(item, 1));

					long key_l=PyLong_AsLong(key.get());
					if (PyErr_Occurred())
					{
						throw DataTypeException("Invalid Key in Map for field " + type1->Name);
					}

					if (key_l < std::numeric_limits<int32_t>::min() || key_l > std::numeric_limits<int32_t>::max())
					{
						throw DataTypeException("Invalid Map for field " + type1->Name);
					}

					boost::shared_ptr<MessageElement> el = PackMessageElement(val.get(), type2, obj, node);
					el->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
					el->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
					el->ElementNumber = (int32_t)key_l;
					mret.push_back(el);
				}

				element->DataCount = mret.size();
				element->SetData(boost::make_shared<MessageElementMap<int32_t> >(mret));
				return element;
			}
			else
			{
				throw DataTypeException("Sequence type or Map type expected for field " + type1->Name);
			}

		}
		else if (type1->ContainerType == DataTypes_ContainerTypes_map_string)
		{
			boost::shared_ptr<TypeDefinition> type2 = type1->Clone();
			type2->RemoveContainers();

			if (!PyMapping_Check(data)) throw DataTypeException("Map type expected for field " + type1->Name);

			std::vector<boost::shared_ptr<MessageElement> > mret;
			PyAutoPtr<PyObject> data_list(PyMapping_Items(data));
			if (!data_list.get()) throw DataTypeException("Invalid Map type for field " + type1->Name);

			PyAutoPtr<PyObject> iter(PyObject_GetIter(data_list.get()));
			if (iter.get() == NULL) throw DataTypeException("Invalid Map type for field " + type1->Name);

			PyObject* item;

			while (item = PyIter_Next(iter.get()))
			{
				PyAutoPtr<PyObject> item1(item);
					
				PyAutoPtr<PyObject> key(PySequence_GetItem(item, 0));
				PyAutoPtr<PyObject> val(PySequence_GetItem(item, 1));

				std::string key_s=PyObjectToUTF8(key.get());
				if (PyErr_Occurred())
				{
					throw DataTypeException("Invalid Key in Map for field " + type1->Name);
				}					
					
				boost::shared_ptr<MessageElement> el = PackMessageElement(val.get(), type2, obj, node);
					
				el->ElementName = key_s;
				mret.push_back(el);
			}

			element->DataCount = mret.size();
			element->SetData(boost::make_shared<MessageElementMap<std::string> >(mret));
			return element;

		}		

		if (IsTypeNumeric(type1->Type) && type1->ArrayType == DataTypes_ArrayTypes_multidimarray)
		{
			if (_NumPyAvailable)
			{
				if (PyArray_Check(data) || PySequence_Check(data))
				{
					boost::shared_ptr<MessageElementMultiDimArray> mm = PackToRRMultiDimArray_numpy(data, type1);
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


			std::vector<boost::shared_ptr<MessageElement> > map_vec;

			PyAutoPtr<PyObject> dimcount = PyObject_GetAttrString(data, "DimCount");
			if (!dimcount.get()) throw DataTypeException("Invalid MultidimArray");
			PyAutoPtr<PyObject> dims = PyObject_GetAttrString(data, "Dims");
			if (!dims.get()) throw DataTypeException("Invalid MultidimArray");
			PyAutoPtr<PyObject> real = PyObject_GetAttrString(data, "Real");
			if (!real.get()) throw DataTypeException("Invalid MultidimArray");
			PyAutoPtr<PyObject> complex = PyObject_GetAttrString(data, "Complex");
			if (!complex.get()) throw DataTypeException("Invalid MultidimArray");

			boost::shared_ptr<TypeDefinition> dims_type = boost::make_shared<TypeDefinition>();
			dims_type->Type = DataTypes_int32_t;
			dims_type->ArrayType = DataTypes_ArrayTypes_array;
			dims_type->ArrayVarLength = true;
			dims_type->ArrayLength.push_back(0);

			map_vec.push_back(boost::make_shared<MessageElement>("dimcount", PackToRRArray(dimcount.get(), dims_type, boost::shared_ptr<RRBaseArray>())));
			map_vec.push_back(boost::make_shared<MessageElement>("dims", PackToRRArray(dims.get(), dims_type, boost::shared_ptr<RRBaseArray>())));

			boost::shared_ptr<TypeDefinition> real_type = boost::make_shared<TypeDefinition>();
			real_type->ArrayType = DataTypes_ArrayTypes_array;
			real_type->ArrayVarLength = true;
			real_type->ArrayLength.push_back(0);
			real_type->Type = type1->Type;
			//type1->Name = "real";

			map_vec.push_back(boost::make_shared<MessageElement>("real", PackToRRArray(real.get(), real_type, boost::shared_ptr<RRBaseArray>())));

			if (PyObject_IsTrue(complex.get()))
			{
				PyAutoPtr<PyObject> imag(PyObject_GetAttrString(data, "Imag"));
				if (!imag.get()) throw DataTypeException("Invalid MultidimArray");

				map_vec.push_back(boost::make_shared<MessageElement>("imag", PackToRRArray(imag.get(), real_type, boost::shared_ptr<RRBaseArray>())));
			}
			
			boost::shared_ptr<MessageElementMultiDimArray> mm = boost::make_shared<MessageElementMultiDimArray>(map_vec);

			if (type1)
			{
				if (!type1->ArrayVarLength)
				{
					VerifyMultiDimArrayLength(mm, type1);
				}
			}

			element->SetData(mm);
			return element;			
		}

		if (type1->Type == DataTypes_namedtype_t)
		{
			std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
			RR_SHARED_PTR<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, node, obj);
			switch (nt->RRDataType())
			{
			case DataTypes_structure_t:
			{

				RR_SHARED_PTR<ServiceEntryDefinition> struct_def = RR_STATIC_POINTER_CAST<ServiceEntryDefinition>(nt);
				std::string typestr = struct_def->ResolveQualifiedName();

				PyAutoPtr<PyObject> rrstructtype(PyObject_GetAttrString(data, "rrstructtype"));
				if (!rrstructtype.get()) throw DataTypeException("Invalid structure for field " + type1->Name);
				std::string typestr2 = PyObjectToUTF8(rrstructtype.get());
				if (PyErr_Occurred())
				{
					throw DataTypeException("Invalid structure for field " + type1->Name);
				}

				if (typestr != typestr2)
				{
					throw DataTypeException("Invalid structure for field " + type1->Name);
				}

				element->ElementType = DataTypes_structure_t;
				element->ElementTypeName = typestr;
																
				std::vector<boost::shared_ptr<MessageElement> > mret;
				BOOST_FOREACH(boost::shared_ptr<MemberDefinition> m_def, struct_def->Members)
				{
					boost::shared_ptr<PropertyDefinition> p_def = boost::dynamic_pointer_cast<PropertyDefinition>(m_def);
					if (!p_def) throw ServiceException("Invalid structure definition: " + typestr);

					PyAutoPtr<PyObject> field_obj(PyObject_GetAttrString(data, p_def->Name.c_str()));
					if (field_obj.get() == NULL)
					{
						throw DataTypeException("Field " + p_def->Name + " not found in structure of type " + typestr);
					}

					boost::shared_ptr<MessageElement> el = PackMessageElement(field_obj.get(), p_def->Type, obj, node);
					el->ElementName = p_def->Name;
					mret.push_back(el);
				}

				element->DataCount = mret.size();
				element->SetData(boost::make_shared<MessageElementStructure>(typestr, mret));
				return element;
			}
			case DataTypes_enum_t:
			{
				RR_SHARED_PTR<TypeDefinition> enum_type=RR_MAKE_SHARED<TypeDefinition>();
				enum_type->Type = DataTypes_int32_t;
				enum_type->Name = "value";
				element->SetData(PackToRRArray(data, enum_type, boost::shared_ptr<RRBaseArray>()));
				return element;
			}
			case DataTypes_cstructure_t:
			{

				if (type1->ArrayType == DataTypes_ArrayTypes_multidimarray)
				{
					std::vector<boost::shared_ptr<MessageElement> > map_vec;
										
					PyAutoPtr<PyObject> dims = PyObject_GetAttrString(data, "Dims");
					if (!dims.get()) throw DataTypeException("Invalid CStructureMultidimArray");
					PyAutoPtr<PyObject> cstruct_array = PyObject_GetAttrString(data, "cstruct_array");
					if (!cstruct_array.get()) throw DataTypeException("Invalid MultidimArray");
					
					boost::shared_ptr<TypeDefinition> dims_type = boost::make_shared<TypeDefinition>();
					dims_type->Type = DataTypes_int32_t;
					dims_type->ArrayType = DataTypes_ArrayTypes_array;
					dims_type->ArrayVarLength = true;
					dims_type->ArrayLength.push_back(0);

					map_vec.push_back(boost::make_shared<MessageElement>("dims", PackToRRArray(dims.get(), dims_type, boost::shared_ptr<RRBaseArray>())));

					boost::shared_ptr<TypeDefinition> array_type = boost::make_shared<TypeDefinition>();
					type1->CopyTo(*array_type);
					array_type->ArrayType = DataTypes_ArrayTypes_array;
					array_type->ArrayLength.clear();
					array_type->ArrayVarLength = true;
					array_type->Name = "array";
					//type1->Name = "real";

					map_vec.push_back(PackMessageElement(cstruct_array.get(), array_type, obj, node));
										
					boost::shared_ptr<MessageElementCStructureMultiDimArray> mm = boost::make_shared<MessageElementCStructureMultiDimArray>(type1->ResolveNamedType()->ResolveQualifiedName(), map_vec);
										
					element->SetData(mm);
					return element;
				}
				else
				{
					element->SetData(PackMessageElement_cstruct(data, type1, obj, node));
					
					return element;
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
					throw DataTypeException("Invalid string for " + type1->Name);
				}
				char* str_dat;
				Py_ssize_t str_len;
				if (PyString_AsStringAndSize(data, &str_dat, &str_len) < 0)
				{
					throw DataTypeException("Invalid string for " + type1->Name);
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
					throw DataTypeException("Invalid string for " + type1->Name);
				}
				element->SetData(AttachRRArrayCopy<char>(str_dat, str_len));
				return element;
			}
			else
			{
				throw DataTypeException("Invalid string for " + type1->Name);
			}
#else
			if (PyUnicode_Check(data))
			{
				ssize_t str_len;
				const char* str_dat=PyUnicode_AsUTF8AndSize(data, &str_len);
				if (!str_dat)
				{
					throw DataTypeException("Invalid string for " + type1->Name);
				}
				element->SetData(AttachRRArrayCopy<char>(str_dat, str_len));
				return element;
			}
			else
			{
				throw DataTypeException("Invalid string for " + type1->Name);
			}
#endif
		}

		if (IsTypeNumeric(type1->Type))
		{
			element->SetData(PackToRRArray(data, type1, boost::shared_ptr<RRBaseArray>()));
			return element;
		}
		
		throw DataTypeException("Could not pack message element " + type1->Name);
	}

	boost::shared_ptr<MessageElement>  PackMessageElement(PyObject* data, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<ServerContext> obj, boost::shared_ptr<RobotRaconteurNode> node)
	{
		if (!node && obj)
		{
			node = obj->GetNode();
		}

		return PackMessageElement(data, type1, boost::shared_ptr<WrappedServiceStub>(), node);

	}

	PyObject* UnpackMessageElement_cstruct(boost::shared_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> stub, boost::shared_ptr<RobotRaconteurNode> node)
	{
		std::string& typestr = element->ElementTypeName;
		boost::tuple<std::string, std::string> typestr_s = SplitQualifiedName(typestr);
		boost::shared_ptr<ServiceDefinition> def;
		if (!stub)
		{
			def = node->GetServiceType(typestr_s.get<0>())->ServiceDef();;
		}
		else
		{
			def = node->GetPulledServiceType(stub, typestr_s.get<0>())->ServiceDef();
		}

		boost::shared_ptr<ServiceEntryDefinition> struct_def = find_by_name(def->CStructures, typestr_s.get<1>());

		boost::shared_ptr<MessageElementCStructureArray> l = element->CastData<MessageElementCStructureArray>();
		PyAutoPtr<PyObject> a(PyList_New(l->Elements.size()));

		if (type1)
		{
			int32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<int32_t>());
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

		for (uint32_t i = 0; i < (uint32_t)l->Elements.size(); i++)
		{
			boost::shared_ptr<MessageElement>& el1 = l->Elements[i];

			if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
			{
				if (i != el1->ElementNumber) throw DataTypeException("Invalid cstruct array specified for " + element->ElementName);
			}
			else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
			{
				if (i != boost::lexical_cast<int32_t>(el1->ElementName)) throw DataTypeException("Invalid list specified for " + element->ElementName);
			}
			else
			{
				throw DataTypeException("Invalid cstruct array specified for " + element->ElementName);
			}

			PyAutoPtr<PyObject> el2(NewStructure(typestr, stub, node));

			boost::shared_ptr<MessageElementCStructure> s = el1->CastData<MessageElementCStructure>();

			BOOST_FOREACH(boost::shared_ptr<MemberDefinition> m, struct_def->Members)
			{
				boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
				boost::shared_ptr<MessageElement> el = MessageElement::FindElement(s->Elements, p->Name);

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

				if (p1->Type == DataTypes_namedtype_t)
				{
					std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
					if (p1->ResolveNamedType(empty_defs, node, stub)->RRDataType() == DataTypes_cstructure_t)
					{						
						PyAutoPtr<PyObject> el1 = UnpackMessageElement_cstruct(el, p1, stub, node);
						PyObject_SetAttrString(el2.get(), p->Name.c_str(), el1.get());
						continue;
					}
				}
				
				{
					PyAutoPtr<PyObject> el1(UnpackMessageElement(el, p1, stub, node));
					PyObject_SetAttrString(el2.get(), p->Name.c_str(), el1.get());
				}
			}

			PyList_SetItem(a.get(), i, el2.get());
			Py_XINCREF(el2.get());
		}

		if (type1)
		{
			if (type1->ArrayType == DataTypes_ArrayTypes_none)
			{
				if (PySequence_Length(a.get()) != 1)
				{
					throw DataTypeException("Scalar array dimension mismatch");
				}
				PyAutoPtr<PyObject> b(PySequence_GetItem(a.get(), 0));
				return b.detach();
			}
		}
		
		return a.detach();

	}

	PyObject* UnpackMessageElement(boost::shared_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<WrappedServiceStub> stub, boost::shared_ptr<RobotRaconteurNode> node)
	{
		if (!element) throw NullValueException("element must not be null for UnpackMessageElement");

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
				if (type1->ContainerType == DataTypes_ContainerTypes_none)
				{
					PyAutoPtr<PyObject> vardata(UnpackMessageElement(element, boost::shared_ptr<TypeDefinition>(), stub, node));

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
						type1_2t->TypeString = element->ElementTypeName;
					}
					else if (element->ElementType == DataTypes_cstructure_t)
					{
						type1_2t->Type = DataTypes_namedtype_t;
						type1_2t->TypeString = element->ElementTypeName;
					}
					else if (element->ElementType == DataTypes_cstructure_array_t)
					{
						type1_2t->Type = DataTypes_namedtype_t;
						type1_2t->TypeString = element->ElementTypeName;
						type1_2t->ArrayVarLength = true;
						type1_2t->ArrayLength.push_back(0);
						type1_2t->ArrayType = DataTypes_ArrayTypes_array;
					}
					else if (element->ElementType == DataTypes_cstructure_multidimarray_t)
					{
						type1_2t->Type = DataTypes_namedtype_t;
						type1_2t->TypeString = element->ElementTypeName;
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
						type1_2t->Type = MessageElement::FindElement(element->CastData<MessageElementMultiDimArray>()->Elements, "real")->ElementType;
						if (!IsTypeNumeric(type1_2t->Type)) throw DataTypeException("Invalid MultiDimArray");
					}
					else
					{
						throw DataTypeException("Invalid data type for field " + type1->Name);
					}

					PyObject* modules = PyImport_GetModuleDict();
					if (!modules) throw InternalErrorException("Internal error");
					PyObject* util = PyDict_GetItemString(modules, "RobotRaconteur.RobotRaconteurPythonUtil");
					if (!modules) throw InvalidOperationException("RobotRaconteur.RobotRaconteurPythonUtil not loaded");
					PyAutoPtr<PyObject> varvalue_type(PyObject_GetAttrString(util, "RobotRaconteurVarValue"));
					if (!varvalue_type.get()) throw InvalidArgumentException("Could not find RobotRaconteurVarValue type");

					std::string type1_2t_str = type1_2t->ToString();
					return PyObject_CallFunction(varvalue_type.get(), "O,s#", vardata.get(), type1_2t_str.c_str(), type1_2t_str.size());
				}
				else
				{
					if (element->ElementType == DataTypes_list_t)
					{
						boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
						type2->Type = DataTypes_varvalue_t;
						type2->Name = "value";

						boost::shared_ptr<MessageElementList> l = element->CastData<MessageElementList>();
						PyAutoPtr<PyObject> ret(PyList_New(l->Elements.size()));

						for (uint32_t i = 0; i < (uint32_t)l->Elements.size(); i++)
						{
							boost::shared_ptr<MessageElement>& el1 = l->Elements[i];

							if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
							{
								if (i != el1->ElementNumber) throw DataTypeException("Invalid list specified for " + type1->Name);
							}
							else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
							{
								if (i != boost::lexical_cast<int32_t>(el1->ElementName)) throw DataTypeException("Invalid list specified for " + type1->Name);
							}
							else
							{
								throw DataTypeException("Invalid list specified for " + type1->Name);
							}

							PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
							PyList_SetItem(ret.get(), i, el2.get());
							Py_XINCREF(el2.get());
						}
												
						return ret.detach();
					}

					if (element->ElementType == DataTypes_vector_t)
					{
						boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
						type2->Type = DataTypes_varvalue_t;
						type2->Name = "value";

						boost::shared_ptr<MessageElementMap<int32_t> > l = element->CastData<MessageElementMap<int32_t> >();
						PyAutoPtr<PyObject> ret(PyDict_New());

						for (int32_t i = 0; i < (int32_t)l->Elements.size(); i++)
						{
							boost::shared_ptr<MessageElement>& el1 = l->Elements[i];
							
							int32_t i2;
							if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
							{
								i2 = el1->ElementNumber;
							}
							else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
							{
								i2 = boost::lexical_cast<int32_t>(el1->ElementName);
							}
							else
							{
								throw DataTypeException("Invalid list specified for " + type1->Name);
							}

							PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
							PyDict_SetItem(ret.get(), PyLong_FromLong(i2), el2.get());
						}
												
						return ret.detach();
					}

					if (element->ElementType == DataTypes_dictionary_t)
					{
						boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
						type2->Type = DataTypes_varvalue_t;
						type2->Name = "value";

						boost::shared_ptr<MessageElementMap<std::string> > l = element->CastData<MessageElementMap<std::string> >();
						PyAutoPtr<PyObject> ret(PyDict_New());

						for (int32_t i = 0; i < (int32_t)l->Elements.size(); i++)
						{
							boost::shared_ptr<MessageElement>& el1 = l->Elements[i];
							
							PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
							PyDict_SetItem(ret.get(), stringToPyObject(el1->ElementName).get(), el2.get());
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
				if (type1->ResolveNamedType(empty_defs, node, stub)->RRDataType() == DataTypes_cstructure_t)
				{
					throw DataTypeException("CStructures must not be None");
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
				case DataTypes_enum_t:
				{
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
			boost::shared_ptr<RRArray<char> > s1 = element->CastData<RRArray<char> >();
			return PyUnicode_DecodeUTF8(s1->ptr(), s1->Length(), "Invalid UTF8 String");
		}

		if (element->ElementType == DataTypes_structure_t)
		{
			
				std::string& typestr = element->ElementTypeName;
				boost::tuple<std::string, std::string> typestr_s = SplitQualifiedName(typestr);
				boost::shared_ptr<ServiceDefinition> def;
				if (!stub)
				{
					def = node->GetServiceType(typestr_s.get<0>())->ServiceDef();;
				}
				else
				{
					def = node->GetPulledServiceType(stub, typestr_s.get<0>())->ServiceDef();
				}

				boost::shared_ptr<ServiceEntryDefinition> struct_def = find_by_name(def->Structures, typestr_s.get<1>());

				PyAutoPtr<PyObject> ret(NewStructure(typestr, stub, node));

				boost::shared_ptr<MessageElementStructure> s = element->CastData<MessageElementStructure>();

				BOOST_FOREACH(boost::shared_ptr<MemberDefinition> m, struct_def->Members)
				{
					boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
					boost::shared_ptr<MessageElement> el = MessageElement::FindElement(s->Elements, p->Name);
					PyAutoPtr<PyObject> el1 = UnpackMessageElement(el, p->Type, stub, node);
					PyObject_SetAttrString(ret.get(), p->Name.c_str(), el1.get());
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

			boost::shared_ptr<MessageElementList> l = element->CastData<MessageElementList>();
			PyAutoPtr<PyObject> ret(PyList_New(l->Elements.size()));

			for (uint32_t i = 0; i < (uint32_t)l->Elements.size(); i++)
			{
				boost::shared_ptr<MessageElement>& el1 = l->Elements[i];

				if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
				{
					if (i != el1->ElementNumber) throw DataTypeException("Invalid list specified for " + type1->Name);
				}
				else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
				{
					if (i != boost::lexical_cast<int32_t>(el1->ElementName)) throw DataTypeException("Invalid list specified for " + type1->Name);
				}
				else
				{
					throw DataTypeException("Invalid list specified for " + type1->Name);
				}

				PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
				PyList_SetItem(ret.get(), i, el2.get());
				Py_XINCREF(el2.get());
			}
						
			return ret.detach();
		}
		
		if (element->ElementType == DataTypes_vector_t)
		{
			boost::shared_ptr<TypeDefinition> type2;
			if (type1)
			{
				type2 = type1->Clone();				
			}

			boost::shared_ptr<MessageElementMap<int32_t> > l = element->CastData<MessageElementMap<int32_t> >();
			PyAutoPtr<PyObject> ret(PyDict_New());

			for (int32_t i = 0; i < (int32_t)l->Elements.size(); i++)
			{
				boost::shared_ptr<MessageElement>& el1 = l->Elements[i];

				int32_t i2;
				if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
				{
					i2 = el1->ElementNumber;
				}
				else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
				{
					i2 = boost::lexical_cast<int32_t>(el1->ElementName);
				}
				else
				{
					throw DataTypeException("Invalid list specified for " + type1->Name);
				}

				PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
#if (PY_MAJOR_VERSION == 2)
				PyDict_SetItem(ret.get(), PyInt_FromLong(i2), el2.get());
#else
				PyDict_SetItem(ret.get(), PyLong_FromLong(i2), el2.get());
#endif

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

			boost::shared_ptr<MessageElementMap<std::string> > l = element->CastData<MessageElementMap<std::string> >();
			PyAutoPtr<PyObject> ret(PyDict_New());

			for (int32_t i = 0; i < (int32_t)l->Elements.size(); i++)
			{
				boost::shared_ptr<MessageElement>& el1 = l->Elements[i];

				PyAutoPtr<PyObject> el2(UnpackMessageElement(el1, type2, stub, node));
				PyDict_SetItem(ret.get(), stringToPyObject(el1->ElementName).get(), el2.get());
			}
						
			return ret.detach();
		}

		if (element->ElementType == DataTypes_multidimarray_t)
		{			
			boost::shared_ptr<MessageElementMultiDimArray> mm = element->CastData<MessageElementMultiDimArray>();

			if (type1)
			{
				if (type1->ArrayType != DataTypes_ArrayTypes_multidimarray) throw DataTypeException("MultiDimArray mismatch for " + type1->Name);
				if (!type1->ArrayVarLength)
				{
					VerifyMultiDimArrayLength(mm, type1);
				}
			}


			bool usenumpy = false;

			PyObject* modules = PyImport_GetModuleDict();
			if (!modules) throw InternalErrorException("Internal error");
			
			if (_NumPyAvailable)
			{
				PyObject* rr_module = PyDict_GetItemString(modules, "RobotRaconteur");
				if (rr_module == NULL) throw InternalErrorException("Could not load RobotRaconetur module");
				PyAutoPtr<PyObject> usenumpy_py(PyObject_GetAttrString(rr_module, "UseNumPy"));
				if (usenumpy_py.get() != NULL)
				{
					if (PyObject_IsTrue(usenumpy_py.get()))
					{
						usenumpy = true;
					}
				}

				if (usenumpy)
				{
					return UnpackFromRRMultiDimArray_numpy(element->CastData<MessageElementMultiDimArray>(), type1);
				}
			}

			PyObject* dt = PyDict_GetItemString(modules, "RobotRaconteur.RobotRaconteurPythonDataTypes");
			if (!dt) throw InternalErrorException("RobotRaconteur.RobotRaconteurPythonDataTypes not loaded");
			PyAutoPtr<PyObject> multi_type(PyObject_GetAttrString(dt, "MultiDimArray"));
			if (!multi_type.get()) throw InternalErrorException("Could not find MultiDimArray type");

			PyAutoPtr<PyObject> ret(PyObject_CallFunction(multi_type.get(), NULL));

			int32_t dimcount=RRArrayToScalar(MessageElement::FindElement(mm->Elements, "dimcount")->CastData<RRArray<int32_t> >());
			PyObject_SetAttrString(ret.get(), "DimCount", PyLong_FromLong(dimcount));

			boost::shared_ptr<TypeDefinition> dims_type = boost::make_shared<TypeDefinition>();
			dims_type->Type = DataTypes_int32_t;
			dims_type->ArrayType = DataTypes_ArrayTypes_array;
			dims_type->ArrayVarLength = true;
			dims_type->ArrayLength.push_back(0);

			boost::shared_ptr<RRArray<int32_t> > dims_rr = MessageElement::FindElement(mm->Elements, "dims")->CastData<RRArray<int32_t> >();
			if (!dims_rr) throw DataTypeException("Invalid MultiDimArray");
			if (dims_rr->Length() != dimcount) throw DataTypeException("MultiDimArray mismatch");

			PyObject_SetAttrString(ret.get(), "Dims", UnpackFromRRArray(dims_rr, dims_type));

			boost::shared_ptr<RRBaseArray> real= MessageElement::FindElement(mm->Elements, "real")->CastData<RRBaseArray>();
			if (!real) throw DataTypeException("Invalid MultiDimArray");
			PyObject_SetAttrString(ret.get(), "Real", UnpackFromRRArray(real, boost::shared_ptr<TypeDefinition>()));

			boost::shared_ptr<MessageElement> imag1;
			if (MessageElement::TryFindElement(mm->Elements, "imag", imag1))
			{
				boost::shared_ptr<RRBaseArray> imag = imag1->CastData<RRBaseArray>();
				if (!imag) throw DataTypeException("Invalid MultiDimArray");
				PyObject_SetAttrString(ret.get(), "Imag", UnpackFromRRArray(imag, boost::shared_ptr<TypeDefinition>()));
				PyObject_SetAttrString(ret.get(), "Complex", Py_True);
			}
			else
			{
				PyObject_SetAttrString(ret.get(), "Complex", Py_False);
			}
						
			return ret.detach();
		}		

		if (element->ElementType == DataTypes_cstructure_multidimarray_t)
		{
			boost::shared_ptr<MessageElementCStructureMultiDimArray> mm = element->CastData<MessageElementCStructureMultiDimArray>();

			PyObject* modules = PyImport_GetModuleDict();
			if (!modules) throw InternalErrorException("Internal error");

			PyObject* dt = PyDict_GetItemString(modules, "RobotRaconteur.RobotRaconteurPythonDataTypes");
			if (!dt) throw InternalErrorException("RobotRaconteur.RobotRaconteurPythonDataTypes not loaded");
			PyAutoPtr<PyObject> multi_type(PyObject_GetAttrString(dt, "CStructureMultiDimArray"));
			if (!multi_type.get()) throw InternalErrorException("Could not find MultiDimArray type");

			PyAutoPtr<PyObject> ret(PyObject_CallFunction(multi_type.get(), NULL));
						
			boost::shared_ptr<TypeDefinition> dims_type = boost::make_shared<TypeDefinition>();
			dims_type->Type = DataTypes_int32_t;
			dims_type->ArrayType = DataTypes_ArrayTypes_array;
			dims_type->ArrayVarLength = true;
			dims_type->ArrayLength.push_back(0);

			boost::shared_ptr<RRArray<int32_t> > dims_rr = MessageElement::FindElement(mm->Elements, "dims")->CastData<RRArray<int32_t> >();
			if (!dims_rr) throw DataTypeException("Invalid CStructureMultiDimArray");			

			PyObject_SetAttrString(ret.get(), "Dims", UnpackFromRRArray(dims_rr, dims_type));

			boost::shared_ptr<MessageElement> array = MessageElement::FindElement(mm->Elements, "array");
			if (!array) throw DataTypeException("Invalid CStructureMultiDimArray");
			boost::shared_ptr<TypeDefinition> type2;
			if (type1)
			{
				type2 = boost::make_shared<TypeDefinition>();
				type1->CopyTo(*type2);
				type2->ArrayType = DataTypes_ArrayTypes_array;
				type2->ArrayVarLength = true;
				type2->ArrayLength.clear();
			}
			PyObject_SetAttrString(ret.get(), "cstruct_array", UnpackMessageElement(array, type2, stub, node));
			return ret.detach();
		}

		if (element->ElementType == DataTypes_cstructure_array_t)
		{
			return UnpackMessageElement_cstruct(element, type1, stub, node);			
		}

		throw DataTypeException("Invalid data type");
	}

	PyObject* UnpackMessageElement(boost::shared_ptr<MessageElement> element, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<ServerContext> obj, boost::shared_ptr<RobotRaconteurNode> node)
	{
		if (!node && obj)
		{
			node = obj->GetNode();
		}

		return UnpackMessageElement(element, type1, boost::shared_ptr<WrappedServiceStub>(), node);
	}

	template<typename type2>
	boost::shared_ptr<RRBaseArray> PackToRRArray1_int(PyObject* array_, boost::shared_ptr<RRBaseArray> destrrarray)
	{
		PyAutoPtr<PyObject> array_fast(PySequence_Fast(array_, "Internal error"));
		if (array_fast.get() == NULL)
		{
			throw InternalErrorException("Internal error");
		}	

		size_t seq_len = (size_t)PySequence_Fast_GET_SIZE(array_fast.get());

		RR_SHARED_PTR<RRArray<type2> > array2;
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

		type2* buf = array2->ptr();

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
						if (v2 <(long)std::numeric_limits<type2>::min())
							throw DataTypeException("Number outside of range limit for specified type");
					}
					if (std::numeric_limits<long>::max() > std::numeric_limits<type2>::max())
					{
						if (v2 >(long)std::numeric_limits<type2>::max())
							throw DataTypeException("Number outside of range limit for specified type");
					}
				}
				else
				{
					if (v2 < 0) throw DataTypeException("Number outside of range limit for specified type");
					if (std::numeric_limits<long>::max() > std::numeric_limits<type2>::max())
					{
						if (v2 >(long)std::numeric_limits<type2>::max())
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
						if (v2 < (long long)std::numeric_limits<type2>::min() || v2 >(long long)std::numeric_limits<type2>::max())
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

						if (v2 < (unsigned long long)std::numeric_limits<type2>::min() || v2 >(unsigned long long)std::numeric_limits<type2>::max())
							throw DataTypeException("Number outside of range limit for specified type");
						buf[i] = (type2)v2;
					}

				}
				else if (_NumPyAvailable && PyArray_CheckScalar(v))
				{
					type2 v2;
					PyAutoPtr<PyArray_Descr> descr = PyArray_DescrFromType(RRTypeIdToNumPyDataType(array2->GetTypeID()));
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

	template<typename type2>
	boost::shared_ptr<RRBaseArray> PackToRRArray1_float(PyObject* array_, boost::shared_ptr<RRBaseArray> destrrarray)
	{
		PyAutoPtr<PyObject> array_fast(PySequence_Fast(array_,"Internal error"));
		if (array_fast.get() == NULL)
		{
			throw InternalErrorException("Internal error");
		}	

		size_t seq_len = (size_t)PySequence_Fast_GET_SIZE(array_fast.get());

		RR_SHARED_PTR<RRArray<type2> > array2;
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

		type2* buf = array2->ptr();

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
			else if (_NumPyAvailable && PyArray_CheckScalar(v))
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
				throw DataTypeException("Invalid value in list provided to PackRRArray");
			}
			if (v2 < -std::numeric_limits<type2>::max() || v2 > std::numeric_limits<type2>::max())
				throw DataTypeException("Number outside of range limit for specified type");
			buf[i] = (type2)v2;
		}

		return array2;
	}

	boost::shared_ptr<RRBaseArray> PackToRRArray(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<RRBaseArray> destrrarray)
	{
		if (!type1) throw NullValueException("PackToRRArray type must not be None");

		if (array_ == Py_None)
		{
			if (type1->ArrayType = DataTypes_ArrayTypes_none) throw DataTypeException("Scalar values must not be none");
			return boost::shared_ptr<RRBaseArray>();
		}

		if (_NumPyAvailable)
		{
			if (PyArray_Check(array_))
			{
				return PackToRRArray_numpy(array_, type1, destrrarray);
			}
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
			
			if (!bytearray_buf) throw DataTypeException("Invalid data argument for PackToRRArray");

			if (bytearray_ssize < 0) throw InternalErrorException("PackToRRArray internal error");
			size_t bytearray_size = (size_t)(bytearray_ssize);

			if (type1->ArrayType == DataTypes_ArrayTypes_none && (bytearray_size != 1)) throw DataTypeException("Invalid array length for PackToRRArray");
			if (type1->ArrayType == DataTypes_ArrayTypes_array)
			{
				if (!type1->ArrayVarLength)
				{
					if(type1->ArrayLength.at(0) != bytearray_size) throw DataTypeException("Invalid array length for PackToRRArray");
				}
				else if (type1->ArrayLength.at(0) != 0)
				{
					if (type1->ArrayLength.at(0) < bytearray_size) throw DataTypeException("Array to long for PackToRRArray");
				}				
			}				

			if (destrrarray)
			{
				if (destrrarray->GetTypeID() != DataTypes_uint8_t
					|| bytearray_size == destrrarray->Length())
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
		if (seq_ssize < 0) throw DataTypeException("Invalid array length for PackToRRArray");
		size_t seq_size = (size_t)seq_ssize;

		if (type1->ArrayType == DataTypes_ArrayTypes_none && (seq_size != 1)) throw DataTypeException("Invalid array length for PackToRRArray");
			if (type1->ArrayType == DataTypes_ArrayTypes_array)
		{
			if (!type1->ArrayVarLength)
			{
				if (type1->ArrayLength.at(0) != seq_size) throw DataTypeException("Invalid array length for PackToRRArray");
			}
			else if (type1->ArrayLength.at(0) != 0)
			{
				if (type1->ArrayLength.at(0) < seq_size) throw DataTypeException("Array to long for PackToRRArray");
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
		default:
			throw DataTypeException("Unknown numeric data type");
		}

	}
		

	template <typename type2, typename NumToPyOp>
	inline PyObject* UnpackFromRRArray1(boost::shared_ptr<RRBaseArray> rrarray, NumToPyOp NumToPy)
	{
		boost::shared_ptr<RRArray<type2> > rrarray1 = RR_DYNAMIC_POINTER_CAST<RRArray<type2> >(rrarray);
		if (!rrarray1) throw DataTypeException("Data type mismatch");

		size_t n = rrarray->Length();
		PyAutoPtr<PyObject> o(PyList_New(n));
		if (!o.get()) throw InternalErrorException("Could not allocate Python list");

		type2* buf = rrarray1->ptr();

		for (size_t i = 0; i < n; i++)
		{			
			PyAutoPtr<PyObject> v(NumToPy(buf[i]));
			if (!v.get())
			{								
				throw InternalErrorException("Could not allocate Python number");				
			}
			PyList_SetItem(o.get(), (Py_ssize_t)(i), v.get());
			Py_XINCREF(v.get());
		}
				
		return o.detach();
	}


	static PyObject* RR_PyInt_FromLong(long ival)
	{
#if (PY_MAJOR_VERSION == 2)
		return PyInt_FromLong(ival);
#else
		return PyLong_FromLong(ival);
#endif
	}


	PyObject* UnpackFromRRArray(boost::shared_ptr<RRBaseArray> rrarray, boost::shared_ptr<TypeDefinition> type1)
	{

		if (type1)
		{
			if (!rrarray)
			{
				if (type1->ArrayType == DataTypes_ArrayTypes_none ) throw DataTypeException("Scalars must not be null");
			}
			else
			{
				if (rrarray->GetTypeID() != type1->Type) throw DataTypeException("Data type mismatch");
				if (type1->ArrayType == DataTypes_ArrayTypes_multidimarray) throw DataTypeException("Data type mismatch");
				if (type1->ArrayType == DataTypes_ArrayTypes_array)
				{
					if (!type1->ArrayVarLength)
					{
						if (rrarray->Length() != type1->ArrayLength.at(0)) throw DataTypeException("Array length mismatch");
					}
					else if (type1->ArrayLength.at(0) != 0)
					{
						if (rrarray->Length() > type1->ArrayLength.at(0)) throw DataTypeException("Array length to long");
					}
				}
				else
				{
					if (rrarray->Length() != 1)
						throw DataTypeException("Array length mismatch");
				}
			}					
		}

		if (!rrarray)
		{
			Py_RETURN_NONE;
		}

		bool usenumpy = false;

		if (_NumPyAvailable)
		{
			PyObject* modules_dict = PyImport_GetModuleDict();
			if (modules_dict == NULL) throw InternalErrorException("Could not load RobotRaconteurPython module");
			PyObject* rr_module = PyDict_GetItemString(modules_dict, "RobotRaconteur");
			if (rr_module == NULL) throw InternalErrorException("Could not load RobotRaconetur module");
			PyAutoPtr<PyObject> usenumpy_py(PyObject_GetAttrString(rr_module, "UseNumPy"));
			if (usenumpy_py.get() != NULL)
			{
				if (PyObject_IsTrue(usenumpy_py.get()))
				{
					usenumpy = true;
				}
			}

			if (usenumpy && !(type1 && type1->ArrayType == DataTypes_ArrayTypes_none))
			{
				return UnpackFromRRArray_numpy(rrarray, type1);
			}
		}

		PyObject* o;

		switch (rrarray->GetTypeID())
		{
		case DataTypes_double_t:
			o= UnpackFromRRArray1<double>(rrarray, &PyFloat_FromDouble);
			break;
		case DataTypes_single_t:
			o= UnpackFromRRArray1<float>(rrarray, &PyFloat_FromDouble);
			break;
		case DataTypes_int8_t:
			o= UnpackFromRRArray1<int8_t>(rrarray, &RR_PyInt_FromLong);
			break;
		case DataTypes_uint8_t:
		{
			boost::shared_ptr<RRArray<uint8_t> > rrarray1 = RR_DYNAMIC_POINTER_CAST<RRArray<uint8_t> >(rrarray);
			o = PyByteArray_FromStringAndSize((const char*)rrarray1->ptr(), rrarray1->Length());
			if (!o)
			{
				throw InternalErrorException("Could not allocate return byte array");				
			}
			break;
		}
			//return UnpackFromRRArray1<uint8_t>(rrarray, &PyInt_FromLong);
		case DataTypes_int16_t:
			o= UnpackFromRRArray1<int16_t>(rrarray, &RR_PyInt_FromLong);
			break;
		case DataTypes_uint16_t:
			o= UnpackFromRRArray1<uint16_t>(rrarray, &RR_PyInt_FromLong);
			break;
		case DataTypes_int32_t:
			o= UnpackFromRRArray1<int32_t>(rrarray, &RR_PyInt_FromLong);
			break;
		case DataTypes_uint32_t:
			o= UnpackFromRRArray1<uint32_t>(rrarray, &PyLong_FromUnsignedLongLong);
			break;
		case DataTypes_int64_t:
			o= UnpackFromRRArray1<int64_t>(rrarray, &PyLong_FromLongLong);
			break;
		case DataTypes_uint64_t:
			o= UnpackFromRRArray1<uint64_t>(rrarray, &PyLong_FromUnsignedLongLong);
			break;
		default:
			throw DataTypeException("Unknown numeric data type");
		}

		if (type1)
		{
			if (type1->ArrayType == DataTypes_ArrayTypes_none)
			{
				PyObject* o1 = PySequence_GetItem(o, 0);
				Py_XDECREF(o);
				return o1;
			}
		}

		return o;
	}

	boost::shared_ptr<RRBaseArray> PackToRRArray_numpy(PyObject* array_, boost::shared_ptr<TypeDefinition> type1, boost::shared_ptr<RRBaseArray> destrrarray)
	{
		if (!PyArray_Check(array_)) throw DataTypeException("numpy array expected");

		PyArrayObject* array1 = (PyArrayObject*)array_;
		
		npy_intp len = PyArray_SIZE(array1);

		if (destrrarray)
		{
			if (destrrarray->GetTypeID() != NumPyDataTypeToRRTypeId(PyArray_TYPE(array1))) throw DataTypeException("Invalid destrrarray specified for PackRRArray");
			if (destrrarray->Length() != len) throw DataTypeException("Invalid destrrarray specified for PackRRArray");
		}
		else
		{
			destrrarray = AllocateRRArrayByType(NumPyDataTypeToRRTypeId(PyArray_TYPE(array1)), len);
		}
		
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
				if (len != 0) throw DataTypeException("Invalid numpy array");
			}
			else if (nonzero_dims != 1)
			{
				throw DataTypeException("Invalid numpy array");
			}			
		}

		PyAutoPtr<PyArrayObject> array2(PyArray_GETCONTIGUOUS(array1));
		
		if (array2.get() == NULL) throw DataTypeException("Internal error");		

		if (PyArray_NBYTES(array2.get()) != destrrarray->Length()*destrrarray->ElementSize())
			throw DataTypeException("numpy data size error in PackToRRArray");

		memcpy(destrrarray->void_ptr(), PyArray_DATA(array2.get()), PyArray_NBYTES(array2.get()));
		
		return destrrarray;
	}

	PyObject* UnpackFromRRArray_numpy(boost::shared_ptr<RRBaseArray> rrarray, boost::shared_ptr<TypeDefinition> type1)
	{
		if (!rrarray)
		{
			Py_RETURN_NONE;
		}

		if (type1)
		{
			if (rrarray->GetTypeID() != type1->Type) throw DataTypeException("Invalid array specified for UnpackFromRRArray");
			if (type1->ArrayType == DataTypes_ArrayTypes_multidimarray) throw DataTypeException("Invalid parameter for UnpackFromRRArray");
			if (type1->ArrayType == DataTypes_ArrayTypes_none)
			{
				if (rrarray->Length() != 1) throw DataTypeException("Invalid length for scalar in UnpackFromRRArray");
			}
			else
			{
				if (!type1->ArrayVarLength)
				{
					if (rrarray->Length() != type1->ArrayLength.at(0)) throw DataTypeException("Invalid length for fixed length array in UnpackFromRRArray");
				}
			}
		}

		npy_intp dims=rrarray->Length();
		PyAutoPtr<PyObject> ret1(PyArray_SimpleNewFromData(1, &dims, RRTypeIdToNumPyDataType(rrarray->GetTypeID()), rrarray->void_ptr()));
		if (ret1.get() == NULL)
		{
			throw InternalErrorException("Could not allocate numpy array");
		}
		PyObject* ret = PyArray_NewCopy((PyArrayObject*)ret1.get(), NPY_ANYORDER);
		if (ret == NULL)
		{
			throw InternalErrorException("Could not allocate numpy array");
		}		

		return ret;
	}
		
	boost::tuple<boost::shared_ptr<RRBaseArray>, boost::shared_ptr<RRBaseArray> > PackToMultiDimArray_numpy1(PyArrayObject* array1)
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
		{
			DataTypes rrtype = NumPyDataTypeToRRTypeId(npy_type);
			size_t count = (size_t)PyArray_SIZE(array1);
			boost::shared_ptr<RRBaseArray> ret = AllocateRRArrayByType(rrtype, count);

			if (count == 0) return boost::make_tuple(ret, boost::shared_ptr<RRBaseArray>());

			if (PyArray_IS_F_CONTIGUOUS(array1))
			{
				if (PyArray_NBYTES(array1) != ret->ElementSize() * count) throw DataTypeException("MultiDimArray type mismatch");
				memcpy(ret->void_ptr(), PyArray_BYTES(array1), PyArray_NBYTES(array1));
				return boost::make_tuple(ret, boost::shared_ptr<RRBaseArray>());
			}
			else
			{
				npy_intp array2_dims = count;
				PyAutoPtr<PyObject> array2(PyArray_New(&PyArray_Type, 1, &array2_dims, npy_type, &array2_dims, ret->void_ptr(), 0, NPY_ARRAY_F_CONTIGUOUS, NULL));
				if (!array2.get()) throw InternalErrorException("internal error: Could not allocate array2 for PackToMultiDimArray_numpy1");

				if (PyArray_CopyInto((PyArrayObject*)array2.get(), array1) < 0)
				{
					throw InternalErrorException("internal error: Could not copy array1 to array2 in PackToMultiDimArray_numpy1");
				}
				return boost::make_tuple(ret, boost::shared_ptr<RRBaseArray>());
			}						
		}
		case NPY_COMPLEX64:
		case NPY_COMPLEX128:
		{
			DataTypes rrtype;
			switch (npy_type)
			{
			case NPY_COMPLEX64:
				rrtype = DataTypes_single_t;
				break;
			case NPY_COMPLEX128:
				rrtype = DataTypes_double_t;
				break;
			}
			size_t count = (size_t)PyArray_SIZE(array1);
			boost::shared_ptr<RRBaseArray> real_ret = AllocateRRArrayByType(rrtype, count);
			boost::shared_ptr<RRBaseArray> imag_ret = AllocateRRArrayByType(rrtype, count);

			size_t rr_element_size = real_ret->ElementSize();
			size_t rr_array_bytes = rr_element_size*count;

			NpyIter_IterNextFunc *iternext;
			char** dataptr;

			NpyIter* iter =NpyIter_New(array1, NPY_ITER_READONLY | NPY_ITER_F_INDEX | NPY_ITER_NBO | NPY_ITER_ALIGNED, NPY_FORTRANORDER, NPY_NO_CASTING, PyArray_DescrFromType(npy_type));
			if (!iter)
			{
				PyErr_PrintEx(0);
				throw InternalErrorException("Internal error: Could not allocate iterator for NPY_COMPLEX");
			}

			iternext = NpyIter_GetIterNext(iter, NULL);
			if (iternext == NULL) 
			{
				NpyIter_Deallocate(iter);
				throw InternalErrorException("Internal error: Could not allocate iterator for NPY_COMPLEX");
			}

			dataptr = NpyIter_GetDataPtrArray(iter);

			char* real_ptr = (char*)real_ret->void_ptr();
			char* imag_ptr = (char*)imag_ret->void_ptr();
			char* real_ptr_end = real_ptr + rr_array_bytes;
			char* imag_ptr_end = imag_ptr + rr_array_bytes;

			do
			{

				if (!(real_ptr < real_ptr_end) || !(imag_ptr < imag_ptr_end))
				{
					NpyIter_Deallocate(iter);
					throw InternalErrorException("Internal error: Could not iterate into real and imag rrarrays");
				}

				char* dataptr1 = *dataptr;
				memcpy(real_ptr, dataptr1, rr_element_size);
				memcpy(imag_ptr, dataptr1 + rr_element_size, rr_element_size);

				real_ptr += rr_element_size;
				imag_ptr += rr_element_size;				

			} while (iternext(iter));
						

			NpyIter_Deallocate(iter);

			if (real_ptr != real_ptr_end || imag_ptr != imag_ptr_end) throw InternalErrorException("Internal error: Could not iterate into real and imag rrarrays");

			return boost::make_tuple(real_ret, imag_ret);
		}

		default:
			throw DataTypeException("Unsupported numpy matrix type");
		}
	}

	boost::shared_ptr<MessageElementMultiDimArray> PackToRRMultiDimArray_numpy(PyObject* array_, boost::shared_ptr<TypeDefinition> type1)
	{
		if (!_NumPyAvailable) throw InvalidOperationException("NumPy is not available");

		if (PyArray_Check(array_))
		{
			PyArrayObject* array1 = (PyArrayObject*)array_;
			int ndim = PyArray_NDIM(array1);
			boost::shared_ptr<RRArray<int32_t> > dimcount = ScalarToRRArray<int32_t>(ndim);
			npy_intp* dims1 = PyArray_SHAPE(array1);
			boost::shared_ptr<RRArray<int32_t> > dims = AllocateRRArray<int32_t>(ndim);
			for (int i = 0; i < ndim; i++) { (*dims)[i] = (int32_t)dims1[i]; }
			
			std::vector<boost::shared_ptr<MessageElement> > ret_vec;
			ret_vec.push_back(boost::make_shared<MessageElement>("dimcount", dimcount));
			ret_vec.push_back(boost::make_shared<MessageElement>("dims",dims));

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
			{
				ret_vec.push_back(boost::make_shared<MessageElement>("real", PackToMultiDimArray_numpy1(array1).get<0>()));
				return boost::make_shared<MessageElementMultiDimArray>(ret_vec);
			}
			case NPY_COMPLEX64:
			case NPY_COMPLEX128:
			{
				boost::tuple<boost::shared_ptr<RRBaseArray>, boost::shared_ptr<RRBaseArray> > array2=PackToMultiDimArray_numpy1(array1);
				ret_vec.push_back(boost::make_shared<MessageElement>("real", array2.get<0>()));
				ret_vec.push_back(boost::make_shared<MessageElement>("imag", array2.get<1>()));
				return boost::make_shared<MessageElementMultiDimArray>(ret_vec);
				
			}
			default:
				throw DataTypeException("Unsupported numpy matrix type");
			}
			
		}
		else
		{
			if (type1->Type >= DataTypes_int8_t && type1->Type <= DataTypes_uint64_t)
			{
				if (!PySequence_Check(array_)) throw DataTypeException("Invalid integer MultiDimArray format");
				if (PySequence_Size(array_) != 2) throw DataTypeException("Invalid integer MultiDimArray format");
				PyAutoPtr<PyObject> py_real(PySequence_GetItem(array_,0));
				PyAutoPtr<PyObject> py_imag(PySequence_GetItem(array_,1));

				if (!PyArray_Check(py_real.get())) throw DataTypeException("Invalid integer MultiDimArray format");
				if (!PyArray_Check(py_imag.get())) throw DataTypeException("Invalid integer MultiDimArray format");

				int npy_type1 = RRTypeIdToNumPyDataType(type1->Type);

				if (PyArray_TYPE((PyArrayObject*)py_real.get()) != npy_type1) throw DataTypeException("Invalid integer MultiDimArray type");
				if (PyArray_TYPE((PyArrayObject*)py_imag.get()) != npy_type1) throw DataTypeException("Invalid integer MultiDimArray type");

				PyArrayObject* py_real1 = (PyArrayObject*)py_real.get();
				PyArrayObject* py_imag1 = (PyArrayObject*)py_imag.get();
				int ndim_real = PyArray_NDIM(py_real1);
				int ndim_imag = PyArray_NDIM(py_imag1);
				if (ndim_real != ndim_imag) throw DataTypeException("Invalid integer MultiDimArray type: real imag mismatch");
				boost::shared_ptr<RRArray<int32_t> > dimcount = ScalarToRRArray<int32_t>(ndim_real);
				npy_intp* real_dims1 = PyArray_SHAPE(py_real1);
				npy_intp* imag_dims1 = PyArray_SHAPE(py_real1);
				boost::shared_ptr<RRArray<int32_t> > dims = AllocateRRArray<int32_t>(ndim_real);
				for (int i = 0; i < ndim_real; i++) 
				{ 
					if (real_dims1[i] != imag_dims1[i])  throw DataTypeException("Invalid integer MultiDimArray type: real imag mismatch");
					(*dims)[i] = (int32_t)real_dims1[i]; 
				}

				std::vector<boost::shared_ptr<MessageElement> > ret_vec;
				ret_vec.push_back(boost::make_shared<MessageElement>("dimcount", dimcount));
				ret_vec.push_back(boost::make_shared<MessageElement>("dims", dims));
				
				ret_vec.push_back(boost::make_shared<MessageElement>("real", PackToMultiDimArray_numpy1(py_real1).get<0>()));
				ret_vec.push_back(boost::make_shared<MessageElement>("imag", PackToMultiDimArray_numpy1(py_imag1).get<0>()));
				return boost::make_shared<MessageElementMultiDimArray>(ret_vec);

			}
			else
			{
				throw DataTypeException("Invalid numpy array format");
			}
		}
	}

	//NO ERROR CHECKING! USE WITH CAUTION!
	static PyObject* UnpackFromRRMultiDimArray_numpy1(boost::shared_ptr<RRBaseArray>& rrarray, int npy_type, int npy_dimcount, npy_intp* npy_dims)
	{
		PyAutoPtr<PyObject> array_a(PyArray_New(&PyArray_Type, npy_dimcount, npy_dims, npy_type, NULL, rrarray->void_ptr(), 0, NPY_ARRAY_F_CONTIGUOUS, NULL));
		if (!array_a.get()) throw InternalErrorException("internal error: Could not create real_a in UnpackFromRRMultiDimArray_numpy");		
		PyAutoPtr<PyObject> array_b(PyArray_SimpleNew(npy_dimcount, npy_dims, npy_type));
		if (!array_b.get()) throw InternalErrorException("internal error: Could not create real_b in UnpackFromRRMultiDimArray_numpy");
		if (PyArray_CopyInto((PyArrayObject*)array_b.get(), (PyArrayObject*)array_a.get()) < 0)
		{
			throw InternalErrorException("internal error: Could not copy real_a to real_b in UnpackFromRRMultiDimArray_numpy");
		}
				
		return array_b.detach();
	}

	//NO ERROR CHECKING! USE WITH CAUTION!
	static PyObject* UnpackFromRRMultiDimArray_numpy2(boost::shared_ptr<RRBaseArray>& rr_real, boost::shared_ptr<RRBaseArray>& rr_imag, int npy_type, int npy_dimcount, npy_intp* npy_dims)
	{
		PyAutoPtr<PyObject> ret(PyArray_SimpleNew(npy_dimcount, npy_dims, npy_type));

		size_t count = rr_real->Length();
		size_t rr_element_size = rr_real->ElementSize();
		size_t rr_array_bytes = rr_element_size*count;

		NpyIter_IterNextFunc *iternext;
		char** dataptr;

		NpyIter* iter = NpyIter_New((PyArrayObject*)ret.get(), NPY_ITER_WRITEONLY | NPY_ITER_F_INDEX | NPY_ITER_NBO | NPY_ITER_ALIGNED, NPY_FORTRANORDER, NPY_NO_CASTING, PyArray_DescrFromType(npy_type));
		if (!iter)
		{
			throw InternalErrorException("Internal error: Could not allocate iterator for NPY_COMPLEX");
		}

		iternext = NpyIter_GetIterNext(iter, NULL);
		if (iternext == NULL)
		{
			NpyIter_Deallocate(iter);
			throw InternalErrorException("Internal error: Could not allocate iterator for NPY_COMPLEX");
		}

		dataptr = NpyIter_GetDataPtrArray(iter);
		
		char* real_ptr = (char*)rr_real->void_ptr();
		char* imag_ptr = (char*)rr_imag->void_ptr();
		char* real_ptr_end = real_ptr + rr_array_bytes;
		char* imag_ptr_end = imag_ptr + rr_array_bytes;

		do
		{
			if (!(real_ptr < real_ptr_end) || !(imag_ptr < imag_ptr_end))
			{
				NpyIter_Deallocate(iter);
				throw InternalErrorException("Internal error: Could not iterate into real and imag rrarrays");
			}

			char* dataptr1 = *dataptr;
			memcpy(dataptr1, real_ptr, rr_element_size);
			memcpy(dataptr1 + rr_element_size, imag_ptr, rr_element_size);

			real_ptr += rr_element_size;
			imag_ptr += rr_element_size;

		} while (iternext(iter));
		
		NpyIter_Deallocate(iter);

		if(real_ptr != real_ptr_end || imag_ptr != imag_ptr_end)
			throw InternalErrorException("Internal error: Could not iterate into real and imag rrarrays");

		return ret.detach();
	}

	PyObject* UnpackFromRRMultiDimArray_numpy(boost::shared_ptr<MessageElementMultiDimArray> rrarray, boost::shared_ptr<TypeDefinition> type1)
	{
		if (!rrarray)
		{
			Py_RETURN_NONE;
		}

		int32_t dimcount = RRArrayToScalar(MessageElement::FindElement(rrarray->Elements, "dimcount")->CastData<RRArray<int32_t> >());
		if (dimcount < 0) throw DataTypeException("Invalid MultiDimArray");
		boost::shared_ptr<RRArray<int32_t> > dims = MessageElement::FindElement(rrarray->Elements, "dims")->CastData<RRArray<int32_t> >();
		if (!dims) throw DataTypeException("Invalid MultiDimArray");
		if (dims->Length() != dimcount) throw DataTypeException("Invalid MultiDimArray");
		boost::shared_ptr<RRBaseArray> real = MessageElement::FindElement(rrarray->Elements, "real")->CastData<RRBaseArray>();
		if (!real) throw DataTypeException("Invalid MultiDimArray");

		boost::shared_ptr<MessageElement> imag_el;
		boost::shared_ptr<RRBaseArray> imag;
		if (MessageElement::TryFindElement(rrarray->Elements, "imag", imag_el))
		{
			imag = imag_el->CastData<RRBaseArray>();
		}

		if (imag)
		{
			if (imag->GetTypeID() != real->GetTypeID() || imag->Length() != real->Length()) throw DataTypeException("Invalid MultiDimArray");
		}


		DataTypes rr_type = real->GetTypeID();
		int npy_type = RRTypeIdToNumPyDataType(rr_type);
		std::vector<npy_intp> npy_dims(dimcount);
		for (size_t i = 0; i < dimcount; i++)
		{
			if ((*dims)[i] < 0) throw DataTypeException("Invalid MultiDimArray");
			npy_dims[i] = (npy_intp)(*dims)[i];
		}

		switch (rr_type)
		{
		case DataTypes_double_t:
		case DataTypes_single_t:
		{
			if (!imag)
			{
				return UnpackFromRRMultiDimArray_numpy1(real, npy_type, dimcount, &npy_dims[0]);
			}
			else
			{
				switch (rr_type)
				{
				case DataTypes_double_t:
					npy_type = NPY_COMPLEX128;
					break;
				case DataTypes_single_t:
					npy_type = NPY_COMPLEX64;
					break;
				}

				return UnpackFromRRMultiDimArray_numpy2(real, imag, npy_type, dimcount, &npy_dims[0]);
			}
		}
		case DataTypes_int8_t:
		case DataTypes_uint8_t:
		case DataTypes_int16_t:
		case DataTypes_uint16_t:
		case DataTypes_int32_t:
		case DataTypes_uint32_t:
		case DataTypes_int64_t:
		case DataTypes_uint64_t:
		{
			if (!imag)
			{
				return UnpackFromRRMultiDimArray_numpy1(real, npy_type, dimcount, &npy_dims[0]);
			}
			else
			{
				PyAutoPtr<PyObject> ret(PyList_New(2));
				if (!ret.get()) throw InternalErrorException("internal error: UnpackFromRRMultiDimArray_numpy");
				PyList_SetItem(ret.get(), 0, UnpackFromRRMultiDimArray_numpy1(real, npy_type, dimcount, &npy_dims[0]));
				PyList_SetItem(ret.get(), 1, UnpackFromRRMultiDimArray_numpy1(imag, npy_type, dimcount, &npy_dims[0]));
				
				return ret.detach();
			}			
		}
		default:
			throw DataTypeException("Invalid MultiDimArray data type");
		}

	}

	DataTypes NumPyDataTypeToRRTypeId(int np_type)
	{
		switch (np_type)
		{
		case NPY_FLOAT64:
			return DataTypes_double_t;
		case NPY_FLOAT32:
			return DataTypes_single_t;
		case NPY_INT8:
			return DataTypes_int8_t;
		case NPY_UINT8:
			return DataTypes_uint8_t;
		case NPY_INT16:
			return DataTypes_int16_t;
		case NPY_UINT16:
			return DataTypes_uint16_t;
		case NPY_INT32:
			return DataTypes_int32_t;
		case NPY_UINT32:
			return DataTypes_uint32_t;
		case NPY_INT64:
			return DataTypes_int64_t;
		case NPY_UINT64:
			return DataTypes_uint64_t;
		default:
			throw DataTypeException("Unknown numpy data type");
		}
	}

	int RRTypeIdToNumPyDataType(DataTypes rr_type)
	{
		switch (rr_type)
		{
		case DataTypes_double_t:
			return NPY_FLOAT64;
		case DataTypes_single_t:
			return NPY_FLOAT32;
		case DataTypes_int8_t:
			return NPY_INT8;
		case DataTypes_uint8_t:
			return NPY_UINT8;
		case DataTypes_int16_t:
			return NPY_INT16;
		case DataTypes_uint16_t:
			return NPY_UINT16;
		case DataTypes_int32_t:
			return NPY_INT32;
		case DataTypes_uint32_t:
			return NPY_UINT32;
		case DataTypes_int64_t:
			return NPY_INT64;
		case DataTypes_uint64_t:
			return NPY_UINT64;
		default:
			throw DataTypeException("Unknown numpy data type");
		}
	}

	static int PythonTypeSupport_Init1()
	{
		return _import_array();
	}

	void PythonTypeSupport_Init()
	{
		if (PythonTypeSupport_Init1() >= 0)
		{
			_NumPyAvailable = true;
		}
		PyErr_Clear();
	}

}



