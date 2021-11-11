#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>
#include <omp.h>

int main(int argc, char const *argv[]) {
    int v_size;
    int size = atoi(argv[1]);
    if (size != 0 && (size & (size - 1)) == 0) v_size = (1024 * 1024 * size) / sizeof(int);
    else return 0;

    int* data_a = (int*) aligned_alloc (32, v_size*sizeof (int));

    __m512i vec_a;
    int i, tid, start, finish;

    #pragma omp parallel shared (data_a) private (i, vec_a, tid, start, finish)
    {
        int chunk_size = v_size / omp_get_num_threads();
        tid = omp_get_thread_num();
        start = tid*chunk_size;
        finish = start + chunk_size;
        for (i = start; i < finish; i += 16) {
            vec_a = _mm512_load_si512 ((__m512i *) &data_a[i]);
            vec_a = _mm512_set1_epi32(1);
            _mm512_store_si512 ((__m512i *) &data_a[i], vec_a);
        }
    }

    printf ("%d\n", data_a[v_size-1]);

    return 0;
}
