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
using System.Linq;

namespace RobotRaconteur
{
/// <summary>
/// Single dimensional numeric primitive random access memory region
/// </summary>
/// <remarks>
/// <para>
/// Memories represent random access memory regions that are typically
/// represented as arrays of various shapes and types. Memories can be
/// declared in service definition files using the `memory` member keyword
/// within service definitions. Services expose memories to clients, and
/// the nodes will proxy read, write, and parameter requests between the client
/// and service. The node will also break up large requests to avoid the
/// message size limit of the transport.
/// </para>
/// <para>
/// The ArrayMemory class is used to represent a single dimensional numeric
/// primitive array. Multidimensional numeric primitive arrays should use
/// MultiDimArrayMemory. Valid types for T are `double`, `float`, `sbyte`,
/// `byte`, `short`, `ushort`, `uint`, `uint`, `long`,
/// `ulong`, `bool`, `CDouble`, and `CSingle`.
/// </para>
/// <para>
/// ArrayMemory instances are attached to an RRArray, either when
/// constructed or later using Attach().
/// </para>
/// <para>
/// ArrayMemory instances returned by clients are special implementations
/// designed to proxy requests to the service. They cannot be attached
/// to an arbitrary array.
/// </para>
/// </remarks>
/// <typeparam name="T"></typeparam>
public class ArrayMemory<T>
{
    private T[] memory;
    /// <summary>
    /// Construct a new ArrayMemory instance
    /// </summary>
    /// <remarks>
    /// New instance will not be attached to an array.
    /// </remarks>
    public ArrayMemory()
    {}

    /// <summary>
    /// Construct a new ArrayMemory instance attached to an array
    /// </summary>
    /// <remarks>
    /// New instance will be constructed attached to an array.
    /// </remarks>
    /// <param name="memory">The array to attach</param>
    public ArrayMemory(T[] memory)
    {
        this.memory = memory;
    }

    /// <summary>
    /// Attach ArrayMemory instance to an array
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memory">The array to attach</param>
    public virtual void Attach(T[] memory)
    {
        this.memory = memory;
    }

    /// <summary>
    /// Return the length of the array memory
    /// </summary>
    /// <remarks>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </remarks>
    public virtual ulong Length
    {
        get {
            return (ulong)memory.LongLength;
        }
    }

    /// <summary>
    /// Read a segment from an array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Read a segment of an array memory into a supplied buffer array. The start positions and length
    /// of the read are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start index in the memory array to read</param>
    /// <param name="buffer">The buffer to receive the read data</param>
    /// <param name="bufferpos">The start index in the buffer to write the data</param>
    /// <param name="count">The number of array elements to read</param>
    public virtual void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {

        Array.Copy(memory, (long)memorypos, buffer, (long)bufferpos, (long)count);
    }

    /// <summary>
    /// Write a segment to an array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Writes a segment to an array memory from a supplied buffer array. The start positions and length
    /// of the write are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start index in the memory array to write</param>
    /// <param name="buffer">The buffer to write the data from</param>
    /// <param name="bufferpos">The start index in the buffer to read the data</param>
    /// <param name="count">The number of array elements to write</param>
    public virtual void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {

        Array.Copy(buffer, (long)bufferpos, memory, (long)memorypos, (long)count);
    }
}

/// <summary>
/// Single dimensional pod random access memory region
/// </summary>
/// <remarks>
/// <para>
/// Memories represent random access memory regions that are typically
/// represented as arrays of various shapes and types. Memories can be
/// declared in service definition files using the `memory` member keyword
/// within service definitions. Services expose memories to clients, and
/// the nodes will proxy read, write, and parameter requests between the client
/// and service. The node will also break up large requests to avoid the
/// message size limit of the transport.
/// </para>
/// <para>
/// The PodArrayMemory class is used to represent a single dimensional pod
/// array. Multidimensional pod arrays should use PodMultiDimArrayMemory.
///  Type T must be declared in a service definition using the `pod`
/// keyword, and generated using RobotRaconteurGen.
/// </para>
/// <para>
/// PodArrayMemory instances are attached to an array, either when
/// constructed or later using Attach().
/// </para>
/// <para>
/// PodArrayMemory instances returned by clients are special implementations
/// designed to proxy requests to the service. They cannot be attached
/// to an arbitrary array.
/// </para>
/// </remarks>
/// <typeparam name="T">The pod type of the array</typeparam>
public class PodArrayMemory<T> : ArrayMemory<T>
    where T : struct
{

    /// <summary>
    /// Construct a new PodArrayMemory instance
    /// </summary>
    /// <remarks>
    /// New instance will not be attached to an array.
    /// </remarks>
    public PodArrayMemory() : base()
    {}

    /// <summary>
    /// Construct a new PodArrayMemory instance attached to an array
    /// </summary>
    /// <remarks>
    /// New instance will be constructed attached to an array.
    /// </remarks>
    /// <param name="memory">The array to attach</param>
    public PodArrayMemory(T[] memory) : base(memory)
    {}
}

/// <summary>
/// Multidimensional numeric primitive random access memory region
/// </summary>
/// <remarks>
/// Memories represent random access memory regions that are typically
/// represented as arrays of various shapes and types. Memories can be
/// declared in service definition files using the `memory` member keyword
/// within service definitions. Services expose memories to clients, and
/// the nodes will proxy read, write, and parameter requests between the client
/// and service. The node will also break up large requests to avoid the
/// message size limit of the transport.
///
/// The MultiDimArrayMemory class is used to represent a multidimensional numeric
/// primitive array. Single dimensional numeric primitive arrays should use
/// ArrayMemory. Valid types for T are `double`, `float`, `sbyte`,
/// `byte`, `short`, `ushort`, `int`, `uint`, `long`,
/// `ulong`, `bool`, `CDouble`, and `CSingle`.
///
/// MultiDimArrayMemory instances are attached to an MultiDimArray,
/// either when constructed or later using Attach().
///
/// MultiDimArrayMemory instances returned by clients are special implementations
/// designed to proxy requests to the service. They cannot be attached
/// to an arbitrary array.
/// </remarks>
/// <typeparam name="T">The numeric primitive type of the array</typeparam>
public class MultiDimArrayMemory<T>
{

    private MultiDimArray multimemory;

    /// <summary>
    /// Construct a new MultiDimArrayMemory instance
    /// </summary>
    /// <remarks>
    /// New instance will not be attached to an array.
    /// </remarks>
    public MultiDimArrayMemory()
    {}

    /// <summary>
    /// Construct a new MultiDimArrayMemory instance attached to a MultiDimArray
    /// </summary>
    /// <remarks>
    /// New instance will be constructed attached to an array.
    /// </remarks>
    /// <param name="memory">The array to attach</param>
    public MultiDimArrayMemory(MultiDimArray memory)
    {

        multimemory = memory;
    }

    /// <summary>
    /// Attach MultiDimArrayMemory instance to a MultiDimArray
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memory">The array to attach</param>
    public virtual void Attach(MultiDimArray memory)
    {

        this.multimemory = memory;
    }

    /// <summary>
    /// Dimensions of the memory array
    /// </summary>
    /// <remarks>
    /// <para>
    /// Returns the dimensions (shape) of the memory array
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </para>
    /// </remarks>
    public virtual ulong[] Dimensions
    {
        get {
            return multimemory.Dims.Select(x => (ulong)x).ToArray();
        }
    }

    /// <summary>
    /// The number of dimensions in the memory array
    /// </summary>
    /// <remarks>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </remarks>
    public virtual ulong DimCount
    {
        get {
            return (ulong)multimemory.Dims.Length;
        }
    }

    /// <summary>
    /// Read a block from a multidimensional array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Read a block of a multidimensional array memory into a supplied buffer multidimensional array.
    /// The start positions and count of the read are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start position in the memory array to read</param>
    /// <param name="buffer">The buffer to receive the read data</param>
    /// <param name="bufferpos">The start position in the buffer to write the data</param>
    /// <param name="count">The count of array elements to read</param>
    public virtual void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {

        multimemory.RetrieveSubArray(memorypos.Select(x => (uint)x).ToArray(), buffer,
                                     bufferpos.Select(x => (uint)x).ToArray(), count.Select(x => (uint)x).ToArray());
    }

    /// <summary>
    /// Write a segment to a multidimensional array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Writes a segment to a multidimensional array memory from a supplied buffer
    /// multidimensional array. The start positions and count
    /// of the write are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start position in the memory array to write</param>
    /// <param name="buffer">The buffer to write the data from</param>
    /// <param name="bufferpos">The start position in the buffer to read the data</param>
    /// <param name="count">The count of array elements to write</param>
    public virtual void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {

        multimemory.AssignSubArray(memorypos.Select(x => (uint)x).ToArray(), buffer,
                                   bufferpos.Select(x => (uint)x).ToArray(), count.Select(x => (uint)x).ToArray());
    }
}

/// <summary>
/// Multidimensional pod random access memory region
/// </summary>
/// <remarks>
/// <para>
/// Memories represent random access memory regions that are typically
/// represented as arrays of various shapes and types. Memories can be
/// declared in service definition files using the `memory` member keyword
/// within service definitions. Services expose memories to clients, and
/// the nodes will proxy read, write, and parameter requests between the client
/// and service. The node will also break up large requests to avoid the
/// message size limit of the transport.
/// </para>
/// <para>
/// The PodMultiDimArrayMemory class is used to represent a multidimensional
/// pod array. Single dimensional pod arrays should use PodArrayMemory.
/// Type T must be declared in a service definition using the `pod`
/// keyword, and generated using RobotRaconteurGen.
/// </para>
/// <para>
/// PodMultiDimArrayMemory instances are attached to an MultiDimArray,
/// either when constructed or later using Attach().
/// </para>
/// <para>
/// PodMultiDimArrayMemory instances returned by clients are special implementations
/// designed to proxy requests to the service. They cannot be attached
/// to an arbitrary array.
/// </para>
/// </remarks>
/// <typeparam name="T"></typeparam>
public class PodMultiDimArrayMemory<T>
    where T : struct
{

    private PodMultiDimArray multimemory;

    /// <summary>
    /// Construct a new PodMultiDimArrayMemory instance
    /// </summary>
    /// <remarks>
    /// New instance will not be attached to an array.
    /// </remarks>
    public PodMultiDimArrayMemory()
    {}

    /// <summary>
    /// Construct a new PodMultiDimArrayMemory instance attached to a PodMultiDimArray
    /// </summary>
    /// <remarks>
    /// New instance will be constructed attached to an array.
    /// </remarks>
    /// <param name="memory">The array to attach</param>
    public PodMultiDimArrayMemory(PodMultiDimArray memory)
    {

        multimemory = memory;
    }

    /// <summary>
    /// Attach PodMultiDimArrayMemory instance to a PodMultiDimArray
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memory">The array to attach</param>
    public virtual void Attach(PodMultiDimArray memory)
    {

        this.multimemory = memory;
    }

    /// <summary>
    /// Dimensions of the memory array
    /// </summary>
    /// <remarks>
    /// <para>
    /// Returns the dimensions (shape) of the memory array
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </para>
    /// </remarks>
    public virtual ulong[] Dimensions
    {
        get {
            return multimemory.Dims.Select(x => (ulong)x).ToArray();
        }
    }

    /// <summary>
    /// The number of dimensions in the memory array
    /// </summary>
    /// <remarks>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </remarks>
    public virtual ulong DimCount
    {
        get {
            return (ulong)multimemory.Dims.Length;
        }
    }

    /// <summary>
    /// Read a block from a multidimensional array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Read a block of a multidimensional array memory into a supplied buffer multidimensional array.
    /// The start positions and count of the read are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start position in the memory array to read</param>
    /// <param name="buffer">The buffer to receive the read data</param>
    /// <param name="bufferpos">The start position in the buffer to write the data</param>
    /// <param name="count">The count of array elements to read</param>
    public virtual void Read(ulong[] memorypos, PodMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {

        multimemory.RetrieveSubArray(memorypos.Select(x => (uint)x).ToArray(), buffer,
                                     bufferpos.Select(x => (uint)x).ToArray(), count.Select(x => (uint)x).ToArray());
    }

    /// <summary>
    /// Write a segment to a multidimensional array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Writes a segment to a multidimensional array memory from a supplied buffer
    /// multidimensional array. The start positions and count
    /// of the write are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start position in the memory array to write</param>
    /// <param name="buffer">The buffer to write the data from</param>
    /// <param name="bufferpos">The start position in the buffer to read the data</param>
    /// <param name="count">The count of array elements to write</param>
    public virtual void Write(ulong[] memorypos, PodMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {

        multimemory.AssignSubArray(memorypos.Select(x => (uint)x).ToArray(), buffer,
                                   bufferpos.Select(x => (uint)x).ToArray(), count.Select(x => (uint)x).ToArray());
    }
}

public class ArrayMemoryClient<T> : ArrayMemory<T>
{
    ArrayMemoryBase innerarray;

    public ArrayMemoryClient(ArrayMemoryBase innerarray)
    {
        this.innerarray = innerarray;
    }

    MemberDefinition_Direction Direction
    {
        get {
            return WrappedArrayMemoryClientUtil.Direction(innerarray);
        }
    }

    public override ulong Length
    {
        get {
            return innerarray.Length();
        }
    }

    public override void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {
        RRBaseArray dat = null;
        try
        {
            dat = WrappedArrayMemoryClientUtil.Read(innerarray, memorypos, count);
            T[] dat2 = (T[])MessageElementDataUtil.RRBaseArrayToArray(dat);
            Array.Copy(dat2, 0, buffer, (long)bufferpos, (long)count);
        }
        finally
        {
            if (dat != null)
                dat.Dispose();
        }
    }

    public override void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {
        T[] buffer3;
        if ((ulong)buffer.Length == count)
        {
            buffer3 = buffer;
        }
        else
        {
            buffer3 = new T[count];
            Array.Copy(buffer, (long)bufferpos, buffer3, 0, (long)count);
        }

        RRBaseArray buffer2 = null;
        try
        {
            buffer2 = MessageElementDataUtil.ArrayToRRBaseArray(buffer3);
            WrappedArrayMemoryClientUtil.Write(innerarray, memorypos, buffer2, 0, count);
        }
        finally
        {
            if (buffer2 != null)
                buffer2.Dispose();
        }
    }
}

public class MultiDimArrayMemoryClient<T> : MultiDimArrayMemory<T>
{
    MultiDimArrayMemoryBase innermem;
    public MultiDimArrayMemoryClient(MultiDimArrayMemoryBase innermem)
    {
        this.innermem = innermem;
    }

    MemberDefinition_Direction Direction
    {
        get {
            return WrappedMultiDimArrayMemoryClientUtil.Direction(innermem);
        }
    }

    public override ulong DimCount
    {
        get {
            return innermem.DimCount();
        }
    }

    public override ulong[] Dimensions
    {
        get {
            return innermem.Dimensions().Select(x => (ulong)x).ToArray();
        }
    }

    public override void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {
        RRMultiDimArrayUntyped dat = null;
        RRBaseArray datDims = null;
        RRBaseArray datArray = null;
        try
        {
            vector_uint64_t memorypos2 = new vector_uint64_t();
            foreach (ulong val in memorypos)
                memorypos2.Add(val);
            vector_uint64_t count2 = new vector_uint64_t();
            foreach (ulong val in count)
                count2.Add(val);

            dat = WrappedMultiDimArrayMemoryClientUtil.Read(innermem, memorypos2, count2);

            datDims = dat.Dims;
            datArray = dat.Array;
            uint[] dims = (uint[])MessageElementDataUtil.RRBaseArrayToArray(datDims);
            T[] array = (T[])MessageElementDataUtil.RRBaseArrayToArray(datArray);

            MultiDimArray dat2 = new MultiDimArray(dims, array);
            buffer.AssignSubArray(bufferpos.Select(x => (uint)x).ToArray(), dat2, new uint[count.Length],
                                  count.Select(x => (uint)x).ToArray());
        }
        finally
        {
            if (dat != null)
            {
                if (datDims != null)
                    datDims.Dispose();
                if (datArray != null)
                    datArray.Dispose();
                dat.Dispose();
            }
        }
    }

    public override void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {
        RRMultiDimArrayUntyped dat2 = null;
        RRBaseArray dat2Dims = null;
        RRBaseArray dat2Array = null;
        try
        {
            int elemcount = 1;
            foreach (ulong v in count)
                elemcount *= (int)v;
            uint[] count2 = count.Select(x => (uint)x).ToArray();
            T[] array = new T[elemcount];

            MultiDimArray writedat1 = new MultiDimArray(count2, array);
            writedat1.AssignSubArray(new uint[count.Length], buffer, bufferpos.Select(x => (uint)x).ToArray(),
                                     count.Select(x => (uint)x).ToArray());

            dat2 = new RRMultiDimArrayUntyped();
            dat2Dims = MessageElementDataUtil.ArrayToRRBaseArray(count2);
            dat2.Dims = dat2Dims;
            dat2Array = MessageElementDataUtil.ArrayToRRBaseArray(array);
            dat2.Array = dat2Array;

            vector_uint64_t memorypos3 = new vector_uint64_t();
            foreach (ulong val in memorypos)
                memorypos3.Add(val);
            vector_uint64_t count3 = new vector_uint64_t();
            foreach (ulong val in count)
                count3.Add(val);
            vector_uint64_t bufferpos3 = new vector_uint64_t();
            for (int i = 0; i < count.Length; i++)
                bufferpos3.Add(0);
            WrappedMultiDimArrayMemoryClientUtil.Write(innermem, memorypos3, dat2, bufferpos3, count3);
        }
        finally
        {
            if (dat2 != null)
            {
                if (dat2Dims != null)
                    dat2Dims.Dispose();
                if (dat2Array != null)
                    dat2Array.Dispose();
                dat2.Dispose();
            }
        }
    }
}

public class PodArrayMemoryClient<T> : PodArrayMemory<T>
    where T : struct
{
    WrappedPodArrayMemoryClient innerclient;

    class bufferdirector : WrappedPodArrayMemoryClientBuffer
    {
        T[] buffer;

        public bufferdirector(T[] buffer)
        {
            this.buffer = buffer;
        }

        public override void UnpackReadResult(MessageElementNestedElementList res, ulong bufferpos, ulong count)
        {
            T[] res1 = RobotRaconteurNode.s.UnpackPodArrayDispose<T>(res);
            Array.Copy(res1, 0, buffer, (long)bufferpos, (long)count);
        }

        public override MessageElementNestedElementList PackWriteRequest(ulong bufferpos, ulong count)
        {
            T[] buffer3;
            if ((ulong)buffer.Length == count)
            {
                buffer3 = buffer;
            }
            else
            {
                buffer3 = new T[count];
                Array.Copy(buffer, (long)bufferpos, buffer3, 0, (long)count);
            }
            return RobotRaconteurNode.s.PackPodArray<T>(buffer3);
        }
    }

    public PodArrayMemoryClient(WrappedPodArrayMemoryClient innerclient)
    {
        this.innerclient = innerclient;
    }

    public override void Attach(T[] memory)
    {
        throw new InvalidOperationException("Invalid for memory client");
    }

    MemberDefinition_Direction Direction
    {
        get {
            return innerclient.Direction();
        }
    }

    public override ulong Length
    {
        get {
            return innerclient.Length();
        }
    }

    public override void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer))
        {
            innerclient.Read(memorypos, buffer1, bufferpos, count);
        }
    }

    public override void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer))
        {
            innerclient.Write(memorypos, buffer1, bufferpos, count);
        }
    }
}

public class PodMultiDimArrayMemoryClient<T> : PodMultiDimArrayMemory<T>
    where T : struct
{
    WrappedPodMultiDimArrayMemoryClient innerclient;

    class bufferdirector : WrappedPodMultiDimArrayMemoryClientBuffer
    {
        PodMultiDimArray buffer;

        public bufferdirector(PodMultiDimArray buffer)
        {
            this.buffer = buffer;
        }

        public override void UnpackReadResult(MessageElementNestedElementList res, vector_uint64_t bufferpos,
                                              vector_uint64_t count)
        {
            using (res) using (bufferpos) using (count)
            {
                PodMultiDimArray res1 = RobotRaconteurNode.s.UnpackPodMultiDimArrayDispose<T>(res);
                buffer.AssignSubArray(bufferpos.Select(x => (uint)x).ToArray(), res1, new uint[buffer.Dims.Length],
                                      count.Select(x => (uint)x).ToArray());
            }
        }

        public override MessageElementNestedElementList PackWriteRequest(vector_uint64_t bufferpos,
                                                                         vector_uint64_t count)
        {
            using (bufferpos) using (count)
            {
                PodMultiDimArray o = new PodMultiDimArray(count.Select(x => (uint)x).ToArray(),
                                                          new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                buffer.RetrieveSubArray(bufferpos.Select(x => (uint)x).ToArray(), o, new uint[buffer.Dims.Length],
                                        count.Select(x => (uint)x).ToArray());
                return RobotRaconteurNode.s.PackPodMultiDimArray<T>(o);
            }
        }
    }

    public PodMultiDimArrayMemoryClient(WrappedPodMultiDimArrayMemoryClient innerclient)
    {
        this.innerclient = innerclient;
    }

    public virtual MemberDefinition_Direction Direction
    {
        get {
            return innerclient.Direction();
        }
    }

    public override void Attach(PodMultiDimArray memory)
    {
        throw new InvalidOperationException("Invalid for memory client");
    }

    public override ulong DimCount
    {
        get {
            return innerclient.DimCount();
        }
    }

    public override ulong[] Dimensions
    {
        get {
            return innerclient.Dimensions().ToArray();
        }
    }

    public override void Read(ulong[] memorypos, PodMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer)) using (
            vector_uint64_t memorypos1 = new vector_uint64_t(
                memorypos)) using (vector_uint64_t bufferpos1 =
                                       new vector_uint64_t(bufferpos)) using (vector_uint64_t count1 =
                                                                                  new vector_uint64_t(count))
        {
            innerclient.Read(memorypos1, buffer1, bufferpos1, count1);
        }
    }

    public override void Write(ulong[] memorypos, PodMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer)) using (
            vector_uint64_t memorypos1 = new vector_uint64_t(
                memorypos)) using (vector_uint64_t bufferpos1 =
                                       new vector_uint64_t(bufferpos)) using (vector_uint64_t count1 =
                                                                                  new vector_uint64_t(count))
        {
            innerclient.Write(memorypos1, buffer1, bufferpos1, count1);
        }
    }
}

public class WrappedArrayMemoryDirectorNET<T> : WrappedArrayMemoryDirector
{
    ArrayMemory<T> mem;

    // public int memoryid = 0;

    public WrappedArrayMemoryDirectorNET(ArrayMemory<T> mem)
    {
        this.mem = mem;

        this.objectheapid = RRObjectHeap.AddObject(this);
    }

    public override ulong Length()
    {
        try
        {
            return mem.Length;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return 0;
            }
        }
    }

    public override void Read(ulong memorypos, RRBaseArray buffer, ulong bufferpos, ulong count)
    {
        try
        {
            using (buffer)
            {
                T[] buffer3 = new T[count];
                mem.Read(memorypos, buffer3, 0, count);

                MessageElementDataUtil.ArrayToRRBaseArray(buffer3, buffer);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }

    public override void Write(ulong memorypos, RRBaseArray buffer, ulong bufferpos, ulong count)
    {
        try
        {
            using (buffer)
            {

                T[] buffer2 = (T[])MessageElementDataUtil.RRBaseArrayToArray(buffer);
                mem.Write(memorypos, buffer2, bufferpos, count);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

public class WrappedMultiDimArrayMemoryDirectorNET<T> : WrappedMultiDimArrayMemoryDirector
{
    MultiDimArrayMemory<T> mem;

    // public int memoryid = 0;

    public WrappedMultiDimArrayMemoryDirectorNET(MultiDimArrayMemory<T> mem)
    {
        this.mem = mem;
        this.objectheapid = RRObjectHeap.AddObject(this);
    }

    public override ulong DimCount()
    {
        try
        {
            return mem.DimCount;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return 0;
            }
        }
    }

    public override vector_uint64_t Dimensions()
    {
        try
        {
            vector_uint64_t o = new vector_uint64_t();
            foreach (ulong i in mem.Dimensions)
                o.Add(i);
            return o;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return new vector_uint64_t();
            }
        }
    }

    public override void Read(WrappedMultiDimArrayMemoryParams p)
    {
        RRMultiDimArrayUntyped pbuffer = null;
        RRBaseArray pbufferDims = null;
        RRBaseArray pbufferArray = null;

        try
        {
            ulong[] count = p.count.ToArray();
            ulong[] bufferpos = p.bufferpos.ToArray();
            ulong[] memorypos = p.memorypos.ToArray();

            ulong elemcount = 1;
            foreach (ulong e in count)
                elemcount *= e;

            pbuffer = p.buffer;
            pbufferDims = pbuffer.Dims;
            pbufferArray = pbuffer.Array;

            T[] array = new T[elemcount];

            MultiDimArray m = new MultiDimArray(count.Select(x => (uint)x).ToArray(), array);

            mem.Read(memorypos, m, bufferpos, count);
            MessageElementDataUtil.ArrayToRRBaseArray(array, pbufferArray);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
        finally
        {
            if (p != null && pbuffer != null)
            {
                if (pbufferDims != null)
                    pbufferDims.Dispose();
                if (pbufferArray != null)
                    pbufferArray.Dispose();
            }
            if (p != null)
            {
                p.Dispose();
            }
        }
    }

    public override void Write(WrappedMultiDimArrayMemoryParams p)
    {
        RRMultiDimArrayUntyped pbuffer = null;
        RRBaseArray pbufferDims = null;
        RRBaseArray pbufferArray = null;

        try
        {
            ulong[] count = p.count.ToArray();
            ulong[] bufferpos = p.bufferpos.ToArray();
            ulong[] memorypos = p.memorypos.ToArray();

            ulong elemcount = 1;
            foreach (ulong e in count)
                elemcount *= e;

            pbuffer = p.buffer;
            pbufferDims = pbuffer.Dims;
            pbufferArray = pbuffer.Array;

            int[] dims = (int[])MessageElementDataUtil.RRBaseArrayToArray(pbufferDims);
            T[] array = (T[])MessageElementDataUtil.RRBaseArrayToArray(pbufferArray);

            MultiDimArray m = new MultiDimArray(count.Select(x => (uint)x).ToArray(), array);

            mem.Write(memorypos, m, bufferpos, count);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
        finally
        {
            if (p != null && pbuffer != null)
            {
                if (pbufferDims != null)
                    pbufferDims.Dispose();
                if (pbufferArray != null)
                    pbufferArray.Dispose();
            }
            if (p != null)
            {
                p.Dispose();
            }
        }
    }
}

public class WrappedPodArrayMemoryDirectorNET<T> : WrappedPodArrayMemoryDirector
    where T : struct
{
    PodArrayMemory<T> mem;

    // public int memoryid = 0;

    public WrappedPodArrayMemoryDirectorNET(PodArrayMemory<T> mem)
    {
        this.mem = mem;

        this.objectheapid = RRObjectHeap.AddObject(this);
    }

    public override ulong Length()
    {
        try
        {
            return mem.Length;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return 0;
            }
        }
    }

    public override MessageElementNestedElementList Read(ulong memorypos, ulong bufferpos, ulong count)
    {
        try
        {
            T[] buffer3 = new T[count];
            mem.Read(memorypos, buffer3, 0, count);

            return RobotRaconteurNode.s.PackPodArray<T>(buffer3);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
            return null;
        }
    }

    public override void Write(ulong memorypos, MessageElementNestedElementList buffer, ulong bufferpos, ulong count)
    {
        try
        {
            using (buffer)
            {

                T[] buffer2 = (T[])RobotRaconteurNode.s.UnpackPodArray<T>(buffer);
                mem.Write(memorypos, buffer2, bufferpos, count);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

public class WrappedPodMultiDimArrayMemoryDirectorNET<T> : WrappedPodMultiDimArrayMemoryDirector
    where T : struct
{
    PodMultiDimArrayMemory<T> mem;

    // public int memoryid = 0;

    public WrappedPodMultiDimArrayMemoryDirectorNET(PodMultiDimArrayMemory<T> mem)
    {
        this.mem = mem;

        this.objectheapid = RRObjectHeap.AddObject(this);
    }

    public override ulong DimCount()
    {
        try
        {
            return mem.DimCount;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return 0;
            }
        }
    }

    public override vector_uint64_t Dimensions()
    {
        try
        {
            return new vector_uint64_t(mem.Dimensions);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return new vector_uint64_t();
            }
        }
    }

    public override MessageElementNestedElementList Read(vector_uint64_t memorypos, vector_uint64_t bufferpos,
                                                         vector_uint64_t count)
    {
        try
        {
            using (memorypos) using (bufferpos) using (count)
            {
                PodMultiDimArray buffer3 = new PodMultiDimArray(count.Select(x => (uint)x).ToArray(),
                                                                new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                mem.Read(memorypos.ToArray(), buffer3, new ulong[count.Count], count.ToArray());
                return RobotRaconteurNode.s.PackPodMultiDimArray<T>(buffer3);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
            return null;
        }
    }

    public override void Write(vector_uint64_t memorypos, MessageElementNestedElementList buffer,
                               vector_uint64_t bufferpos, vector_uint64_t count)
    {
        try
        {
            using (buffer) using (memorypos) using (bufferpos) using (count)
            {
                PodMultiDimArray buffer2 = RobotRaconteurNode.s.UnpackPodMultiDimArray<T>(buffer);
                mem.Write(memorypos.ToArray(), buffer2, bufferpos.ToArray(), count.ToArray());
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

/// <summary>
/// Single dimensional namedarray random access memory region
/// </summary>
/// <remarks>
/// <para>
/// Memories represent random access memory regions that are typically
/// represented as arrays of various shapes and types. Memories can be
/// declared in service definition files using the `memory` member keyword
/// within service definitions. Services expose memories to clients, and
/// the nodes will proxy read, write, and parameter requests between the client
/// and service. The node will also break up large requests to avoid the
/// message size limit of the transport.
/// </para>
/// <para>
/// The NamedArrayMemory class is used to represent a single dimensional named
/// array. Multidimensional named arrays should use NamedMultiDimArrayMemory.
/// Type T must be declared in a service definition using the `namedarray`
/// keyword, and generated using RobotRaconteurGen.
/// </para>
/// <para>
/// NamedArrayMemory instances are attached to an array, either when
/// constructed or later using Attach().
/// </para>
/// <para>
/// NamedArrayMemory instances returned by clients are special implementations
/// designed to proxy requests to the service. They cannot be attached
/// to an arbitrary array.
/// </para>
/// </remarks>
/// <typeparam name="T">The namedarray type of the array</typeparam>
public class NamedArrayMemory<T> : ArrayMemory<T>
    where T : struct
{
    /// <summary>
    /// Construct a new NamedArrayMemory instance
    /// </summary>
    /// <remarks>
    /// New instance will not be attached to an array.
    /// </remarks>
    public NamedArrayMemory() : base()
    {}

    /// <summary>
    /// Construct a new NamedArrayMemory instance attached to an array
    /// </summary>
    /// <remarks>
    /// New instance will be constructed attached to an array.
    /// </remarks>
    /// <param name="memory">The array to attach</param>
    /// <returns></returns>
    public NamedArrayMemory(T[] memory) : base(memory)
    {}
}

/// <summary>
/// Multidimensional namedarray random access memory region
/// </summary>
/// <remarks>
/// <para>
/// Memories represent random access memory regions that are typically
/// represented as arrays of various shapes and types. Memories can be
/// declared in service definition files using the `memory` member keyword
/// within service definitions. Services expose memories to clients, and
/// the nodes will proxy read, write, and parameter requests between the client
/// and service. The node will also break up large requests to avoid the
/// message size limit of the transport.
/// </para>
/// <para>
/// The NamedMultiDimArrayMemory class is used to represent a multidimensional
/// named array. Single dimensional named arrays should use NamedArrayMemory.
/// Type T must be declared in a service definition using the `namedarray`
/// keyword, and generated using RobotRaconteurGen.
/// </para>
/// <para>
/// NamedMultiDimArrayMemory instances are attached to an NamedMultiDimArray,
/// either when constructed or later using Attach().
/// </para>
/// <para>
/// NamedMultiDimArrayMemory instances returned by clients are special implementations
/// designed to proxy requests to the service. They cannot be attached
/// to an arbitrary array.
/// </para>
/// </remarks>
/// <typeparam name="T">The namedarray type of the array</typeparam>
public class NamedMultiDimArrayMemory<T>
    where T : struct
{

    private NamedMultiDimArray multimemory;

    /// <summary>
    /// Construct a new NamedMultiDimArrayMemory instance
    /// </summary>
    /// <remarks>
    /// New instance will not be attached to an array.
    /// </remarks>
    public NamedMultiDimArrayMemory()
    {}

    /// <summary>
    /// Construct a new NamedMultiDimArrayMemory instance attached to an NamedMultiDimArray
    /// </summary>
    /// <remarks>
    /// New instance will be constructed attached to an array.
    /// </remarks>
    /// <param name="memory">The array to attach</param>
    public NamedMultiDimArrayMemory(NamedMultiDimArray memory)
    {

        multimemory = memory;
    }

    /// <summary>
    /// Attach PodMultiDimArrayMemory instance to a PodMultiDimArray
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="memory">The array to attach</param>
    public virtual void Attach(NamedMultiDimArray memory)
    {

        this.multimemory = memory;
    }

    /// <summary>
    /// Dimensions of the memory array
    /// </summary>
    /// <remarks>
    /// <para>
    /// Returns the dimensions (shape) of the memory array
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </para>
    /// </remarks>
    public virtual ulong[] Dimensions
    {
        get {
            return multimemory.Dims.Select(x => (ulong)x).ToArray();
        }
    }

    /// <summary>
    /// The number of dimensions in the memory array
    /// </summary>
    /// <remarks>
    /// When used with a memory returned by a client, this function will
    /// call the service to execute the request.
    /// </remarks>
    public virtual ulong DimCount
    {
        get {
            return (ulong)multimemory.Dims.Length;
        }
    }

    /// <summary>
    /// Read a block from a multidimensional array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Read a block of a multidimensional array memory into a supplied buffer multidimensional array.
    /// The start positions and count of the read are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start position in the memory array to read</param>
    /// <param name="buffer">The buffer to receive the read data</param>
    /// <param name="bufferpos">The start position in the buffer to write the data</param>
    /// <param name="count">The count of array elements to read</param>
    public virtual void Read(ulong[] memorypos, NamedMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {

        multimemory.RetrieveSubArray(memorypos.Select(x => (uint)x).ToArray(), buffer,
                                     bufferpos.Select(x => (uint)x).ToArray(), count.Select(x => (uint)x).ToArray());
    }

    /// <summary>
    /// Write a segment to a multidimensional array memory
    /// </summary>
    /// <remarks>
    /// <para>
    /// Writes a segment to a multidimensional array memory from a supplied buffer
    /// multidimensional array. The start positions and count
    /// of the write are specified.
    /// </para>
    /// <para>
    /// When used with a memory returned by a client, this function will call
    /// the service to execute the request.
    /// </para>
    /// </remarks>
    /// <param name="memorypos">The start position in the memory array to write</param>
    /// <param name="buffer">The buffer to write the data from</param>
    /// <param name="bufferpos">The start position in the buffer to read the data</param>
    /// <param name="count">The count of array elements to write</param>
    public virtual void Write(ulong[] memorypos, NamedMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {

        multimemory.AssignSubArray(memorypos.Select(x => (uint)x).ToArray(), buffer,
                                   bufferpos.Select(x => (uint)x).ToArray(), count.Select(x => (uint)x).ToArray());
    }
}

public class NamedArrayMemoryClient<T> : NamedArrayMemory<T>
    where T : struct
{
    WrappedNamedArrayMemoryClient innerclient;

    class bufferdirector : WrappedNamedArrayMemoryClientBuffer
    {
        T[] buffer;

        public bufferdirector(T[] buffer)
        {
            this.buffer = buffer;
        }

        public override void UnpackReadResult(MessageElementNestedElementList res, ulong bufferpos, ulong count)
        {
            T[] res1 = RobotRaconteurNode.s.UnpackNamedArrayDispose<T>(res);
            Array.Copy(res1, 0, buffer, (long)bufferpos, (long)count);
        }

        public override MessageElementNestedElementList PackWriteRequest(ulong bufferpos, ulong count)
        {
            T[] buffer3;
            if ((ulong)buffer.Length == count)
            {
                buffer3 = buffer;
            }
            else
            {
                buffer3 = new T[count];
                Array.Copy(buffer, (long)bufferpos, buffer3, 0, (long)count);
            }
            return RobotRaconteurNode.s.PackNamedArray<T>(buffer3);
        }
    }

    public NamedArrayMemoryClient(WrappedNamedArrayMemoryClient innerclient)
    {
        this.innerclient = innerclient;
    }

    public override void Attach(T[] memory)
    {
        throw new InvalidOperationException("Invalid for memory client");
    }

    MemberDefinition_Direction Direction
    {
        get {
            return innerclient.Direction();
        }
    }

    public override ulong Length
    {
        get {
            return innerclient.Length();
        }
    }

    public override void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer))
        {
            innerclient.Read(memorypos, buffer1, bufferpos, count);
        }
    }

    public override void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer))
        {
            innerclient.Write(memorypos, buffer1, bufferpos, count);
        }
    }
}

public class NamedMultiDimArrayMemoryClient<T> : NamedMultiDimArrayMemory<T>
    where T : struct
{
    WrappedNamedMultiDimArrayMemoryClient innerclient;

    class bufferdirector : WrappedNamedMultiDimArrayMemoryClientBuffer
    {
        NamedMultiDimArray buffer;

        public bufferdirector(NamedMultiDimArray buffer)
        {
            this.buffer = buffer;
        }

        public override void UnpackReadResult(MessageElementNestedElementList res, vector_uint64_t bufferpos,
                                              vector_uint64_t count)
        {
            using (res) using (bufferpos) using (count)
            {
                NamedMultiDimArray res1 = RobotRaconteurNode.s.UnpackNamedMultiDimArrayDispose<T>(res);
                buffer.AssignSubArray(bufferpos.Select(x => (uint)x).ToArray(), res1, new uint[buffer.Dims.Length],
                                      count.Select(x => (uint)x).ToArray());
            }
        }

        public override MessageElementNestedElementList PackWriteRequest(vector_uint64_t bufferpos,
                                                                         vector_uint64_t count)
        {
            using (bufferpos) using (count)
            {
                NamedMultiDimArray o = new NamedMultiDimArray(count.Select(x => (uint)x).ToArray(),
                                                              new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                buffer.RetrieveSubArray(bufferpos.Select(x => (uint)x).ToArray(), o, new uint[buffer.Dims.Length],
                                        count.Select(x => (uint)x).ToArray());
                return RobotRaconteurNode.s.PackNamedMultiDimArray<T>(o);
            }
        }
    }

    public NamedMultiDimArrayMemoryClient(WrappedNamedMultiDimArrayMemoryClient innerclient)
    {
        this.innerclient = innerclient;
    }

    public virtual MemberDefinition_Direction Direction
    {
        get {
            return innerclient.Direction();
        }
    }

    public override void Attach(NamedMultiDimArray memory)
    {
        throw new InvalidOperationException("Invalid for memory client");
    }

    public override ulong DimCount
    {
        get {
            return innerclient.DimCount();
        }
    }

    public override ulong[] Dimensions
    {
        get {
            return innerclient.Dimensions().ToArray();
        }
    }

    public override void Read(ulong[] memorypos, NamedMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer)) using (
            vector_uint64_t memorypos1 = new vector_uint64_t(
                memorypos)) using (vector_uint64_t bufferpos1 =
                                       new vector_uint64_t(bufferpos)) using (vector_uint64_t count1 =
                                                                                  new vector_uint64_t(count))
        {
            innerclient.Read(memorypos1, buffer1, bufferpos1, count1);
        }
    }

    public override void Write(ulong[] memorypos, NamedMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
    {
        using (bufferdirector buffer1 = new bufferdirector(buffer)) using (
            vector_uint64_t memorypos1 = new vector_uint64_t(
                memorypos)) using (vector_uint64_t bufferpos1 =
                                       new vector_uint64_t(bufferpos)) using (vector_uint64_t count1 =
                                                                                  new vector_uint64_t(count))
        {
            innerclient.Write(memorypos1, buffer1, bufferpos1, count1);
        }
    }
}

public class WrappedNamedArrayMemoryDirectorNET<T> : WrappedNamedArrayMemoryDirector
    where T : struct
{
    NamedArrayMemory<T> mem;

    // public int memoryid = 0;

    public WrappedNamedArrayMemoryDirectorNET(NamedArrayMemory<T> mem)
    {
        this.mem = mem;

        this.objectheapid = RRObjectHeap.AddObject(this);
    }

    public override ulong Length()
    {
        try
        {
            return mem.Length;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return 0;
            }
        }
    }

    public override MessageElementNestedElementList Read(ulong memorypos, ulong bufferpos, ulong count)
    {
        try
        {
            T[] buffer3 = new T[count];
            mem.Read(memorypos, buffer3, 0, count);

            return RobotRaconteurNode.s.PackNamedArray<T>(buffer3);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
            return null;
        }
    }

    public override void Write(ulong memorypos, MessageElementNestedElementList buffer, ulong bufferpos, ulong count)
    {
        try
        {
            using (buffer)
            {

                T[] buffer2 = (T[])RobotRaconteurNode.s.UnpackNamedArray<T>(buffer);
                mem.Write(memorypos, buffer2, bufferpos, count);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

public class WrappedNamedMultiDimArrayMemoryDirectorNET<T> : WrappedNamedMultiDimArrayMemoryDirector
    where T : struct
{
    NamedMultiDimArrayMemory<T> mem;

    // public int memoryid = 0;

    public WrappedNamedMultiDimArrayMemoryDirectorNET(NamedMultiDimArrayMemory<T> mem)
    {
        this.mem = mem;

        this.objectheapid = RRObjectHeap.AddObject(this);
    }

    public override ulong DimCount()
    {
        try
        {
            return mem.DimCount;
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return 0;
            }
        }
    }

    public override vector_uint64_t Dimensions()
    {
        try
        {
            return new vector_uint64_t(mem.Dimensions);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return new vector_uint64_t();
            }
        }
    }

    public override MessageElementNestedElementList Read(vector_uint64_t memorypos, vector_uint64_t bufferpos,
                                                         vector_uint64_t count)
    {
        try
        {
            using (memorypos) using (bufferpos) using (count)
            {
                NamedMultiDimArray buffer3 = new NamedMultiDimArray(
                    count.Select(x => (uint)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                mem.Read(memorypos.ToArray(), buffer3, new ulong[count.Count], count.ToArray());
                return RobotRaconteurNode.s.PackNamedMultiDimArray<T>(buffer3);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
            return null;
        }
    }

    public override void Write(vector_uint64_t memorypos, MessageElementNestedElementList buffer,
                               vector_uint64_t bufferpos, vector_uint64_t count)
    {
        try
        {
            using (buffer) using (memorypos) using (bufferpos) using (count)
            {
                NamedMultiDimArray buffer2 = RobotRaconteurNode.s.UnpackNamedMultiDimArray<T>(buffer);
                mem.Write(memorypos.ToArray(), buffer2, bufferpos.ToArray(), count.ToArray());
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
            }
        }
    }
}

}
