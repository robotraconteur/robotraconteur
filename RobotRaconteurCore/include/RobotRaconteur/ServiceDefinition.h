/**
 * @file ServiceDefinition.h
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

#pragma once

#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"

#include <boost/tuple/tuple.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API ServiceEntryDefinition;
class ROBOTRACONTEUR_CORE_API MemberDefinition;
class ROBOTRACONTEUR_CORE_API FunctionDefinition;
class ROBOTRACONTEUR_CORE_API PropertyDefinition;
class ROBOTRACONTEUR_CORE_API EventDefinition;
class ROBOTRACONTEUR_CORE_API ObjRefDefinition;
class ROBOTRACONTEUR_CORE_API PipeDefinition;
class ROBOTRACONTEUR_CORE_API CallbackDefinition;
class ROBOTRACONTEUR_CORE_API WireDefinition;
class ROBOTRACONTEUR_CORE_API MemoryDefinition;
class ROBOTRACONTEUR_CORE_API TypeDefinition;
class ROBOTRACONTEUR_CORE_API ExceptionDefinition;
class ROBOTRACONTEUR_CORE_API UsingDefinition;
class ROBOTRACONTEUR_CORE_API ConstantDefinition;
class ROBOTRACONTEUR_CORE_API EnumDefinition;
class ROBOTRACONTEUR_CORE_API EnumDefinitionValue;

class ROBOTRACONTEUR_CORE_API ServiceDefinitionParseException;
class ROBOTRACONTEUR_CORE_API ServiceDefinitionVerifyException;

/**
 * @brief Service definition parse information
 *
 * Stores the service name, the file path, the line contents,
 * and the line number of a declaration in a service definition file.
 * Used to generate diagnostic messages.
 *
 */
struct ROBOTRACONTEUR_CORE_API ServiceDefinitionParseInfo
{
    /** @brief The name of the service containing the declaration */
    std::string ServiceName;
    /** @brief The file path of the service definition file containing the declaration */
    std::string RobDefFilePath;
    /** @brief The text of the declaration */
    std::string Line;
    /** @brief The line number of the declaration in the file */
    int32_t LineNumber;

    /** @brief Construct a new ServiceDefinitionParseInfo structure */
    ServiceDefinitionParseInfo();
    /** @brief Reset all fields */
    void Reset();
};

/**
 * @brief Robot Raconteur Version storage class
 *
 * Stores a semantic version:
 *
 *     MAJOR.MINOR.PATCH(.tweak)?
 *
 * Versioning should follow the Semantic Versioning 2.0.0 standard (http://semver.org/)
 *
 */
class ROBOTRACONTEUR_CORE_API RobotRaconteurVersion
{
  public:
    /**
     * @brief Construct a new default RobotRaconteurVersion instance
     *
     * Version will be set to 0.0.0
     */
    RobotRaconteurVersion();

    /**
     * @brief Construct a new RobotRaconteurVersion instance with a given version
     *
     * @param major Major version
     * @param minor Minor version
     * @param patch Patch version
     * @param tweak Tweak version
     */
    RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch = 0, uint32_t tweak = 0);

    /**
     * @brief Construct a new RobotRaconteurVersion instance with a version parsed from as string
     *
     * String must be in the form "MAJOR.MINOR.PATCH.TWEAK", ie "1.0.4"
     *
     * PATCH and TWEAK are optional
     *
     * @param v The version as a string
     */
    RobotRaconteurVersion(boost::string_ref v);

    /**
     * @brief Get the version as a string
     *
     * Returns a string in the form "MAJOR.MINOR.PATCH.TWEAK". PATCH and TWEAK
     * will be omitted if both are zero.
     *
     * @return std::string The version as a string
     */
    std::string ToString() const;

    /**
     * @brief Parse a version string and update version fields
     *
     * Fills major, minor, patch, and tweak fields from a string.
     * String must be in the form "MAJOR.MINOR.PATCH.TWEAK", ie "1.0.4".
     * PATCH and TWEAK are optional. Parse information may optionally
     * be specified.
     *
     * @param v The version string to parse
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref v, const ServiceDefinitionParseInfo* parse_info = NULL);

    /** @brief Equality operator */
    ROBOTRACONTEUR_CORE_API friend bool operator==(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2);
    /** @brief Inequality operator */
    ROBOTRACONTEUR_CORE_API friend bool operator!=(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2);
    /** @brief Greater than operator */
    ROBOTRACONTEUR_CORE_API friend bool operator>(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2);
    /** @brief Greater than or equal operator */
    ROBOTRACONTEUR_CORE_API friend bool operator>=(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2);
    /** @brief Less than operator */
    ROBOTRACONTEUR_CORE_API friend bool operator<(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2);
    /** @brief Less than or equal operator */
    ROBOTRACONTEUR_CORE_API friend bool operator<=(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2);

    /**
     * @brief Boolean operator test if version is specified
     *
     * The version is considered unspecified if version is all zeros
     *
     * @return true The version is not all zeros
     * @return false The version is all zeros
     */
    operator bool() const;

    /** @brief The major version */
    uint32_t major;
    /** @brief The minor version */
    uint32_t minor;
    /** @brief The patch version */
    uint32_t patch;
    /** @brief The tweak version */
    uint32_t tweak;

    /** @brief Parsing diagnostic information */
    ServiceDefinitionParseInfo ParseInfo;
};

/**
 * @brief Exception thrown when a parsing error occurs
 *
 * Extends ServiceDefinitionException. When used with messages,
 * has error code MessageErrorType_ServiceDefinitionError (28)
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceDefinitionParseException : public ServiceDefinitionException
{

  public:
    /** @brief Parsing diagnostic information */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Short description of the error
     *
     * Do not include line number, file name, line number, or line text in ShortMessage. This
     * information is stored in the ParseInfo field.
     */
    std::string ShortMessage;

    /**
     * @brief Construct a new ServiceDefinitionParseException
     *
     * @param e The short message
     */
    ServiceDefinitionParseException(const std::string& e);

    /**
     * @brief Construct a new ServiceDefinitionParseException
     *
     * @param e The short message
     * @param info Parsing diagnostic information
     */
    ServiceDefinitionParseException(const std::string& e, const ServiceDefinitionParseInfo& info);

    /**
     * @brief Convert exception to string
     *
     * Returns a diagnostic message containing the service name, line number, and short message.
     *
     * @return std::string The diagnostic message
     */
    virtual std::string ToString();

    virtual const char* what() const throw();

    ~ServiceDefinitionParseException() throw() {}

  private:
    std::string what_store;
};

/**
 * @brief Exception thrown when a service definition verification failure occurs
 *
 * Extends ServiceDefinitionException. When used with messages,
 * has error code MessageErrorType_ServiceDefinitionError (28)
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceDefinitionVerifyException : public ServiceDefinitionException
{

  public:
    /** @brief Parsing diagnostic information */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Short description of the error
     *
     * Do not include line number, file name, line number, or line text in ShortMessage. This
     * information is stored in the ParseInfo field.
     */
    std::string ShortMessage;

    /**
     * @brief Construct a new ServiceDefinitionVerifyException
     *
     * @param e The short message
     */
    ServiceDefinitionVerifyException(const std::string& e);

    /**
     * @brief Construct a new ServiceDefinitionVerifyException
     *
     * @param e The short message
     * @param info Parsing diagnostic information
     */
    ServiceDefinitionVerifyException(const std::string& e, const ServiceDefinitionParseInfo& info);

    /**
     * @brief Convert exception to string
     *
     * Returns a diagnostic message containing the service name, line number, and short message.
     *
     * @return std::string The diagnostic message
     */
    virtual std::string ToString();

    virtual const char* what() const throw();

    ~ServiceDefinitionVerifyException() throw() {}

  private:
    std::string what_store;
};

/**
 * @brief Class representing a service definition
 *
 * Service definitions are used to specify user defined types. Service definition files
 * are a form of Interface Definition Language (IDL) that can be parsed by this class to
 * load the sure defined types. See \ref service_definitions.md for more information on
 * service definitions.
 *
 * Use FromString() or FromStream() to parse a service definition. Use ToString() or
 * ToStream() to convert the service definition to text.
 */
class ROBOTRACONTEUR_CORE_API ServiceDefinition : public RR_ENABLE_SHARED_FROM_THIS<ServiceDefinition>
{
  public:
    /** @brief The name of the service definition */
    std::string Name;

    /** @brief The structures declared in the service definition */
    std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Structures;
    /** @brief The pods declared in the service definition */
    std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Pods;
    /** @brief The named arrays declared in the service definition */
    std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > NamedArrays;
    /** @brief The objects declared in the service definition */
    std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Objects;

    /** @brief The options declared in the service definition (deprecated) */
    std::vector<std::string> Options;

    /** @brief The names of service definitions imported by this service definition */
    std::vector<std::string> Imports;

    /** @brief The using types declared in this service definition */
    std::vector<RR_SHARED_PTR<UsingDefinition> > Using;

    /** @brief The exceptions declared in this service definition */
    std::vector<RR_SHARED_PTR<ExceptionDefinition> > Exceptions;

    /** @brief The top level constants declared in the service definition */
    std::vector<RR_SHARED_PTR<ConstantDefinition> > Constants;

    /** @brief The enums declared in this service definition */
    std::vector<RR_SHARED_PTR<EnumDefinition> > Enums;

    /** @brief The service definition standard version required by this service definition */
    RobotRaconteurVersion StdVer;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by the FromString() or FromStream() functions
     */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Top level documentation string for the service
     *
     */
    std::string DocString;

    virtual ~ServiceDefinition() {}

    /**
     * @brief Convert service definition to a string
     *
     * Writes service definition to a string that can be stored as a file
     *
     * @return std::string The service definition file as a string
     */
    virtual std::string ToString();

    /**
     * @brief Parses a service definition from a string
     *
     * Parses the string specified in s. String must be in the Service Definition IDL format.
     *
     * Service definition must be verified using VerifyServiceDefinitions() after
     * parsing
     *
     * @param s The service definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Parses a service definition from a string with warnings
     *
     * Parses the string specified in s. String must be in the Service Definition IDL format.
     * Parse warnings will be added to the warnings parameter.
     *
     * Service definition must be verified using VerifyServiceDefinitions() after
     * parsing
     *
     * @param s The service definition as a string
     * @param warnings A vector to receive parse warnings
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, std::vector<ServiceDefinitionParseException>& warnings,
                    const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Convert service definition to a text stream
     *
     * Writes service definition to a stream that can be stored as a file
     *
     * @param os The stream to write the service definition to
     */
    virtual void ToStream(std::ostream& os);

    /**
     * @brief Parse a service definition from a stream
     *
     * Parses the stream specified in is. Stream text must be in the Service Definition IDL format.
     *
     * Service definition must be verified using VerifyServiceDefinitions() after
     * parsing
     *
     * @param is The stream containing the service definition text
     * @param parse_info Parsing diagnostic information
     */
    void FromStream(std::istream& is, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Parse a service definition from a stream with warnings
     *
     * Parses the stream specified in is. Stream text must be in the Service Definition IDL format.
     * Parse warnings will be added to the warnings parameter.
     *
     * Service definition must be verified using VerifyServiceDefinitions() after
     * parsing
     *
     * @param is The stream containing the service definition text
     * @param warnings A vector to receive parse warnings
     * @param parse_info Parsing diagnostic information
     */
    void FromStream(std::istream& is, std::vector<ServiceDefinitionParseException>& warnings,
                    const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Check that the service definition does not require a newer version of Robot Raconteur
     *
     * The `stdver` keyword can be declared in a service definition to require a greater or
     * equal version of the Robot Raconteur library. CheckVersion() will compare the library
     * version and throw an exception if the service definition requires a newer library version.
     *
     * @param ver The test Robot Raconteur version. If all zero (unspecified), uses current library version
     */
    void CheckVersion(RobotRaconteurVersion ver = RobotRaconteurVersion(0, 0));

    /**
     * @brief Construct a new empty ServiceDefinition
     *
     * Must be constructed with boost::make_shared<ServiceDefinition>()
     *
     */
    ServiceDefinition();

    /**
     * @brief Clear all fields
     *
     */
    void Reset();
};

/**
 * @brief Base class for user-defined named types
 *
 * User-defined types are called "named types", since they are named in the declaration,
 * and that name is stored within messages when serialized in the `ElementTypeName` field.
 *
 */
class ROBOTRACONTEUR_CORE_API NamedTypeDefinition
{
  public:
    /** @brief The unqualified name of the type */
    std::string Name;
    /** @brief The type code of the type */
    virtual DataTypes RRDataType() = 0;
    /**
     * @brief Resolves and returns the qualified name
     *
     * The Name field contains the unqualified name, meaning that it does not
     * contain the service definition name. A qualified name contains the service
     * definition name, a dot, and the unqualified name. An example is the qualified
     * name "example.named_example.struct1", where "example.named_example" is the
     * service definition name, and "struct1" is the unqualified name of the struct.
     *
     * ResolveQualifiedName() will attempt to determine the fully qualified name
     * of the type, or throw an exception if it is unable.
     */
    virtual std::string ResolveQualifiedName() = 0;
};

/**
 * @brief Class representing a service entry definition
 *
 * Structures, Pods, NamedArrays, and Objects are service entries.
 *
 * Service entries are contained within a parent ServiceDefinition.
 *
 * See \ref service_definitions.md for more information on
 * service entry definitions and service definitions.
 *
 * Use FromString() or FromStream() to parse a service entry definition. Use ToString() or
 * ToStream() to convert the service entry definition to text.
 */
class ROBOTRACONTEUR_CORE_API ServiceEntryDefinition : public RR_ENABLE_SHARED_FROM_THIS<ServiceEntryDefinition>,
                                                       public NamedTypeDefinition
{
  public:
    /**
     * @brief The members of the entry definition
     *
     * Structure, Pods, and NamedArrays can only contain Field members.
     *
     * Objects may contain Property, Function, Event, ObjRef, Pipe,
     * Callback, Wire, and Memory members.
     *
     */
    std::vector<RR_SHARED_PTR<MemberDefinition> > Members;

    /**
     * @brief The type code of the entry type
     *
     * Valid type codes for service entries are DataTypes_structure_t,
     * DataTypes_pod_t, DataTypes_namedarray_t, and DataTypes_object_t
     *
     */
    DataTypes EntryType;

    /**
     * @brief Object types implemented by this object
     *
     * Only valid for objects
     */
    std::vector<std::string> Implements;

    /** @brief The options declared in the service entry definition (deprecated) */
    std::vector<std::string> Options;

    /** @brief The constants declared in the service entry definition */
    std::vector<RR_SHARED_PTR<ConstantDefinition> > Constants;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by the FromString() or FromStream() functions
     */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Documentation string for the service entry
     *
     */
    std::string DocString;

    /**
     * @brief Construct a new empty ServiceEntryDefinition
     *
     * Must be constructed with boost::make_shared<ServiceEntryDefinition>()
     *
     * @param def The parent service definition containing this service entry
     */
    ServiceEntryDefinition(RR_SHARED_PTR<ServiceDefinition> def);

    virtual ~ServiceEntryDefinition() {}

    /**
     * @brief Convert service entry definition to a string
     *
     * Converts service entry definition to a string
     *
     * @return std::string The service entry definition as a string
     */
    virtual std::string ToString();

    /**
     * @brief Convert service definition to a text stream
     *
     * Writes service entry definition to a stream
     *
     * @param os The stream to write the service entry definition to
     */
    virtual void ToStream(std::ostream& os);

    /**
     * @brief Parse a service entry definition from a string
     *
     * Parsing individual service entries is not recommended. Use
     * ServiceDefinition::FromString() to parse the entire service definition
     * instead.
     *
     * @param s The service entry definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Parse a service entry definition from a string with warnings
     *
     * Parsing individual service entries is not recommended. Use
     * ServiceDefinition::FromString() to parse the entire service definition
     * instead.
     *
     * @param s The service entry definition as a string
     * @param warnings A vector to receive parse warnings
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, std::vector<ServiceDefinitionParseException>& warnings,
                    const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Parse a service entry definition from a stream
     *
     * Parsing individual service entries is not recommended. Use
     * ServiceDefinition::FromStream() to parse the entire service definition
     * instead.
     *
     * @param is The stream containing the service entry definition text
     * @param parse_info Parsing diagnostic information
     */
    void FromStream(std::istream& is, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Parse a service entry definition from a stream with warnings
     *
     * Parsing individual service entries is not recommended. Use
     * ServiceDefinition::FromStream() to parse the entire service definition
     * instead.
     *
     * @param is The stream containing the service entry definition text
     * @param warnings A vector to receive parse warnings
     * @param parse_info Parsing diagnostic information
     */
    void FromStream(std::istream& is, std::vector<ServiceDefinitionParseException>& warnings,
                    const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief The parent service definition
     *
     * Stored as a weak_ptr to prevent circular reference counts
     */
    RR_WEAK_PTR<ServiceDefinition> ServiceDefinition_;

    /**
     * @brief Clear all fields
     *
     */
    void Reset();

    virtual DataTypes RRDataType();
    virtual std::string ResolveQualifiedName();
};

/**
 * @brief Base class for member definitions
 *
 * Members are contained within service entry definitions. Subclasses of
 * MemberDefinition represent each specific member type.
 *
 */
class ROBOTRACONTEUR_CORE_API MemberDefinition : public RR_ENABLE_SHARED_FROM_THIS<MemberDefinition>
{
  public:
    /** @brief The name of the member */
    std::string Name;
    /**
     * @brief The parent service entry definition
     *
     * Stored as a weak_ptr to prevent circular reference counts
     *
     */
    RR_WEAK_PTR<ServiceEntryDefinition> ServiceEntry;

    /**
     * @brief Modifiers for this member
     *
     * Modifiers are used to modify the behavior of the member. See
     * \ref service_definitions.md for more information on valid
     * modifiers.
     */
    std::vector<std::string> Modifiers;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by the FromString() function of the member
     */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Documentation string for the member
     *
     */
    std::string DocString;

    MemberDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    /**
     * @brief Get member locking behavior
     *
     * Members may use modifiers to change behavior when an object is locked
     * using RobotRaconteurNode::RequestObjectLock() or ServerContext::RequestObjectLock()
     * By default, access to the object is protected against other users/sessions. Members
     * can be declared `nolock` or `nolockread` to allow objects to be unlocked, or unlocked
     * for read operations. See \ref object_locks.md for more information.
     *
     * @return MemberDefinition_NoLock The locking behavior
     */
    virtual MemberDefinition_NoLock NoLock();

    virtual ~MemberDefinition() {}

    /**
     * @brief Convert member to a string
     *
     * Returned member string is in Service Definition IDL format
     *
     * @return std::string The member as a string
     */
    virtual std::string ToString() { return ""; }

    /** @brief Clear all fields */
    virtual void Reset();
};

/**
 * @brief Class for property and field member definitions
 *
 * PropertyDefinition is used to store Property member definitions when
 * used with an Object parent, or used to store a Field member
 * definition when stored in a Structure, Pod, or Named Array.
 *
 * See \ref service_definitions.md for more information.
 */
class ROBOTRACONTEUR_CORE_API PropertyDefinition : public MemberDefinition
{
  public:
    /** @brief The value type of the property or field */
    RR_SHARED_PTR<TypeDefinition> Type;

    /**
     * @brief Construct a new empty PropertyDefinition object
     *
     * Must be constructed with boost::make_shared<PropertyDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    PropertyDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual ~PropertyDefinition() {}

    /**
     * @brief Convert to a property definition string
     *
     * Use ToString(true) to convert to a field definition
     */
    virtual std::string ToString();

    /**
     * @brief Convert to a property or field definition string
     *
     * If isstruct is true, the returned string is a field definition.
     * If false, the returned string is a property definition.
     *
     * @param isstruct true for field, false for property
     * @return std::string The property or field as a string
     */
    std::string ToString(bool isstruct);

    /**
     * @brief Parse a property or field definition from string
     *
     * @param s The property or field as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();

    /**
     * @brief The direction of the property
     *
     * Only valid for properties. Not valid for fields.
     *
     * Properties may be declared `readonly` or `writeonly`
     * using member modifiers. If no modifier is present,
     * the property is read/write.
     *
     * @return MemberDefinition_Direction The direction of the property
     */
    MemberDefinition_Direction Direction();
};

/**
 * @brief Class for function member definitions
 *
 * FunctionDefinition is used to store Function member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API FunctionDefinition : public MemberDefinition
{
  public:
    /** @brief The return type of the function. May be `void`. */
    RR_SHARED_PTR<TypeDefinition> ReturnType;
    /** @brief The parameter types and names of the function. */
    std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

    /**
     * @brief Construct a new empty FunctionDefinition object
     *
     * Must be constructed with boost::make_shared<FunctionDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    FunctionDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse a function definition from string
     *
     * @param s The function definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();

    /**
     * @brief Check if function member is a generator function
     *
     * Function members are generators if the return and/or last parameter
     * type have a {generator} container type. These functions will return
     * a Generator instance when called.
     *
     * @return true The function member is a generator function
     * @return false The function member is not a generator function
     */
    bool IsGenerator();
};

/**
 * @brief Class for event member definitions
 *
 * EventDefinition is used to store Event member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API EventDefinition : public MemberDefinition
{
  public:
    /** @brief The parameter types and names of the event. */
    std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

    /**
     * @brief Construct a new empty EventDefinition object
     *
     * Must be constructed with boost::make_shared<EventDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    EventDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse an event definition from string
     *
     * @param s The event definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();
};

/**
 * @brief Class for objref member definitions
 *
 * ObjRefDefinition is used to store ObjRef member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API ObjRefDefinition : public MemberDefinition
{
  public:
    /**
     * @brief The object type name
     *
     * Object type name should be fully qualified if imported from another service definition.
     */
    std::string ObjectType;

    /**
     * @brief The array type of the ObjRef member
     *
     * ObjRefs may have array types DataTypes_ArrayTypes_none or DataTypes_ArrayTypes_array
     *
     * Array and container types may not be combined.
     *
     */
    DataTypes_ArrayTypes ArrayType;

    /**
     * @brief The container type of the ObjRef member
     *
     * ObjRefs may have container types DataTypes_ContainerTypes_none,
     * DataTypes_ContainerTypes_map_int32, or DataTypes_ContainerTypes_map_string
     *
     * Array and container types may not be combined.
     */
    DataTypes_ContainerTypes ContainerType;

    /**
     * @brief Construct a new empty ObjRefDefinition object
     *
     * Must be constructed with boost::make_shared<ObjRefDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    ObjRefDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse an objref definition from string
     *
     * @param s The objref definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();
};

/**
 * @brief Class for pipe member definitions
 *
 * PipeDefinition is used to store Pipe member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API PipeDefinition : public MemberDefinition
{
  public:
    /** @brief The pipe packet value type */
    RR_SHARED_PTR<TypeDefinition> Type;

    /**
     * @brief Construct a new empty PipeDefinition object
     *
     * Must be constructed with boost::make_shared<PipeDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    PipeDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse a pipe definition from string
     *
     * @param s The pipe definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();

    /**
     * @brief The direction of the pipe
     *
     * Pipes may be declared `readonly` or `writeonly`
     * using member modifiers. If no modifier is present,
     * the pipe is read/write.
     *
     * @return MemberDefinition_Direction The direction of the pipe
     */
    MemberDefinition_Direction Direction();

    /**
     * @brief Pipe reliability
     *
     * Pipes may be declared `unreliable` using member modifiers.
     * Unreliable pipes do not guarantee reliable operation,
     * and packets may be dropped or arrive out of order.
     *
     * @return true Pipe is unreliable
     * @return false Pipe is reliable
     */
    bool IsUnreliable();
};

/**
 * @brief Class for callback member definitions
 *
 * CallbackDefinition is used to store Callback member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API CallbackDefinition : public MemberDefinition
{
  public:
    virtual ~CallbackDefinition() {}

    /** @brief The return type of the callback. May be `void`. */
    RR_SHARED_PTR<TypeDefinition> ReturnType;
    /** @brief The parameter types and names of the callback. */
    std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

    /**
     * @brief Construct a new empty CallbackDefinition object
     *
     * Must be constructed with boost::make_shared<CallbackDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    CallbackDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse a callback definition from string
     *
     * @param s The callback definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();
};

/**
 * @brief Class for wire member definitions
 *
 * WireDefinition is used to store Wire member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API WireDefinition : public MemberDefinition
{
  public:
    /** @brief The wire value type */
    RR_SHARED_PTR<TypeDefinition> Type;

    /**
     * @brief Construct a new empty WireDefinition object
     *
     * Must be constructed with boost::make_shared<WireDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    WireDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse a wire definition from string
     *
     * @param s The wire definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();

    /**
     * @brief The direction of the wire
     *
     * Wires may be declared `readonly` or `writeonly`
     * using member modifiers. If no modifier is present,
     * the wire is read/write.
     *
     * @return MemberDefinition_Direction The direction of the wire
     */
    MemberDefinition_Direction Direction();
};

/**
 * @brief Class for memory member definitions
 *
 * MemoryDefinition is used to store Memory member definitions
 * used with an Object parent.
 *
 * See \ref service_definitions.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API MemoryDefinition : public MemberDefinition
{
  public:
    /** @brief The value type of the memory */
    RR_SHARED_PTR<TypeDefinition> Type;

    /**
     * @brief Construct a new empty MemoryDefinition object
     *
     * Must be constructed with boost::make_shared<MemoryDefinition>()
     *
     * @param ServiceEntry The parent service entry definition
     */
    MemoryDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

    virtual std::string ToString();

    /**
     * @brief Parse a memory definition from string
     *
     * @param s The memory definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    virtual void Reset();

    /**
     * @brief The direction of the memory
     *
     * Memories may be declared `readonly` or `writeonly`
     * using member modifiers. If no modifier is present,
     * the memory is read/write.
     *
     * @return MemberDefinition_Direction The direction of the memory
     */
    MemberDefinition_Direction Direction();
};

/**
 * @brief Class for type definitions
 *
 * See \ref service_definitions.md for more information
 */
class ROBOTRACONTEUR_CORE_API TypeDefinition
{
  public:
    virtual ~TypeDefinition() {}

    /**
     * @brief The name of the type definition
     *
     * Used for parameter names
     */
    std::string Name;

    /** @brief The type code of the type definition */
    DataTypes Type;
    /** @brief The type name used if Type is DataTypes_namedtype_t */
    std::string TypeString;

    /** @brief The array type of the type definition */
    DataTypes_ArrayTypes ArrayType;
    /** @brief If true, array is variable length. If false, fixed length */
    bool ArrayVarLength;
    /** @brief The array length */
    std::vector<int32_t> ArrayLength;

    /** @brief The container type of the type definition */
    DataTypes_ContainerTypes ContainerType;

    /** @brief The parent member of the type definition */
    RR_WEAK_PTR<MemberDefinition> member;

    /** @brief Parsing diagnostic information */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Construct a new empty TypeDefinition
     *
     */
    TypeDefinition();

    /**
     * @brief Construct a new empty TypeDefinition
     *
     * @param member The parent member definition
     */
    TypeDefinition(RR_SHARED_PTR<MemberDefinition> member);

    /**
     * @brief Convert property definition to string
     *
     *
     */
    virtual std::string ToString();

    /**
     * @brief Parse a type definition from string
     *
     * @param s The type definition as a string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Get the DataTypes type code from a string
     *
     * Converts a string name to a type code. Array and containers
     * must be removed from string before use.
     *
     * All unknown strings return DataTypes_namedtype_t
     *
     * @param d The string
     * @return DataTypes The type code
     */
    static DataTypes DataTypeFromString(boost::string_ref d);

    /**
     * @brief Convert a DataTypes type code to a string
     *
     * @param d The type code
     * @return std::string The string type
     */
    static std::string StringFromDataType(DataTypes d);

    /**
     * @brief Clear all fields
     *
     */
    void Reset();

    /**
     * @brief Copy type definition to def
     *
     * @param def The target type definition
     */
    void CopyTo(TypeDefinition& def) const;

    /**
     * @brief Clones the type definition
     *
     * @return RR_SHARED_PTR<TypeDefinition> The cloned type definition
     */
    RR_SHARED_PTR<TypeDefinition> Clone() const;

    /**
     * @brief Rename the type definition
     *
     * Updates the Name field
     *
     * @param name The new name
     */
    void Rename(boost::string_ref name);

    /**
     * @brief Removes containers from the type
     *
     * Sets ContainerType to DataTypes_ContainerTypes_none
     *
     */
    void RemoveContainers();

    /**
     * @brief Removes arrays from the type
     *
     * Sets ArrayType to DataTypes_ArrayTypes_none
     *
     */
    void RemoveArray();

    /**
     * @brief Searches the service definition for a matching using declaration,
     * and replaces type with qualified name
     *
     * member field must be a valid parent member. Service definition must
     * be available by searching the parents of member.
     *
     */
    void QualifyTypeStringWithUsing();

    /**
     * @brief Searches the service definition for a matching using declaration,
     * and replaces type with unqualified name
     *
     * member field must be a valid parent member. Service definition must
     * be available by searching the parents of member.
     *
     */
    void UnqualifyTypeStringWithUsing();

    /**
     * @brief Resolve the named type to a NamedTypeDefinition
     *
     * Named types are user defined types that are declared in service definition files.
     * Types that are not primitive types are named types. They have the type code
     * DataTypes_namedtype_t when stored in TypeDefinition, and use the TypeString
     * field to store the qualified or unqualified name. ResolveNamedType will
     * search the service definition to find the matching NamedTypeDefinition. This
     * will either have the subclass ServiceEntryDefinition or EnumDefinition.
     *
     * ResolveNamedType searches the parent tree starting member, the service
     * definitions specified in other_defs, the service types registered by
     * the specified node, or the service types pulled by the specified client,
     * in that order.
     *
     * Failure will result in a ServiceDefinitionException being thrown.
     *
     * @param other_defs Service definitions to search for type
     * @param node Search service types registered in node
     * @param client Search service types pulled by client
     * @return RR_SHARED_PTR<NamedTypeDefinition> The resolved type definition
     */
    RR_SHARED_PTR<NamedTypeDefinition> ResolveNamedType(
        std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs = std::vector<RR_SHARED_PTR<ServiceDefinition> >(),
        RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>(),
        RR_SHARED_PTR<RRObject> client = RR_SHARED_PTR<RRObject>());

    // Don't modify directly, use ResolveNamedType. Left public for specalized use.
    RR_WEAK_PTR<NamedTypeDefinition> ResolveNamedType_cache;
};

/**
 * @brief Class for exception definitions
 *
 * See \ref service_definitions.md for more information.
 */
class ROBOTRACONTEUR_CORE_API ExceptionDefinition
{
  public:
    virtual ~ExceptionDefinition();

    /** @brief The name of the exception */
    std::string Name;
    /** @brief Documentation string for the exception */
    std::string DocString;

    /** @brief The parent service definition */
    RR_WEAK_PTR<ServiceDefinition> service;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by FromString()
     */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Construct a new empty ExceptionDefinition object
     *
     * Must be constructed with boost::make_shared<ExceptionDefinition>()
     *
     * @param service The parent service definition
     */
    ExceptionDefinition(RR_SHARED_PTR<ServiceDefinition> service);

    /**
     * @brief Convert exception definition to string
     *
     * @return std::string The exception definition string
     */
    std::string ToString();

    /**
     * @brief Parse an exception definition from string
     *
     * @param s The exception definition string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    void Reset();
};

/**
 * @brief Class for using definitions
 *
 * See \ref service_definitions.md for more information.
 */
class ROBOTRACONTEUR_CORE_API UsingDefinition
{
  public:
    virtual ~UsingDefinition();

    /** @brief The qualified name of the imported type */
    std::string QualifiedName;
    /** @brief The unqualified name of the imported type */
    std::string UnqualifiedName;

    /** @brief The parent service definition */
    RR_WEAK_PTR<ServiceDefinition> service;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by FromString()
     */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Construct a new empty UsingDefinition object
     *
     * Must be constructed with boost::make_shared<UsingDefinition>()
     *
     * @param service The parent service definition
     */
    UsingDefinition(RR_SHARED_PTR<ServiceDefinition> service);

    /**
     * @brief Convert using definition to string
     *
     * @return std::string The using definition string
     */
    std::string ToString();

    /**
     * @brief Parse an using definition from string
     *
     * @param s The using definition string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);
};

/**
 * @brief Class for constant structure field definitions
 *
 * See \ref service_definitions.md for more information.
 */
struct ROBOTRACONTEUR_CORE_API ConstantDefinition_StructField
{
    /** @brief The name of the constant structure field */
    std::string Name;
    /** @brief The name of the constant the field refers to */
    std::string ConstantRefName;
};

/**
 * @brief Class for constant definitions
 *
 * See \ref service_definitions.md for more information.
 */
class ROBOTRACONTEUR_CORE_API ConstantDefinition
{
  public:
    virtual ~ConstantDefinition();

    /** @brief The name of the constant */
    std::string Name;

    /** @brief The value type of the constant */
    RR_SHARED_PTR<TypeDefinition> Type;

    /** @brief The value of the constant, as a string */
    std::string Value;

    /** @brief Documentation string for the constant */
    std::string DocString;

    /**
     * @brief The parent service definition
     *
     * service and service_entry are mutually exclusive
     */
    RR_WEAK_PTR<ServiceDefinition> service;
    /**
     * @brief The parent service entry
     *
     * service and service_entry are mutually exclusive
     */
    RR_WEAK_PTR<ServiceEntryDefinition> service_entry;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by FromString()
     */
    ServiceDefinitionParseInfo ParseInfo;

    /**
     * @brief Construct a new ConstantDefinition with a parent ServiceDefinition
     *
     * Must be constructed with boost::make_shared<ConstantDefinition>()
     *
     * @param service The parent service definition
     */
    ConstantDefinition(RR_SHARED_PTR<ServiceDefinition> service);

    /**
     * @brief Construct a new ConstantDefinition with a parent ServiceEntryDefinition
     *
     * Must be constructed with boost::make_shared<ConstantDefinition>()
     *
     * @param service_entry The parent service entry definition
     */
    ConstantDefinition(RR_SHARED_PTR<ServiceEntryDefinition> service_entry);

    /**
     * @brief Convert constant definition to string
     *
     * @return std::string The constant definition string
     */
    std::string ToString();

    /**
     * @brief Parse a constant definition from string
     *
     * @param s The constant definition string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Clear all fields
     *
     */
    void Reset();

    /**
     * @brief Verify that the specified constant type and value are valid
     *
     * @param t The type of the constant
     * @param value The value of the constant
     * @return true The type and value are valid
     * @return false The type and/or value are invalid
     */
    static bool VerifyTypeAndValue(TypeDefinition& t, boost::string_ref value);

    /**
     * @brief Verify that this constant definition has a valid value
     *
     * @return true The constant definition value is valid
     * @return false The constant definition value is invalid
     */
    bool VerifyValue();

    /**
     * @brief Convert the constant value to a numeric scalar
     *
     * @tparam T The numeric scalar type
     * @return T The numeric scalar
     */
    template <typename T>
    T ValueToScalar()
    {
        T v;
        if (!detail::try_convert_string_to_number(boost::trim_copy(Value), v))
        {
            throw ServiceDefinitionParseException("Invalid constant", ParseInfo);
        }
        return v;
    }

    /**
     * @brief Convert the constant scalar to a numeric primitive array
     *
     * @tparam T The numeric primitive type
     * @return RR_INTRUSIVE_PTR<RRArray<T> > The numeric primitive array
     */
    template <typename T>
    RR_INTRUSIVE_PTR<RRArray<T> > ValueToArray()
    {
        boost::iterator_range<std::string::const_iterator> value1(Value);
        value1 = boost::trim_copy_if(value1, boost::is_any_of(" \t{}"));
        value1 = boost::trim_copy(value1);
        if (value1.empty())
            return AllocateRRArray<T>(0);

        size_t n = 0;

        // Count number of elements
        typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
        for (string_split_iterator e = boost::make_split_iterator(
                 value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
             e != string_split_iterator(); e++)
        {
            n++;
        }

        RR_INTRUSIVE_PTR<RRArray<T> > o = AllocateRRArray<T>(n);

        size_t i = 0;

        // Read elements
        typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
        for (string_split_iterator e = boost::make_split_iterator(
                 value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
             e != string_split_iterator(); e++)
        {
            if (!detail::try_convert_string_to_number(boost::trim_copy(*e), (*o)[i]))
            {
                throw ServiceDefinitionParseException("Invalid constant", ParseInfo);
            }
            i++;
        }

        if (i != n)
            throw InternalErrorException("Internal error");
        return o;
    }

    /**
     * @brief Convert the constant value to string
     *
     * @return std::string The constant value as a string
     */
    std::string ValueToString();

    /**
     * @brief Convert the constant value to structure fields
     *
     * @return std::vector<ConstantDefinition_StructField> The constant as structure fields
     */
    std::vector<ConstantDefinition_StructField> ValueToStructFields();

    static std::string UnescapeString(boost::string_ref in);
    static std::string EscapeString(boost::string_ref in);
};

/**
 * @brief Class for enum definitions
 *
 * See \ref service_definitions.md for more information.
 */
class ROBOTRACONTEUR_CORE_API EnumDefinition : public NamedTypeDefinition
{
  public:
    virtual ~EnumDefinition();

    /** @brief The values in the enum */
    std::vector<EnumDefinitionValue> Values;

    /** @brief The parent service definition */
    RR_WEAK_PTR<ServiceDefinition> service;

    /**
     * @brief Parsing diagnostic information
     *
     * The ParseInfo field is populated by FromString()
     */
    ServiceDefinitionParseInfo ParseInfo;

    /** @brief Documentation string for the enum */
    std::string DocString;

    /**
     * @brief Construct a new empty EnumDefinition with a parent ServiceDefinition
     *
     * Must be constructed with boost::make_shared<EnumDefinition>()
     *
     * @param service The parent service definition
     */
    EnumDefinition(RR_SHARED_PTR<ServiceDefinition> service);

    /**
     * @brief Convert enum definition to string
     *
     * @return std::string The enum definition string
     */
    std::string ToString();

    /**
     * @brief Parse an enum definition from string
     *
     * @param s The enum definition string
     * @param parse_info Parsing diagnostic information
     */
    void FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info = NULL);

    /**
     * @brief Verify that enum values are valid
     *
     * @return true Enum values are valid
     * @return false Enum values are invalid
     */
    bool VerifyValues();

    /**
     * @brief Clear all fields
     *
     */
    void Reset();

    virtual DataTypes RRDataType();
    virtual std::string ResolveQualifiedName();
};

/**
 * @brief Class representing an enum definition value
 *
 * See EnumDefinition
 */
class ROBOTRACONTEUR_CORE_API EnumDefinitionValue
{
  public:
    /**
     * @brief Construct a new EnumDefinitionValue
     *
     * Must *not* be constructed as a shared pointer
     *
     */
    EnumDefinitionValue();

    /** @brief The name of the enum value */
    std::string Name;
    /** @brief The numeric value of the enum value */
    int32_t Value;
    /** @brief If true, the enum value is implicitly incremented by 1 from the previous value */
    bool ImplicitValue;
    /** @brief Value is represented using a hex literal */
    bool HexValue;

    /** @brief Documentations tring for the enum value */
    std::string DocString;
};

/**
 * @brief Verify that service definitions are valid
 *
 * Verify that service definition follows all the rules for service definitions as specified by the standard.
 * See \ref service_definitions.md for more information. Throws exceptions on verification failure.
 *
 * ServiceDefinition::FromString() or ServiceDefinition::FromStream() does not validate service
 * definitions. VerifyServiceDefinition() must be called after parsing.
 *
 * @param def The collection of service definitions to verify
 * @param warnings A vector to receive non-fatal verification warnings
 */
ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def,
                                                      std::vector<ServiceDefinitionParseException>& warnings);

/**
 * @brief Verify that service definitions are valid
 *
 * Verify that service definition follows all the rules for service definitions as specified by the standard.
 * See \ref service_definitions.md for more information. Throws exceptions on verification failure.
 *
 * ServiceDefinition::FromString() or ServiceDefinition::FromStream() does not validate service
 * definitions. VerifyServiceDefinition() must be called after parsing.
 *
 * This overload does not return warnings.
 *
 * @param def The collection of service definitions to verify
 */
ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def);

/**
 * @brief Compare two service definitions to see if they are identical
 *
 * @param def1 Test service definition 1
 * @param def2 Test service definition 2
 * @return true Service definitions are identical
 * @return false Service definitions are different
 */
ROBOTRACONTEUR_CORE_API bool CompareServiceDefinitions(RR_SHARED_PTR<ServiceDefinition> def1,
                                                       RR_SHARED_PTR<ServiceDefinition> def2);

/**
 * @brief Split a qualified name into its service definition name and unqualified name parts
 *
 * Note that this function returns a *reference* to the name parameter. If the memory backing name
 * is destroyed, the returned boost::string_ref tuple will be invalid.
 *
 * @param name
 * @return  boost::tuple<boost::string_ref, boost::string_ref> The service type name and unqualified name in a tuple
 */
ROBOTRACONTEUR_CORE_API boost::tuple<boost::string_ref, boost::string_ref> SplitQualifiedName(boost::string_ref name);

/**
 * @brief Try finding a vector element by the Name field
 *
 * @tparam T The type of the vector field (contained in a boost::shared_ptr)
 * @param v The vector of elements
 * @param name The name of the element to search for
 * @return RR_SHARED_PTR<T> The found element, or null if not found
 */
template <typename T>
RR_SHARED_PTR<T> TryFindByName(std::vector<RR_SHARED_PTR<T> >& v, boost::string_ref name)
{
    for (typename std::vector<RR_SHARED_PTR<T> >::iterator e = v.begin(); e != v.end(); ++e)
    {
        if (*e)
        {
            if ((*e)->Name == name)
            {
                return *e;
            }
        }
    }

    return RR_SHARED_PTR<T>();
}

ROBOTRACONTEUR_CORE_API size_t EstimatePodPackedElementSize(
    RR_SHARED_PTR<ServiceEntryDefinition> def,
    std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs = std::vector<RR_SHARED_PTR<ServiceDefinition> >(),
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>(),
    RR_SHARED_PTR<RRObject> client = RR_SHARED_PTR<RRObject>());

ROBOTRACONTEUR_CORE_API boost::tuple<DataTypes, size_t> GetNamedArrayElementTypeAndCount(
    RR_SHARED_PTR<ServiceEntryDefinition> def,
    std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs = std::vector<RR_SHARED_PTR<ServiceDefinition> >(),
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>(),
    RR_SHARED_PTR<RRObject> client = RR_SHARED_PTR<RRObject>());

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for ServiceDefinition shared_ptr */
using ServiceDefinitionPtr = RR_SHARED_PTR<ServiceDefinition>;
/** @brief Convenience alias for NamedTypeDefinition shared_ptr */
using NamedTypeDefinitionPtr = RR_SHARED_PTR<NamedTypeDefinition>;
/** @brief Convenience alias for ServiceEntryDefinition shared_ptr */
using ServiceEntryDefinitionPtr = RR_SHARED_PTR<ServiceEntryDefinition>;
/** @brief Convenience alias for memberDefinition shared_ptr */
using MemberDefinitionPtr = RR_SHARED_PTR<MemberDefinition>;
/** @brief Convenience alias for FunctionDefinition shared_ptr */
using FunctionDefinitionPtr = RR_SHARED_PTR<FunctionDefinition>;
/** @brief Convenience alias for PropertyDefinition shared_ptr */
using PropertyDefinitionPtr = RR_SHARED_PTR<PropertyDefinition>;
/** @brief Convenience alias for EventDefinition shared_ptr */
using EventDefinitionPtr = RR_SHARED_PTR<EventDefinition>;
/** @brief Convenience alias for ObjRefDefinition shared_ptr */
using ObjRefDefinitionPtr = RR_SHARED_PTR<ObjRefDefinition>;
/** @brief Convenience alias for PipeDefinition shared_ptr */
using PipeDefinitionPtr = RR_SHARED_PTR<PipeDefinition>;
/** @brief Convenience alias for CallbackDefinition shared_ptr */
using CallbackDefinitionPtr = RR_SHARED_PTR<CallbackDefinition>;
/** @brief Convenience alias for WireDefinition shared_ptr */
using WireDefinitionPtr = RR_SHARED_PTR<WireDefinition>;
/** @brief Convenience alias for MemoryDefinition shared_ptr */
using MemoryDefinitionPtr = RR_SHARED_PTR<MemoryDefinition>;
/** @brief Convenience alias for TypeDefinition shared_ptr */
using TypeDefinitionPtr = RR_SHARED_PTR<TypeDefinition>;
/** @brief Convenience alias for UsingDefinition shared_ptr */
using UsingDefinitionPtr = RR_SHARED_PTR<UsingDefinition>;
/** @brief Convenience alias for EnumDefinition shared_ptr */
using EnumDefinitionPtr = RR_SHARED_PTR<EnumDefinition>;
/** @brief Convenience alias for ConstantDefinition shared_ptr */
using ConstantDefinitionPtr = RR_SHARED_PTR<ConstantDefinition>;
#endif

} // namespace RobotRaconteur
