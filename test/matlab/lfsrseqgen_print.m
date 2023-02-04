function lfsrseqgen_print(seed, key)

if nargin < 2
   key = 'Hello World!';
end

if nargin < 1
   seed = 35913226;
end

count = 8;

seqgen = LFSRSeqGen(seed,key);

disp(seqgen.next_int8_array(count));
disp(seqgen.next_uint8_array(count));
disp(seqgen.next_int16_array(count));
disp(seqgen.next_uint16_array(count));
disp(seqgen.next_int32_array(count));
disp(seqgen.next_uint32_array(count));
disp(seqgen.next_int64_array(count));
disp(seqgen.next_uint64_array(count));
disp(seqgen.next_float_array(count));
disp(seqgen.next_double_array(count));
disp(seqgen.next_string(count));
disp(seqgen.next_bool_array(count));
disp(seqgen.next_cfloat_array(count));
disp(seqgen.next_cdouble_array(count));
disp('')
disp(seqgen.next_int8_array_var_len(count));
disp(seqgen.next_uint8_array_var_len(count));
disp(seqgen.next_int16_array_var_len(count));
disp(seqgen.next_uint16_array_var_len(count));
disp(seqgen.next_int32_array_var_len(count));
disp(seqgen.next_uint32_array_var_len(count));
disp(seqgen.next_int64_array_var_len(count));
disp(seqgen.next_uint64_array_var_len(count));
disp(seqgen.next_float_array_var_len(count));
disp(seqgen.next_double_array_var_len(count));
disp(seqgen.next_string_var_len(count));
disp(seqgen.next_bool_array_var_len(count));
disp(seqgen.next_cfloat_array_var_len(count));
disp(seqgen.next_cdouble_array_var_len(count));
