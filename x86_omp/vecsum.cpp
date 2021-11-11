#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>
#include <omp.h>

int main(int argc, char const *argv[]) {
    int v_size;
    int size = atoi(argv[1]);
    if (size != 0 && (size & (size - 1)) == 0) v_size = (1024 * 1024 * size) / sizeof(float);
    else return 0;
    
    __m512 vec_a, vec_b, vec_c;
    float* data_a = (float*) aligned_alloc (32, v_size*sizeof (float));
    float* data_b = (float*) aligned_alloc (32, v_size*sizeof (float));
    float* data_c = (float*) aligned_alloc (32, v_size*sizeof (float));
    
    int i, tid, start, finish;
    
    #pragma omp parallel shared (data_a, data_b, data_c) private (i, vec_a, vec_b, vec_c, tid, start, finish)
    {
        int chunk_size = v_size / omp_get_num_threads();
        tid = omp_get_thread_num();
        start = tid*chunk_size;
        finish = start + chunk_size;
        for (i = start; i < finish; i += 16) {
            vec_a = _mm512_load_ps (&data_a[i]);
            vec_b = _mm512_load_ps (&data_b[i]);
            vec_c = _mm512_add_ps(vec_a, vec_b);
            _mm512_store_ps (&data_c[i], vec_c);
        }
    }

    printf ("%f\n", data_c[v_size-1]);
    return 0;
}
