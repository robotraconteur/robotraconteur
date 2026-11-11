classdef ServiceTest2_test_sequence_gen < handle
    %SERVICETEST2_TEST_SEQUENCE_GEN Summary of this class goes here
    %   Detailed explanation goes here

    % cSpell: ignore numel

    properties
        counter
        step
        double_constants = test_vectors_get('seq.double_constants', 'double')
        uint32_constants = test_vectors_get('seq.uint32_constants', 'uint32')
    end

    methods
        function obj = ServiceTest2_test_sequence_gen(seed)
            %SERVICETEST2_TEST_SEQUENCE_GEN Construct an instance of this class
            %   Detailed explanation goes here
            obj.counter = mod(seed,7823);
            obj.step = mod(seed, 17) + 7;
        end

        function d = get_double(obj)
            obj.counter = obj.counter + obj.step;
            d = obj.double_constants(mod(obj.counter, numel(obj.double_constants))+1);
        end

        function d = get_uint32(obj)
            obj.counter = obj.counter + obj.step;
            d = obj.uint32_constants(mod(obj.counter,numel(obj.uint32_constants)) + 1);
        end

        function d = get_int8(obj)
            obj.counter = obj.counter + obj.step;
            d = int8(int64(mod(obj.counter, 255)) - 128);
        end
    end
end
