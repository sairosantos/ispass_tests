#include "../../intrinsics/vima/vima.hpp"
#include <math.h>

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
        __v32s n_vectors = ceil(((double)m_size/(double) 64));
        __v32f *matrix_a = (__v32f *) aligned_alloc (256, sizeof(__v32f) * m_size * (64 * n_vectors));
        __v32f *matrix_b = (__v32f *) aligned_alloc (256, sizeof(__v32f) * m_size * (64 * n_vectors));
        __v32f *matrix_c = (__v32f *) aligned_alloc (256, sizeof(__v32f) * m_size * (64 * n_vectors));
        __v32f *mask = (__v32f *) aligned_alloc (256, sizeof(__v32f) * 64);
        __v32f *aux_vec = (__v32f *) aligned_alloc (256, sizeof(__v32f) * 64);
        __v32f *aux_vec_a = (__v32f *) aligned_alloc (256, sizeof(__v32f) * 64);
        __v32f *aux_vec_b = (__v32f *) aligned_alloc (256, sizeof(__v32f) * 64);
        __v32f *partial_sum = (__v32f *) aligned_alloc (256, sizeof(__v32f) * m_size * (64 * n_vectors));
         for (int i = 0; i < m_size; i++) mask[i] = 1.0;

        /*for (int x = 0; x < m_size*VECTOR_SIZE*n_vectors; x++){
            matrix_a[x] = rand() % 10 + 1;
            matrix_b[x] = rand() % 10 + 1;
        }

        printf ("a: \n");
        for (int i = 0; i < m_size*VECTOR_SIZE*n_vectors; i++) {
            printf ("%.0f ", matrix_a[i]);
            //if (i != 0 && i % 512 == 0) printf ("\n");
        }
        printf ("\n\nb: \n");
        for (int i = 0; i < m_size*VECTOR_SIZE*n_vectors; i++) {
            printf ("%.0f ", matrix_b[i]);
            //if (i != 0 && i % 512 == 0) printf ("\n");
        }*/
        ORCS_tracing_start();
        for (__v32s i = 0; i < m_size; ++i) {
            for (__v32s j = 0; j < m_size; ++j) {
                for (__v32s k = 0; k < n_vectors; ++k) {
                    _vim64_fmuls(&matrix_a[(i * 64 * n_vectors) + (k * 64)], mask, aux_vec_a);
                    _vim64_fmuls(&matrix_b[(i * 64 * n_vectors) + (k * 64)], mask, aux_vec_b);
                    _vim64_fmuls(aux_vec_a, aux_vec_b, aux_vec);
                    _vim64_fcums(aux_vec, &partial_sum[k]);
                    //printf ("a: %p | b: %p | aux: %p\n", &matrix_a[(i * VECTOR_SIZE * n_vectors) + (k * VECTOR_SIZE)], &matrix_b[(j * VECTOR_SIZE * n_vectors) + (k * VECTOR_SIZE)], &aux_vec[k * VECTOR_SIZE]);
                }
                _vim64_fcums(partial_sum, &matrix_c[(i * m_size) + j]);
                //printf ("C: %.0f\n", matrix_c[(i * VECTOR_SIZE) + j]);
                //printf ("C: %.0f\n", matrix_c[(i * VECTOR_SIZE) + j]);
            }
        }
        ORCS_tracing_stop();

        printf ("%f\n", matrix_c[m_size-1]);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}