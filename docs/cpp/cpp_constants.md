# C++ Constants {#cpp_constants}

Service definitions may include constants and/or enums. The constants may be numbers, numeric arrays, or strings. Constants may exist at the service definition scope, within objects, or within structures. See \ref constants for more information. Enums exist as the service definition level. See \ref enums for more information.

An example of a service definition containing constants and enums:

    service example.constants_example1

    constant uint32 myconst 0xFB
    constant double[] myarray {10.3, 584.9, 594}
    constant string mystring "Hello world!"

    enum myenum
        value1 = -1,
        value2 = 0xF1,
        value3,
        value4
    end

The \ref robotraconteurgen is used to generate thunk source for the service definition, using the utility directly, or using the CMake macro. See \ref cpp_intro_cmake_thunk_source_gen for more information. The thunk source places constants in a special constants namespace for the service definition. This namespace is within the namespace of the service definition, and has the fully qualified name with dots replaced with two underscores, with `Constants` prefixed. For instance, the service definition above with name `experimental.constants_example1` will place the constants in the namespace `example::constants_example1::example__constants_example1Constants`. Enums are placed in a namespace to prevent values from leaking into the service definition namespace.

The above example will generate the following code in the thunk source:

    namespace example
    {
    namespace constants_example1
    {
    
    namespace example__constants_example1Constants 
    {
        static const uint32_t myconst=0xFB;
        static const double myarray[]={10.3, 584.9, 594};
        static const char* mystring="Hello world!";
    }

    namespace myenum
    {
    enum myenum
    {
        value1 = -1,
        value2 = 0xf1,
        value3 = 0xf2,
        value4 = 0xf3
    };    
    }

    }
    }