# C++ Value Types {#cpp_value_types}

Each valid Robot Raconteur type has a corresponding C++ data type.  This mapping is similar to Python. The following table shows the mapping between Robot Raconteur and C++ data types:

| Robot Raconteur Type | C++ Type    | Notes |
| ---                  | ---         | ---   |
| `double`             | `double`    |       |
| `single`             | `float`     |       |
| `int8`               | `int8_t`    |       |
| `uint8`              | `uint8_t`   |       |
| `int16`              | `int16_t`   |       |
| `uint16`             | `uint16_t`  |       |
| `int32`              | `int32_t`   |       |
| `uint32`             | `uint32_t`  |       |
| `int64`              | `int64_t`   |       |
| `uint64`             | `uint64_t`  |       |
| `double[]`           | `RRArrayPtr<double>`   |      |
| `single[]`           | `RRArrayPtr<float>`    |      |
| `int8[]`             | `RRArrayPtr<int8_t>`   |      |
| `uint8[]`            | `RRArrayPtr<uint8_t>`  |      |
| `int16[]`            | `RRArrayPtr<int16_t>`  |      |
| `uint16[]`           | `RRArrayPtr<uint16_t>` |      |
| `int32[]`            | `RRArrayPtr<int32_t>`  |      |
| `uint32[]`           | `RRArrayPtr<uint32_t>` |      |
| `int64[]`            | `RRArrayPtr<int64_t>`  |      |
| `uint64[]`           | `RRArrayPtr<uint64_t>` |      |
| `N[*]`               | `RRMultiDimArrayPtr<N>` | Multi-dim array of type numeric type `N` |
| `string`             | `std::string` or `RRArrayPtr<char>` | Strings are always UTF-8 encoded |
| `struct`             | *varies*               | Use generated C++ type |
| `pod`                | *varies*               | Use generated C++ type |
| `T[]` where T is a `pod` | `RRPodArrayPtr<T>`     | Pod array with C++ type `T` |
| `T[*]` where T is a `pod` | `RRPodMultiDimArrayPtr<T>`     | Pod multi-dim array with C++ type `T` |
| `namedarray`         | *varies*               | Use generated C++ `union` type |
| `T[]` where T is a `namedarray` | `RRNamedArrayPtr<T>`     | Named array with C++ type `T` |
| `T[*]` where T is a `namedarray` | `RRNamedMultiDimArrayPtr<T>`     | Named multi-dim array with C++ type `T` |
| `T{int32}`           | `RRMapPtr<int32_t,T>`  | Map type, `T` is a template |
| `T{string}`          | `RRMapPtr<std::string,T>` | Map type, `T` is a template |
| `T{list}`            | `RRListPtr<T>`         | List type, `T` is a template |
| `varvalue`           | `RRValuePtr` |       |
| `varobject`          | `RRObjectPtr` |       |

## Stack Values and Value Smart Pointers {#cpp_stack_and_pointers}

C++ data can be ["stack" and "heap"](https://www.learncpp.com/cpp-tutorial/79-the-stack-and-the-heap/) allocated. The stack is local contiguous memory allocated when a function is called. "Stack allocated" data is stored in this contiguous memory region. Stack allocation has the advantage of being pre-allocated, and destroyed automatically. Stack data storage needs to have a fixed size known at compile time, and is automatically destroyed when the function exits, meaning that any pointers or references to that data will be invalid once the function exits. Heap allocated memory allocated dynamically using `new`, and must be freed with `delete`. A pointer to the allocated memory is returned by `new`. Smart pointers can be used to determine when the allocated memory should be deleted by keeping an active count of how many pointers exist to the data.

C++ structures and class instances are stored as contiguous memory, with individual fields stored within parts of the contiguous memory. The memory used to store structures and class instances can be allocated on the stack or the heap. Fields which are stack allocated types are stored within this contiguous memory, regardless if the structure/class memory was allocated on the stack or the heap. Pointers are used for fields which are heap allocated.

Robot Raconteur uses both stack and heap allocated memory to store value types. Stack allocation is used to store scalar numbers, scalar `pod`, scalar `namedarray`, and fields contained in both `pod` and `namedarray`. Heap allocation is used for all other types, using `boost::intrusive_ptr` (or alias `RR_INTRUSIVE_PTR`) to track the number of active pointers and delete when the count goes to zero. All heap allocated Robot Raconteur value types inherit from RobotRaconteur::RRValue. RRValue inherits `boost::intrusive_ref_counter`, implementing the reference counting storage required for `boost::intrusive_ptr`. Convenience aliases are used to simplify the declarations of types using smart pointers. These convenience alias end with `Ptr`.

**Note that value types are passed by value, meaning that the data is copied between nodes. Modifying the local data wil not change the remote data. The modified data must be sent using a member.**

## Null Pointers {#cpp_null_pointers}

The `boost::intrusive_ptr` can contain `nullptr`. An attempt to dereference a `nullptr` will result in a segmentation fault, which will typically immediately terminate the program. The RobotRaconteur::rr_null_check() can be used to check if a smart pointer. is null. It will throw RobotRaconteur::NullReferenceException if the pointer is `nullptr`.

## Boxing {#cpp_boxing}

Container types like `map` and `list`, and `varvalue` can only store types inheritin from RobotRaconteur::RRValue and stored in a `boost::intrusive_ptr`. Since some value types are stored on the stack instead of in a smart pointer, these types cannot be stored in the container types. These values must be "boxed", meaning they are converted to a form that con be stored in the container. The procedure to box and unbox types when required is discussed for each type.

## Numeric Types and Arrays {#cpp_numeric_types}

Robot Raconteur supports floating point, integer, unsigned integer, complex, and logical types as shown in the table above. Floating point numbers use `double` and `float`. Integer types use types defined in `<stdint.h>`. Complex numbers use RobotRaconteur::cdouble and RobotRaconteur::csingle. Logical values use RobotRaconteur::rr_bool.

Scalar values use stack allocated, and are not stored using pointers are smart pointers. Some example declarations:

    using namespace RobotRaconteur;
    int32_t a = 1;
    double b = 1.234;
    cdouble c = (1.23,4.56);

Numeric arrays are stored using RobotRaconteur::RRArray, with smart pointer alias RobotRaconteur::RRArrayPtr. RobotRaconteur::RRArray must always be stored in a smart pointer. RobotRaconteur::RRArray is similar to `std::array<T>`, and implements most of the C++ container concept.

RobotRaconteur::RRArrayPtr is allocated using RobotRaconteur::AllocateRRArray(), RobotRaconteur::AttachRRArray(), RobotRaconteur::AttachRRArrayCopy(), RobotRaconteur::AllocateRRArrayByType(), or AllocateEmptyRRArray(). The most commonly used are RobotRaconteur::AttachRRArray() and RobotRaconteur::AttachRRArrayCopy(). A few examples:

    using namespace RobotRaconteur;
    RRArrayPtr<double> my_array1 = AllocateRRArray<double>(10);
    RRArrayPtr<cdouble> = my_array2 = AllocateRREmptyArray(16);

    double existing_data[4] = {1.1, 2.2, 3.3, 4.4};
    RRArrayPtr<double> my_array3 = AttachRRArrayCopy(existing_data, 4);

Once the array is allocated, it can be used the same as a `std::array<T>*` type.

    double v1 = my_array1->at(0);
    double v2 = (*my_array1)[1];
    my_array1->at(2) = 5.5;
    (*my_array1)[3] = 6.6;
    double* my_array1_raw = my_array1->data();

RobotRaconteur::RRArray implements `begin()` and `end()`, so it can be used as an STL container.

    for (double v : *my_array)
    {
        // Use v
    }

Becaue RobotRaconteur::RRArrayPtr is a smart pointer, the allocated memory is destroyed automatically.

Multidimensional arrays stored using RobotRaconteur::RRMultiDimArray. This type has two fields, `Dims` and `Array`. The `Dims` field contains the shape of the array, in column-major order. The `Array` field contains the array element data, stored in column-major (Fortran) order. RobotRaconteur::RRMultiDimArrayPtr is allocated using RobotRaconteur::AllocateRRMultiDimArray() or RobotRaconteur::AllocateEmptyRRMultiDimArray(). An examples:

    using namespace RobotRaconteur;
    std::vector<uint32_t> dims = {3,3};
    RRMultiDimArrayPtr<double> my_multidimarray = AllocateEmptyRRMultiDimArray<double>(dims);

RRArray and RRMultiDimArray are non-nullable, meaning that they cannot be `nullptr`. If a `nullptr` of this type is passed to a member, it will result in an error.

### Numeric Types Boxing

Scalar numbers cannot be stored in containers because they are stack allocated types. The functions RobotRaconteur::ScalarToRRArray() and RobotRaconteur::RRArrayToScalar() are used to box and unbox scalars. They convert scalars to and from single element arrays.

## Strings {#cpp_string_types}

Strings are stored as `std::string`. Strings are always UTF-8 formatted. When passed as parameters, `const std::string&` is used to prevent making a copy.

Strings cannot be stored in an array or multidimarray. They can be stored in `list` and `map` when boxed.

### String Boxing

`std::string` cannot be stored in container types since it does not inherit from RobotRaconteur::RRValue. The boxed type for strings is `RobotRaconteur::RRArrayPtr<char>`. Use RobotRaconteur::stringToRRArray() and RobotRaconteur::RRArrayToString() to box and unbox `std::string`.

Strings are non-nullable, meaning that `RRArrayPtr<char>` cannot be `nullptr`. If a `nullptr` of this type is passed to a member, it will result in an error.

## Structure Types {#cpp_structure_types}

Structure types are defined in service definitions using the `struct` keyword. See \ref service_definition for more information on `struct` definitions. The implementation for structure types defined in service definitions are generated by `RobotRaconteurGen` as part of the thunk source. Structures are always heap allocated, and use `boost::intrusive_ptr` smart pointer. The thunk source includes a *`Ptr` convenience aliases for structure types.

Structures can store any valid Robot Raconteur value type, including itself. Structures may not be stored in arrays or multidimarrays, but may be stored in `list` and `map` types.

Structure types must be created using `new` and stored in a smart pointer. Consider the following example structure definition:

    service experimental.struct_example

    struct MyStruct
        field double a
        field double[] b
    end

To create and use in C++:

    experimental::struct_example::MyStructPtr my_struct(new experimental::struct_example::MyStruct());

    my_struct->a = 10;
    my_struct->b = AllocateRRArray<double>(10);
    my_struct->b->at(2) = 5.7;

Struct types do not require boxing.

Struture types are nullable. RobotRaconteur::rr_null_check() should be used to test for a null structure before use if it is being received from a remote node.

## Pod Types {#cpp_pod_types}

Pod types are defined in service definitions using the keyword `pod`. See \ref service_definition for more information an `pod` definitions. Pods are similar in concept to structures, except that they are stack allocated types, with the memory for all fields local to the pod. Pointers are not used to store field data. Instead, the storage memory is in-line with the pod itself. Pod arrays are stored in a contiguous c-style array, with one pod following the other in memory. In C++, this is referred to as "plain-old-data", hence the keyword `pod`. Pods are always the same size.

The implementation for pod types defined in service definitions are generated by `RobotRaconteurGen` as part of the thunk source. Pods may be stored in arrays using RobotRaconteur::RRPodArray and RobotRaconteur::RRPodMultiDimArray. These types operate the same as RobotRaconteur::RRArray and RobotRaconteur::RRMultiDimArray, but store pods instead of numeric types. They are allocated using RobotRaconteur::AllocateEmptyRRPodArray() and RobotRaconteur::AllocateEmptyRRPodMultiDimArray().

Pods can contain numeric types, numeric arrays with fixed or maximum length, fixed shape numeric multidimarrays, other pods, other pod arrays with fixed or maximum length, fixed shape pod multidimarrays, namedarrays, namedarrays arrays with fixed or maximum length, and fixed shape namedarray multidimarrays.

Array fields for pods are implemented in C++ using the RobotRaconteur::pod_field_array structure. This structure is stack allocated, keeping the storage memory local to where it is used. It has a fixed length. For variable maximum size arrays, the memory is always allocated for the full size array, and a separate value tracks the current length. RobotRaconteur::pod_field_array behaves identically to `std::array` when fixed length, and only changes in having a variable size up to a maximum value when not fixed. `resize()` and `size()` is used to change and read the size. Multidimarrays are stored as flattened arrays in column-major order.

### Pod Boxing

Because scalar pods are stack allocated, they cannot be stored in containers or passed as `varvalue`. The functions RobotRaconteur::ScalarToRRPodArray() and RobotRaconteur::RRPodArrayToScalar() are used to box and unbox scalar pods. They convert scalar pods to and from single element pod arrays.

## Namedarray Types {#cpp_namedarray_types}

Pod types are defined in service definitions using the keyword `namedarray`. See \ref service_definition for more information an `namedarray` definitions. Namedarrays are used when data can be interpreted as either a structure or an array. An example is a three element vector. It can be interpreted as a structure with (x,y,z), or as a three element array. The numeric type in a namedarray must be the same for all fields. In C++, `union` types to represent namedarray. (Note that `union` types are stack allocated.) An example of the union type for a three element vector defined in a service definition as:

    namedarray
        field double x
        field double y
        field double z
    end

Is implemented as:

    union Vector3
    {
        double a[3];
        struct s_type {
            double x;
            double y;
            double z;
        } s;
    };

In this example, the union has two fields. Field `a` is a three element array, while field `s` is a structure with fields `x`, `y`, and `z`. The union store `a` and `s` in the same memory region, meaning that accessing an array element in `a` is equivalent to accessing a field in `s`. For example:

    Vector3 my_vector;
    my_vector.s.y = 2;
    assert(my_vector.a[1] == 2) // true!

`my_vector.s.y` is stored in the same memory location as `my_vector.a[1]` because of the use of a union.

Namedarray may contain fixed length numeric arrays, other namedarray, and other namedarray fixed length arrays. The numeric type of the other namedarrays must match the namedarray field type. When arrays are used as a field within a named array, a c-style array is used. For example:

    namedarray MyNamedArray1
        field double a
        field double[3] b
    end

Will have the implementation:

    union MyNamedArray1
    {
        double a[4];
        struct s_type {
            double a;
            double[3] b;
        } s;
    };

Arrays of namedarray may be stored in RobotRaconteur::RRNamedArray and RobotRaconteur::RRNamedMultiDimArray. They are allocated using RobotRaconteur::AllocateEmptyRRNamedArray() and RobotRaconteur::AllocateEmptyRRNamedMultiDimArray(). The functions RobotRaconteur::RRNamedArray::GetNumericArray() can be used to retrieve the entire array as a flattened numeric array. The constructor RobotRaconteur::RRNamedArray::RRNamedArray() can be used to provide an existing array to store the array data. The RobotRaconteur::RRNamedArray provides a union view to the existing data.

### Namedarray Boxing

Because unions are stack allocated, they cannot be stored in containers or passed as `varvalue`. The functions RobotRaconteur::ScalarToRRNamedArray() and RobotRaconteur::RRNamedArrayToScalar() are used to box and unbox namedarray unions. They convert unions to and from single element namedarray arrays.

## Map and List Container Types {#cpp_container_types}

Containers are used to store other values in a map or a list. They can store any valid Robot Raconteur value type, except for other containers. For example, `string{list}{list}` is invalid because it contains a container of a container. (The exception to this rule is if the container is used as a `varvalue`, see below.) The value type is always a `boost::intrusive_ptr` type. Stack allocated scalar types must be "boxed" to convert them to a form that can be stored in containers. See the individual types for more information.

Maps are stored using RobotRaconteur::RRMap. This type must always be stored in a `boost::intrusive_ptr`, optionally using the convenience alias RobotRaconteur::RRMapPtr. RobotRaconteur::RRMap is a template type, taking a key and a value. The key must be either `int32_t` or `std::string`. RobotRaconteur::RRMap is roughly equivalent to `std::map`. It can be allocated using RobotRaconteur::AllocateEmptyRRMap().

Lists are stored using RobotRaconteur::RRList. This type must always be stored in a `boost::intrusive_ptr`, optionally using the convenience alias RobotRaconteur::RRListPtr. RobotRaconteur::RRList is a template type with a single value type template parameter.RobotRaconteur::RRMap is roughly equivalent to `std::list`. It can be allocated using RobotRaconteur::AllocateEmptyRRList().

## Varvalue Type {#cpp_varvalue_type}

The varvalue type is used with service members to declare a "wildcard" parameter or packet type. It uses RobotRaconteur::RRValue for storage. This type must always be stored in `boost::intrusive_ptr`, optionally using the convenience alias RobotRaconteur::RRValuePtr. Scalar types must be "boxed" to be stored as varvalue. See the individual types for more information.

The RobotRaconteur::RRValue must be downcasted to be used. This can be accomplished using `boost::dynamic_pointer_cast`. This function will attempt to cast the pointer, returning `nullptr` if the cast is invalid. For instance, assume that the function `get_a()` returns a varvalue. Use `boost::dynamic_pointer_cast` to determine what type it is:

    using namespace RobotRaconteur;
    RRValuePtr a = client->get_a();
    RRArrayPtr<double> a_double = boost::dynamic_pointer_cast<RRArray<double> >(a);
    if (a_double)
    {
        // Do something with a_double
    }
    RRArrayPtr<char> a_char = boost::dynamic_pointer_cast<RRArray<char> >(a);
    if (a_char)
    {
        std::string a_string = RRArrayToString(a_char);
        // Do something with a_string
    }

    // Oops, unexpected type returned!
    throw InvalidOperationException("Unexpected varvalue type");

`varvalue` may also be declared as a container type. The following table shows the corresponding C++ type:

| Robot Raconteur Type | C++ Type |
| ---                  | ---      |
| `varvalue`           | `RRValuePtr` |
| `varvalue{list}`     | `RRListPtr<RRValue>` |
| `varvalue{int32}`    | `RRMapPtr<int32_t,RRValue>` |
| `varvalue{string}`   | `RRMapPtr<string,RRValue>` |

Containers passed with `varvalue` may contain other containers. The prohibition of containers containing other containers is only for declared types.
