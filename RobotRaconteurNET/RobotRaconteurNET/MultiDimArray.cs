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

using System;
using System.Collections.Generic;

namespace RobotRaconteur
{

/// <summary>
/// Numeric primitive multidimensional array value type
/// </summary>
/// <remarks>
/// This class stores a numeric primitive multidimensional arrays.
/// Multidimensional arrays are stored as a uint array of
/// dimensions, and an array of the flattened elements.
/// Arrays are stored in column major, or "Fortran" order.
///
/// Valid types for array are `bool`, `double`, `float`, `sbyte`, `byte`, `short`,
/// `ushort`, `int`, `uint`, `long`, `ulong`, `CDouble`,
/// or `CSingle`. Attempts to use any other types will result in a compiler error.
/// </remarks>
public class MultiDimArray
{

    /// <summary>
    /// Construct empty MultiDimArray
    /// </summary>
    /// <remarks>None</remarks>
    public MultiDimArray()
    {}

    /// <summary>
    /// Construct MultiDimArray with dims and array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="Dims">The dimensions of the array</param>
    /// <param name="Array_">The array data in fortran order</param>
    public MultiDimArray(uint[] Dims, Array Array_)
    {

        this.Dims = Dims;
        this.Array_ = Array_;
    }

    /// <summary>
    /// The dimensions of the array
    /// </summary>
    /// <remarks>None</remarks>
    public uint[] Dims;
    /// <summary>
    /// The data of the array in flattened "Fortran" order
    /// </summary>
    /// <remarks>None</remarks>
    public Array Array_;

    /// <summary>
    /// Retrieve a subset of an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memorypos">Position in array to read</param>
    /// <param name="buffer">Buffer to store retrieved data</param>
    /// <param name="bufferpos">Position within buffer to store data</param>
    /// <param name="count">Count of data to retrieve</param>
    public virtual void RetrieveSubArray(uint[] memorypos, MultiDimArray buffer, uint[] bufferpos, uint[] count)
    {

        MultiDimArray mema = this;
        MultiDimArray memb = buffer;

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(
            new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims),
            new vectoruint32(bufferpos), new vectoruint32(count));

        uint indexa;
        uint indexb;
        uint len;

        while (iter.Next(out indexa, out indexb, out len))
        {
            Array.Copy(mema.Array_, (long)indexa, memb.Array_, (long)indexb, (long)len);
        }
    }

    /// <summary>
    /// Assign a subset of an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memorypos">Position within array to store data</param>
    /// <param name="buffer">Buffer to assign data from</param>
    /// <param name="bufferpos">Position within buffer to assign from</param>
    /// <param name="count">Count of data to assign</param>
    public virtual void AssignSubArray(uint[] memorypos, MultiDimArray buffer, uint[] bufferpos, uint[] count)
    {

        MultiDimArray mema = this;
        MultiDimArray memb = buffer;

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(
            new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims),
            new vectoruint32(bufferpos), new vectoruint32(count));

        uint indexa;
        uint indexb;
        uint len;

        while (iter.Next(out indexa, out indexb, out len))
        {
            Array.Copy(memb.Array_, (long)indexb, mema.Array_, (long)indexa, (long)len);
        }
    }
}
/// <summary>
/// `pod` multidimensional array value type
/// </summary>
/// <remarks>
/// This class stores a pod multidimensional array.
/// Multidimensional arrays are stored as a uint32_t array of
/// dimensions, and an array of the flattened elements.
/// Arrays are stored in column major, or "Fortran" order.
///
/// Stored type must be a od type that has been generated as part
/// of the thunk source.
/// </remarks>
public class PodMultiDimArray
{
    /// <summary>
    /// Construct empty PodMultiDimArray
    /// </summary>
    /// <remarks>None</remarks>
    public PodMultiDimArray()
    {
        Dims = new uint[] { 0 };
    }

    /// <summary>
    /// Construct PodMultiDimArray with dims and array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="dims">The dimensions of the array</param>
    /// <param name="array">The array data in fortran order</param>
    public PodMultiDimArray(uint[] dims, Array array)
    {
        Dims = dims;
        pod_array = array;
    }

    /// <summary>
    /// The dimensions of the array
    /// </summary>
    /// <remarks>None</remarks>
    public uint[] Dims;
    /// <summary>
    /// The data of the array in flattened "Fortran" order
    /// </summary>
    /// <remarks>None</remarks>
    public Array pod_array;

    /// <summary>
    /// Retrieve a subset of an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memorypos">Position in array to read</param>
    /// <param name="buffer">Buffer to store retrieved data</param>
    /// <param name="bufferpos">Position within buffer to store data</param>
    /// <param name="count">Count of data to retrieve</param>
    public virtual void RetrieveSubArray(uint[] memorypos, PodMultiDimArray buffer, uint[] bufferpos, uint[] count)
    {

        PodMultiDimArray mema = this;
        PodMultiDimArray memb = buffer;

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(
            new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims),
            new vectoruint32(bufferpos), new vectoruint32(count));

        uint indexa;
        uint indexb;
        uint len;

        while (iter.Next(out indexa, out indexb, out len))
        {
            Array.Copy(mema.pod_array, (long)indexa, memb.pod_array, (long)indexb, (long)len);
        }
    }

    /// <summary>
    /// Assign a subset of an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memorypos">Position within array to store data</param>
    /// <param name="buffer">Buffer to assign data from</param>
    /// <param name="bufferpos">Position within buffer to assign from</param>
    /// <param name="count">Count of data to assign</param>
    public virtual void AssignSubArray(uint[] memorypos, PodMultiDimArray buffer, uint[] bufferpos, uint[] count)
    {
        PodMultiDimArray mema = this;
        PodMultiDimArray memb = buffer;

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(
            new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims),
            new vectoruint32(bufferpos), new vectoruint32(count));

        uint indexa;
        uint indexb;
        uint len;

        while (iter.Next(out indexa, out indexb, out len))
        {
            Array.Copy(memb.pod_array, (long)indexb, mema.pod_array, (long)indexa, (long)len);
        }
    }
}

/// <summary>
/// `namedarray` multidimensional array value type
/// </summary>
/// <remarks>
/// This class stores a namedarray multidimensional array.
/// Multidimensional arrays are stored as a uint32_t array of
/// dimensions, and an array of the flattened elements.
/// Arrays are stored in column major, or "Fortran" order.
/// </remarks>
public class NamedMultiDimArray
{
    /// <summary>
    /// Construct empty NamedMultiDimArray
    /// </summary>
    /// <remarks>None</remarks>
    public NamedMultiDimArray()
    {
        Dims = new uint[] { 0 };
    }

    /// <summary>
    /// Construct NamedMultiDimArray with dims and array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="dims">The dimensions of the array</param>
    /// <param name="array">The array data in fortran order</param>
    public NamedMultiDimArray(uint[] dims, Array array)
    {
        Dims = dims;
        namedarray_array = array;
    }

    /// <summary>
    /// The dimensions of the array
    /// </summary>
    /// <remarks>None</remarks>
    public uint[] Dims;
    /// <summary>
    /// The data of the array in flattened "Fortran" order
    /// </summary>
    /// <remarks>None</remarks>
    public Array namedarray_array;
    /// <summary>
    /// Retrieve a subset of an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memorypos">Position in array to read</param>
    /// <param name="buffer">Buffer to store retrieved data</param>
    /// <param name="bufferpos">Position within buffer to store data</param>
    /// <param name="count">Count of data to retrieve</param>
    public virtual void RetrieveSubArray(uint[] memorypos, NamedMultiDimArray buffer, uint[] bufferpos, uint[] count)
    {

        NamedMultiDimArray mema = this;
        NamedMultiDimArray memb = buffer;

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(
            new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims),
            new vectoruint32(bufferpos), new vectoruint32(count));

        uint indexa;
        uint indexb;
        uint len;

        while (iter.Next(out indexa, out indexb, out len))
        {
            Array.Copy(mema.namedarray_array, (long)indexa, memb.namedarray_array, (long)indexb, (long)len);
        }
    }

    /// <summary>
    /// Assign a subset of an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memorypos">Position within array to store data</param>
    /// <param name="buffer">Buffer to assign data from</param>
    /// <param name="bufferpos">Position within buffer to assign from</param>
    /// <param name="count">Count of data to assign</param>
    public virtual void AssignSubArray(uint[] memorypos, NamedMultiDimArray buffer, uint[] bufferpos, uint[] count)
    {
        NamedMultiDimArray mema = this;
        NamedMultiDimArray memb = buffer;

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(
            new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims),
            new vectoruint32(bufferpos), new vectoruint32(count));

        uint indexa;
        uint indexb;
        uint len;

        while (iter.Next(out indexa, out indexb, out len))
        {
            Array.Copy(memb.namedarray_array, (long)indexb, mema.namedarray_array, (long)indexa, (long)len);
        }
    }
}
}
