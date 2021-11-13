#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
//#include <arm_neon.h>
#include <immintrin.h>
#include <omp.h>
#include "../../intrinsics/vima/vima.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

#define VECTOR_SIZE 16
using namespace std;

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

void bloom_chk_step (int32_t *input_keys, size_t input_size, size_t functions, int32_t *mask_factors, int32_t* shift_m, int32_t *bloom_filter, size_t bloom_filter_size, int32_t* output, size_t *output_count){
	size_t i = 0;
	__mmask16 kk;
	__mmask16 k = _mm512_int2mask (0xFFFF);
	__m512i mask_1 = _mm512_set1_epi32 (1);
	__m512i mask_31 = _mm512_set1_epi32 (31);
	__m512i fun = _mm512_set1_epi32 (0);
	__m512i key = _mm512_mask_loadu_epi32(key, k, &input_keys[i]);
	__m512i mul_factors = _mm512_loadu_si512 ((__m512i*) mask_factors);
	__m512i shift_amounts = _mm512_loadu_si512 ((__m512i*) shift_m);
	__m512i fun_max = _mm512_set1_epi32(functions - 1);
	__m512i fac, shi, bit, bit_div, bit_mod;

	uint32_t* aux_vec1 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));

	key = _mm512_load_si512 (&input_keys[0]);
	
	do {
		key = _mm512_maskz_compress_epi32 (k, key);
		fun = _mm512_maskz_compress_epi32 (k, fun);
		fac = _mm512_permutexvar_epi32(fun, mul_factors);
		shi = _mm512_permutexvar_epi32(fun, shift_amounts);
        bit = _mm512_mullo_epi32 (key, fac);
		bit = _mm512_mullo_epi32 (bit, fac);
		bit = _mm512_sllv_epi32 (bit, shi);
		_mm512_storeu_si512	 (aux_vec1, bit);
	    for (int i = 0; i < VECTOR_SIZE; i++) aux_vec1[i] %= bloom_filter_size;
		bit = _mm512_loadu_si512(aux_vec1);
		__m512i bit_div = _mm512_srli_epi32 (bit, 5);
		bit_div = _mm512_i32gather_epi32 (bit_div, bloom_filter, 4); //buscamos os inteiros
        __m512i bit_mod = _mm512_and_epi32 (bit, mask_31);
		bit_mod = _mm512_sllv_epi32 (mask_1, bit_mod);
		k = _mm512_test_epi32_mask(bit_div, bit_mod); //comparação; sim significa que o essa entrada continua para a próxima função hash
		kk = _mm512_mask_cmpeq_epi32_mask(k, fun, fun_max); //vemos quais entradas chegaram ao índice final
		_mm512_mask_compressstoreu_epi32(&output[*output_count], kk, key);
		*output_count += _mm_popcnt_u32(_mm512_mask2int (kk));
		
		k = _mm512_kor(_mm512_knot(k), kk);
		i += _mm_popcnt_u32 (_mm512_mask2int (k));
		
		fun = _mm512_add_epi32(fun, mask_1); //somamos 1 a todas aos índices de função hash
		k = _mm512_knot(k);
	} while (i < VECTOR_SIZE);
}

int* bloom_create (int n, float p, size_t *size, size_t *functions){
    *size = ceil((n * log(p)) / log(1 / pow(2, log(2))));
    *functions = round((*size / n) * log(2));

    int* bf = (int*) aligned_alloc (32, *size/32 * sizeof (int));
    for (int i = 0; i < *size/32; i++) bf[i] = 0;

    return bf;
}

void bloom_set_step (int32_t* entries, int entries_size, int32_t* bloom_filter, size_t bloom_filter_size, int32_t* factors, int32_t* shift_m, size_t functions){
	__m512i mask_1 = _mm512_set1_epi32(1);
	__m512i mask_31 = _mm512_set1_epi32(31);

	__m512i facts = _mm512_loadu_si512((__m512i*) factors);
	__m512i shift = _mm512_loadu_si512((__m512i*) shift_m);
	__m512i key = _mm512_load_epi32 (&entries[0]);
	__m512i fun = _mm512_set1_epi32(0);
	uint32_t *aux_vec1 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
	uint32_t *aux_vec2 = (uint32_t*) malloc (VECTOR_SIZE * sizeof(uint32_t));
	
	for (int j = 0; j < functions; j++) {
        __m512i fac = _mm512_permutexvar_epi32(fun, facts);
		__m512i shi = _mm512_permutexvar_epi32(fun, shift);
        __m512i bit = _mm512_mullo_epi32 (key, fac);
		bit = _mm512_mullo_epi32 (bit, fac);
		bit = _mm512_sllv_epi32 (bit, shi);
		_mm512_storeu_si512	 (aux_vec1, bit);
	    for (int i = 0; i < VECTOR_SIZE; i++) aux_vec1[i] %= bloom_filter_size;
		bit = _mm512_loadu_si512(aux_vec1);
		__m512i bit_div = _mm512_srli_epi32 (bit, 5);
        __m512i bit_mod = _mm512_and_epi32 (bit, mask_31);
		bit_mod = _mm512_sllv_epi32 (mask_1, bit_mod);

		_mm512_storeu_si512 (aux_vec1, bit_mod);
	    _mm512_storeu_si512 (aux_vec2, bit_div);
		for (int i = 0; i < VECTOR_SIZE; i++) bloom_filter[aux_vec2[i]] = bloom_filter[aux_vec2[i]] | aux_vec1[i];

		fun = _mm512_add_epi32 (mask_1, fun);
	}
}

int main (__v32s argc, char const *argv[]){
    ORCS_tracing_stop();

    srand(time(NULL));
    int vector_size, prob;
    int *bitmap, *o_orderkey, *l_orderkey, *filter_vec;
    int prime_numbers[] = {2, 3, 5, 7, 9, 11, 13, 17, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    int shift[] = {3, 5, 6, 5, 0, 6, 0, 6, 3, 3, 2, 6, 1};
    vector_size = atoi(argv[1]);
    prob = atoi(argv[2]);
    
    int32_t v_size = (1024 * 1024 * vector_size)/sizeof(int);
    o_orderkey = (int*) aligned_alloc (64, (int) v_size/4 * sizeof (int));
    l_orderkey = (int*) aligned_alloc (64, v_size * sizeof (int));

    std::cout << "v_size = " << v_size << "\n";

    int32_t max = 0;
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
    size_t output_count = 0;
    
    int *bloom_filter = bloom_create ((int) v_size/4, p, &bloom_filter_size, &hash_functions);
    int *output = (int*) aligned_alloc (32, v_size * sizeof (int));
    for (int i = 0; i < v_size; i ++) output[i] = 0;

    int *hash_function_factors = (int*) malloc (hash_functions * sizeof (int));    
    int *shift_amounts = (int*) malloc (hash_functions * sizeof (int));

    std::cout << "bloom_filter_size = " << bloom_filter_size << "\n";
    std::cout << "hash_functions = " << hash_functions << "\n";

    for (int i = 0; i < hash_functions; i++) {
        hash_function_factors[i] = prime_numbers[i % 15];
        shift_amounts[i] = shift[i];
    }

    ORCS_tracing_start();

    int i, tid, start, finish;

    #pragma omp parallel for
    for (int i = 0; i < v_size/4; i += VECTOR_SIZE) {
        bloom_set_step (&o_orderkey[i], (int) v_size/4, bloom_filter, bloom_filter_size, hash_function_factors, shift_amounts, hash_functions);
    }

    #pragma omp parallel for
    for (int i = 0; i < v_size; i += VECTOR_SIZE) {
        bloom_chk_step (&l_orderkey[i], v_size, hash_functions, hash_function_factors, shift_amounts, bloom_filter, bloom_filter_size, output, &output_count);
    }
    
    printf ("output_count = %lu\n", output_count);
    ORCS_tracing_stop();

    free (o_orderkey);
    free (l_orderkey);
    free (hash_function_factors);
    free (shift_amounts);
}

