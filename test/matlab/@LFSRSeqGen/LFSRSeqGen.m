classdef LFSRSeqGen < handle
    %LFSRSEQGEN Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        lfsr
    end
    
    methods
        function obj = LFSRSeqGen(seed,key)
           if nargin == 2
              obj.lfsr = LFSRSeqGen.lfsr_initkey(uint32(seed),key); 
           else
              obj.lfsr = uint32(seed);
           end
        end
        
        function ret = lfsr_next1(obj)
            obj.lfsr = LFSRSeqGen.lfsr_next(obj.lfsr);
            ret = obj.lfsr;
        end
        
        function ret = lfsr_next2(obj)
            lfsr2 = obj.lfsr_next1();
            lfsr3 = obj.lfsr_next1();
            ret = bitor(bitshift(uint64(lfsr2),32), uint64(lfsr3));
        end
        
        function ret = next_int8(obj)
           ret = typecast(obj.lfsr_next1,'int8');
           ret = ret(1);
        end
        
        function ret = next_uint8(obj)
            ret = typecast(obj.lfsr_next1,'uint8');
            ret = ret(1);
        end
        
        function ret = next_int16(obj)
            ret = typecast(obj.lfsr_next1,'int16');
            ret = ret(1);
        end
        
        function ret = next_uint16(obj)
            ret = typecast(obj.lfsr_next1,'uint16');
            ret = ret(1);
        end
        
        function ret = next_int32(obj)
            ret = typecast(obj.lfsr_next1,'int32');
        end
        
        function ret = next_uint32(obj)
            ret = typecast(obj.lfsr_next1,'uint32');
        end
        
        function ret = next_int64(obj)
            ret = typecast(obj.lfsr_next2,'int64');
        end
        
        function ret = next_uint64(obj)
            ret = typecast(obj.lfsr_next2,'uint64');
        end
        
        function ret = next_float(obj)
            ret = typecast(obj.lfsr_next1,'single');
        end
        
        function ret = next_double(obj)
            ret = typecast(obj.lfsr_next2,'double');
        end
        
        function ret = next_bool(obj)
           ret = bitand(obj.next_uint8(),1) ~= 0;
        end
        
        function ret = next_char(obj)
            chars = ' !"#$%&''()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~';
            lfsr2 = obj.lfsr_next1();
            ind_ = mod(lfsr2, strlength(chars));
            ret = chars(ind_ + 1);
        end
        
        function ret = next_cfloat(obj)
            ret = obj.next_float() + obj.next_float()*1i;
        end
        
        function ret = next_cdouble(obj)
            ret = obj.next_double() + obj.next_double()*1i;
        end
        
        function ret = next_len(obj, max_len)
           lfsr2 = obj.lfsr_next1();
           if max_len < 32
               ret = mod(lfsr2,max_len);
           else
              ret = 8 + mod(lfsr2, max_len-8); 
           end
        end
        
        function ret = next_dist(obj, min_, max_)
           assert (max_ > min_);
           lfsr2 = obj.lfsr_next1();
           ret = mod(lfsr2, ((max_-min_)+1)) + min_;
        end
        
        function ret = next_int8_array(obj,count)
           ret = int8(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int8(); 
           end
        end
        
        function ret = next_uint8_array(obj,count)
           ret = uint8(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint8(); 
           end
        end
        
        function ret = next_int16_array(obj,count)
           ret = uint16(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int16(); 
           end
        end
        
        function ret = next_uint16_array(obj,count)
           ret = int16(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint16(); 
           end
        end
        
        function ret = next_int32_array(obj,count)
           ret = int32(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int32(); 
           end
        end
        
        function ret = next_uint32_array(obj,count)
           ret = int32(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint32(); 
           end
        end
        
        function ret = next_int64_array(obj,count)
           ret = int64(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int64(); 
           end
        end
        
        function ret = next_uint64_array(obj,count)
           ret = uint64(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint64(); 
           end
        end
        
        function ret = next_float_array(obj,count)
           ret = single(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_float(); 
           end
        end
        
        function ret = next_double_array(obj,count)
           ret = double(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_double(); 
           end
        end
        
        function ret = next_cfloat_array(obj,count)
           ret = single(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_cfloat(); 
           end
        end
        
        function ret = next_cdouble_array(obj,count)
           ret = double(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_cdouble(); 
           end
        end
        
        function ret = next_bool_array(obj,count)
           ret = logical(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_bool(); 
           end
        end
        
        function ret = next_string(obj,count)
           ret = char(zeros(1,count));
           for i=1:count
              ret(i) = obj.next_char(); 
           end
        end
        
        function ret = next_int8_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = int8(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int8(); 
           end
        end
        
        function ret = next_uint8_array_var_len(obj,max_len)
            count = obj.next_len(max_len);
           ret = uint8(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint8(); 
           end
        end
        
        function ret = next_int16_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = uint16(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int16(); 
           end
        end
        
        function ret = next_uint16_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = int16(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint16(); 
           end
        end
        
        function ret = next_int32_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = int32(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int32(); 
           end
        end
        
        function ret = next_uint32_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = int32(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint32(); 
           end
        end
        
        function ret = next_int64_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = int64(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_int64(); 
           end
        end
        
        function ret = next_uint64_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = uint64(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_uint64(); 
           end
        end
        
        function ret = next_float_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = single(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_float(); 
           end
        end
        
        function ret = next_double_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = double(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_double(); 
           end
        end
        
        function ret = next_cfloat_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = single(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_cfloat(); 
           end
        end
        
        function ret = next_cdouble_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = double(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_cdouble(); 
           end
        end
        
        function ret = next_bool_array_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = logical(zeros(count,1));
           for i=1:count
              ret(i) = obj.next_bool(); 
           end
        end
        
        function ret = next_string_var_len(obj,max_len)
           count = obj.next_len(max_len);
           ret = char(zeros(1,count));
           for i=1:count
              ret(i) = obj.next_char(); 
           end
        end
    end
    
    methods(Static)
        function ret = lfsr_next(lfsr)
            lfsr = uint64(lfsr);
            bit = bitand(bitxor(bitxor(bitxor(bitshift(lfsr, 0), bitshift(lfsr, -1)), bitshift(lfsr, -21)), bitshift(lfsr, -31)), 1);
            ret = uint32(LFSRSeqGen.clip_uint32(bitor(bitshift(lfsr, -1), bitshift(bit, 31))));
        end
        
        function ret = clip_uint32(x)
           ret=uint32(bitand(uint64(x),uint64(0xFFFFFFFF)));
        end
        
        function ret = lfsr_initkey(seed, key)            
            % Run the LFSR a few times on the seed to shuffle
            for i = 1:128
                seed = LFSRSeqGen.lfsr_next(seed);
            end

            i = 0;
            hash_ = LFSRSeqGen.lfsr_next(seed);
            while (i ~= strlength(key))
                disp('Begin loop')
                disp(i)
                hash_ = uint64(LFSRSeqGen.clip_uint32(uint64(hash_))) + uint64(key(1,i+1));
                disp(hash_)
                i = i + 1;
                hash_ = LFSRSeqGen.clip_uint32(uint64(hash_) + uint64(LFSRSeqGen.clip_uint32(bitshift(uint64(hash_), 10))));
                disp(hash_)
                hash_ = bitxor(uint64(hash_), uint64(LFSRSeqGen.clip_uint32(bitshift(uint64(hash_), -6))));
                disp(hash_)
            end
            disp(hash_)

            hash_ = LFSRSeqGen.clip_uint32(uint64(hash_) + uint64(LFSRSeqGen.clip_uint32(bitshift(uint64(hash_), 3))));
            hash_ = bitxor(uint64(hash_), bitshift(uint64(hash_), -11));
            hash_ = LFSRSeqGen.clip_uint32(uint64(hash_) + uint64(LFSRSeqGen.clip_uint32(bitshift(uint64(hash_), 15))));

            % Shuffle again
            for i= 1:128
                hash_ = LFSRSeqGen.lfsr_next(hash_);
            end

            ret = LFSRSeqGen.lfsr_next(hash_);
        end
        
    end
end

