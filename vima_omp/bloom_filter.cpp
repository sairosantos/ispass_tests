#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>
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

void bloom_set (uint32_t* entries, uint32_t* bloom_filter, size_t bloom_filter_size, uint32_t* factors, uint32_t* shift_m, size_t functions){
    uint32_t *mask_1, *mask_31;
    uint32_t *shift_vec, *shift5_vec; 
    uint32_t *key, *aux;
    uint32_t *fun, *fac;
    uint32_t *bit, *bit_div, *bit_mod;

    key = (uint32_t*) malloc (64 * sizeof(uint32_t));
    aux = (uint32_t*) malloc (64 * sizeof(uint32_t));
    fun = (uint32_t*) malloc (64 * sizeof(uint32_t));
    fac = (uint32_t*) malloc (64 * sizeof(uint32_t));
    bit = (uint32_t*) malloc (64 * sizeof(uint32_t));
    bit_div = (uint32_t*) malloc (64 * sizeof(uint32_t));
    bit_mod = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_1 = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_31 = (uint32_t*) malloc (64 * sizeof(uint32_t));
    shift_vec = (uint32_t*) malloc (64 * sizeof(uint32_t));
    shift5_vec = (uint32_t*) malloc (64 * sizeof(uint32_t));

    _vim64_imovu (1, mask_1);
    _vim64_imovu (31, mask_31);
    _vim64_imovu (5, shift5_vec);
    
    _vim64_ilmku (entries, mask_1, key); //load new entries according to the mask.
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

void bloom_chk(uint32_t* entries, uint32_t* bloom_filter, size_t bloom_filter_size, uint32_t* factors, uint32_t* shift_m, size_t functions, uint32_t* output, uint32_t* output_count){
    uint32_t *mask_k, *mask_kk; //mask that says which entries to continue calculating
    uint32_t *mask_0, *mask_1, *mask_31;
    uint32_t *shift_vec, *shift5_vec; 
    uint32_t *key;
    uint32_t *fun, *fac, *fun_max;
    uint32_t *bit, *bit_div, *bit_mod;

    key = (uint32_t*) malloc (64 * sizeof(uint32_t));
    fun = (uint32_t*) malloc (64 * sizeof(uint32_t));
    fac = (uint32_t*) malloc (64 * sizeof(uint32_t));
    fun_max = (uint32_t*) malloc (64 * sizeof(uint32_t));
    bit = (uint32_t*) malloc (64 * sizeof(uint32_t));
    bit_div = (uint32_t*) malloc (64 * sizeof(uint32_t));
    bit_mod = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_k = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_kk = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_0 = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_1 = (uint32_t*) malloc (64 * sizeof(uint32_t));
    mask_31 = (uint32_t*) malloc (64 * sizeof(uint32_t));
    shift_vec = (uint32_t*) malloc (64 * sizeof(uint32_t));
    shift5_vec = (uint32_t*) malloc (64 * sizeof(uint32_t));

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
    uint32_t j = 0;

    do {
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
    } while (i < 64);
    
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
    //VIMA_VECTOR_SIZE = atoi(argv[2]);
    
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
    double p = 0.0000001;
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

    __v32s i;
    int tid, start, finish;
    #pragma omp parallel shared (o_orderkey, bloom_filter) private (i, tid, start, finish)
    {
        int chunk_size = (v_size/4) / omp_get_num_threads();
        tid = omp_get_thread_num();
        start = tid*chunk_size;
        finish = start + chunk_size;

        start = start + tid * 64;
        finish = finish + (tid+1) * 64;
        for (int i = start; i < finish; i += 64) {
            bloom_set (&o_orderkey[i], bloom_filter, bloom_filter_size, hash_function_factors, shift_amounts, hash_functions);
        }
    }

    #pragma omp parallel shared (l_orderkey, bloom_filter, output, output_count) private (i, tid, start, finish)
    {
        int chunk_size = v_size / omp_get_num_threads();
        tid = omp_get_thread_num();
        start = tid*chunk_size;
        finish = start + chunk_size;

        start = start + tid * 64;
        finish = finish + (tid+1) * 64;
        for (int i = start; i < finish; i += 64) {
            bloom_chk (&l_orderkey[i], bloom_filter, bloom_filter_size, hash_function_factors, shift_amounts, hash_functions, output, &output_count);
        }
    }

    ORCS_tracing_stop();
    std::cout << output_count << " positives.\n";

    free (o_orderkey);
    free (l_orderkey);
    free (hash_function_factors);
    free (shift_amounts);
}
