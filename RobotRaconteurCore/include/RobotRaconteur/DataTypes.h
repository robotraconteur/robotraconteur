/**
 * @file DataTypes.h
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

#include <string>
#include <vector>

#include <stdint.h>
#include <map>
#include <deque>
#include <list>

#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>

#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/Error.h"

#include <stdexcept>

#include "RobotRaconteur/RobotRaconteurConfig.h"

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
#include <boost/container/small_vector.hpp>
#endif

#include <boost/asio/buffer.hpp>

#include <boost/utility/value_init.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/numeric.hpp>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/utility/string_ref.hpp>

#include <boost/variant.hpp>

#pragma once

namespace RobotRaconteur
{
/**
 * @brief Complex double precision floating point number
 *
 * 128-bit complex float
 */
struct ROBOTRACONTEUR_CORE_API cdouble
{
    /** @brief real component */
    double real;
    /** @brief imaginary component */
    double imag;
    /** @brief Construct a new cdouble with 0.0 real and imag */
    cdouble() : real(0.0), imag(0.0) {}
    /**
     * @brief Construct a new cdouble
     *
     * @param r real component
     * @param i imaginary component
     */
    cdouble(double r, double i) : real(r), imag(i) {}
};

/**
 * @brief Complex single precision floating point number
 *
 * 64-bit complex float
 */
struct ROBOTRACONTEUR_CORE_API cfloat
{
    /** @brief real component */
    float real;
    /** @brief imaginary component */
    float imag;
    /** @brief Construct a new csingle with 0.0 real and imag */
    cfloat() : real(0.0), imag(0.0) {}
    /**
     * @brief Construct a new cfloat
     *
     * @param r real component
     * @param i imaginary component
     */
    cfloat(float r, float i) : real(r), imag(i) {}
};

/**
 * @brief Logical boolean represented using 8 bits
 *
 * C++ does not have a standard 8 bit representation for boolean. Use
 * this struct for booleans with Robot Raconteur messages.	 *
 */
struct ROBOTRACONTEUR_CORE_API rr_bool
{
    /**
     * @brief The vaue of the boolean
     *
     * 0 for `false`, >0 for `true`
     */
    uint8_t value;

    /** @brief Construct a new `false` rr_bool */
    rr_bool() : value(0) {}
    /**
     * @brief Construct a new rr_bool object
     *
     * @param b initial logical value
     */
    rr_bool(uint8_t b) : value(b) {}
};

ROBOTRACONTEUR_CORE_API bool operator==(const cdouble& c1, const cdouble& c2);
ROBOTRACONTEUR_CORE_API bool operator!=(const cdouble& c1, const cdouble& c2);
ROBOTRACONTEUR_CORE_API bool operator==(const cfloat& c1, const cfloat& c2);
ROBOTRACONTEUR_CORE_API bool operator!=(const cfloat& c1, const cfloat& c2);
ROBOTRACONTEUR_CORE_API bool operator==(const rr_bool& c1, const rr_bool& c2);
ROBOTRACONTEUR_CORE_API bool operator!=(const rr_bool& c1, const rr_bool& c2);

template <typename T, typename U>
class rr_cast_support
{
  public:
    static RR_SHARED_PTR<T> rr_cast(const RR_SHARED_PTR<U>& objin)
    {
        if (!objin)
            return RR_SHARED_PTR<T>();

        RR_SHARED_PTR<T> c = RR_DYNAMIC_POINTER_CAST<T>(objin);
        if (!c)
        {
            throw DataTypeMismatchException("Data type cast error");
        }

        return c;
    }

    static RR_INTRUSIVE_PTR<T> rr_cast(const RR_INTRUSIVE_PTR<U>& objin)
    {
        if (!objin)
            return RR_INTRUSIVE_PTR<T>();

        RR_INTRUSIVE_PTR<T> c = RR_DYNAMIC_POINTER_CAST<T>(objin);
        if (!c)
        {
            throw DataTypeMismatchException("Data type cast error");
        }

        return c;
    }
};

template <typename T>
class rr_cast_support<T, T>
{
  public:
    static const RR_SHARED_PTR<T>& rr_cast(const RR_SHARED_PTR<T>& objin) { return objin; }

    static const RR_INTRUSIVE_PTR<T>& rr_cast(const RR_INTRUSIVE_PTR<T>& objin) { return objin; }
};

/**
 * @brief Dynamic cast a RR_SHARED_PTR type. Throws DataTypeMismatchException if cast
 * is invalid instead of returning null
 *
 * @tparam T Output cast pointer type
 * @tparam U Input pointer type
 * @param objin The object to cast
 * @return RR_SHARED_PTR<T> The object dynamically cast to RR_SHARED_PTR of type T
 */
template <typename T, typename U>
static RR_SHARED_PTR<T> rr_cast(const RR_SHARED_PTR<U>& objin)
{
    return rr_cast_support<T, U>::rr_cast(objin);
}

/**
 * @brief Dynamic cast a RR_INTRUSIVE_PTR type. This is used to cast value types stored
 * using intrusive pointers. Throws DataTypeMismatchException if cast
 * is invalid instead of returning null
 *
 * @tparam T Output cast pointer type
 * @tparam U Input pointer type
 * @param objin The object to cast
 * @return RR_INTRUSIVE_PTR<T> The object dynamically cast to RR_INTRUSIVE_PTR of type T
 */
template <typename T, typename U>
static RR_INTRUSIVE_PTR<T> rr_cast(const RR_INTRUSIVE_PTR<U>& objin)
{
    return rr_cast_support<T, U>::rr_cast(objin);
}

/**
 * @brief Base class for all Robot Raconteur objects
 *
 * RRObject is always stored in RR_SHARED_PTR smart pointers. Allocating
 * on stack or without the RR_SHARED_PTR smart pointer is undefined. RR_MAKE_SHARED
 * should always be used to allocate subclasses of RRObject.	 *
 */
class ROBOTRACONTEUR_CORE_API RRObject : boost::noncopyable
{
  public:
    RRObject();

    virtual ~RRObject() {}

    /**
     * @brief Gets the type of the object as a string. This string is in
     * C++ format, using two colons to separate namespaces instead of dots.
     *
     * @return std::string
     */
    virtual std::string RRType() = 0;
};

/**
 * @brief Base class for all Robot Raconteur value types (except primitives)
 *
 * RRValue is always stored in an RR_INTRUSIVE_PTR container. Allocating on the
 * stack is undefined. RRValue extends boost::intrusive_ref_counter to support
 * RR_INTRUSIVE_PTR
 *
 */
class ROBOTRACONTEUR_CORE_API RRValue : public boost::intrusive_ref_counter<RRValue>, boost::noncopyable
{
  public:
    RRValue();

    virtual ~RRValue() {}

    virtual std::string RRType() = 0;
};

namespace detail
{
class ROBOTRACONTEUR_CORE_API MessageStringData
{
  public:
    std::string str;
};

class ROBOTRACONTEUR_CORE_API MessageStringData_string_ref
{
  public:
    boost::string_ref ref;
    MessageStringData_string_ref(const boost::string_ref& r) : ref(r) {}
};
class ROBOTRACONTEUR_CORE_API MessageStringData_static_string
{
  public:
    boost::string_ref ref;
    MessageStringData_static_string(const boost::string_ref& r) : ref(r) {}
};

} // namespace detail

class ROBOTRACONTEUR_CORE_API MessageStringRef;

class ROBOTRACONTEUR_CORE_API MessageStringPtr
{
  private:
    boost::variant<detail::MessageStringData, detail::MessageStringData_static_string> _str_ptr;

  public:
    friend class MessageStringRef;

    MessageStringPtr();

    MessageStringPtr(const std::string& str);
    MessageStringPtr(boost::string_ref str, bool is_static = false);
    MessageStringPtr(const MessageStringPtr& str_ptr);
    MessageStringPtr(const MessageStringRef& str_ref);
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    MessageStringPtr(std::string&& str);
#endif
    // WARNING: ONLY USE WITH STRING LITERALS OR STATIC STRINGS!
    template <size_t N>
    inline MessageStringPtr(const char (&str)[N]) // NOLINT(cppcoreguidelines-avoid-c-arrays)
    {
        init_literal(str, strlen(str));
    }

    boost::string_ref str() const;
    void reset();

    bool operator==(MessageStringRef b) const;
    bool operator!=(MessageStringRef b) const;
    bool operator<(MessageStringRef b) const;

  private:
    void init_literal(const char* str, size_t len);
};

class ROBOTRACONTEUR_CORE_API MessageStringRef
{
  private:
    boost::variant<const detail::MessageStringData*, detail::MessageStringData_static_string,
                   detail::MessageStringData_string_ref>
        _str;

  public:
    friend class MessageStringPtr;

    MessageStringRef(const std::string& str);
    MessageStringRef(boost::string_ref str, bool is_static = false);
    MessageStringRef(const MessageStringPtr& str_ptr);
    MessageStringRef(const MessageStringRef& str_ref);

    // WARNING: ONLY USE WITH STRING LITERALS OR STATIC STRINGS!
    template <size_t N>
    inline MessageStringRef(const char (&str)[N]) // NOLINT(cppcoreguidelines-avoid-c-arrays)
    {
        init_literal(str, strlen(str));
    }

    bool operator==(MessageStringRef b) const;
    bool operator!=(MessageStringRef b) const;

    boost::string_ref str() const;

  private:
    void init_literal(const char* str, size_t len);
};

std::size_t hash_value(const RobotRaconteur::MessageStringPtr& k);

ROBOTRACONTEUR_CORE_API std::ostream& operator<<(std::ostream& out, const MessageStringPtr& str);
ROBOTRACONTEUR_CORE_API std::ostream& operator<<(std::ostream& out, const MessageStringRef& str);

// boost::string_ref operations
inline boost::iterator_range<boost::string_ref::const_iterator> to_range(const boost::string_ref& str)
{
    return boost::iterator_range<boost::string_ref::const_iterator>(str.cbegin(), str.cend());
}

inline std::string operator+(const char* lhs, boost::string_ref rhs) { return lhs + rhs.to_string(); }

inline std::string operator+(const std::string& lhs, boost::string_ref rhs) { return lhs + rhs.to_string(); }

inline std::string operator+(boost::string_ref lhs, const char* rhs) { return lhs.to_string() + rhs; }

/**
 * @brief Base class for types that can be stored in MessageElement
 *
 */
class ROBOTRACONTEUR_CORE_API MessageElementData : public RRValue
{
  public:
    virtual MessageStringPtr GetTypeString() = 0;
    virtual DataTypes GetTypeID() = 0;
};

template <typename T>
class RRArray;

/**
 * @brief Convert a scalar number into an array with one element
 *
 * @tparam T The type of the number
 * @param value The value of the number
 * @return RR_INTRUSIVE_PTR<RRArray<T> > The new array with one element
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > ScalarToRRArray(T value);

/**
 * @brief Convert an array with one element into a scalar
 *
 * @tparam T The type of the number
 * @param value The array with one element
 * @return T The scalar value of the first element in the array
 */
template <typename T>
static T RRArrayToScalar(const RR_INTRUSIVE_PTR<RRArray<T> >& value);

/**
 * @brief Convert a string to an array of characters
 *
 * This function creates a new copy of the data stored in the returned array
 *
 * @param str The string t convert
 * @return RR_INTRUSIVE_PTR<RRArray<char> >
 */
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRArray<char> > stringToRRArray(boost::string_ref str);

/**
 * @brief Convert an array of characters into std::string
 *
 * This function creates a new copy of the data stored in the returned std::string.
 *
 * @param arr The array of characters
 * @return std::string
 */
ROBOTRACONTEUR_CORE_API std::string RRArrayToString(const RR_INTRUSIVE_PTR<RRArray<char> >& arr);

template <typename T>
class RRPrimUtil
{
  public:
    static DataTypes GetTypeID() { return DataTypes_void_t; }

    static RR_INTRUSIVE_PTR<T> PrePack(const T& val) { return val; }

    template <typename U>
    static T PreUnpack(U& val)
    {
        return rr_cast<T>(val);
    }

    virtual ~RRPrimUtil(){};

    typedef RR_INTRUSIVE_PTR<RRValue> BoxedType;
};

template <typename T>
class RRPrimUtil<RR_INTRUSIVE_PTR<T> >
{
  public:
    static DataTypes GetTypeID() { return DataTypes_void_t; }
    static MessageStringPtr GetElementTypeString() { return MessageStringPtr(""); }

    static boost::string_ref GetRRElementTypeString() { return ""; }

    static RR_INTRUSIVE_PTR<RRValue> PrePack(const RR_INTRUSIVE_PTR<T>& val) { return rr_cast<RRValue>(val); }

    template <typename U>
    static RR_INTRUSIVE_PTR<T> PreUnpack(const U& val)
    {
        return rr_cast<T>(val);
    }

    virtual ~RRPrimUtil(){};

    typedef RR_INTRUSIVE_PTR<T> BoxedType;
};

template <>
class RRPrimUtil<std::string>
{
  public:
    static DataTypes GetTypeID() { return DataTypes_string_t; }
    static MessageStringPtr GetElementTypeString() { return MessageStringPtr(""); }
    static boost::string_ref GetRRElementTypeString() { return ""; }
    static RR_INTRUSIVE_PTR<RRValue> PrePack(boost::string_ref val) { return rr_cast<RRValue>(stringToRRArray(val)); }
    static std::string PreUnpack(const RR_INTRUSIVE_PTR<RRValue>& val)
    {
        return RRArrayToString(rr_cast<RRArray<char> >(val));
    }

    virtual ~RRPrimUtil(){};

    typedef RR_INTRUSIVE_PTR<RRArray<char> > BoxedType;
};

#define RRPrimUtilNumeric(x, code)                                                                                     \
    template <>                                                                                                        \
    class RRPrimUtil<x>                                                                                                \
    {                                                                                                                  \
      public:                                                                                                          \
        static DataTypes GetTypeID() { return code; }                                                                  \
        static MessageStringPtr GetElementTypeString() { return MessageStringPtr(""); }                                \
        static boost::string_ref GetRRElementTypeString() { return ""; }                                               \
        static RR_INTRUSIVE_PTR<RRArray<x> > PrePack(const x& val) { return ScalarToRRArray(val); }                    \
        template <typename U>                                                                                          \
        static x PreUnpack(const U& val)                                                                               \
        {                                                                                                              \
            return RRArrayToScalar(rr_cast<RRArray<x> >(val));                                                         \
        }                                                                                                              \
        typedef RR_INTRUSIVE_PTR<RRArray<x> > BoxedType;                                                               \
    };

RRPrimUtilNumeric(double, DataTypes_double_t);
RRPrimUtilNumeric(float, DataTypes_single_t);

RRPrimUtilNumeric(int8_t, DataTypes_int8_t);
RRPrimUtilNumeric(uint8_t, DataTypes_uint8_t);
RRPrimUtilNumeric(int16_t, DataTypes_int16_t);
RRPrimUtilNumeric(uint16_t, DataTypes_uint16_t);
RRPrimUtilNumeric(int32_t, DataTypes_int32_t);
RRPrimUtilNumeric(uint32_t, DataTypes_uint32_t);
RRPrimUtilNumeric(int64_t, DataTypes_int64_t);
RRPrimUtilNumeric(uint64_t, DataTypes_uint64_t);
RRPrimUtilNumeric(char, DataTypes_string_t);
RRPrimUtilNumeric(cdouble, DataTypes_cdouble_t);
RRPrimUtilNumeric(cfloat, DataTypes_csingle_t);
RRPrimUtilNumeric(rr_bool, DataTypes_bool_t);

ROBOTRACONTEUR_CORE_API std::string GetRRDataTypeString(DataTypes type);
ROBOTRACONTEUR_CORE_API bool IsTypeRRArray(DataTypes type);
ROBOTRACONTEUR_CORE_API bool IsTypeNumeric(DataTypes type);

/**
 * @brief Base class for numeric and character array value types
 *
 * Arrays should be allocated using AllocateRRArray<T>(),
 * AttachRRArray<T>(), AttachRRArrayCopy<T>(), AllocateRRArrayByType<T>(),
 * or AllocateEmptyRRArray<T>()
 *
 */
class ROBOTRACONTEUR_CORE_API RRBaseArray : public MessageElementData
{
  public:
    RR_OVIRTUAL MessageStringPtr GetTypeString() RR_OVERRIDE
    {
        std::string type = GetRRDataTypeString(GetTypeID());
        return type + "[]";
    }

    RR_OVIRTUAL ~RRBaseArray() RR_OVERRIDE {}

    /**
     * @brief Get the number of elements in the array
     *
     * @return size_t
     */
    virtual size_t size() = 0;

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE;

    /**
     * @brief Get a void pointer to the contained array
     *
     * @return void*
     */
    virtual void* void_ptr() = 0;

    /**
     * @brief Get the number of bytes per element of the array
     *
     * @return size_t
     */
    virtual size_t ElementSize() = 0;
};

/**
 * @brief Numeric primitive or character array value type
 *
 * This class stores a numeric primitive or character array. Arrays should
 * always be  allocated using AllocateRRArray<T>(),
 * AttachRRArray<T>(), AttachRRArrayCopy<T>(), AllocateRRArrayByType<T>(),
 * or AllocateEmptyRRArray<T>()
 *
 * Valid values for T are `rr_bool`, `double`, `float`, `int8_t`, `uint8_t`, `int16_t`,
 * `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `cdouble`,
 * `csingle`, or `char`. Attempts to use any other types will result in a compiler error.
 *
 * RRArray must be stored in RR_INTRUSIVE_PTR. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam T The type of the array elements
 */
template <typename T>
class RRArray : public RRBaseArray
{
  public:
    RR_OVIRTUAL DataTypes GetTypeID() RR_OVERRIDE { return RRPrimUtil<T>::GetTypeID(); }

    RRArray(T* data, size_t length, bool owned) : data_(data), owned(owned), element_count(length) {}

    RR_OVIRTUAL ~RRArray() RR_OVERRIDE
    {
        if (owned)
            delete[] data_;
    }

    RR_OVIRTUAL void* void_ptr() RR_OVERRIDE { return data(); }

    T* operator->() const { return data; }

    RR_OVIRTUAL size_t size() RR_OVERRIDE { return element_count; }

    RR_OVIRTUAL size_t ElementSize() RR_OVERRIDE { return sizeof(T); }

    // C++ container support functions based on boost::array

    // type definitions
    /** value_type */
    typedef T value_type;
    /** iterator */
    typedef T* iterator;
    /** const_iterator */
    typedef const T* const_iterator;
    /** reference */
    typedef T& reference;
    /** const_reference */
    typedef const T& const_reference;
    /** size_type */
    typedef std::size_t size_type;
    /** difference_type */
    typedef std::ptrdiff_t difference_type;

    // iterator support

    /** @brief returns an iterator to the beginning */
    iterator begin() { return data_; }
    /** @brief returns an iterator to the beginning */
    const_iterator begin() const { return data_; }
    /** @brief returns a const iterator to the beginning */
    const_iterator cbegin() const { return data_; }

    /** @brief returns an iterator to the end */
    iterator end() { return data_ + element_count; }
    /** @brief returns an iterator to the end */
    const_iterator end() const { return data_ + element_count; }
    /** @brief returns a const iterator to the end */
    const_iterator cend() const { return data_ + element_count; }

    /** reverse_iterator */
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    /** const_reverse_iterator */
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

    /** @brief returns a reverse iterator to the beginning */
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }

    /** @brief returns a reverse iterator to the end */
    reverse_iterator rend() { return reverse_iterator(begin()); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    /** @brief access specified element */
    reference operator[](size_type i)
    {
        BOOST_ASSERT_MSG(i < element_count, "out of range");
        return data_[i];
    }

    /** @brief access specified element */
    const_reference operator[](size_type i) const
    {
        BOOST_ASSERT_MSG(i < element_count, "out of range");
        return data_[i];
    }

    /** @brief access specified element with bounds checking */
    reference at(size_type i)
    {
        rangecheck(i);
        return data_[i];
    }
    /** @brief access specified element with bounds checking */
    const_reference at(size_type i) const
    {
        rangecheck(i);
        return data_[i];
    }

    /** @brief access the first element */
    reference front() { return data_[0]; }

    /** @brief access the first element */
    const_reference front() const { return data_[0]; }

    /** @brief access the last element */
    reference back() { return data_[element_count - 1]; }

    /** @brief access the last element */
    const_reference back() const { return data_[element_count - 1]; }

    /** @brief checks whether the container is empty */
    bool empty() { return false; }
    /** @brief returns the maximum possible number of elements */
    size_type max_size() { return element_count; }

    /** @brief direct access to the underlying array */
    const T* data() const { return data_; }
    /** @brief direct access to the underlying array */
    T* data() { return data_; }

    T* c_array() { return data_; }

    /**
     * @brief assignment with type conversion
     *
     * @tparam T2 The type of the input array
     * @param rhs The input array to assign (and copy) to this array
     * @return RRArray<T>& reference to this array
     */
    template <typename T2>
    RRArray<T>& operator=(const RRArray<T2>& rhs)
    {
        std::copy(rhs.begin(), rhs.end(), begin());
        return *this;
    }

    /**
     * @brief Assign one value to all elements
     *
     * Synonynm for fill()
     *
     * @param value The value to assign all elements
     */
    void assign(const T& value) { fill(value); }
    /**
     * @brief Fill array with value
     *
     * @param value The value to fill array with
     */
    void fill(const T& value) { std::fill_n(begin(), size(), value); }

    void rangecheck(size_type i)
    {
        if (i >= size())
        {
            std::out_of_range e("array<>: index out of range");
            boost::throw_exception(e);
        }
    }

  private:
    T* data_;
    size_t element_count;
    bool owned;
};

template <typename T>
class RRMap_keytype
{
  public:
    static boost::string_ref get()
    {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Invalid key type for Robot Raconteur map");
        throw DataTypeException("Unknown data type");
    }

    static std::string get_map_rrtype()
    {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Invalid key type for Robot Raconteur map");
        throw DataTypeException("Unknown data type");
    }
};

template <>
class RRMap_keytype<int32_t>
{
  public:
    static boost::string_ref get() { return "int32_t"; }

    static std::string get_map_rrtype() { return "RobotRaconteur.RRMap<int32_t>"; }
};

template <>
class RRMap_keytype<std::string>
{
  public:
    static boost::string_ref get() { return "string"; }

    static std::string get_map_rrtype() { return "RobotRaconteur.RRMap<string>"; }
};

/**
 * @brief Map container value type
 *
 * Implementation of map container types for Robot Raconteur. Must be stored
 * using RR_INTRUSIVE_PTR. Use AllocateEmptyRRMap() to allocate. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam K Map key type. Must be int32_t or std::string
 * @tparam T Stored value type. Must extend RRValue
 */
template <typename K, typename T>
class RRMap : public RRValue
{
  protected:
    std::map<K, RR_INTRUSIVE_PTR<T> > map;

  public:
    RRMap() {}

    RRMap(const std::map<K, RR_INTRUSIVE_PTR<T> >& mapin) : map(mapin) {}

    RR_OVIRTUAL ~RRMap() RR_OVERRIDE {}

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return RRMap_keytype<K>::get_map_rrtype(); }

    // C++ container support based on boost::container::map

    /** key_type */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::key_type key_type;
    /** mapped_type */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::mapped_type mapped_type;
    /** value_type */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::value_type value_type;
    /** pointer */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::pointer pointer;
    /** const_pointer */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_pointer const_pointer;
    /** reference */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::reference reference;
    /** const_reference */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_reference const_reference;
    /** size_type */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::size_type size_type;
    /** iterator */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::iterator iterator;
    /** const_iterator */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_iterator const_iterator;
    /** reverse_iterator */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::reverse_iterator reverse_iterator;
    /** const_reverse_iterator */
    typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_reverse_iterator const_reverse_iterator;

    /** @brief returns an iterator to the beginning */
    iterator begin() { return map.begin(); }
    /** @brief returns an iterator to the beginning */
    const_iterator cbegin() const { return map.cbegin(); }
    /** @brief returns an iterator to the beginning */
    const_iterator begin() const { return map.begin(); }
    /** @brief returns an iterator to the end */
    iterator end() { return map.end(); }
    /** @brief returns an iterator to the end */
    const_iterator cend() const { return map.end(); }
    /** @brief returns an iterator to the end */
    const_iterator end() const { return map.end(); }
    /** @brief returns a reverse iterator to the beginning */
    reverse_iterator rbegin() { return map.rbegin(); };
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator crbegin() const { return map.crbegin(); };
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator rbegin() const { return map.rbegin(); };
    /** @brief returns a reverse iterator to the end */
    reverse_iterator rend() { return map.rend(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator crend() const { return map.rend(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator rend() const { return map.rend(); }
    /** @brief checks whether the container is empty */
    bool empty() const { return map.empty(); }
    /** @brief returns the number of elements */
    size_type size() const { return map.size(); }
    /** @brief returns the maximum possible number of elements */
    size_type max_size() const { return map.max_size(); };
    /** @brief access or insert the specified element */
    mapped_type& operator[](const key_type& k) { return map[k]; }
    /** @brief access specified element with bounds checking */
    mapped_type& at(const key_type& k) { return map.at(k); }
    /** @brief access specified element with bounds checking */
    const mapped_type& at(const key_type& k) const { return map.at(k); }
    /** @brief inserts elements */
    std::pair<iterator, bool> insert(const value_type& x) { return map.insert(x); }
    /** @brief inserts elements */
    iterator insert(const_iterator p, const value_type& x)
    {
        RR_UNUSED(p);
        return map.insert(x);
    }
    /** @brief inserts elements */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        map.insert(first, last);
    }
    /** @brief erases element */
    void erase(iterator p) { map.erase(p); }
    /** @brief erases element */
    size_type erase(const key_type& x) { return map.erase(x); }
    /** @brief erases element */
    void erase(iterator first, iterator last) { map.erase(first, last); }
    /** @brief clears the contents */
    void clear() { map.clear(); }
    /** @brief finds element with the specified key */
    iterator find(const key_type& x) { return map.find(x); }
    /** @brief finds element with the specified key */
    const_iterator find(const key_type& x) const { return map.find(x); }
    /** @brief returns the number of elemnts matching the specified key */
    size_type count(const key_type& x) const { return map.count(x); }

    /**
     * @brief Get the underlying storage container
     *
     * WARNING: this may change, use with caution!
     *
     * @return std::map<K, RR_INTRUSIVE_PTR<T> >&
     */
    typename std::map<K, RR_INTRUSIVE_PTR<T> >& GetStorageContainer() { return map; }
};

/**
 * @brief List container value type
 *
 * Implementation of list container types for Robot Raconteur. Must be stored
 * using RR_INTRUSIVE_PTR. Use AllocateEmptyRRList() to allocate. It will be
 * released automatically when the reference count goes to zero.
 *
 * @tparam T Stored value type. Must extend RRValue
 */
template <typename T>
class RRList : public RRValue
{
  protected:
    std::list<RR_INTRUSIVE_PTR<T> > list;

  public:
    RRList() {}

    RRList(std::list<RR_INTRUSIVE_PTR<T> > listin) { list = listin; }

    RR_OVIRTUAL ~RRList() RR_OVERRIDE {}

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.RRList"; }

    // C++ container support based on boost::container::list

    /** value_type */
    typedef RR_INTRUSIVE_PTR<T> value_type;
    /** pointer */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::pointer pointer;
    /** const_pointer */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_pointer const_pointer;
    /** reference */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::reference reference;
    /** const_reference */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_reference const_reference;
    /** size_type */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::size_type size_type;
    /** difference_type */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::difference_type difference_type;
    /** iterator */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::iterator iterator;
    /** const_iterator */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_iterator const_iterator;
    /** reverse_iterator */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::reverse_iterator reverse_iterator;
    /** const_reverse_iterator */
    typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_reverse_iterator const_reverse_iterator;

    /** @brief returns an iterator to the beginning */
    iterator begin() { return list.begin(); }
    /** @brief returns an iterator to the beginning */
    const_iterator begin() const { return list.begin(); }
    /** @brief returns an iterator to the end */
    iterator end() { return list.end(); }
    /** @brief returns an iterator to the end */
    const_iterator end() const { return list.end(); }
    /** @brief returns a reverse iterator to the beginning */
    reverse_iterator rbegin() { return list.rbegin(); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator rbegin() const { return list.rbegin(); }
    /** @brief returns a reverse iterator to the end */
    reverse_iterator rend() { return list.rend(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator crend() const { return list.crend(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator rend() const { return list.rend(); }
    /** @brief returns an iterator to the beginning */
    const_iterator cbegin() const { return list.cbegin(); }
    /** @brief returns an iterator to the end */
    const_iterator cend() const { return list.cend(); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator const crbegin() { return list.crbegin(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator const crend() { return list.crend(); }
    /** @brief checks whether the container is empty */
    bool empty() const { return list.empty(); }
    /** @brief returns the number of elements */
    virtual size_type size() const { return list.size(); }
    /** @brief returns the maximum possible number of elements */
    size_type max_size() const { return list.max_size(); }
    /** @brief access the first element */
    reference front() { return list.front(); }
    /** @brief access the first element */
    const_reference front() const { return list.front(); }
    /** @brief access the last element */
    reference back() { return list.back(); }
    /** @brief access the last element */
    const_reference back() const { return list.back(); }
    /** @brief inserts an element at the beginning */
    void push_front(const RR_INTRUSIVE_PTR<T>& x) { list.push_front(x); }
    /** @brief adds an element to the end */
    void push_back(const RR_INTRUSIVE_PTR<T>& x) { list.push_back(x); }
    /** @brief removes the first element */
    void pop_front() { list.pop_front(); }
    /** @brief removes the last element */
    void pop_back() { list.pop_back(); }
    /** @brief inserts element */
    iterator insert(const_iterator p, const RR_INTRUSIVE_PTR<T>& x) { list.insert(p, x); }
    /** @brief erases element */
    iterator erase(const_iterator p) { list.erase(p); }
    /** @brief erases element */
    iterator erase(const_iterator first, const_iterator last) { list.erase(first, last); }
    /** @brief clears the contents */
    void clear() { list.clear(); }
    /** @brief removes elements equal to value */
    void remove(const RR_INTRUSIVE_PTR<T>& value) { list.remove(value); }

    /**
     * @brief Get the underlying storage container
     *
     * WARNING: this may change, use with caution!
     *
     * @return std::list<RR_INTRUSIVE_PTR<T> >&
     */
    typename std::list<RR_INTRUSIVE_PTR<T> >& GetStorageContainer() { return list; }
};

/**
 * @brief Base class for user defined `structure` value types
 *
 * Structure types are defined in service definition (robdef)
 * files. The implementation for these structures is generated
 * as part of the thunk source.
 *
 */
class ROBOTRACONTEUR_CORE_API RRStructure : public RRValue
{
  public:
    RR_OVIRTUAL ~RRStructure() RR_OVERRIDE {}
};

/**
 * @brief Allocate a numeric primitive or character array with the
 * specified type and length
 *
 * This function does not initialize the returned array. The contents
 * will not be set to zero.
 *
 * Valid values for T are `rr_bool`, `double`, `float`, `int8_t`, `uint8_t`, `int16_t`,
 * `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `cdouble`,
 * `csingle`, or `char`. Attempts to use any other types will result in a compiler error.
 *
 * @tparam T The type of the array elements
 * @param length The length of the returned array (element count)
 * @return RR_INTRUSIVE_PTR<RRArray<T> > The allocated array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > AllocateRRArray(size_t length)
{
    return RR_INTRUSIVE_PTR<RRArray<T> >(new RRArray<T>(new T[length], length, true));
}

/**
 * @brief Allocates an array object and attaches to existing numeric primitive or character
 * array pointer
 *
 * This function will attach to an existing numeric array pointer, and provide read/write access
 * to its contents. If owned is true, the array will be deleted using `delete[]` when
 * the reference count of the array object goes to zero.
 *
 * Valid values for T are `rr_bool`, `double`, `float`, `int8_t`, `uint8_t`, `int16_t`,
 * `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `cdouble`,
 * `csingle`, or `char`. Attempts to use any other types will result in a compiler error.
 *
 * @tparam T The type of the array elements
 * @param data Pointer to existing numeric primitive or character array
 * @param length Length of existing array (element count)
 * @param owned true if the data lifetime is owned by the new array object
 * @return RR_INTRUSIVE_PTR<RRArray<T> > The allocated array object
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > AttachRRArray(T* data, size_t length, bool owned)
{
    return RR_INTRUSIVE_PTR<RRArray<T> >(new RRArray<T>(data, length, owned));
}

/**
 * @brief Allocates an array object and copies existing numeric
 *
 * Valid values for T are `rr_bool`, `double`, `float`, `int8_t`, `uint8_t`, `int16_t`,
 * `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `cdouble`,
 * `csingle`, or `char`. Attempts to use any other types will result in a compiler error.
 *
 * @tparam T The type of the array elements
 * @param data Pointer to existing numeric
 * @param length Length of existing array (element count)
 * @return RR_INTRUSIVE_PTR<RRArray<T> > The allocated array object with copied data
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > AttachRRArrayCopy(const T* data, const size_t length)
{
    RR_INTRUSIVE_PTR<RRArray<T> > ret(new RRArray<T>(new T[length], length, true));
    memcpy(ret->void_ptr(), data, length * sizeof(T));
    return ret;
}

/**
 * @brief Allocate an RRBaseArray by type code
 *
 * This function does not initialize the returned array. The contents
 * will not be set to zero.
 *
 * Valid values for type are `DataTypes_bool_t`, `DataTypes_double_t`,
 * `DataTypes_single_t`, `DataTypes_int8_t`, `DataTypes_uint8_t`,
 * `DataTypes_int16_t`, `DataTypes_uint16_t`, `DataTypes_int32_t`,
 * `DataTypes_uint32_t`, `DataTypes_int64_t`, `DataTypes_uint64_t`,
 * `DataTypes_cdouble_t`, or`DataTypes_csingle_t`. Attempts to use any other
 * types will result in an InvalidArgumentException.
 *
 * @param type The type code
 * @param length The length of the returned array (element count)
 * @return RR_INRUSIVE_PTR<RRBaseArray> The allocated array
 */
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRBaseArray> AllocateRRArrayByType(DataTypes type, size_t length);

/**
 * @brief Allocate a numeric primitive or character array with the
 * specified type and length and initialize to zero
 *
 * All elements are initialized to zero
 *
 * Valid values for T are `rr_bool`, `double`, `float`, `int8_t`, `uint8_t`, `int16_t`,
 * `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `cdouble`,
 * `csingle`, or `char`. Attempts to use any other types will result in a compiler error.
 *
 * @tparam T The type of the array elements
 * @param length The length of the returned array (element count)
 * @return RR_INTRUSIVE_PTR<RRArray<T> > The allocated array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > AllocateEmptyRRArray(size_t length)
{
    RR_INTRUSIVE_PTR<RRArray<T> > o = AllocateRRArray<T>(length);
    if (length > 0)
    {
        memset(o->data(), 0, length * sizeof(T));
    }
    return o;
}

/**
 * @brief Get the number of bytes to store a numeric primitive scalar
 *
 * Valid values for type are `DataTypes_bool_t`, `DataTypes_double_t`,
 * `DataTypes_single_t`, `DataTypes_int8_t`, `DataTypes_uint8_t`,
 * `DataTypes_int16_t`, `DataTypes_uint16_t`, `DataTypes_int32_t`,
 * `DataTypes_uint32_t`, `DataTypes_int64_t`, `DataTypes_uint64_t`,
 * `DataTypes_cdouble_t`, or`DataTypes_csingle_t`. Attempts to use any other
 * types will result in a InvalidArgumentException.
 *
 * @param type The numeric type
 * @return size_t The number of bytes
 */
ROBOTRACONTEUR_CORE_API size_t RRArrayElementSize(DataTypes type);

/**
 * @brief Convert a scalar number into an array with one element
 *
 * @tparam T The type of the number
 * @param value The value of the number
 * @return RR_INTRUSIVE_PTR<RRArray<T> > The new array with one element
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > ScalarToRRArray(T value)
{
    return AttachRRArrayCopy(&value, 1);
}

/**
 * @brief Convert an array with one element into a scalar
 *
 * @tparam T The type of the number
 * @param value The array with one element
 * @return T The scalar value of the first element in the array
 */
template <typename T>
static T RRArrayToScalar(const RR_INTRUSIVE_PTR<RRArray<T> >& value)
{
    if (!value)
    {
        throw NullValueException("Null pointer");
    }

    if (value->size() == 0)
        throw OutOfRangeException("Index out of range");

    return (*value)[0];
}

/**
 * @brief Convert an RRArray\<U> to a std::vector<Y>
 *
 * This function creates a copy of the data in an alloctade RRArray<T>
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output std::vector elements
 * @tparam U The type of the input RRArray elements
 * @param in The input RRArray to be converted
 * @return std::vector<Y> The output std::vector filled with the values converted
 * from in
 */
template <typename Y, typename U>
static std::vector<Y> RRArrayToVector(const RR_INTRUSIVE_PTR<RRArray<U> >& in)
{
    if (!in)
        throw NullValueException("Unexpected null array");
    std::vector<Y> out(in->size());
    for (size_t i = 0; i < in->size(); i++)
        out[i] = boost::numeric_cast<Y>((*in)[i]);
    return out;
}

/**
 * @brief Convert a std::vector<U> to an RRArray<Y>
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output RRArray elements
 * @tparam U The type of the input std::vector elements
 * @param in The input std::vector to be converted
 * @return RR_INTRUSIVE_PTR<RRArray<Y> > The output RRArray filled with the
 * values converted from in
 */
template <typename Y, typename U>
static RR_INTRUSIVE_PTR<RRArray<Y> > VectorToRRArray(const std::vector<U>& in)
{
    RR_INTRUSIVE_PTR<RRArray<Y> > out = AllocateRRArray<Y>(in.size());
    for (size_t i = 0; i < in.size(); i++)
        (*out)[i] = boost::numeric_cast<Y>(in[i]);
    return out;
}

/**
 * @brief Convert an RRArray to a boost::array
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output boost::array elements
 * @tparam N The number of elements of the boost::array
 * @tparam U The type of the output boost::array elements to be converted
 * @param in The input RRArray to be converted
 * @return boost::array<Y,N> The output boost::array filled with values
 * converted from in
 */
template <typename Y, std::size_t N, typename U>
static boost::array<Y, N> RRArrayToArray(const RR_INTRUSIVE_PTR<RRArray<U> >& in)
{
    if (!in)
        throw NullValueException("Unexpected null array");
    if (in->size() != N)
        throw OutOfRangeException("Array is incorrect size");
    boost::array<Y, N> out = {};
    for (size_t i = 0; i < N; i++)
        out[i] = boost::numeric_cast<Y>((*in)[i]);
    return out;
}

/**
 * @brief Convert an RRArray to a boost::array
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output boost::array elements
 * @tparam N The number of elements of the boost::array
 * @tparam U The type of the output boost::array elements to be converted
 * @param in The input RRArray to be converted
 * @param out The target boost::array for converted elements
 */
template <typename Y, std::size_t N, typename U>
static void RRArrayToArray(boost::array<Y, N>& out, const RR_INTRUSIVE_PTR<RRArray<U> >& in)
{
    if (!in)
        throw NullValueException("Unexpected null array");
    if (in->size() != N)
        throw OutOfRangeException("Array is incorrect size");
    for (size_t i = 0; i < N; i++)
        out[i] = boost::numeric_cast<Y>((*in)[i]);
}

/**
 * @brief Convert a boost::array to an RRArray
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output RRArray elements
 * @tparam U The type of the input boost::array elements
 * @tparam N The number of elements in the input boost::array
 * @param in The input boost::array to be converted
 * @return RR_INTRUSIVE_PTR<RRArray<Y> > The output RRArray filled
 * with values converted from in
 */
template <typename Y, typename U, std::size_t N>
static RR_INTRUSIVE_PTR<RRArray<Y> > ArrayToRRArray(boost::array<U, N> in)
{
    RR_INTRUSIVE_PTR<RRArray<Y> > out = AllocateRRArray<Y>(N);
    for (size_t i = 0; i < N; i++)
        (*out)[i] = boost::numeric_cast<Y>(in[i]);
    return out;
}

/**
 * @brief Convert an RRArray to a boost::static_vector
 *
 * boost::container::static vector is a vector with fixed maximum capacity N
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output boost::static_vector elements
 * @tparam N The maximum element capacity of the output vector
 * @tparam U The type of the input RRArray elements
 * @param in The input RRArray to be converted
 * @return boost::container::static_vector<Y, N> The output
 * boost::container::static_vector with values converted from in
 */
template <typename Y, std::size_t N, typename U>
static boost::container::static_vector<Y, N> RRArrayToStaticVector(const RR_INTRUSIVE_PTR<RRArray<U> >& in)
{
    if (!in)
        throw NullValueException("Unexpected null array");
    if (in->size() > N)
        throw OutOfRangeException("Array is too large for static vector size");
    boost::container::static_vector<Y, N> out(in->size());
    for (size_t i = 0; i < in->size(); i++)
        out[i] = boost::numeric_cast<Y>((*in)[i]);
    return out;
}

/**
 * @brief Convert an RRArray to a boost::static_vector
 *
 * boost::container::static vector is a vector with fixed maximum capacity N
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the output boost::static_vector elements
 * @tparam N The maximum element capacity of the output vector
 * @tparam U The type of the input RRArray elements
 * @param out The target boost::container::static_vector for converted
 * elements
 * @param in The input RRArray to be converted
 */
template <typename Y, std::size_t N, typename U>
static void RRArrayToStaticVector(boost::container::static_vector<Y, N>& out, const RR_INTRUSIVE_PTR<RRArray<U> >& in)
{
    if (!in)
        throw NullValueException("Unexpected null array");
    if (in->size() > N)
        throw OutOfRangeException("Array is too large for static vector size");
    out.resize(in->size());
    for (size_t i = 0; i < in->size(); i++)
        out[i] = boost::numeric_cast<Y>((*in)[i]);
}

/**
 * @brief Convert a boost::container::static_vector to an RRArray
 *
 * boost::container::static vector is a vector with fixed maximum capacity N
 *
 * boost::numeric_cast<Y>() is used to convert between numeric types
 *
 * @tparam Y The type of the input RRArray elements
 * @tparam U The type of the output boost::static_vector elements
 * @tparam N The maximum element capacity of the input vector
 * @param in The input boost::container::static_vector to be converted
 * @return RR_INTRUSIVE_PTR<RRArray<Y> > The output
 * RRArray with values converted from in
 */
template <typename Y, typename U, std::size_t N>
static RR_INTRUSIVE_PTR<RRArray<Y> > StaticVectorToRRArray(boost::container::static_vector<U, N> in)
{
    RR_INTRUSIVE_PTR<RRArray<Y> > out = AllocateRRArray<Y>(in.size());
    for (size_t i = 0; i < in.size(); i++)
        (*out)[i] = boost::numeric_cast<Y>(in[i]);
    return out;
}

/**
 * @brief Convert a string vector to a RRList
 *
 * This function creates copies of the strings
 *
 * @param string_vector The input string vector to be converted
 * @return RR_INTRUSIVE_PTR<RRList<RRArray<char> > > The converted RRList
 */
RR_INTRUSIVE_PTR<RRList<RRArray<char> > > stringVectorToRRList(const std::vector<std::string>& string_vector);

/**
 * @brief Convert a RRList containing strings to a string vector
 *
 * This function creates copies of the strings
 *
 * @param list The input RRList to be converted
 * @return std::vector<std::string> The converted string vector
 */
std::vector<std::string> RRListToStringVector(const RR_INTRUSIVE_PTR<RRList<RRArray<char> > >& list);

/**
 * @brief Checks if a value RR_INTRUSIVE_PTR is null
 *
 * If the value is null, throws NullValueException
 *
 * Returns reference to ptr
 *
 * @tparam T The type of the value contained in RR_INTRUSIVE_PTR
 * @param ptr The value to check
 * @return RR_INTRUSIVE_PTR<T>& Reference to ptr
 */
template <typename T>
RR_INTRUSIVE_PTR<T>& rr_null_check(RR_INTRUSIVE_PTR<T>& ptr)
{
    if (!ptr)
    {
        throw NullValueException("Unexpected null value");
    }
    return ptr;
}

/**
 * @brief Checks if a value RR_INTRUSIVE_PTR is null with exception message
 *
 * If the value is null, throws NullValueException with the message specified
 * in msg
 *
 * Returns reference to ptr
 *
 * @tparam T The type of the value contained in RR_INTRUSIVE_PTR
 * @param ptr The value to check
 * @param msg Error message for NullValueException if ptr is null
 * @return RR_INTRUSIVE_PTR<T>& Reference to ptr
 */
template <typename T>
RR_INTRUSIVE_PTR<T>& rr_null_check(RR_INTRUSIVE_PTR<T>& ptr, const char* msg)
{
    if (!ptr)
    {
        throw NullValueException(msg);
    }
    return ptr;
}

/**
 * @brief Checks if a value const RR_INTRUSIVE_PTR is null
 *
 * If the value is null, throws NullValueException
 *
 * Returns reference to ptr
 *
 * @tparam T The type of the value contained in RR_INTRUSIVE_PTR
 * @param ptr The value to check
 * @return RR_INTRUSIVE_PTR<T>& Reference to ptr
 */
template <typename T>
const RR_INTRUSIVE_PTR<T>& rr_null_check(const RR_INTRUSIVE_PTR<T>& ptr)
{
    if (!ptr)
    {
        throw NullValueException("Unexpected null value");
    }
    return ptr;
}

/**
 * @brief Checks if a value const RR_INTRUSIVE_PTR is null with exception message
 *
 * If the value is null, throws NullValueException with the message specified
 * in msg
 *
 * Returns reference to ptr
 *
 * @tparam T The type of the value contained in RR_INTRUSIVE_PTR
 * @param ptr The value to check
 * @param msg Error message for NullValueException if ptr is null
 * @return RR_INTRUSIVE_PTR<T>& Reference to ptr
 */
template <typename T>
const RR_INTRUSIVE_PTR<T>& rr_null_check(const RR_INTRUSIVE_PTR<T>& ptr, const char* msg)
{
    if (!ptr)
    {
        throw NullValueException(msg);
    }
    return ptr;
}

#define RR_NULL_CHECK rr_null_check

template <typename T>
static RR_INTRUSIVE_PTR<RRArray<T> > VerifyRRArrayLength(const RR_INTRUSIVE_PTR<RRArray<T> >& a, size_t len,
                                                         bool varlength)
{
    if (!a)
        throw NullValueException("Arrays must not be null");
    if (len != 0)
    {
        if (varlength && (a->size() > len))
        {
            throw DataTypeException("Array dimension mismatch");
        }
        if (!varlength && (a->size() != len))
        {
            throw DataTypeException("Array dimension mismatch");
        }
    }
    return a;
}

template <typename T>
static RR_INTRUSIVE_PTR<RRList<T> > VerifyRRArrayLength(const RR_INTRUSIVE_PTR<RRList<T> >& a, size_t len,
                                                        bool varlength)
{
    if (!a)
        return a;
    BOOST_FOREACH (RR_INTRUSIVE_PTR<T>& aa, (*a))
    {
        VerifyRRArrayLength(aa, len, varlength);
    }
    return a;
}

template <typename K, typename T>
static RR_INTRUSIVE_PTR<RRMap<K, T> > VerifyRRArrayLength(const RR_INTRUSIVE_PTR<RRMap<K, T> >& a, size_t len,
                                                          bool varlength)
{
    if (!a)
        return a;
    BOOST_FOREACH (const RR_INTRUSIVE_PTR<T>& aa, *a | boost::adaptors::map_values)
    {
        VerifyRRArrayLength(aa, len, varlength);
    }
    return a;
}

/**
 * @brief Base class for numeric multidimensional arrays
 *
 * Multidimensional arrays should be allocated using
 * AllocateRRMultiDimArray() or AllocateEmptyRRMultiDimArray()
 *
 */
class ROBOTRACONTEUR_CORE_API RRMultiDimBaseArray : public RRValue
{
  public:
    RR_OVIRTUAL ~RRMultiDimBaseArray() RR_OVERRIDE {}

    virtual DataTypes GetElementTypeID() = 0;
};

namespace detail
{
ROBOTRACONTEUR_CORE_API class MultiDimArray_CalculateCopyIndicesIter
{
  public:
    virtual bool Next(uint32_t& indexa, uint32_t& indexb, uint32_t& len) = 0;

    virtual ~MultiDimArray_CalculateCopyIndicesIter();
};

ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<MultiDimArray_CalculateCopyIndicesIter>
MultiDimArray_CalculateCopyIndicesBeginIter(const std::vector<uint32_t>& mema_dims,
                                            const std::vector<uint32_t>& mema_pos,
                                            const std::vector<uint32_t>& memb_dims,
                                            const std::vector<uint32_t>& memb_pos, const std::vector<uint32_t>& count);
} // namespace detail

/**
 * @brief Numeric primitive multidimensional array value type
 *
 * This class stores a numeric primitive multidimensional arrays.
 * Multidimensional arrays are stored as a uint32_t array of
 * dimensions, and an array of the flattened elements.
 * Arrays are stored in column major, or "Fortran" order.
 *
 * Multidimensional arrays should be allocated using
 * AllocateRRMultiDimArray() or AllocateEmptyRRMultiDimArray()
 *
 * Valid values for T are `rr_bool`, `double`, `float`, `int8_t`, `uint8_t`, `int16_t`,
 * `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `cdouble`,
 * or `csingle`. Attempts to use any other types will result in a compiler error.
 *
 * RRMultiDimArray must be stored in RR_INTRUSIVE_PTR. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam T The type of the array elements
 */
template <typename T>
class RRMultiDimArray : public RRMultiDimBaseArray
{
  public:
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims;

    RR_INTRUSIVE_PTR<RRArray<T> > Array;

    RRMultiDimArray(){};

    RRMultiDimArray(const RR_INTRUSIVE_PTR<RRArray<uint32_t> >& Dims, const RR_INTRUSIVE_PTR<RRArray<T> >& Array)
    {
        this->Dims = Dims;
        this->Array = Array;
    }

    RR_OVIRTUAL ~RRMultiDimArray() RR_OVERRIDE {}

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.RRMultiDimArray"; }

    RR_OVIRTUAL DataTypes GetElementTypeID() RR_OVERRIDE { return RRPrimUtil<T>::GetTypeID(); }

    /**
     * @brief Retrieve a subset of an array
     *
     * @param memorypos Position in array to read
     * @param buffer Buffer to store retrieved data
     * @param bufferpos Position within buffer to store data
     * @param count Count of data to retrieve
     */
    virtual void RetrieveSubArray(const std::vector<uint32_t>& memorypos,
                                  const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                                  const std::vector<uint32_t>& bufferpos, const std::vector<uint32_t>& count)
    {

        std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
        std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
        RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
            detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

        uint32_t len = 0;
        uint32_t indexa = 0;
        uint32_t indexb = 0;

        while (iter->Next(indexa, indexb, len))
        {
            memcpy((buffer->Array->data() + indexb), (Array->data()) + indexa, len * sizeof(T));
        }
    }

    /**
     * @brief Assign a subset of an array
     *
     * @param memorypos Position within array to store data
     * @param buffer Buffer to assign data from
     * @param bufferpos Position within buffer to assign from
     * @param count Count of data to assign
     */
    virtual void AssignSubArray(const std::vector<uint32_t>& memorypos,
                                const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                                const std::vector<uint32_t>& bufferpos, const std::vector<uint32_t>& count)
    {

        std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
        std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
        RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
            detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

        uint32_t len = 0;
        uint32_t indexa = 0;
        uint32_t indexb = 0;

        while (iter->Next(indexa, indexb, len))
        {
            memcpy((Array->data() + indexa), (buffer->Array->data() + indexb), len * sizeof(T));
        }
    }
};

template <size_t Ndims, typename T>
static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > VerifyRRMultiDimArrayLength(const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& a,
                                                                         size_t n_elems,
                                                                         const boost::array<uint32_t, Ndims>& dims)
{
    if (!a)
        throw NullValueException("Arrays must not be null");

    if (a->Dims->size() != Ndims)
    {
        throw DataTypeException("Array dimension mismatch");
    }

    if (a->Array->size() != n_elems)
    {
        throw DataTypeException("Array dimension mismatch");
    }

    for (size_t i = 0; i < Ndims; i++)
    {
        if ((*a->Dims)[i] != dims[i])
        {
            throw DataTypeException("Array dimension mismatch");
        }
    }

    return a;
}

template <size_t Ndims, typename T>
static RR_INTRUSIVE_PTR<RRList<T> > VerifyRRMultiDimArrayLength(const RR_INTRUSIVE_PTR<RRList<T> >& a, size_t n_elems,
                                                                const boost::array<uint32_t, Ndims>& dims)
{
    if (!a)
        throw NullValueException("Arrays must not be null");
    else
    {
        BOOST_FOREACH (const RR_INTRUSIVE_PTR<T>& aa, (*a))
        {
            VerifyRRMultiDimArrayLength<Ndims>(aa, n_elems, dims);
        }
    }
    return a;
}

template <size_t Ndims, typename K, typename T>
static RR_INTRUSIVE_PTR<RRMap<K, T> > VerifyRRMultiDimArrayLength(const RR_INTRUSIVE_PTR<RRMap<K, T> >& a,
                                                                  size_t n_elems,
                                                                  const boost::array<uint32_t, Ndims>& dims)
{
    if (!a)
        throw NullValueException("Arrays must not be null");
    else
    {
        BOOST_FOREACH (const RR_INTRUSIVE_PTR<T>& aa, *a | boost::adaptors::map_values)
        {
            VerifyRRMultiDimArrayLength<Ndims>(aa, n_elems, dims);
        }
        return a;
    }
}

/**
 * @brief Allocate an empty multidimensional array with the
 * specified dimensions
 *
 * The elements of the returned array are initialized to zero
 *
 * @tparam T The element type of the array
 * @param dims The dimensions of the new multidimensional array
 * @return RR_INTRUSIVE_PTR<RRMultiDimArray<T> > The allocated multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > AllocateEmptyRRMultiDimArray(const std::vector<uint32_t>& dims)
{
    uint32_t n_elems = boost::accumulate(dims, 1, std::multiplies<uint32_t>());
    return new RRMultiDimArray<T>(VectorToRRArray<uint32_t>(dims), AllocateEmptyRRArray<T>(n_elems));
}

/**
 * @brief Allocate an empty multidimensional array
 *
 * @tparam T The element type of the array
 * @return RR_INTRUSIVE_PTR<RRMultiDimArray<T> > The allocated empty multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > AllocateEmptyRRMultiDimArray()
{
    std::vector<uint32_t> length;
    length.push_back(0);
    return new RRMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRArray<T>(0));
}

/**
 * @brief Allocate a multidimensional using existing dimensions and data array
 *
 * This function does not copy the provided arrays
 *
 * @tparam T The element type of the array
 * @param dims The existing
 * @param array_ The existing flattened array
 * @return RR_INTRUSIVE_PTR<RRMultiDimArray<T> > The allocated multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > AllocateRRMultiDimArray(const RR_INTRUSIVE_PTR<RRArray<uint32_t> >& dims,
                                                                     const RR_INTRUSIVE_PTR<RRArray<T> >& array_)
{
    return new RRMultiDimArray<T>(dims, array_); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Allocate an empty RRList
 *
 * @tparam T The type of value stored in the list
 * @return RR_INTRUSIVE_PTR<RRList<T> > The allocated empty list
 */
template <typename T>
RR_INTRUSIVE_PTR<RRList<T> > AllocateEmptyRRList()
{
    return new RRList<T>(); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Allocate RRList with a value
 *
 * @tparam T The type of value stored in the list
 * @tparam U The type of the argument to pass to RRList constructor
 * @param c Existing list to copy
 * @return RR_INTRUSIVE_PTR<RRList<T> > The allocated list
 */
template <typename T, typename U>
RR_INTRUSIVE_PTR<RRList<T> > AllocateRRList(const U& c)
{
    return new RRList<T>(c); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Allocate an empty RRMap
 *
 * @tparam K The key type for map. Must be `int32_t` or `std::string`
 * @tparam T The type of value stored in map
 * @return RR_INTRUSIVE_PTR<RRMap<K,T> > The allocated map
 */
template <typename K, typename T>
RR_INTRUSIVE_PTR<RRMap<K, T> > AllocateEmptyRRMap()
{
    return new RRMap<K, T>(); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Allocate RRMap with value
 *
 * @tparam K The key type for map. Must be `int32_t` or `std::string`
 * @tparam T The type of value stored in map
 * @tparam U The type of the argument to pass to RRMap constructor
 * @param c Existing map to copy
 * @return RR_INTRUSIVE_PTR<RRMap<K, T> > The allocated map
 */
template <typename K, typename T, typename U>
RR_INTRUSIVE_PTR<RRMap<K, T> > AllocateRRMap(const U& c)
{
    return new RRMap<K, T>(c); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Base class for user defined `pod` value types
 *
 * Pod types are defined in service definition (robdef)
 * files. The implementation for these structures is generated
 * as part of the thunk source.
 */
class ROBOTRACONTEUR_CORE_API RRPod
{
  public:
};

/**
 * @brief Base class for `pod` array value types
 *
 * Pod arrays should be allocated using AllocateEmptyRRPodArray()
 *
 */
class RRPodBaseArray : public RRValue
{
  public:
    virtual boost::string_ref RRElementTypeString() = 0;
};

/**
 * @brief `pod` array value type
 *
 * This class stores a pod array. Pod arrays should
 * always be allocated using AllocateEmptyRRPodArray()
 *
 * T must be a pod type that has been generated as
 * part of the thunk source.
 *
 * RRPodArray must be stored in RR_INTRUSIVE_PTR. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam T The type of the array elements
 */
template <typename T>
class RRPodArray : public RRPodBaseArray
{
  protected:
    typename std::vector<T> pod_array;

  public:
    RRPodArray(size_t n) { pod_array.resize(n); }

    RRPodArray(typename std::vector<T>& array_in) : pod_array(array_in) {}

    RRPodArray(const T& value_in) { pod_array.push_back(value_in); }

    RR_OVIRTUAL ~RRPodArray() RR_OVERRIDE {}

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.RRPodArray"; }

    RR_OVIRTUAL boost::string_ref RRElementTypeString() RR_OVERRIDE { return RRPrimUtil<T>::GetRRElementTypeString(); }

    // C++ container support based on boost::container::vector

    /** value_type */
    typedef T value_type;
    /** pointer */
    typedef typename std::vector<T>::pointer pointer;
    /** const_pointer */
    typedef typename std::vector<T>::const_pointer const_pointer;
    /** reference */
    typedef typename std::vector<T>::reference reference;
    /** const_reference */
    typedef typename std::vector<T>::const_reference const_reference;
    /** size_type */
    typedef typename std::vector<T>::size_type size_type;
    /** iterator */
    typedef typename std::vector<T>::iterator iterator;
    /** const_iterator */
    typedef typename std::vector<T>::const_iterator const_iterator;
    /** reverse_iterator */
    typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    /** const_reverse_iterator */
    typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;

    /** @brief returns an iterator to the beginning */
    iterator begin() { return pod_array.begin(); }
    /** @brief retuns an iterator to the beginning */
    const_iterator begin() const { return pod_array.begin(); }
    /** @brief returns an iterator to the end */
    iterator end() { return pod_array.end(); }
    /** @brief returns an iterator to the end */
    const_iterator end() const { return pod_array.end(); }
    /** @brief returns a reverse iterator to the beginning */
    reverse_iterator rbegin() { return pod_array.rbegin(); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator rbegin() const { return pod_array.rbegin(); }
    /** @brief returns a reverse iterator to the end */
    reverse_iterator rend() { return pod_array.rend(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator rend() const { return pod_array.rend(); }
    /** @brief returns an iterator to the beginning */
    const_iterator cbegin() const { return pod_array.cbegin(); }
    /** @brief returns an iterator to the end */
    const_iterator cend() const { return pod_array.cend(); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator const crbegin() { return pod_array.crbegin(); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator const crend() { return pod_array.crend(); }
    /** @brief checks whether the container is empty */
    bool empty() const { return pod_array.empty(); }
    /** @brief returns the number of elements */
    virtual size_type size() const { return pod_array.size(); }
    /** @brief returns the maximum possible number of elements */
    size_type max_size() const { return size(); }
    /** @brief access the first element */
    reference front() { return pod_array.front(); }
    /** @brief access the first element */
    const_reference front() const { return pod_array.front(); }
    /** @brief access the last element */
    reference back() { return pod_array.back(); }
    /** @brief access the last element */
    const_reference back() const { return pod_array.back(); }
    /** @brief access the specified element */
    reference operator[](size_type i) { return pod_array[i]; }
    /** @brief access the specified element */
    const_reference operator[](size_type i) const { return pod_array[i]; }
    /** @brief access the specified element with bounds checking */
    reference at(size_type i) { return pod_array.at(i); }
    /** @brief access the specified element with bounds checking */
    const_reference at(size_type i) const { return pod_array.at(i); }
    /** @brief direct access to the underlying array */
    T* data() { return pod_array.data(); }
    /** @brief direct access to the underlying array */
    const T* data() const { return pod_array.data(); }

    /**
     * @brief Get the underlying storage container
     *
     * WARNING: this may change, use with caution!
     *
     * @return std::vector<T>&
     */
    typename std::vector<T>& GetStorageContainer() { return pod_array; }
};

#define RRPrimUtilPod(x, type_string)                                                                                  \
    template <>                                                                                                        \
    class RRPrimUtil<x>                                                                                                \
    {                                                                                                                  \
      public:                                                                                                          \
        static DataTypes GetTypeID() { return DataTypes_pod_t; }                                                       \
        static MessageStringPtr GetElementTypeString() { return MessageStringPtr(type_string); }                       \
        static boost::string_ref GetRRElementTypeString() { return type_string; }                                      \
        static RR_INTRUSIVE_PTR<RRPodArray<x> > PrePack(const x& val) { return ScalarToRRPodArray(val); }              \
        template <typename U>                                                                                          \
        static x PreUnpack(const U& val)                                                                               \
        {                                                                                                              \
            return RRPodArrayToScalar(rr_cast<RRPodArray<x> >(val));                                                   \
        }                                                                                                              \
        typedef RR_INTRUSIVE_PTR<RRPodArray<x> > BoxedType;                                                            \
    };

/**
 * @brief Base class for pod multidimensional arrays
 *
 * Pod multidimensional arrays should be allocated using
 * AllocateEmptyRRPodMultiDimArray()
 *
 */
class RRPodBaseMultiDimArray : public RRValue
{
  public:
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims;
    virtual boost::string_ref RRElementTypeString() = 0;
};

/**
 * @brief `pod` multidimensional array value type
 *
 * This class stores a pod multidimensional array.
 * Multidimensional arrays are stored as a uint32_t array of
 * dimensions, and an array of the flattened elements.
 * Arrays are stored in column major, or "Fortran" order.
 *
 * Multidimensional arrays should be allocated using
 * AllocateEmptyRRPodMultiDimArray()
 *
 * T must be a pod type that has been generated as
 * part of the thunk source.
 *
 * RRPodMultiDimArray must be stored in RR_INTRUSIVE_PTR. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam T The type of the array elements
 */
template <typename T>
class RRPodMultiDimArray : public RRPodBaseMultiDimArray
{
  public:
    typename RR_INTRUSIVE_PTR<RRPodArray<T> > PodArray;

    RRPodMultiDimArray() {}

    RRPodMultiDimArray(const RR_INTRUSIVE_PTR<RRArray<uint32_t> >& dims, const RR_INTRUSIVE_PTR<RRPodArray<T> >& a)
    {
        this->Dims = dims;
        this->PodArray = a;
    }

    RR_OVIRTUAL ~RRPodMultiDimArray() RR_OVERRIDE {}

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.RRPodMultiDimArray"; }

    RR_OVIRTUAL boost::string_ref RRElementTypeString() RR_OVERRIDE { return RRPrimUtil<T>::GetRRElementTypeString(); }

    /**
     * @brief Retrieve a subset of an array
     *
     * @param memorypos Position in array to read
     * @param buffer Buffer to store retrieved data
     * @param bufferpos Position within buffer to store data
     * @param count Count of data to retrieve
     */
    virtual void RetrieveSubArray(const std::vector<uint32_t>& memorypos,
                                  const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer,
                                  const std::vector<uint32_t>& bufferpos, const std::vector<uint32_t>& count)
    {

        std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
        std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
        RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
            detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

        uint32_t len = 0;
        uint32_t indexa = 0;
        uint32_t indexb = 0;

        while (iter->Next(indexa, indexb, len))
        {
            for (size_t i = 0; i < len; i++)
            {
                buffer->PodArray->at(indexb + i) = PodArray->at(indexa + i);
            }
        }
    }

    /**
     * @brief Assign a subset of an array
     *
     * @param memorypos Position within array to store data
     * @param buffer Buffer to assign data from
     * @param bufferpos Position within buffer to assign from
     * @param count Count of data to assign
     */
    virtual void AssignSubArray(const std::vector<uint32_t>& memorypos,
                                const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer,
                                const std::vector<uint32_t>& bufferpos, const std::vector<uint32_t>& count)
    {

        std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
        std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
        RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
            detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

        uint32_t len = 0;
        uint32_t indexa = 0;
        uint32_t indexb = 0;

        while (iter->Next(indexa, indexb, len))
        {
            for (size_t i = 0; i < len; i++)
            {
                PodArray->at(indexa + i) = buffer->PodArray->at(indexb + i);
            }
        }
    }
};

/**
 * @brief Convert a scalar pod into a pod array with one element
 *
 * @tparam T The type of the pod
 * @param value The pod
 * @return RR_INTRUSIVE_PTR<RRPodArray<T> > The new pod array with one element
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRPodArray<T> > ScalarToRRPodArray(const T& value)
{
    return new RRPodArray<T>(value); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Convert a pod array with one element into a scalar pod
 *
 * @tparam T The type of the pod
 * @param value The pod array with one element
 * @return T The first pod in the array
 */
template <typename T>
static T RRPodArrayToScalar(const RR_INTRUSIVE_PTR<RRPodArray<T> >& value)
{
    if (!value)
    {
        throw NullValueException("Null pointer");
    }

    if (value->size() == 0)
        throw OutOfRangeException("Index out of range");

    return value->at(0);
}

/**
 * @brief Allocate a pod array with the
 * specified type and length and initialize to zero
 *
 * All elements are initialized to zero
 *
 * T must be a pod type that has been generated as
 * part of the thunk source.
 *
 * @tparam T The type of the array elements
 * @param length The length of the returned array (element count)
 * @return RR_INTRUSIVE_PTR<RRPodArray<T> > The allocated array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRPodArray<T> > AllocateEmptyRRPodArray(size_t length)
{
    return new RRPodArray<T>(length); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Allocate an empty multidimensional pod array with the
 * specified dimensions
 *
 * The elements of the returned array are initialized to zero
 *
 * T must be a pod type that has been generated as
 * part of the thunk source.
 *
 * @tparam T The element type of the array
 * @param dims The dimensions of the new multidimensional array
 * @return RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > The allocated multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > AllocateEmptyRRPodMultiDimArray(const std::vector<uint32_t>& dims)
{
    uint32_t n_elems = boost::accumulate(dims, 1, std::multiplies<uint32_t>());
    return new RRPodMultiDimArray<T>(VectorToRRArray<uint32_t>(dims), AllocateEmptyRRPodArray<T>(n_elems));
}

/**
 * @brief Allocate an empty multidimensional pod array with
 * zero elements
 *
 * T must be a pod type that has been generated as
 * part of the thunk source.
 *
 * @tparam T The element type of the array
 * @return RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > The allocated multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > AllocateEmptyRRPodMultiDimArray()
{
    std::vector<uint32_t> length;
    length.push_back(0);
    return new RRPodMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRPodArray<T>(0));
}

#define RRPrimUtilNamedArray(x, type_string, array_type)                                                               \
    template <>                                                                                                        \
    class RRPrimUtil<x>                                                                                                \
    {                                                                                                                  \
      public:                                                                                                          \
        static DataTypes GetTypeID() { return DataTypes_pod_t; }                                                       \
        static MessageStringPtr GetElementTypeString() { return MessageStringPtr(type_string); }                       \
        static boost::string_ref GetRRElementTypeString() { return type_string; }                                      \
        static RR_INTRUSIVE_PTR<RRNamedArray<x> > PrePack(const x& val) { return ScalarToRRNamedArray(val); }          \
        template <typename U>                                                                                          \
        static x PreUnpack(const U& val)                                                                               \
        {                                                                                                              \
            return RRNamedArrayToScalar(rr_cast<RRNamedArray<x> >(val));                                               \
        }                                                                                                              \
        typedef RR_INTRUSIVE_PTR<RRNamedArray<x> > BoxedType;                                                          \
        typedef array_type ElementArrayType;                                                                           \
        static const size_t ElementArrayCount = sizeof(x) / sizeof(array_type);                                        \
        static size_t GetElementArrayCount() { return ElementArrayCount; }                                             \
        static DataTypes GetElementArrayTypeID() { return RRPrimUtil<array_type>::GetTypeID(); }                       \
    };

/**
 * @brief Base class for `namedarray` array value types
 *
 * NamedArray arrays should be allocated using AllocateEmptyRRNamedArray()
 *
 */
class ROBOTRACONTEUR_CORE_API RRNamedBaseArray : public RRValue
{
  public:
    virtual DataTypes ElementArrayType() = 0;

    virtual size_t ElementSize() = 0;

    virtual size_t ElementArrayCount() = 0;

    virtual RR_INTRUSIVE_PTR<RRBaseArray> GetNumericBaseArray() = 0;

    virtual boost::string_ref RRElementTypeString() = 0;
};

/**
 * @brief `namedarray` array value type
 *
 * This class stores a named array. Named arrays should
 * always be  allocated using AllocateEmptyRRNamedArray()
 *
 * T must be a `namedarray` type that has been generated as
 * part of the thunk source.
 *
 * RRNamedArray must be stored in RR_INTRUSIVE_PTR. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam T The type of the array elements
 */
template <typename T>
class RRNamedArray : public RRNamedBaseArray
{
  protected:
    RR_INTRUSIVE_PTR<RRArray<typename RRPrimUtil<T>::ElementArrayType> > rr_array;

  public:
    /**
     * @brief Construct a RRNamedArray object
     *
     * Must be stored in a boost::intrusive_ptr
     *
     * The RRNamedArray creates a memory view of the provided numeric
     * array parameter rr_array. The reference count is increased, so the
     * array will not be destroyed if the caller releases its smart pointers.
     *
     *
     * @param rr_array The array to use for storage
     */
    RRNamedArray(const RR_INTRUSIVE_PTR<RRArray<typename RRPrimUtil<T>::ElementArrayType> >& rr_array)
    {
        if (!rr_array)
            throw NullValueException("Numeric array for namedarray must not be null");
        this->rr_array = rr_array;
    }

    virtual DataTypes GetTypeID() { return RRPrimUtil<T>::GetTypeID(); }
    /** @brief returns the number of elements (`namedarray` elements)*/
    virtual size_t size() const { return rr_array->size() / (RRPrimUtil<T>::ElementArrayCount); }
    /** @brief returns the number of elements (`namedarray` elements)*/
    virtual size_t size() { return rr_array->size() / (RRPrimUtil<T>::ElementArrayCount); }
    /** @brief get a void pointer to the underlying numeric array */
    virtual void* void_ptr() { return rr_array->void_ptr(); }
    /** @brief get the number of bytes per `namedarray` element of the array */
    RR_OVIRTUAL size_t ElementSize() RR_OVERRIDE { return sizeof(T); }
    /** @brief get the type of the underlying numeric array */
    RR_OVIRTUAL DataTypes ElementArrayType() RR_OVERRIDE { return RRPrimUtil<T>::GetElementArrayTypeID(); }
    /** @brief get the total number of elements in the underlying numeric array */
    RR_OVIRTUAL size_t ElementArrayCount() RR_OVERRIDE { return RRPrimUtil<T>::ElementArrayCount; }
    /** @brief get the underlying numeric RRArray */
    virtual RR_INTRUSIVE_PTR<RRArray<typename RRPrimUtil<T>::ElementArrayType> > GetNumericArray() { return rr_array; }
    /** @brief get the underlying numeric array as an RRBaseArray */
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RRBaseArray> GetNumericBaseArray() RR_OVERRIDE { return rr_array; }

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.RRNamedArray"; }

    RR_OVIRTUAL boost::string_ref RRElementTypeString() RR_OVERRIDE { return RRPrimUtil<T>::GetRRElementTypeString(); }

    // C++ container support functions based on boost::array

    // type definitions
    /** value_type */
    typedef T value_type;
    /** iterator */
    typedef T* iterator;
    /** const_iterator */
    typedef const T* const_iterator;
    /** reference */
    typedef T& reference;
    /** const_reference */
    typedef const T& const_reference;
    /** size_type */
    typedef std::size_t size_type;
    /** difference_type */
    typedef std::ptrdiff_t difference_type;

    // iterator support

    /** @brief returns an iterator to the beginning */
    iterator begin() { return static_cast<T*>(rr_array->begin()); }
    /** @brief returns an iterator to the beginning */
    const_iterator begin() const { return static_cast<const T*>(rr_array->begin()); }
    /** @brief returns an iterator to the beginning */
    const_iterator cbegin() const { return static_cast<const T*>(rr_array->begin()); }

    /** @brief returns an iterator to the end */
    iterator end() { return static_cast<T*>(rr_array->end()); }
    /** @brief returns an iterator to the end */
    const_iterator end() const { return static_cast<T*>(rr_array->end()); }
    /** @brief returns an iterator to the end */
    const_iterator cend() const { return static_cast<T*>(rr_array->end()); }

    /** reverse_iterator */
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    /** const_reverse_iterator */
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

    /** @brief returns a reverse iterator to the beginning */
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    /** @brief returns a reverse iterator to the beginning */
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }

    /** @brief returns a reverse iterator to the end */
    reverse_iterator rend() { return reverse_iterator(begin()); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    /** @brief returns a reverse iterator to the end */
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    /** @brief access specified `namedarray` element */
    reference operator[](size_type i)
    {
        BOOST_ASSERT_MSG(i < size(), "out of range");
        return (static_cast<T*>(rr_array->void_ptr())[i]);
    }

    /** @brief access specified `namedarray` element */
    const_reference operator[](size_type i) const
    {
        BOOST_ASSERT_MSG(i < size(), "out of range");
        return (static_cast<T*>(rr_array->void_ptr())[i]);
    }

    /** @brief access specified `namedarray` element with bounds checking */
    reference at(size_type i)
    {
        rangecheck(i);
        return (static_cast<T*>(rr_array->void_ptr())[i]);
    }
    /** @brief access specified `namedarray` element with bounds checking */
    const_reference at(size_type i) const
    {
        rangecheck(i);
        (static_cast<T*>(rr_array->void_ptr())[i]);
    }

    /** @brief access the first element */
    reference front() { return this->at(0); }
    /** @brief access the first element */
    const_reference front() const { return this->at(0); }

    /** @brief access the last element */
    reference back() { return this->at(size() - 1); }
    /** @brief access the last element */
    const_reference back() const { return this->at(size() - 1); }

    /** @brief checks whether the container is empty */
    bool empty() { return rr_array->empty(); }
    /** @brief returns the maximum possible number of elements */
    size_type max_size() { return size(); }

    void rangecheck(size_type i)
    {
        if (i >= size())
        {
            std::out_of_range e("array<>: index out of range");
            boost::throw_exception(e);
        }
    }
};

/**
 * @brief Base class for namedarray multidimensional arrays
 *
 * Namedarray multidimensional arrays should be allocated using
 * AllocateEmptyRRNamedMultiDimArray()
 *
 */
class RRNamedBaseMultiDimArray : public RRValue
{
  public:
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims;
    virtual boost::string_ref RRElementTypeString() = 0;
};

/**
 * @brief `namedarray` multidimensional array value type
 *
 * This class stores a namedarray multidimensional array.
 * Multidimensional arrays are stored as a uint32_t array of
 * dimensions, and an array of the flattened elements.
 * Arrays are stored in column major, or "Fortran" order.
 *
 * Multidimensional arrays should be allocated using
 * AllocateEmptyRRNamedMultiDimArray()
 *
 * T must be a namedarray type that has been generated as
 * part of the thunk source.
 *
 * RRNamedMultiDimArray must be stored in RR_INTRUSIVE_PTR. It will be
 * deleted automatically when the reference count goes to zero.
 *
 * @tparam T The type of the array elements
 */
template <typename T>
class RRNamedMultiDimArray : public RRNamedBaseMultiDimArray
{
  public:
    typename RR_INTRUSIVE_PTR<RRNamedArray<T> > NamedArray;

    RRNamedMultiDimArray() {}

    RRNamedMultiDimArray(const RR_INTRUSIVE_PTR<RRArray<uint32_t> >& dims, const RR_INTRUSIVE_PTR<RRNamedArray<T> >& a)
    {
        this->Dims = dims;
        this->NamedArray = a;
    }

    RR_OVIRTUAL ~RRNamedMultiDimArray() RR_OVERRIDE {}

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.RRNamedMultiDimArray"; }

    RR_OVIRTUAL boost::string_ref RRElementTypeString() RR_OVERRIDE { return RRPrimUtil<T>::GetRRElementTypeString(); }

    /**
     * @brief Retrieve a subset of an array
     *
     * @param memorypos Position in array to read
     * @param buffer Buffer to store retrieved data
     * @param bufferpos Position within buffer to store data
     * @param count Count of data to retrieve
     */
    virtual void RetrieveSubArray(const std::vector<uint32_t>& memorypos,
                                  const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer,
                                  const std::vector<uint32_t>& bufferpos, const std::vector<uint32_t>& count)
    {

        std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
        std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
        RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
            detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

        uint32_t len = 0;
        uint32_t indexa = 0;
        uint32_t indexb = 0;

        while (iter->Next(indexa, indexb, len))
        {
            for (size_t i = 0; i < len; i++)
            {
                (*buffer->NamedArray)[(indexb + i)] = (*NamedArray)[(indexa + i)];
            }
        }
    }

    /**
     * @brief Assign a subset of an array
     *
     * @param memorypos Position within array to store data
     * @param buffer Buffer to assign data from
     * @param bufferpos Position within buffer to assign from
     * @param count Count of data to assign
     */
    virtual void AssignSubArray(const std::vector<uint32_t>& memorypos,
                                const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer,
                                const std::vector<uint32_t>& bufferpos, const std::vector<uint32_t>& count)
    {

        std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
        std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
        RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
            detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

        uint32_t len = 0;
        uint32_t indexa = 0;
        uint32_t indexb = 0;

        while (iter->Next(indexa, indexb, len))
        {
            for (size_t i = 0; i < len; i++)
            {
                (*NamedArray)[(indexa + i)] = (*buffer->NamedArray)[(indexb + i)];
            }
        }
    }
};

/**
 * @brief Allocate a `namedarray` array with the
 * specified type and length and initialize to zero
 *
 * All elements are initialized to zero
 *
 * T must be a namedarray type that has been generated as
 * part of the thunk source.
 *
 * @tparam T The type of the array elements
 * @param length The length of the returned array (element count)
 * @return RR_INTRUSIVE_PTR<RRNamedrray<T> > The allocated array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRNamedArray<T> > AllocateEmptyRRNamedArray(size_t length)
{
    typedef typename RRPrimUtil<T>::ElementArrayType a_type;
    RR_INTRUSIVE_PTR<RRArray<a_type> > a = AllocateRRArray<a_type>(length * RRPrimUtil<T>::GetElementArrayCount());
    return new RRNamedArray<T>(a); // NOLINT(cppcoreguidelines-owning-memory)
}

/**
 * @brief Allocate an empty multidimensional namedarray array with the
 * specified dimensions
 *
 * The elements of the returned array are initialized to zero
 *
 * T must be a namedarray type that has been generated as
 * part of the thunk source.
 *
 * @tparam T The element type of the array
 * @param dims The dimensions of the new multidimensional array
 * @return RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > The allocated multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > AllocateEmptyRRNamedMultiDimArray(const std::vector<uint32_t>& dims)
{
    uint32_t n_elems = boost::accumulate(dims, 1, std::multiplies<uint32_t>());
    return new RRNamedMultiDimArray<T>(VectorToRRArray<uint32_t>(dims), AllocateEmptyRRNamedArray<T>(n_elems));
}

/**
 * @brief Allocate an empty multidimensional namedarray array with
 * zero elements
 *
 * T must be a namedarray type that has been generated as
 * part of the thunk source.
 *
 * @tparam T The element type of the array
 * @return RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > The allocated multidimensional array
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > AllocateEmptyRRNamedMultiDimArray()
{
    std::vector<uint32_t> length;
    length.push_back(0);
    return new RRNamedMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRNamedArray<T>(0));
}

/**
 * @brief Convert a scalar namedarray into a namedarray array with one element
 *
 * Note that this acts on the namedarray union, not a scalar number within the
 * namedarray union
 *
 * @tparam T The type of the namedarray
 * @param value The namedarray
 * @return RR_INTRUSIVE_PTR<RRPodArray<T> > The new namedarray array with one element
 */
template <typename T>
static RR_INTRUSIVE_PTR<RRNamedArray<T> > ScalarToRRNamedArray(const T& value)
{
    RR_INTRUSIVE_PTR<RRNamedArray<T> > a = AllocateEmptyRRNamedArray<T>(1);
    (*a)[0] = value;
    return a;
}

/**
 * @brief Convert a namedarray array with one element into a namedarray
 *
 * Note that this acts on the namedarray union, not a scalar number within the
 * namedarray union
 *
 * @tparam T The type of the namedarray
 * @param value The namedarray array with one element
 * @return T The first namedarray in the array
 */
template <typename T>
static T RRNamedArrayToScalar(const RR_INTRUSIVE_PTR<RRNamedArray<T> >& value)
{
    if (!value)
    {
        throw NullValueException("Null pointer");
    }

    if (value->size() == 0)
        throw OutOfRangeException("Index out of range");

    return (*value)[0];
}

#define RRPrimUtilEnum(x)                                                                                              \
    template <>                                                                                                        \
    class RRPrimUtil<x>                                                                                                \
    {                                                                                                                  \
      public:                                                                                                          \
        static DataTypes GetTypeID() { return DataTypes_enum_t; }                                                      \
        static MessageStringPtr GetElementTypeString() { return MessageStringPtr(""); }                                \
        static boost::string_ref GetRRElementTypeString() { return ""; }                                               \
        static RR_INTRUSIVE_PTR<RRArray<int32_t> > PrePack(const x& val) { return ScalarToRRArray((int32_t)val); }     \
        template <typename U>                                                                                          \
        static x PreUnpack(const U& val)                                                                               \
        {                                                                                                              \
            return (x)RRArrayToScalar(rr_cast<RRArray<int32_t> >(val));                                                \
        }                                                                                                              \
        typedef RR_INTRUSIVE_PTR<RRArray<int32_t> > BoxedType;                                                         \
    };

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

/**
 * @brief Represents. a point in time. Used by `wire` members to
 * timestamp packets
 *
 * Time is always in UTC
 *
 * Time is relative to the UNIX epoch "1970-01-01T00:00:00Z"
 *
 */
class ROBOTRACONTEUR_CORE_API TimeSpec
{
  public:
    /** @brief Seconds since epoch */
    int64_t seconds;
    /** @brief Nanoseconds from epoch. Normalized to be between 0 and 1e9-1 */
    int32_t nanoseconds;

    /** @brief Construct empty timespec */
    TimeSpec();

    /**
     * @brief Construct timespec with specified time
     *
     * @param seconds Seconds since epoch
     * @param nanoseconds Nanoseconds since epoch
     */
    TimeSpec(int64_t seconds, int32_t nanoseconds);

    /** @brief equality comparison */
    bool operator==(const TimeSpec& t2) const;
    /** @brief inequality comparison */
    bool operator!=(const TimeSpec& t2) const;
    /** @brief subtraction operator */
    TimeSpec operator-(const TimeSpec& t2) const;
    /** @brief addition operator */
    TimeSpec operator+(const TimeSpec& t2) const;
    /** @brief greater-than comparison */
    bool operator>(const TimeSpec& t2) const;
    /** @brief greater-than-or-equal comparison */
    bool operator>=(const TimeSpec& t2) const;
    /** @brief less-then comparison */
    bool operator<(const TimeSpec& t2) const;
    /** @brief less-than-or-equal comparison */
    bool operator<=(const TimeSpec& t2) const;

    /** @brief normalize nanoseconds to be within 0 and 1e9-1 */
    void cleanup_nanosecs();
};

/**
 * @brief Convert a TimeSpec into a boost::posix_time::ptime
 *
 * @param ts The TimeSpec to convert
 * @return boost::posix_time::ptime The converted ptime
 */

boost::posix_time::ptime ROBOTRACONTEUR_CORE_API TimeSpecToPTime(const TimeSpec& ts);
/**
 * @brief Convert a boost::posix_time::ptime into a TimeSpec
 *
 * @param t The ptime to convert
 * @return TimeSpec The converted TimeSpec
 */
TimeSpec ROBOTRACONTEUR_CORE_API ptimeToTimeSpec(const boost::posix_time::ptime& t);

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
typedef boost::container::small_vector<boost::asio::const_buffer, 4> const_buffers;
typedef boost::container::small_vector<boost::asio::mutable_buffer, 4> mutable_buffers;
#else
typedef std::vector<boost::asio::const_buffer> const_buffers;
typedef std::vector<boost::asio::mutable_buffer> mutable_buffers;
#endif

template <typename BufferSequence>
void buffers_consume(BufferSequence& b, size_t count)
{
    typename BufferSequence::iterator e = b.begin();
    ;
    typename BufferSequence::iterator end = b.end();
    for (; e != end;)
    {
        if (count > 0)
        {
            if (count > boost::asio::buffer_size(*e))
            {
                count -= boost::asio::buffer_size(*e);
                e = b.erase(e);
            }
            else
            {
                *e = *e + count;
                count = 0;
                ++e;
            }
        }
        else
        {
            break;
        }
    }
}

template <typename BufferSequence>
BufferSequence buffers_consume_copy(BufferSequence& b, size_t count)
{
    BufferSequence o;
    typename BufferSequence::iterator e = b.begin();
    ;
    typename BufferSequence::iterator end = b.end();
    for (; e != end; e++)
    {
        if (count > 0)
        {
            if (count > boost::asio::buffer_size(*e))
            {
                count -= boost::asio::buffer_size(*e);
            }
            else
            {
                o.push_back(*e + count);
                count = 0;
            }
        }
        else
        {
            o.push_back(*e);
        }
    }

    return o;
}

template <typename BufferSequence>
BufferSequence buffers_consume_copy(const BufferSequence& b, size_t count)
{
    BufferSequence o;
    typename BufferSequence::const_iterator e = b.begin();
    ;
    typename BufferSequence::const_iterator end = b.end();
    for (; e != end; e++)
    {
        if (count > 0)
        {
            if (count > boost::asio::buffer_size(*e))
            {
                count -= boost::asio::buffer_size(*e);
            }
            else
            {
                o.push_back(*e + count);
                count = 0;
            }
        }
        else
        {
            o.push_back(*e);
        }
    }

    return o;
}

template <typename BufferSequence>
BufferSequence buffers_truncate(BufferSequence& b, size_t count)
{
    BufferSequence o;
    typename BufferSequence::iterator e = b.begin();
    ;
    typename BufferSequence::iterator end = b.end();
    size_t p = 0;
    for (; e != end; e++)
    {
        if (p + boost::asio::buffer_size(*e) <= count)
        {
            o.push_back(*e);
            p += boost::asio::buffer_size(*e);
        }
        else
        {
            o.push_back(boost::asio::buffer(*e, count - p));
            break;
        }
    }

    return o;
}

template <typename BufferSequence>
BufferSequence buffers_truncate(const BufferSequence& b, size_t count)
{
    BufferSequence o;
    typename BufferSequence::const_iterator e = b.begin();
    ;
    typename BufferSequence::const_iterator end = b.end();
    size_t p = 0;
    for (; e != end; e++)
    {
        if (p + boost::asio::buffer_size(*e) <= count)
        {
            o.push_back(*e);
            p += boost::asio::buffer_size(*e);
        }
        else
        {
            o.push_back(boost::asio::buffer(*e, count - p));
            break;
        }
    }

    return o;
}

namespace detail
{
ROBOTRACONTEUR_CORE_API std::string encode_index(boost::string_ref index);

ROBOTRACONTEUR_CORE_API std::string decode_index(boost::string_ref index);

template <typename T, typename U>
bool try_convert_string_to_number(const U& arg, T& result)
{
    if (boost::conversion::try_lexical_convert(arg, result))
    {
        return true;
    }

    if (!boost::is_integral<T>::value)
    {
        return false;
    }

    boost::regex hex_regex("^[+\\-]?0x[\\da-fA-F]+$");
    if (!boost::regex_match(arg.begin(), arg.end(), hex_regex))
    {
        return false;
    }

    std::stringstream ss;
    ss << std::hex << arg;
    T v;
    ss >> v;
    if (ss.fail() || !ss.eof())
    {
        return false;
    }

    result = v;

    return true;
}

} // namespace detail

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for RRObject shared_ptr */
using RRObjectPtr = RR_SHARED_PTR<RRObject>;
/** @brief Convenience alias for RRObject const shared_ptr */
using RRObjectConstPtr = RR_SHARED_PTR<const RRObject>;
/** @brief Convenience alias for RRValue intrusive_ptr */
using RRValuePtr = RR_INTRUSIVE_PTR<RRValue>;
/** @brief Convenience alias for RRValue const intrusive_ptr */
using RRValueConstPtr = RR_INTRUSIVE_PTR<const RRValue>;
/** @brief Convenience alias for MessageElementData intrusive_ptr */
using MessageElementDataPtr = RR_INTRUSIVE_PTR<MessageElementData>;
/** @brief Convenience alias for MessageElementData const intrusive_ptr */
using MessageElementDataConstPtr = RR_INTRUSIVE_PTR<const MessageElementData>;
/** @brief Convenience alias for RRBaseArray intrusive_ptr */
using RRBaseArrayPtr = RR_INTRUSIVE_PTR<RRBaseArray>;
/** @brief Convenience alias for RRBaseArray const intrusive_ptr */
using RRBaseArrayConstPtr = RR_INTRUSIVE_PTR<const RRBaseArray>;
/** @brief Convenience alias for RRArray intrusive_ptr */
template <typename T>
using RRArrayPtr = RR_INTRUSIVE_PTR<RRArray<T> >;
/** @brief Convenience alias for RRArray const intrusive_ptr */
template <typename T>
using RRArrayConstPtr = RR_INTRUSIVE_PTR<const RRArray<T> >;
/** @brief Convenience alias for RRMap intrusive_ptr */
template <typename K, typename T>
using RRMapPtr = RR_INTRUSIVE_PTR<RRMap<K, T> >;
/** @brief Convenience alias for RRMap const intrusive_ptr */
template <typename K, typename T>
using RRMapConstPtr = RR_INTRUSIVE_PTR<const RRMap<K, T> >;
/** @brief Convenience alias for RRList intrusive_ptr */
template <typename T>
using RRListPtr = RR_INTRUSIVE_PTR<RRList<T> >;
/** @brief Convenience alias for RRList const intrusive_ptr */
template <typename T>
using RRListConstPtr = RR_INTRUSIVE_PTR<const RRList<T> >;
/** @brief Convenience alias for RRStructure intrusive_ptr */
using RRStructurePtr = RR_INTRUSIVE_PTR<RRStructure>;
/** @brief Convenience alias for RRStructure const intrusive_ptr */
using RRStructureConstPtr = RR_INTRUSIVE_PTR<const RRStructure>;
/** @brief Convenience alias for RRMultiDimBaseArray intrusive_ptr */
using RRMultiDimBaseArrayPtr = RR_INTRUSIVE_PTR<RRMultiDimBaseArray>;
/** @brief Convenience alias for RRMultiDimBaseArray const intrusive_ptr */
using RRMultiDimBaseArrayConstPtr = RR_INTRUSIVE_PTR<const RRMultiDimBaseArray>;
/** @brief Convenience alias for RRMultiDimArray intrusive_ptr */
template <typename T>
using RRMultiDimArrayPtr = RR_INTRUSIVE_PTR<RRMultiDimArray<T> >;
/** @brief Convenience alias for RRMultiDimArray const intrusive_ptr */
template <typename T>
using RRMultiDimArrayConstPtr = RR_INTRUSIVE_PTR<const RRMultiDimArray<T> >;
/** @brief Convenience alias for RRPodBaseArray intrusive_ptr */
using RRPodBaseArrayPtr = RR_INTRUSIVE_PTR<RRPodBaseArray>;
/** @brief Convenience alias for RRPodBaseArray const intrusive_ptr */
using RRPodBaseArrayConstPtr = RR_INTRUSIVE_PTR<const RRPodBaseArray>;
/** @brief Convenience alias for RRPodArray intrusive_ptr */
template <typename T>
using RRPodArrayPtr = RR_INTRUSIVE_PTR<RRPodArray<T> >;
/** @brief Convenience alias for RRPodArray const intrusive_ptr */
template <typename T>
using RRPodArrayConstPtr = RR_INTRUSIVE_PTR<const RRPodArray<T> >;
/** @brief Convenience alias for RRPodBaseMultiDimArray intrusive_ptr */
using RRPodBaseMultiDimArrayPtr = RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray>;
/** @brief Convenience alias for RRPodBaseMultiDimArray const intrusive_ptr */
using RRPodBaseMultiDimBaseArrayConstPtr = RR_INTRUSIVE_PTR<const RRPodBaseMultiDimArray>;
/** @brief Convenience alias for RRPodMultiDimArray intrusive_ptr */
template <typename T>
using RRPodMultiDimArrayPtr = RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >;
/** @brief Convenience alias for RRPodMultiDimArray const intrusive_ptr */
template <typename T>
using RRPodMultiDimArrayConstPtr = RR_INTRUSIVE_PTR<const RRPodMultiDimArray<T> >;
/** @brief Convenience alias for RRNamedBaseArray intrusive_ptr */
using RRNamedBaseArrayPtr = RR_INTRUSIVE_PTR<RRNamedBaseArray>;
/** @brief Convenience alias for RRNamedBaseArray const intrusive_ptr */
using RRNamedBaseArrayConstPtr = RR_INTRUSIVE_PTR<const RRNamedBaseArray>;
/** @brief Convenience alias for RRNamedArray intrusive_ptr */
template <typename T>
using RRNamedArrayPtr = RR_INTRUSIVE_PTR<RRNamedArray<T> >;
/** @brief Convenience alias for RRNamedArray const intrusive_ptr */
template <typename T>
using RRNamedArrayConstPtr = RR_INTRUSIVE_PTR<const RRNamedArray<T> >;
/** @brief Convenience alias for RRNamedBaseMultiDimArray intrusive_ptr */
using RRNamedBaseMultiDimArrayPtr = RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray>;
/** @brief Convenience alias for RRNamedBaseMultiDimArray const intrusive_ptr */
using RRNamedBaseMultiDimBaseArrayConstPtr = RR_INTRUSIVE_PTR<const RRNamedBaseMultiDimArray>;
/** @brief Convenience alias for RRNamedMultiDimArray intrusive_ptr */
template <typename T>
using RRNamedMultiDimArrayPtr = RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >;
/** @brief Convenience alias for RRNamedMultiDimArray const intrusive_ptr */
template <typename T>
using RRNamedMultiDimArrayConstPtr = RR_INTRUSIVE_PTR<const RRNamedMultiDimArray<T> >;
#endif
} // namespace RobotRaconteur
