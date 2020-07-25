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
    /// Represents an unpacked MultiDimArray.  This class stores
    /// the real and complex data as column vectors that is 
    /// reshaped based on Dims.
    /// </summary>
    public class MultiDimArray
    {

        public MultiDimArray() { }

        public MultiDimArray(uint[] Dims, Array Array_)
        {
            
            this.Dims = Dims;
            this.Array_ = Array_;
        }
                
        public uint[] Dims;        
        public Array Array_;
       
        public virtual void RetrieveSubArray(uint[] memorypos, MultiDimArray buffer, uint[] bufferpos, uint[] count)
        {

            MultiDimArray mema = this;
            MultiDimArray memb = buffer;
                        
            MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(new vectoruint32(mema.Dims), new vectoruint32(memorypos),  new vectoruint32(memb.Dims), new vectoruint32(bufferpos), new vectoruint32(count));

            uint indexa;
            uint indexb;
            uint len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(mema.Array_, (long)indexa, memb.Array_, (long)indexb, (long)len);                
            }

        }

        public virtual void AssignSubArray(uint[] memorypos, MultiDimArray buffer, uint[] bufferpos, uint[] count)
        {


            MultiDimArray mema = this;
            MultiDimArray memb = buffer;
                        
            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims), new vectoruint32(bufferpos), new vectoruint32(count));

            uint indexa;
            uint indexb;
            uint len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(memb.Array_, (long)indexb, mema.Array_, (long)indexa, (long)len);
                
            }
        }

    }

    public class PodMultiDimArray
    {
        public PodMultiDimArray()
        {
            Dims = new uint[] { 0 };
        }

        public PodMultiDimArray(uint[] dims, Array array)
        {
            Dims = dims;
            pod_array = array;
        }
        
        public uint[] Dims;
        public Array pod_array;

        public virtual void RetrieveSubArray(uint[] memorypos, PodMultiDimArray buffer, uint[] bufferpos, uint[] count)
        {

            PodMultiDimArray mema = this;
            PodMultiDimArray memb = buffer;
           
            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims), new vectoruint32(bufferpos), new vectoruint32(count));

            uint indexa;
            uint indexb;
            uint len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(mema.pod_array, (long)indexa, memb.pod_array, (long)indexb, (long)len);                
            }

        }

        public virtual void AssignSubArray(uint[] memorypos, PodMultiDimArray buffer, uint[] bufferpos, uint[] count)
        {
            PodMultiDimArray mema = this;
            PodMultiDimArray memb = buffer;

            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims), new vectoruint32(bufferpos), new vectoruint32(count));

            uint indexa;
            uint indexb;
            uint len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(memb.pod_array, (long)indexb, mema.pod_array, (long)indexa, (long)len);                
            }
        }

    }

    public class NamedMultiDimArray
    {
        public NamedMultiDimArray()
        {
            Dims = new uint[] { 0 };
        }

        public NamedMultiDimArray(uint[] dims, Array array)
        {
            Dims = dims;
            namedarray_array = array;
        }

        public uint[] Dims;
        public Array namedarray_array;

        public virtual void RetrieveSubArray(uint[] memorypos, NamedMultiDimArray buffer, uint[] bufferpos, uint[] count)
        {

            NamedMultiDimArray mema = this;
            NamedMultiDimArray memb = buffer;

            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims), new vectoruint32(bufferpos), new vectoruint32(count));

            uint indexa;
            uint indexb;
            uint len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(mema.namedarray_array, (long)indexa, memb.namedarray_array, (long)indexb, (long)len);
            }

        }

        public virtual void AssignSubArray(uint[] memorypos, NamedMultiDimArray buffer, uint[] bufferpos, uint[] count)
        {
            NamedMultiDimArray mema = this;
            NamedMultiDimArray memb = buffer;

            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter( new vectoruint32(mema.Dims), new vectoruint32(memorypos), new vectoruint32(memb.Dims), new vectoruint32(bufferpos), new vectoruint32(count));

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