/** 
 * @file robotraconteur_test_lfsr.h
 * 
 * @author John Wason, PhD
 * 
 * @copyright Copyright 2011-2021 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifdef _WIN32
#ifdef ROBOTRACONTEUR_TEST_LFSR_EXPORTS
#define ROBOTRACONTEUR_TEST_LFSR_API __declspec(dllexport)
#elif ROBOTRACONTEUR_TEST_LFSR_IMPORTS
#define ROBOTRACONTEUR_TEST_LFSR_API __declspec(dllimport)
#else
#define ROBOTRACONTEUR_TEST_LFSR_API
#endif
#else
#define ROBOTRACONTEUR_TEST_LFSR_API
#endif

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Sequence generator using a simple Fibonacci linear-feedback shift register (LFSR)
 * 
 * See https://en.wikipedia.org/wiki/Linear-feedback_shift_register
 * 
 * Polynomial [32, 22, 2, 1, 0] used for LFSR feedback
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Initialize the lfsr value from a seed and a string key
     * 
     * Used to initialize lfsr with a seed value and a name for
     * working with object members. The key is typically the
     * member name, or a the member name with "_get", "_set",
     * etc appended
     * 
     * Implemented using Jenkins on_at_a_time hash algorithm
     * 
     * https://en.wikipedia.org/wiki/Jenkins_hash_function
     * 
     * The initial hash for the one_at_a_time algorthim is the seed after 
     * lfsr_next 128 times
     * 
     * The output hash is passed through lfsr_next 128 times
     * 
     * @param seed Initial seed
     * @param key String key to hash
     * @param key_len String key length
     * @return uint32_t 
     */
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_initkey(uint32_t seed, const char* key, uint32_t key_len);

    /**
     * @brief Execute the lfsr and receive the next lfsr value
     * 
     * @param lfsr Input lfsr value
     * @return uint32_t Next lfsr value
     */
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next(uint32_t lfsr);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_int8(uint32_t lfsr, int8_t* val_out);
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_uint8(uint32_t lfsr, uint8_t* val_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_int16(uint32_t lfsr, int16_t* val_out);
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_uint16(uint32_t lfsr, uint16_t* val_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_int32(uint32_t lfsr, int32_t* val_out);
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_uint32(uint32_t lfsr, uint32_t* val_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_int64(uint32_t lfsr, int64_t* val_out);
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_uint64(uint32_t lfsr, uint64_t* val_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_float(uint32_t lfsr, float* val_out);
    
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_double(uint32_t lfsr, double* val_out);
    
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_bool(uint32_t lfsr, uint8_t* val_out);
    
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_cfloat(uint32_t lfsr, float* r_val_out, float* i_val_out);
    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_cdouble(uint32_t lfsr, double* r_val_out, double* i_val_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_char(uint32_t lfsr, char* val_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_len(uint32_t lfsr, size_t max_len, size_t* len_out);

    ROBOTRACONTEUR_TEST_LFSR_API uint32_t robotraconteur_test_lfsr_next_dist(uint32_t lfsr, uint32_t min_, uint32_t max_, uint32_t* val_out);

#ifdef __cplusplus
}
#endif