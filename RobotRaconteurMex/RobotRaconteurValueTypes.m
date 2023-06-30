%RobotRaconteurValueTypes Overview of Robot Raconteur Value Types in MATLAB
%
%    Each valid Robot Raconteur type has a corresponding MATLAB data type.  
%    Scalars, strings, numeric arrays, structures, and multi-dimensional 
%    arrays map directly to MATLAB data types.  (Multi-dimensional arrays 
%    do not have special data types in MATLAB due to the native support 
%    within MATLAB.) The following table shows how data types map between
%    Robot Raconteur and MATLAB:
%
%    | Robot Raconteur | MATLAB               | Notes
%    | ----------------|----------------------|------------
%    | double          | double               |
%    | single          | single               |
%    | int8            | int8                 |
%    | uint8           | uint8                |
%    | int16           | int16                |
%    | uint16          | uint16               |
%    | int32           | int32                |
%    | uint32          | uint32               |
%    | int64           | int64                |
%    | uint64          | uint64               |
%    | int8            | int8                 |
%    | string          | char[]               | Old-style MATLAB strings
%    | T{int32}        | containers.Map       | See below
%    | T{string}       | containers.Map       | See below
%    | T{list}         | cell column array    | Cell vector containing the entries of the list
%    | structure       | struct               | Standard MATLAB structure
%    | N[*]            | N                    | Normal MATLAB array of type N for multidimarrays
%    | pod             | struct               | Standard matlab structure with optional array shape
%    | namedarray      | N[]                  | Namedarrays map to base numeric type with ndims+1 dimensions
%    | varvalue        | *                    | RobotRaconteurVarValue
%    | varobject       | RobotRaconteurObject |
%
%    In MATLAB, the default is for numbers to be type double. When passing
%    numeric arguments to Robot Raconteur it is necessary to match the
%    expected type. THis is easily done using the type name as a function,
%    for example:
%
%      bytevalue=uint8([10;20]);
%
%    All arrays are column vectors. Multi-dimensional arrays behave like
%    normal arrays.
%
%    Maps in MATLAB use the built in containers.Map type. The key must be
%    type char or int32. As an example, map of type double[]{int32}:
%
%      mymap=containers.Map(int32({1;2}), {[10; 20]; [30; 40]})
%
%    Robot Raconteur structures may directly to MATLAB structures without
%    any extra work. The structures must have matching fields and types. A
%    structure can be created dynamically. For instance, to create a
%    SensorPacket structure as defined in experimental.create2:
%
%      s = struct;
%      s.ID=uint8(19)
%      s.Data=uint8([1; 2; 3])
%
%    s can now be passed where SensorPacket is expected.
%
%    Pods behave exactly like structures, except they may also be indexed, 
%    if the pod type is either an array or a multi-dimensional array.
%
%    Namedarrays are mapped to their base numeric type and passed as an
%    array with one additional dimension. This additional dimension 
%    represents the named portion of the array, but in a flattened form.
%
%    null values are represented using `missing` in MATLAB.
%
%    varvalue uses the RobotRaconteurVarValue type to represent a
%    dynamically typed value. The type can be determined using the
%    `datatype` property. The value can be accessed using the `data` 
%    property. The datatypes will be in the same format used
%    for Robot Raconteur service definition types. RobotRaconteurVarValue
%    can be constructed passing data and datatype as arguments.
