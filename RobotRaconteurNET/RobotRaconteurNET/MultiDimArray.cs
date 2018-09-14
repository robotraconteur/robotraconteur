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

        public MultiDimArray(int[] Dims, Array Real, Array Imag = null)
        {
            this.DimCount = Dims.Length;
            this.Dims = Dims;
            this.Real = Real;

            if (Imag != null)
            {
                this.Complex = true;
                this.Imag = Imag;
            }

        }

        /// <summary>
        /// The number of array dimensions
        /// </summary>
        public int DimCount;
        /// <summary>
        /// The dimensions of the array
        /// </summary>
        public int[] Dims;
        /// <summary>
        /// The column vector representation of real values
        /// </summary>
        public Array Real;

        /// <summary>
        /// True if this array is complex, false if it is real
        /// </summary>
        public bool Complex = false;
        /// <summary>
        /// The column vector representation of the imaginary values
        /// </summary>
        public Array Imag;

        public virtual void RetrieveSubArray(int[] memorypos, MultiDimArray buffer, int[] bufferpos, int[] count)
        {

            MultiDimArray mema = this;
            MultiDimArray memb = buffer;

            if (mema.Complex != memb.Complex) throw new ArgumentException("Complex mismatch");

            MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(mema.DimCount, new vectorint32(mema.Dims), new vectorint32(memorypos), memb.DimCount, new vectorint32(memb.Dims), new vectorint32(bufferpos), new vectorint32(count));

            int indexa;
            int indexb;
            int len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(mema.Real, (long)indexa, memb.Real, (long)indexb, (long)len);
                if (mema.Complex)
                    Array.Copy(mema.Imag, (long)indexa, memb.Imag, (long)indexb, (long)len);
            }

        }

        public virtual void AssignSubArray(int[] memorypos, MultiDimArray buffer, int[] bufferpos, int[] count)
        {


            MultiDimArray mema = this;
            MultiDimArray memb = buffer;

            if (mema.Complex != memb.Complex) throw new ArgumentException("Complex mismatch");

            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(mema.DimCount, new vectorint32(mema.Dims), new vectorint32(memorypos), memb.DimCount, new vectorint32(memb.Dims), new vectorint32(bufferpos), new vectorint32(count));

            int indexa;
            int indexb;
            int len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(memb.Real, (long)indexb, mema.Real, (long)indexa, (long)len);
                if (mema.Complex)
                    Array.Copy(memb.Imag, (long)indexb, mema.Imag, (long)indexa, (long)len);
            }
        }

    }

    public class CStructureMultiDimArray
    {
        public CStructureMultiDimArray()
        {
            Dims = new int[] { 0 };
        }

        public CStructureMultiDimArray(int[] dims, Array array)
        {
            Dims = dims;
            cstruct_array = array;
        }
        
        public int[] Dims;
        public Array cstruct_array;

        public virtual void RetrieveSubArray(int[] memorypos, CStructureMultiDimArray buffer, int[] bufferpos, int[] count)
        {

            CStructureMultiDimArray mema = this;
            CStructureMultiDimArray memb = buffer;
           
            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(mema.Dims.Length, new vectorint32(mema.Dims), new vectorint32(memorypos), memb.Dims.Length, new vectorint32(memb.Dims), new vectorint32(bufferpos), new vectorint32(count));

            int indexa;
            int indexb;
            int len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(mema.cstruct_array, (long)indexa, memb.cstruct_array, (long)indexb, (long)len);                
            }

        }

        public virtual void AssignSubArray(int[] memorypos, CStructureMultiDimArray buffer, int[] bufferpos, int[] count)
        {
            CStructureMultiDimArray mema = this;
            CStructureMultiDimArray memb = buffer;

            MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurNET.MultiDimArray_CalculateCopyIndicesBeginIter(mema.Dims.Length, new vectorint32(mema.Dims), new vectorint32(memorypos), memb.Dims.Length, new vectorint32(memb.Dims), new vectorint32(bufferpos), new vectorint32(count));

            int indexa;
            int indexb;
            int len;

            while (iter.Next(out indexa, out indexb, out len))
            {
                Array.Copy(memb.cstruct_array, (long)indexb, mema.cstruct_array, (long)indexa, (long)len);                
            }
        }

    }
}