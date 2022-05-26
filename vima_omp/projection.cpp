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
        __v32u v_size = (1024 * 1024 * size)/sizeof(__v32u);
        __v32u *vector1 = (__v32u *)    malloc(sizeof(__v32u) * v_size);
        __v32u *vector2 = (__v32u *)    malloc(sizeof(__v32u) * v_size);
        __v32u *bitmap = (__v32u *)     malloc(sizeof(__v32u) * v_size);
        __v32u *filter_vec = (__v32u *) malloc(sizeof(__v32u) * vector_size);
        __v32u *result = (__v32u *)    malloc(sizeof(__v32u) * v_size);
        __v32s i;

        int tid, start, finish;
        if (vector_size == 64){
            for (int j = 0; j < v_size; j += vector_size) _vim64_imovu (1, &bitmap[j]);
            _vim64_imovu (15, filter_vec);
            for (int j = 0; j < v_size; j += vector_size) _vim64_isltu(filter_vec, &vector1[i], &bitmap[i]);
            ORCS_tracing_start();
            #pragma omp parallel shared (vector2, bitmap, result) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim64_ilmku (&vector2[i], &bitmap[i], &result[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 128){
            for (int j = 0; j < v_size; j += vector_size) _vim128_imovu (1, &bitmap[j]);
            _vim128_imovu (15, filter_vec);
            for (int j = 0; j < v_size; j += vector_size) _vim128_isltu(filter_vec, &vector1[i], &bitmap[i]);
            ORCS_tracing_start();
            #pragma omp parallel shared (vector2, bitmap, result) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim128_ilmku (&vector2[i], &bitmap[i], &result[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 256){
            for (int j = 0; j < v_size; j += vector_size) _vim256_imovu (1, &bitmap[j]);
            _vim256_imovu (15, filter_vec);
            for (int j = 0; j < v_size; j += vector_size) _vim256_isltu(filter_vec, &vector1[i], &bitmap[i]);
            ORCS_tracing_start();
            #pragma omp parallel shared (vector2, bitmap, result) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim256_ilmku (&vector2[i], &bitmap[i], &result[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 512){
            for (int j = 0; j < v_size; j += vector_size) _vim512_imovu (1, &bitmap[j]);
            _vim512_imovu (15, filter_vec);
            for (int j = 0; j < v_size; j += vector_size) _vim512_isltu(filter_vec, &vector1[i], &bitmap[i]);
            ORCS_tracing_start();
            #pragma omp parallel shared (vector2, bitmap, result) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim512_ilmku (&vector2[i], &bitmap[i], &result[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 1024){
            for (int j = 0; j < v_size; j += vector_size) _vim1K_imovu (1, &bitmap[j]);
            _vim1K_imovu (15, filter_vec);
            for (int j = 0; j < v_size; j += vector_size) _vim1K_isltu(filter_vec, &vector1[i], &bitmap[i]);
            ORCS_tracing_start();
            #pragma omp parallel shared (vector2, bitmap, result) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim1K_ilmku (&vector2[i], &bitmap[i], &result[i]);
            }
            ORCS_tracing_stop();
        }

        if (vector_size == 2048){
            for (int j = 0; j < v_size; j += vector_size) _vim2K_imovu (1, &bitmap[j]);
            _vim2K_imovu (15, filter_vec);
            for (int j = 0; j < v_size; j += vector_size) _vim2K_isltu(filter_vec, &vector1[i], &bitmap[i]);
            ORCS_tracing_start();
            #pragma omp parallel shared (vector2, bitmap, result) private (i, tid, start, finish)
            {
                int chunk_size = v_size / omp_get_num_threads();
                tid = omp_get_thread_num();
                start = tid*chunk_size;
                finish = start + chunk_size;

                start = start + tid * vector_size;
                finish = finish + (tid+1) * vector_size;
                if (finish > v_size) finish = v_size;
                printf ("v_size: %d | %d of %d threads, %d to %d\n", v_size, tid, omp_get_num_threads(), start, finish);
                for (i = start; i < finish; i += vector_size) _vim2K_ilmku (&vector2[i], &bitmap[i], &result[i]);
            }
            ORCS_tracing_stop();
        }

        printf ("%d ", vector1[i]);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}
