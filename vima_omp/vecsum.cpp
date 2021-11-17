#include "../../intrinsics/vima/vima.hpp"
#include <omp.h>

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

__v32s main(__v32s argc, char const *argv[]) {
    __v32s size = atoi(argv[1]);
    if (size != 0 && (size & (size - 1)) == 0){
        __v32u v_size = (1024 * 1024 * size) / sizeof(__v32f);
        __v32f *vector_a = (__v32f *)malloc(sizeof(__v32f) * v_size);
        __v32f *vector_b = (__v32f *)malloc(sizeof(__v32f) * v_size);
        __v32f *vector_c = (__v32f *)malloc(sizeof(__v32f) * v_size);
        __v32s i;
        int tid, start, finish;
        ORCS_tracing_start();
            
        #pragma omp parallel shared (vector_a, vector_b, vector_c) private (i, tid, start, finish)
        {
            int chunk_size = v_size / omp_get_num_threads();
            tid = omp_get_thread_num();
            start = tid*chunk_size;
            finish = start + chunk_size;

            start = start + tid * 64;
            finish = finish + (tid+1) * 64;
            if (finish > v_size) finish = v_size;
            printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
            for (i = start; i < finish; i += 64) {
                _vim64_fadds(&vector_a[i], &vector_b[i], &vector_c[i]);
            }
        }

        ORCS_tracing_stop();
        
        printf ("%f ", vector_c[v_size-1]);

        free (vector_a);
        free (vector_b);
        free (vector_c);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}
