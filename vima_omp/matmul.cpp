#include "../../intrinsics/vima/vima.hpp"
#include <math.h>
#include <omp.h>

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

__v32s main(__v32s argc, char const *argv[]) {
    __v32s size = atoi(argv[1]);
    
    if (size != 0 && (size & (size - 1)) == 0) {
        __v32s m_size = sqrt((1024 * 1024 * size) / sizeof(__v32f));
        while (m_size % 128 != 0) m_size++;
        __v32s n_vectors = ceil(((double)m_size/(double)64));
        __v32f *matrix_a = (__v32f *)malloc(sizeof(__v32f) * m_size * (64 * n_vectors));
        __v32f *matrix_b = (__v32f *)malloc(sizeof(__v32f) * m_size * (64 * n_vectors));
        __v32f *matrix_c = (__v32f *)malloc(sizeof(__v32f) * m_size * (64 * n_vectors));
        __v32f *aux_vec = (__v32f *)malloc(sizeof(__v32f) * 64 * n_vectors);
        __v32f *partial_sum;

        __v32s i, j, k;
        int tid, start, finish;

        ORCS_tracing_start();

        #pragma omp parallel shared (matrix_a, matrix_b, matrix_c) private (partial_sum)
        {
            int chunk_size = m_size / omp_get_num_threads();
            tid = omp_get_thread_num();
            start = tid*chunk_size;
            finish = start + chunk_size;

            start = start + tid * 64;
            finish = finish + (tid+1) * 64;
            if (finish > m_size) finish = m_size;
            printf ("m_size: %d | %d of %d threads, %d to %d\n", m_size, tid, omp_get_num_threads(), start, finish);
            #pragma omp for schedule (static)
            for (__v32s i = start; i < finish; ++i) {
                partial_sum = (__v32f *)malloc(sizeof(__v32f) * 64 * n_vectors);
                for (__v32s j = 0; j < m_size; ++j) {
                    for (__v32s k = 0; k < n_vectors; ++k) {
                        _vim64_fmuls(&matrix_a[(i * 64 * n_vectors) + (k * 64)], &matrix_b[(j * 64 * n_vectors) + (k * 64)], &aux_vec[k * 64]);
                    }
                    _vim64_fcums(partial_sum, &matrix_c[(i * m_size) + j]);
                }
                free (partial_sum);
            }
        } 

        ORCS_tracing_stop();

        printf ("%f\n", matrix_c[m_size-1]);

        free(matrix_a);
        free(matrix_b);
        free(matrix_c);
        free(aux_vec);

    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}