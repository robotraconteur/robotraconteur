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

using System;
using System.Collections.Generic;
using System.Linq;

namespace RobotRaconteur
{
    public class ArrayMemory<T>
    {
        private T[] memory;

        public ArrayMemory()
        {

        }

        public ArrayMemory(T[] memory)
        {
            this.memory = memory;

        }

        public virtual void Attach(T[] memory)
        {
            this.memory = memory;

        }

        public virtual ulong Length
        {
            get
            {
                return (ulong)memory.LongLength;
            }
        }

        public virtual void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {

            Array.Copy(memory, (long)memorypos, buffer, (long)bufferpos, (long)count);
        }

        public virtual void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {

            Array.Copy(buffer, (long)bufferpos, memory, (long)memorypos, (long)count);
        }
    }

    public class CStructureArrayMemory<T> : ArrayMemory<T> where T : struct
    {
        public CStructureArrayMemory() : base()
        {
        }

        public CStructureArrayMemory(T[] memory) : base(memory)
        {
        }
    }

    public class MultiDimArrayMemory<T>
    {

        private MultiDimArray multimemory;

        public MultiDimArrayMemory()
        {


        }

        public MultiDimArrayMemory(MultiDimArray memory)
        {

            multimemory = memory;
        }

        public virtual void Attach(MultiDimArray memory)
        {

            this.multimemory = memory;

        }

        public virtual ulong[] Dimensions
        {
            get
            {
                return multimemory.Dims.Select(x => (ulong)x).ToArray();
            }
        }

        public virtual ulong DimCount
        {
            get
            {
                return (ulong)multimemory.DimCount;
            }
        }

        public virtual bool Complex
        {
            get
            {
                return multimemory.Complex;
            }
        }

        public virtual void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.RetrieveSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

        }


        public virtual void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.AssignSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());
        }
    }

    public class CStructureMultiDimArrayMemory<T> where T : struct
    {

        private CStructureMultiDimArray multimemory;

        public CStructureMultiDimArrayMemory()
        {


        }

        public CStructureMultiDimArrayMemory(CStructureMultiDimArray memory)
        {

            multimemory = memory;
        }

        public virtual void Attach(CStructureMultiDimArray memory)
        {

            this.multimemory = memory;

        }

        public virtual ulong[] Dimensions
        {
            get
            {
                return multimemory.Dims.Select(x => (ulong)x).ToArray();
            }
        }

        public virtual ulong DimCount
        {
            get
            {
                return (ulong)multimemory.Dims.Length;
            }
        }

        public virtual void Read(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.RetrieveSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

        }


        public virtual void Write(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.AssignSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());
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
            get
            {
                return WrappedArrayMemoryClientUtil.Direction(innerarray);
            }
        }

        public override ulong Length
        {
            get
            {
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
                if (dat != null) dat.Dispose();
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
            get
            {
                return WrappedMultiDimArrayMemoryClientUtil.Direction(innermem);
            }
        }

        public override ulong DimCount
        {
            get
            {
                return innermem.DimCount();
            }
        }

        public override ulong[] Dimensions
        {
            get
            {
                return innermem.Dimensions().Select(x => (ulong)x).ToArray();
            }
        }

        public override bool Complex
        {
            get
            {
                return innermem.Complex();
            }
        }

        public override void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            RRMultiDimArrayUntyped dat = null;
            RRBaseArray datDims = null;
            RRBaseArray datReal = null;
            RRBaseArray datImag = null;
            try
            {
                vector_uint64_t memorypos2 = new vector_uint64_t();
                foreach (ulong val in memorypos) memorypos2.Add(val);
                vector_uint64_t count2 = new vector_uint64_t();
                foreach (ulong val in count) count2.Add(val);

                dat = WrappedMultiDimArrayMemoryClientUtil.Read(innermem, memorypos2, count2);

                datDims = dat.Dims;
                datReal = dat.Real;
                datImag = dat.Imag;
                int[] dims = (int[])MessageElementDataUtil.RRBaseArrayToArray(datDims);
                T[] real = (T[])MessageElementDataUtil.RRBaseArrayToArray(datReal);
                T[] imag = (T[])MessageElementDataUtil.RRBaseArrayToArray(datImag);

                MultiDimArray dat2 = new MultiDimArray(dims, real, imag);
                buffer.AssignSubArray(bufferpos.Select(x => (int)x).ToArray(), dat2, new int[count.Length], count.Select(x => (int)x).ToArray());
            }
            finally
            {
                if (dat != null)
                {
                    if (datDims != null) datDims.Dispose();
                    if (datReal != null) datReal.Dispose();
                    if (datImag != null) datImag.Dispose();
                    dat.Dispose();
                }
            }




        }

        public override void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            RRMultiDimArrayUntyped dat2 = null;
            RRBaseArray dat2Dims = null;
            RRBaseArray dat2Real = null;
            RRBaseArray dat2Imag = null;
            try
            {
                int elemcount = 1;
                foreach (ulong v in count) elemcount *= (int)v;
                int[] count2 = count.Select(x => (int)x).ToArray();
                T[] real = new T[elemcount];
                T[] imag = null;
                if (Complex) imag = new T[elemcount];

                MultiDimArray writedat1 = new MultiDimArray(count2, real, imag);
                writedat1.AssignSubArray(new int[count.Length], buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

                dat2 = new RRMultiDimArrayUntyped();
                dat2.DimCount = count2.Length;
                dat2Dims = MessageElementDataUtil.ArrayToRRBaseArray(count2);
                dat2.Dims = dat2Dims;
                dat2Real = MessageElementDataUtil.ArrayToRRBaseArray(real);
                dat2.Real = dat2Real;
                dat2.Complex = false;
                if (imag != null)
                {
                    dat2.Complex = true;
                    dat2Imag = MessageElementDataUtil.ArrayToRRBaseArray(imag);
                    dat2.Imag = dat2Imag;
                }

                vector_uint64_t memorypos3 = new vector_uint64_t();
                foreach (ulong val in memorypos) memorypos3.Add(val);
                vector_uint64_t count3 = new vector_uint64_t();
                foreach (ulong val in count) count3.Add(val);
                vector_uint64_t bufferpos3 = new vector_uint64_t();
                for (int i = 0; i < count.Length; i++) bufferpos3.Add(0);
                WrappedMultiDimArrayMemoryClientUtil.Write(innermem, memorypos3, dat2, bufferpos3, count3);
            }
            finally
            {
                if (dat2 != null)
                {
                    if (dat2Dims != null) dat2Dims.Dispose();
                    if (dat2Real != null) dat2Real.Dispose();
                    if (dat2Imag != null) dat2Imag.Dispose();
                    dat2.Dispose();
                }
            }
        }
    }

    public class CStructureArrayMemoryClient<T> : CStructureArrayMemory<T> where T : struct
    {
        WrappedCStructureArrayMemoryClient innerclient;

        class bufferdirector : WrappedCStructureArrayMemoryClientBuffer
        {
            T[] buffer;

            public bufferdirector(T[] buffer)
            {
                this.buffer = buffer;
            }

            public override void UnpackReadResult(MessageElementCStructureArray res, ulong bufferpos, ulong count)
            {
                T[] res1 = RobotRaconteurNode.s.UnpackCStructureArrayDispose<T>(res);
                Array.Copy(res1, 0, buffer, (long)bufferpos, (long)count);
            }

            public override MessageElementCStructureArray PackWriteRequest(ulong bufferpos, ulong count)
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
                return RobotRaconteurNode.s.PackCStructureArray<T>(buffer3);
            }
        }

        public CStructureArrayMemoryClient(WrappedCStructureArrayMemoryClient innerclient)
        {
            this.innerclient = innerclient;
        }

        public override void Attach(T[] memory)
        {
            throw new InvalidOperationException("Invalid for memory client");
        }

        MemberDefinition_Direction Direction
        {
            get
            {
                return innerclient.Direction();
            }
        }

        public override ulong Length
        {
            get
            {
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

    public class CStructureMultiDimArrayMemoryClient<T> : CStructureMultiDimArrayMemory<T> where T : struct
    {
        WrappedCStructureMultiDimArrayMemoryClient innerclient;

        class bufferdirector : WrappedCStructureMultiDimArrayMemoryClientBuffer
        {
            CStructureMultiDimArray buffer;

            public bufferdirector(CStructureMultiDimArray buffer)
            {
                this.buffer = buffer;
            }

            public override void UnpackReadResult(MessageElementCStructureMultiDimArray res, vector_uint64_t bufferpos, vector_uint64_t count)
            {
                using (res)
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray res1 = RobotRaconteurNode.s.UnpackCStructureMultiDimArrayDispose<T>(res);
                    buffer.AssignSubArray(bufferpos.Select(x => (int)x).ToArray(), res1, new int[buffer.Dims.Length], count.Select(x => (int)x).ToArray());
                }
            }

            public override MessageElementCStructureMultiDimArray PackWriteRequest(vector_uint64_t bufferpos, vector_uint64_t count)
            {
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray o = new CStructureMultiDimArray(count.Select(x => (int)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                    buffer.RetrieveSubArray(bufferpos.Select(x => (int)x).ToArray(), o, new int[buffer.Dims.Length], count.Select(x => (int)x).ToArray());
                    return RobotRaconteurNode.s.PackCStructureMultiDimArray<T>(o);
                }
            }
        }

        public CStructureMultiDimArrayMemoryClient(WrappedCStructureMultiDimArrayMemoryClient innerclient)
        {
            this.innerclient = innerclient;
        }

        public virtual MemberDefinition_Direction Direction
        {
            get
            {
                return innerclient.Direction();
            }
        }

        public override void Attach(CStructureMultiDimArray memory)
        {
            throw new InvalidOperationException("Invalid for memory client");
        }

        public override ulong DimCount
        {
            get
            {
                return innerclient.DimCount();
            }
        }

        public override ulong[] Dimensions
        {
            get
            {
                return innerclient.Dimensions().ToArray();
            }
        }



        public override void Read(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            using (vector_uint64_t memorypos1 = new vector_uint64_t(memorypos))
            using (vector_uint64_t bufferpos1 = new vector_uint64_t(bufferpos))
            using (vector_uint64_t count1 = new vector_uint64_t(count))
            {
                innerclient.Read(memorypos1, buffer1, bufferpos1, count1);
            }
        }

        public override void Write(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            using (vector_uint64_t memorypos1 = new vector_uint64_t(memorypos))
            using (vector_uint64_t bufferpos1 = new vector_uint64_t(bufferpos))
            using (vector_uint64_t count1 = new vector_uint64_t(count))
            {
                innerclient.Write(memorypos1, buffer1, bufferpos1, count1);
            }
        }
    }

    public class WrappedArrayMemoryDirectorNET<T> : WrappedArrayMemoryDirector
    {
        ArrayMemory<T> mem;

        //public int memoryid = 0;

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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
                }

            }
        }
    }

    public class WrappedMultiDimArrayMemoryDirectorNET<T> : WrappedMultiDimArrayMemoryDirector
    {
        MultiDimArrayMemory<T> mem;

        //public int memoryid = 0;

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
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }
            }
        }

        public override vector_uint64_t Dimensions()
        {
            try
            {
                vector_uint64_t o = new vector_uint64_t();
                foreach (ulong i in mem.Dimensions) o.Add(i);
                return o;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return new vector_uint64_t();
                }
            }
        }

        public override bool Complex()
        {
            try
            {
                return mem.Complex;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return false;
                }
            }
        }

        public override void Read(WrappedMultiDimArrayMemoryParams p)
        {
            RRMultiDimArrayUntyped pbuffer = null;
            RRBaseArray pbufferDims = null;
            RRBaseArray pbufferReal = null;
            RRBaseArray pbufferImag = null;

            try
            {
                ulong[] count = p.count.ToArray();
                ulong[] bufferpos = p.bufferpos.ToArray();
                ulong[] memorypos = p.memorypos.ToArray();

                ulong elemcount = 1;
                foreach (ulong e in count) elemcount *= e;

                pbuffer = p.buffer;
                pbufferDims = pbuffer.Dims;
                pbufferReal = pbuffer.Real;
                pbufferImag = pbuffer.Imag;

                if (mem.Complex != pbuffer.Complex) throw new ArgumentException("Complex mismatch");
                T[] real = new T[elemcount];
                T[] imag = null;
                if (mem.Complex) imag = new T[elemcount];

                MultiDimArray m = new MultiDimArray(count.Select(x => (int)x).ToArray(), real, imag);

                mem.Read(memorypos, m, bufferpos, count);
                MessageElementDataUtil.ArrayToRRBaseArray(real, pbufferReal);
                if (imag != null) MessageElementDataUtil.ArrayToRRBaseArray(imag, pbufferImag);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }

            }
            finally
            {
                if (p != null && pbuffer != null)
                {
                    if (pbufferDims != null) pbufferDims.Dispose();
                    if (pbufferReal != null) pbufferReal.Dispose();
                    if (pbufferImag != null) pbufferImag.Dispose();
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
            RRBaseArray pbufferReal = null;
            RRBaseArray pbufferImag = null;
            try
            {
                ulong[] count = p.count.ToArray();
                ulong[] bufferpos = p.bufferpos.ToArray();
                ulong[] memorypos = p.memorypos.ToArray();

                ulong elemcount = 1;
                foreach (ulong e in count) elemcount *= e;

                pbuffer = p.buffer;
                pbufferDims = pbuffer.Dims;
                pbufferReal = pbuffer.Real;
                pbufferImag = pbuffer.Imag;

                if (mem.Complex != pbuffer.Complex) throw new ArgumentException("Complex mismatch");

                int[] dims = (int[])MessageElementDataUtil.RRBaseArrayToArray(pbufferDims);
                T[] real = (T[])MessageElementDataUtil.RRBaseArrayToArray(pbufferReal);
                T[] imag = null;
                if (pbuffer.Complex) imag = (T[])MessageElementDataUtil.RRBaseArrayToArray(pbufferImag);

                MultiDimArray m = new MultiDimArray(count.Select(x => (int)x).ToArray(), real, imag);

                mem.Write(memorypos, m, bufferpos, count);


            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }

            }
            finally
            {
                if (p != null && pbuffer != null)
                {
                    if (pbufferDims != null) pbufferDims.Dispose();
                    if (pbufferReal != null) pbufferReal.Dispose();
                    if (pbufferImag != null) pbufferImag.Dispose();
                }
                if (p != null)
                {
                    p.Dispose();
                }


            }
        }

    }

    public class WrappedCStructureArrayMemoryDirectorNET<T> : WrappedCStructureArrayMemoryDirector where T : struct
    {
        CStructureArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedCStructureArrayMemoryDirectorNET(CStructureArrayMemory<T> mem)
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
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }

            }
        }

        public override MessageElementCStructureArray Read(ulong memorypos, ulong bufferpos, ulong count)
        {
            try
            {
                T[] buffer3 = new T[count];
                mem.Read(memorypos, buffer3, 0, count);

                return RobotRaconteurNode.s.PackCStructureArray<T>(buffer3);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
                return null;
            }
        }

        public override void Write(ulong memorypos, MessageElementCStructureArray buffer, ulong bufferpos, ulong count)
        {
            try
            {
                using (buffer)
                {

                    T[] buffer2 = (T[])RobotRaconteurNode.s.UnpackCStructureArray<T>(buffer);
                    mem.Write(memorypos, buffer2, bufferpos, count);
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }
        }
    }

    public class WrappedCStructureMultiDimArrayMemoryDirectorNET<T> : WrappedCStructureMultiDimArrayMemoryDirector where T : struct
    {
        CStructureMultiDimArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedCStructureMultiDimArrayMemoryDirectorNET(CStructureMultiDimArrayMemory<T> mem)
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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
                    return new vector_uint64_t();
                }

            }
        }

        public override MessageElementCStructureMultiDimArray Read(vector_uint64_t memorypos, vector_uint64_t bufferpos, vector_uint64_t count)
        {
            try
            {
                using (memorypos)
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray buffer3 = new CStructureMultiDimArray(count.Select(x => (int)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                    mem.Read(memorypos.ToArray(), buffer3, new ulong[count.Count], count.ToArray());
                    return RobotRaconteurNode.s.PackCStructureMultiDimArray<T>(buffer3);
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
                return null;
            }
        }

        public override void Write(vector_uint64_t memorypos, MessageElementCStructureMultiDimArray buffer, vector_uint64_t bufferpos, vector_uint64_t count)
        {
            try
            {
                using (buffer)
                using (memorypos)
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray buffer2 = RobotRaconteurNode.s.UnpackCStructureMultiDimArray<T>(buffer);
                    mem.Write(memorypos.ToArray(), buffer2, bufferpos.ToArray(), count.ToArray());
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }
        }
    }

    public class AStructureArrayMemory<T> : ArrayMemory<T> where T : struct
    {
        public AStructureArrayMemory() : base()
        {
        }

        public AStructureArrayMemory(T[] memory) : base(memory)
        {
        }
    }

    public class AStructureMultiDimArrayMemory<T> where T : struct
    {

        private AStructureMultiDimArray multimemory;

        public AStructureMultiDimArrayMemory()
        {


        }

        public AStructureMultiDimArrayMemory(AStructureMultiDimArray memory)
        {

            multimemory = memory;
        }

        public virtual void Attach(AStructureMultiDimArray memory)
        {

            this.multimemory = memory;

        }

        public virtual ulong[] Dimensions
        {
            get
            {
                return multimemory.Dims.Select(x => (ulong)x).ToArray();
            }
        }

        public virtual ulong DimCount
        {
            get
            {
                return (ulong)multimemory.Dims.Length;
            }
        }

        public virtual void Read(ulong[] memorypos, AStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.RetrieveSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

        }


        public virtual void Write(ulong[] memorypos, AStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.AssignSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());
        }
    }

    public class AStructureArrayMemoryClient<T> : AStructureArrayMemory<T> where T : struct
    {
        WrappedAStructureArrayMemoryClient innerclient;

        class bufferdirector : WrappedAStructureArrayMemoryClientBuffer
        {
            T[] buffer;

            public bufferdirector(T[] buffer)
            {
                this.buffer = buffer;
            }

            public override void UnpackReadResult(MessageElementAStructureArray res, ulong bufferpos, ulong count)
            {
                T[] res1 = RobotRaconteurNode.s.UnpackAStructureArrayDispose<T>(res);
                Array.Copy(res1, 0, buffer, (long)bufferpos, (long)count);
            }

            public override MessageElementAStructureArray PackWriteRequest(ulong bufferpos, ulong count)
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
                return RobotRaconteurNode.s.PackAStructureArray<T>(buffer3);
            }
        }

        public AStructureArrayMemoryClient(WrappedAStructureArrayMemoryClient innerclient)
        {
            this.innerclient = innerclient;
        }

        public override void Attach(T[] memory)
        {
            throw new InvalidOperationException("Invalid for memory client");
        }

        MemberDefinition_Direction Direction
        {
            get
            {
                return innerclient.Direction();
            }
        }

        public override ulong Length
        {
            get
            {
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

    public class AStructureMultiDimArrayMemoryClient<T> : AStructureMultiDimArrayMemory<T> where T : struct
    {
        WrappedAStructureMultiDimArrayMemoryClient innerclient;

        class bufferdirector : WrappedAStructureMultiDimArrayMemoryClientBuffer
        {
            AStructureMultiDimArray buffer;

            public bufferdirector(AStructureMultiDimArray buffer)
            {
                this.buffer = buffer;
            }

            public override void UnpackReadResult(MessageElementAStructureMultiDimArray res, vector_uint64_t bufferpos, vector_uint64_t count)
            {
                using (res)
                using (bufferpos)
                using (count)
                {
                    AStructureMultiDimArray res1 = RobotRaconteurNode.s.UnpackAStructureMultiDimArrayDispose<T>(res);
                    buffer.AssignSubArray(bufferpos.Select(x => (int)x).ToArray(), res1, new int[buffer.Dims.Length], count.Select(x => (int)x).ToArray());
                }
            }

            public override MessageElementAStructureMultiDimArray PackWriteRequest(vector_uint64_t bufferpos, vector_uint64_t count)
            {
                using (bufferpos)
                using (count)
                {
                    AStructureMultiDimArray o = new AStructureMultiDimArray(count.Select(x => (int)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                    buffer.RetrieveSubArray(bufferpos.Select(x => (int)x).ToArray(), o, new int[buffer.Dims.Length], count.Select(x => (int)x).ToArray());
                    return RobotRaconteurNode.s.PackAStructureMultiDimArray<T>(o);
                }
            }
        }

        public AStructureMultiDimArrayMemoryClient(WrappedAStructureMultiDimArrayMemoryClient innerclient)
        {
            this.innerclient = innerclient;
        }

        public virtual MemberDefinition_Direction Direction
        {
            get
            {
                return innerclient.Direction();
            }
        }

        public override void Attach(AStructureMultiDimArray memory)
        {
            throw new InvalidOperationException("Invalid for memory client");
        }

        public override ulong DimCount
        {
            get
            {
                return innerclient.DimCount();
            }
        }

        public override ulong[] Dimensions
        {
            get
            {
                return innerclient.Dimensions().ToArray();
            }
        }



        public override void Read(ulong[] memorypos, AStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            using (vector_uint64_t memorypos1 = new vector_uint64_t(memorypos))
            using (vector_uint64_t bufferpos1 = new vector_uint64_t(bufferpos))
            using (vector_uint64_t count1 = new vector_uint64_t(count))
            {
                innerclient.Read(memorypos1, buffer1, bufferpos1, count1);
            }
        }

        public override void Write(ulong[] memorypos, AStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            using (vector_uint64_t memorypos1 = new vector_uint64_t(memorypos))
            using (vector_uint64_t bufferpos1 = new vector_uint64_t(bufferpos))
            using (vector_uint64_t count1 = new vector_uint64_t(count))
            {
                innerclient.Write(memorypos1, buffer1, bufferpos1, count1);
            }
        }
    }

    public class WrappedAStructureArrayMemoryDirectorNET<T> : WrappedAStructureArrayMemoryDirector where T : struct
    {
        AStructureArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedAStructureArrayMemoryDirectorNET(AStructureArrayMemory<T> mem)
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
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }

            }
        }

        public override MessageElementAStructureArray Read(ulong memorypos, ulong bufferpos, ulong count)
        {
            try
            {
                T[] buffer3 = new T[count];
                mem.Read(memorypos, buffer3, 0, count);

                return RobotRaconteurNode.s.PackAStructureArray<T>(buffer3);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
                return null;
            }
        }

        public override void Write(ulong memorypos, MessageElementAStructureArray buffer, ulong bufferpos, ulong count)
        {
            try
            {
                using (buffer)
                {

                    T[] buffer2 = (T[])RobotRaconteurNode.s.UnpackAStructureArray<T>(buffer);
                    mem.Write(memorypos, buffer2, bufferpos, count);
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }
        }
    }

    public class WrappedAStructureMultiDimArrayMemoryDirectorNET<T> : WrappedAStructureMultiDimArrayMemoryDirector where T : struct
    {
        AStructureMultiDimArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedAStructureMultiDimArrayMemoryDirectorNET(AStructureMultiDimArrayMemory<T> mem)
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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
                    return new vector_uint64_t();
                }

            }
        }

        public override MessageElementAStructureMultiDimArray Read(vector_uint64_t memorypos, vector_uint64_t bufferpos, vector_uint64_t count)
        {
            try
            {
                using (memorypos)
                using (bufferpos)
                using (count)
                {
                    AStructureMultiDimArray buffer3 = new AStructureMultiDimArray(count.Select(x => (int)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                    mem.Read(memorypos.ToArray(), buffer3, new ulong[count.Count], count.ToArray());
                    return RobotRaconteurNode.s.PackAStructureMultiDimArray<T>(buffer3);
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
                return null;
            }
        }

        public override void Write(vector_uint64_t memorypos, MessageElementAStructureMultiDimArray buffer, vector_uint64_t bufferpos, vector_uint64_t count)
        {
            try
            {
                using (buffer)
                using (memorypos)
                using (bufferpos)
                using (count)
                {
                    AStructureMultiDimArray buffer2 = RobotRaconteurNode.s.UnpackAStructureMultiDimArray<T>(buffer);
                    mem.Write(memorypos.ToArray(), buffer2, bufferpos.ToArray(), count.ToArray());
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }
        }
    }

}
