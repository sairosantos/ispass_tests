#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
//#include <arm_neon.h>
#include <immintrin.h>
#include "../../intrinsics/vima/vima.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

uint32_t* bloom_create (uint32_t n, float p, size_t *size, size_t *functions){
    *size = ceil((n * log(p)) / log(1 / pow(2, log(2))));
    *functions = round((*size / n) * log(2));

    //std::cout << *size << " bits.\n";
    //std::cout << *functions << " functions.\n";

    uint32_t* bf = (uint32_t*) malloc (*size/32 * sizeof (uint32_t));
    for (int i = 0; i < *size/32; i++) bf[i] = 0;

    return bf;
}

void bloom_set (uint32_t VECTOR_SIZE, uint32_t* entries, size_t entries_size, uint32_t* bloom_filter, size_t bloom_filter_size, uint32_t* factors, uint32_t* shift_m, size_t functions){
    uint32_t *mask_1, *mask_31;
    uint32_t *shift_vec, *shift5_vec; 
    uint32_t *key, *aux;
    uint32_t *fun, *fac;
    uint32_t *bit, *bit_div, *bit_mod;

    key = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    aux = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fun = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fac = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit_div = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit_mod = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_1 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_31 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    shift_vec = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    shift5_vec = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));

    if (VECTOR_SIZE == 2048){
        _vim2K_imovu (1, mask_1);
        _vim2K_imovu (31, mask_31);
        _vim2K_imovu (5, shift5_vec);
        
        for (int i = 0; i < entries_size; i += VECTOR_SIZE) {
            _vim2K_ilmku (&entries[i], mask_1, key); //load new entries according to the mask.
            _vim2K_irmku (fun, mask_1); //take the entries of the hash counters back to 0 if they were reset.
            for (int j = 0; j < functions; j++){
                _vim2K_icpyu (key, bit);
                _vim2K_ipmtu (factors, fun, fac);
                _vim2K_ipmtu (shift_m, fun, shift_vec);
                _vim2K_imulu (bit, fac, bit); //first step of hash
                _vim2K_isllu (bit, shift_vec, bit); //second step of hash
                _vim2K_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
                _vim2K_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
                _vim2K_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
                _vim2K_isllu (mask_1, bit_mod, bit); //bits are positioned!
                _vim2K_iscou (bit, bit_div, bloom_filter);
                _vim2K_iaddu (fun, mask_1, fun);
            }
        };
    }

    if (VECTOR_SIZE == 1024){
        _vim1K_imovu (1, mask_1);
        _vim1K_imovu (31, mask_31);
        _vim1K_imovu (5, shift5_vec);
        
        for (int i = 0; i < entries_size; i += VECTOR_SIZE) {
            _vim1K_ilmku (&entries[i], mask_1, key); //load new entries according to the mask.
            _vim1K_irmku (fun, mask_1); //take the entries of the hash counters back to 0 if they were reset.
            for (int j = 0; j < functions; j++){
                _vim1K_icpyu (key, bit);
                _vim1K_ipmtu (factors, fun, fac);
                _vim1K_ipmtu (shift_m, fun, shift_vec);
                _vim1K_imulu (bit, fac, bit); //first step of hash
                _vim1K_isllu (bit, shift_vec, bit); //second step of hash
                _vim1K_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
                _vim1K_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
                _vim1K_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
                _vim1K_isllu (mask_1, bit_mod, bit); //bits are positioned!
                _vim1K_iscou (bit, bit_div, bloom_filter);
                _vim1K_iaddu (fun, mask_1, fun);
            }
        };
    }

    if (VECTOR_SIZE == 512){
        _vim512_imovu (1, mask_1);
        _vim512_imovu (31, mask_31);
        _vim512_imovu (5, shift5_vec);
        
        for (int i = 0; i < entries_size; i += VECTOR_SIZE) {
            _vim512_ilmku (&entries[i], mask_1, key); //load new entries according to the mask.
            _vim512_irmku (fun, mask_1); //take the entries of the hash counters back to 0 if they were reset.
            for (int j = 0; j < functions; j++){
                _vim512_icpyu (key, bit);
                _vim512_ipmtu (factors, fun, fac);
                _vim512_ipmtu (shift_m, fun, shift_vec);
                _vim512_imulu (bit, fac, bit); //first step of hash
                _vim512_isllu (bit, shift_vec, bit); //second step of hash
                _vim512_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
                _vim512_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
                _vim512_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
                _vim512_isllu (mask_1, bit_mod, bit); //bits are positioned!
                _vim512_iscou (bit, bit_div, bloom_filter);
                _vim512_iaddu (fun, mask_1, fun);
            }
        };
    }

    if (VECTOR_SIZE == 256){
        _vim256_imovu (1, mask_1);
        _vim256_imovu (31, mask_31);
        _vim256_imovu (5, shift5_vec);
        
        for (int i = 0; i < entries_size; i += VECTOR_SIZE) {
            _vim256_ilmku (&entries[i], mask_1, key); //load new entries according to the mask.
            _vim256_irmku (fun, mask_1); //take the entries of the hash counters back to 0 if they were reset.
            for (int j = 0; j < functions; j++){
                _vim256_icpyu (key, bit);
                _vim256_ipmtu (factors, fun, fac);
                _vim256_ipmtu (shift_m, fun, shift_vec);
                _vim256_imulu (bit, fac, bit); //first step of hash
                _vim256_isllu (bit, shift_vec, bit); //second step of hash
                _vim256_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
                _vim256_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
                _vim256_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
                _vim256_isllu (mask_1, bit_mod, bit); //bits are positioned!
                _vim256_iscou (bit, bit_div, bloom_filter);
                _vim256_iaddu (fun, mask_1, fun);
            }
        };
    }

    if (VECTOR_SIZE == 128){
        _vim128_imovu (1, mask_1);
        _vim128_imovu (31, mask_31);
        _vim128_imovu (5, shift5_vec);
        
        for (int i = 0; i < entries_size; i += VECTOR_SIZE) {
            _vim128_ilmku (&entries[i], mask_1, key); //load new entries according to the mask.
            _vim128_irmku (fun, mask_1); //take the entries of the hash counters back to 0 if they were reset.
            for (int j = 0; j < functions; j++){
                _vim128_icpyu (key, bit);
                _vim128_ipmtu (factors, fun, fac);
                _vim128_ipmtu (shift_m, fun, shift_vec);
                _vim128_imulu (bit, fac, bit); //first step of hash
                _vim128_isllu (bit, shift_vec, bit); //second step of hash
                _vim128_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
                _vim128_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
                _vim128_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
                _vim128_isllu (mask_1, bit_mod, bit); //bits are positioned!
                _vim128_iscou (bit, bit_div, bloom_filter);
                _vim128_iaddu (fun, mask_1, fun);
            }
        };
    }

    if (VECTOR_SIZE == 64){
        _vim64_imovu (1, mask_1);
        _vim64_imovu (31, mask_31);
        _vim64_imovu (5, shift5_vec);
        
        for (int i = 0; i < entries_size; i += VECTOR_SIZE) {
            _vim64_ilmku (&entries[i], mask_1, key); //load new entries according to the mask.
            _vim64_irmku (fun, mask_1); //take the entries of the hash counters back to 0 if they were reset.
            for (int j = 0; j < functions; j++){
                _vim64_icpyu (key, bit);
                _vim64_ipmtu (factors, fun, fac);
                _vim64_ipmtu (shift_m, fun, shift_vec);
                _vim64_imulu (bit, fac, bit); //first step of hash
                _vim64_isllu (bit, shift_vec, bit); //second step of hash
                _vim64_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
                _vim64_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
                _vim64_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
                _vim64_isllu (mask_1, bit_mod, bit); //bits are positioned!
                _vim64_iscou (bit, bit_div, bloom_filter);
                _vim64_iaddu (fun, mask_1, fun);
            }
        };
    }

    free (key);
    free (aux);
    free (fun);
    free (fac);
    free (bit);
    free (bit_div);
    free (bit_mod);
    free (mask_1);
    free (mask_31);
    free (shift_vec);
    free (shift5_vec);
}

void bloom_chk(uint32_t VECTOR_SIZE, uint32_t* entries, size_t entries_size, uint32_t* bloom_filter, size_t bloom_filter_size, uint32_t* factors, uint32_t* shift_m, size_t functions, uint32_t* output, uint32_t* output_count){
    uint32_t *mask_k, *mask_kk; //mask that says which entries to continue calculating
    uint32_t *mask_0, *mask_1, *mask_31;
    uint32_t *shift_vec, *shift5_vec; 
    uint32_t *key;
    uint32_t *fun, *fac, *fun_max;
    uint32_t *bit, *bit_div, *bit_mod;

    key = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fun = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fac = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fun_max = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit_div = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit_mod = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_k = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_kk = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_0 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_1 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_31 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    shift_vec = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    shift5_vec = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));

    if (VECTOR_SIZE == 2048){
        _vim2K_imovu (0, key);
        _vim2K_imovu (1, mask_k);
        _vim2K_imovu (0, mask_kk);
        _vim2K_imovu (0, mask_0);
        _vim2K_imovu (1, mask_1);
        _vim2K_imovu (31, mask_31);
        _vim2K_imovu (5, shift5_vec);
        _vim2K_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        for (int i = 0; i <= entries_size; ) {
            _vim2K_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
            i += j;
            _vim2K_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim2K_icpyu (key, bit);
            _vim2K_ipmtu (factors, fun, fac);
            _vim2K_ipmtu (shift_m, fun, shift_vec);
            _vim2K_imulu (bit, fac, bit); //first step of hash
            _vim2K_isllu (bit, shift_vec, bit); //second step of hash
            _vim2K_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim2K_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim2K_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim2K_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim2K_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim2K_iandu (bit, bit_div, bit); //check if the bits are set
            _vim2K_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim2K_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim2K_idptu (mask_kk, &j);
            if (j > 0) {
                _vim2K_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim2K_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim2K_idptu (mask_k, &j);
            _vim2K_iaddu (fun, mask_1, fun);
        };
    }

    if (VECTOR_SIZE == 64){
        _vim64_imovu (0, key);
        _vim64_imovu (1, mask_k);
        _vim64_imovu (0, mask_kk);
        _vim64_imovu (0, mask_0);
        _vim64_imovu (1, mask_1);
        _vim64_imovu (31, mask_31);
        _vim64_imovu (5, shift5_vec);
        _vim64_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        for (int i = 0; i <= entries_size; ) {
            _vim64_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
            i += j;
            _vim64_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim64_icpyu (key, bit);
            _vim64_ipmtu (factors, fun, fac);
            _vim64_ipmtu (shift_m, fun, shift_vec);
            _vim64_imulu (bit, fac, bit); //first step of hash
            _vim64_isllu (bit, shift_vec, bit); //second step of hash
            _vim64_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim64_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim64_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim64_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim64_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim64_iandu (bit, bit_div, bit); //check if the bits are set
            _vim64_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim64_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim64_idptu (mask_kk, &j);
            if (j > 0) {
                _vim64_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim64_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim64_idptu (mask_k, &j);
            _vim64_iaddu (fun, mask_1, fun);
        };
    }
    
    free (key);
    free (fun);
    free (fac);
    free (fun_max);
    free (bit);
    free (bit_div);
    free (bit_mod);
    free (mask_k);
    free (mask_kk);
    free (mask_0);
    free (mask_1);
    free (mask_31);
    free (shift_vec);
    free (shift5_vec);
}

void bloom_chk_step (uint32_t VECTOR_SIZE, uint32_t* entries, size_t entries_size, uint32_t* bloom_filter, size_t bloom_filter_size, uint32_t* factors, uint32_t* shift_m, size_t functions, uint32_t* output, uint32_t* output_count){
    uint32_t *mask_k, *mask_kk; //mask that says which entries to continue calculating
    uint32_t *mask_0, *mask_1, *mask_31;
    uint32_t *shift_vec, *shift5_vec; 
    uint32_t *key;
    uint32_t *fun, *fac, *fun_max;
    uint32_t *bit, *bit_div, *bit_mod;

    key = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fun = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fac = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    fun_max = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit_div = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    bit_mod = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_k = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_kk = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_0 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_1 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    mask_31 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    shift_vec = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
    shift5_vec = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));

    if (VECTOR_SIZE == 2048){
        _vim2K_imovu (0, key);
        _vim2K_imovu (1, mask_k);
        _vim2K_imovu (0, mask_kk);
        _vim2K_imovu (0, mask_0);
        _vim2K_imovu (1, mask_1);
        _vim2K_imovu (31, mask_31);
        _vim2K_imovu (5, shift5_vec);
        _vim2K_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = 0;

        _vim2K_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
        do {
            i += j;
            _vim2K_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim2K_icpyu (key, bit);
            _vim2K_ipmtu (factors, fun, fac);
            _vim2K_ipmtu (shift_m, fun, shift_vec);
            _vim2K_imulu (bit, fac, bit); //first step of hash
            _vim2K_isllu (bit, shift_vec, bit); //second step of hash
            _vim2K_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim2K_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim2K_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim2K_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim2K_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim2K_iandu (bit, bit_div, bit); //check if the bits are set
            _vim2K_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim2K_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim2K_idptu (mask_kk, &j);
            if (j > 0) {
                _vim2K_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim2K_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim2K_idptu (mask_k, &j);
            _vim2K_iaddu (fun, mask_1, fun);
        } while (i < VECTOR_SIZE);
    }

    if (VECTOR_SIZE == 1024){
        _vim1K_imovu (0, key);
        _vim1K_imovu (1, mask_k);
        _vim1K_imovu (0, mask_kk);
        _vim1K_imovu (0, mask_0);
        _vim1K_imovu (1, mask_1);
        _vim1K_imovu (31, mask_31);
        _vim1K_imovu (5, shift5_vec);
        _vim1K_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        _vim1K_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
        do {
            i += j;
            _vim1K_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim1K_icpyu (key, bit);
            _vim1K_ipmtu (factors, fun, fac);
            _vim1K_ipmtu (shift_m, fun, shift_vec);
            _vim1K_imulu (bit, fac, bit); //first step of hash
            _vim1K_isllu (bit, shift_vec, bit); //second step of hash
            _vim1K_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim1K_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim1K_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim1K_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim1K_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim1K_iandu (bit, bit_div, bit); //check if the bits are set
            _vim1K_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim1K_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim1K_idptu (mask_kk, &j);
            if (j > 0) {
                _vim1K_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim1K_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim1K_idptu (mask_k, &j);
            _vim1K_iaddu (fun, mask_1, fun);
        } while (i < VECTOR_SIZE);
    }

    if (VECTOR_SIZE == 512){
        _vim512_imovu (0, key);
        _vim512_imovu (1, mask_k);
        _vim512_imovu (0, mask_kk);
        _vim512_imovu (0, mask_0);
        _vim512_imovu (1, mask_1);
        _vim512_imovu (31, mask_31);
        _vim512_imovu (5, shift5_vec);
        _vim512_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        _vim512_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
        do {
            i += j;
            _vim512_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim512_icpyu (key, bit);
            _vim512_ipmtu (factors, fun, fac);
            _vim512_ipmtu (shift_m, fun, shift_vec);
            _vim512_imulu (bit, fac, bit); //first step of hash
            _vim512_isllu (bit, shift_vec, bit); //second step of hash
            _vim512_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim512_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim512_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim512_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim512_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim512_iandu (bit, bit_div, bit); //check if the bits are set
            _vim512_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim512_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim512_idptu (mask_kk, &j);
            if (j > 0) {
                _vim512_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim512_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim512_idptu (mask_k, &j);
            _vim512_iaddu (fun, mask_1, fun);
        } while (i < VECTOR_SIZE);
    }

    if (VECTOR_SIZE == 256){
        _vim256_imovu (0, key);
        _vim256_imovu (1, mask_k);
        _vim256_imovu (0, mask_kk);
        _vim256_imovu (0, mask_0);
        _vim256_imovu (1, mask_1);
        _vim256_imovu (31, mask_31);
        _vim256_imovu (5, shift5_vec);
        _vim256_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        _vim256_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
        do {
            i += j;
            _vim256_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim256_icpyu (key, bit);
            _vim256_ipmtu (factors, fun, fac);
            _vim256_ipmtu (shift_m, fun, shift_vec);
            _vim256_imulu (bit, fac, bit); //first step of hash
            _vim256_isllu (bit, shift_vec, bit); //second step of hash
            _vim256_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim256_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim256_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim256_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim256_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim256_iandu (bit, bit_div, bit); //check if the bits are set
            _vim256_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim256_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim256_idptu (mask_kk, &j);
            if (j > 0) {
                _vim256_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim256_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim256_idptu (mask_k, &j);
            _vim256_iaddu (fun, mask_1, fun);
        } while (i < VECTOR_SIZE);
    }

    if (VECTOR_SIZE == 128){
        _vim128_imovu (0, key);
        _vim128_imovu (1, mask_k);
        _vim128_imovu (0, mask_kk);
        _vim128_imovu (0, mask_0);
        _vim128_imovu (1, mask_1);
        _vim128_imovu (31, mask_31);
        _vim128_imovu (5, shift5_vec);
        _vim128_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        _vim128_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
        do {
            i += j;
            _vim128_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim128_icpyu (key, bit);
            _vim128_ipmtu (factors, fun, fac);
            _vim128_ipmtu (shift_m, fun, shift_vec);
            _vim128_imulu (bit, fac, bit); //first step of hash
            _vim128_isllu (bit, shift_vec, bit); //second step of hash
            _vim128_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim128_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim128_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim128_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim128_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim128_iandu (bit, bit_div, bit); //check if the bits are set
            _vim128_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim128_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim128_idptu (mask_kk, &j);
            if (j > 0) {
                _vim128_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim128_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim128_idptu (mask_k, &j);
            _vim128_iaddu (fun, mask_1, fun);
        } while (i < VECTOR_SIZE);
    }

    if (VECTOR_SIZE == 64){
        _vim64_imovu (0, key);
        _vim64_imovu (1, mask_k);
        _vim64_imovu (0, mask_kk);
        _vim64_imovu (0, mask_0);
        _vim64_imovu (1, mask_1);
        _vim64_imovu (31, mask_31);
        _vim64_imovu (5, shift5_vec);
        _vim64_imovu (functions - 1, fun_max);
        int i = 0;
        *output_count = 0;
        uint32_t j = VECTOR_SIZE;

        _vim64_ilmku (&entries[i], mask_k, key); //load new entries according to the mask.
        do {
            i += j;
            _vim64_irmku (fun, mask_k); //take the entries of the hash counters back to 0 if they were reset.
            _vim64_icpyu (key, bit);
            _vim64_ipmtu (factors, fun, fac);
            _vim64_ipmtu (shift_m, fun, shift_vec);
            _vim64_imulu (bit, fac, bit); //first step of hash
            _vim64_isllu (bit, shift_vec, bit); //second step of hash
            _vim64_imodu (bit, bloom_filter_size, bit); //which bit within the bloom filter to set!
            _vim64_isrlu (bit, shift5_vec, bit_div); //which integer inside the bloom filter?
            _vim64_iandu (bit, mask_31, bit_mod); //which bit inside that integer?
            _vim64_isllu (mask_1, bit_mod, bit); //bits are positioned!
            _vim64_igtru (bloom_filter, bit_div, bit_div); //we gather those integers from the bloom filter!
            _vim64_iandu (bit, bit_div, bit); //check if the bits are set
            _vim64_icmqu (bit, mask_0, mask_k); //if they're not, those entries don't go on
            _vim64_icmqu (fun, fun_max, mask_kk); //which entries have gotten to the last hash function?
            
            _vim64_idptu (mask_kk, &j);
            if (j > 0) {
                _vim64_ismku (key, mask_kk, &output[*output_count]);
                *output_count += j;
            }

            _vim64_iorun (mask_k, mask_kk, mask_k); //which ones passed that also? those are ready and must be added to the result.
            _vim64_idptu (mask_k, &j);
            _vim64_iaddu (fun, mask_1, fun);
        } while (i < VECTOR_SIZE);
    }
    
    free (key);
    free (fun);
    free (fac);
    free (fun_max);
    free (bit);
    free (bit_div);
    free (bit_mod);
    free (mask_k);
    free (mask_kk);
    free (mask_0);
    free (mask_1);
    free (mask_31);
    free (shift_vec);
    free (shift5_vec);
}

int main (__v32s argc, char const *argv[]){
    ORCS_tracing_stop();

    srand(time(NULL));
    uint32_t vector_size, prob, VIMA_VECTOR_SIZE;
    uint32_t *bitmap, *o_orderkey, *l_orderkey, *filter_vec;
    uint32_t prime_numbers[] = {2, 3, 5, 7, 9, 11, 13, 17, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    uint32_t shift[] = {3, 5, 6, 5, 0, 6, 0, 6, 3, 3, 2, 6, 1};
    vector_size = atoi(argv[1]);
    prob = 0;
    VIMA_VECTOR_SIZE = atoi(argv[2]);
    
    __v32u v_size = (1024 * 1024 * vector_size)/sizeof(__v32u);
    o_orderkey = (uint32_t*) malloc ((uint32_t) v_size/4 * sizeof (uint32_t));
    l_orderkey = (uint32_t*) malloc (v_size * sizeof (uint32_t));

    uint32_t max = 0;
    for (int i = 0; i < v_size/4; i++) {
        o_orderkey[i] = i + (rand() % UINT32_MAX/10);
        if (o_orderkey[i] > max) max = o_orderkey[i];
    }
    for (int i = 0; i < v_size; i++) {
        if (i % 10 < prob) l_orderkey[i] = o_orderkey[rand() % v_size/4];
        else l_orderkey[i] = max + (rand() % UINT32_MAX/10);
    }

    size_t bloom_filter_size = 0;
    size_t hash_functions = 0;
    double p = 0.0001;
    uint32_t output_count = 0;
    
    uint32_t *bloom_filter = bloom_create ((uint32_t) v_size/4, p, &bloom_filter_size, &hash_functions);
    uint32_t *output = (uint32_t*) malloc (v_size * sizeof (uint32_t));
    uint32_t *hash_function_factors = (uint32_t*) malloc (hash_functions * sizeof (uint32_t));    
    uint32_t *shift_amounts = (uint32_t*) malloc (hash_functions * sizeof (uint32_t));

    std::cout << "v_size = " << v_size << "\n";
    std::cout << "bf_size = " << bloom_filter_size << "\n";
    std::cout << "functions = " << hash_functions << "\n";

    for (int i = 0; i < hash_functions; i++) {
        hash_function_factors[i] = prime_numbers[i % 15];
        shift_amounts[i] = shift[i % 13];
    }

    ORCS_tracing_start();

    bloom_set (VIMA_VECTOR_SIZE, o_orderkey, (uint32_t) v_size/4, bloom_filter, bloom_filter_size, hash_function_factors, shift_amounts, hash_functions);
    for (int x = 0; x < v_size; x += VIMA_VECTOR_SIZE){
        bloom_chk_step (VIMA_VECTOR_SIZE, &l_orderkey[x], v_size, bloom_filter, bloom_filter_size, hash_function_factors, shift_amounts, hash_functions, output, &output_count);
    }

    ORCS_tracing_stop();
    std::cout << output_count << " positives.\n";

    free (o_orderkey);
    free (l_orderkey);
    free (hash_function_factors);
    free (shift_amounts);
}
