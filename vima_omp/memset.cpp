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
    __v32s vector_size = atoi(argv[2]);
    if (size != 0 && (size & (size - 1)) == 0){
        __v32s v_size = (1024 * 1024 * size)/sizeof(__v32s);
        __v32s *vector = (__v32s *)malloc(sizeof(__v32s) * v_size);
        __v32s i;
        int tid, start, finish;
        if (vector_size == 64){
            ORCS_tracing_start();
            #pragma omp parallel shared (vector) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim64_imovs(1, &vector[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 128){
            ORCS_tracing_start();
            #pragma omp parallel shared (vector) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim128_imovs(1, &vector[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 256){
            ORCS_tracing_start();
            #pragma omp parallel shared (vector) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim256_imovs(1, &vector[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 512){
            ORCS_tracing_start();
            #pragma omp parallel shared (vector) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim512_imovs(1, &vector[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 1024){
            ORCS_tracing_start();
            #pragma omp parallel shared (vector) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim1K_imovs(1, &vector[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 2048){
            ORCS_tracing_start();
            #pragma omp parallel shared (vector) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim2K_imovs(1, &vector[i]);
            }
            ORCS_tracing_stop();
        }

        printf ("%d ", vector[i]);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}
