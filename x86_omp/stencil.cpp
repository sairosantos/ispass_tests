#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>
#include <omp.h>

int main(int argc, char const *argv[]) {
    int size = atoi(argv[1]);
    int v_size = (1024 * 1024 * size) / sizeof(float);
    int i;
    int elem = sqrt (v_size);
    while (elem % 16 != 0) elem++;
    int remainder = 0;
    for (int i = 0; i+elem+16 < v_size; i+=16) remainder = i;

    __m512 vec_a = _mm512_set1_ps((float) 1.0);
    float* data_a = (float*) aligned_alloc (32, v_size*sizeof (float));
    for (i = 0; i < v_size; i += 16) _mm512_store_ps (&data_a[i], vec_a);

    vec_a = _mm512_set1_ps((float) 0.0);
    float* data_b = (float*) aligned_alloc (32, v_size*sizeof (float));
    for (i = 0; i < v_size; i += 16) _mm512_store_ps (&data_b[i], vec_a);

    __m512 elem_a1, elem_a2, elem_a3, elem_a4, elem_a5, elem_b;
    __m512 mul = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
    #pragma omp parallel shared (data_a, data_b, remainder) private (i, elem_a1, elem_a2, elem_a3, elem_a4, elem_a5, elem_b)
    {
	#pragma omp for schedule (static)
    	for (i = elem; i < remainder; i += 16) {
            elem_a1 = _mm512_load_ps (&data_a[i-elem]);
            elem_a2 = _mm512_loadu_ps (&data_a[i-1]);
            elem_a3 = _mm512_load_ps (&data_a[i]);
            elem_a4 = _mm512_loadu_ps (&data_a[i+1]);
            elem_a5 = _mm512_load_ps (&data_a[i+elem]);
            elem_b = _mm512_add_ps(elem_a1, elem_a2);
            elem_b = _mm512_add_ps(elem_b, elem_a3);
            elem_b = _mm512_add_ps(elem_b, elem_a4);
            elem_b = _mm512_add_ps(elem_b, elem_a5);
            elem_b = _mm512_mul_ps(elem_b, mul);
            _mm512_stream_ps (&data_b[i], elem_b);
    	}
    
        #pragma omp for schedule (static)
        for (int i = 0; i < elem; i += 16){
	    if (i-elem > 0) elem_a1 = _mm512_load_ps (&data_a[i-elem]);
            if (i-1 > 0) elem_a2 = _mm512_loadu_ps (&data_a[i-1]);
            elem_a3 = _mm512_load_ps (&data_a[i]);
            elem_a4 = _mm512_loadu_ps (&data_a[i+1]);
            elem_a5 = _mm512_load_ps (&data_a[i+elem]);
            elem_b = _mm512_add_ps(elem_a1, elem_a2);
            elem_b = _mm512_add_ps(elem_b, elem_a3);
            elem_b = _mm512_add_ps(elem_b, elem_a4);
            elem_b = _mm512_add_ps(elem_b, elem_a5);
            elem_b = _mm512_mul_ps(elem_b, mul);
            _mm512_stream_ps (&data_b[i], elem_b);
        }

        #pragma omp for schedule (static)
        for (int i = remainder; i < v_size; i+=16){
	    elem_a1 = _mm512_load_ps (&data_a[i-elem]);
            elem_a2 = _mm512_loadu_ps (&data_a[i-1]);
            elem_a3 = _mm512_load_ps (&data_a[i]);
            if (i+1 < v_size) elem_a4 = _mm512_loadu_ps (&data_a[i+1]);
            if (i+elem < v_size) elem_a5 = _mm512_load_ps (&data_a[i+elem]);
            elem_b = _mm512_add_ps(elem_a1, elem_a2);
            elem_b = _mm512_add_ps(elem_b, elem_a3);
            elem_b = _mm512_add_ps(elem_b, elem_a4);
            elem_b = _mm512_add_ps(elem_b, elem_a5);
            elem_b = _mm512_mul_ps(elem_b, mul);
            _mm512_stream_ps (&data_b[i], elem_b);
        }
    }

    printf ("%f\n", data_b[v_size-1]);
    
    return 0;
}

