#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "../../intrinsics/vima/vima.hpp"

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

int main(__v32s argc, char const *argv[]) {
    __v32u size = atoi(argv[1]);
    if (size != 0 && (size & (size - 1)) == 0){
        __v32u i;
        __v32u v_size = (1024 * 1024 * size) / sizeof(__v32f);
        __v32f *vector_a = (__v32f *)malloc(sizeof(__v32f) * v_size);
        __v32f *vector_b = (__v32f *)malloc(sizeof(__v32f) * v_size);
        __v32f *mul = (__v32f *)malloc(sizeof(__v32f) * v_size);
        for (i = 0; i < v_size; i += 64) {
            _vim64_fmovs(1, &vector_a[i]);
            _vim64_fmovs(0, &vector_b[i]);
            _vim64_fmovs(1, &mul[i]);
        }
        
        int elem = sqrt (v_size);
        int remainder = 0;
        for (int i = 0; i+elem+64 < v_size; i+=64) remainder = i;

        int tid, start, finish;
        ORCS_tracing_start();
        #pragma omp parallel shared (vector_a, vector_b, remainder) private (i)
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
                _vim64_fadds(&vector_b[i], &vector_a[i-elem], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i-1], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i+1], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i+elem], &vector_b[i]);
                _vim64_fmuls(&vector_b[i], &mul[i], &vector_b[i]);
            }
        
            #pragma omp for schedule (static)
            for (int i = 0; i < elem; i++){
                if (i-elem > 0) vector_b[i] += vector_a[i-elem];
                if (i-1 > 0) vector_b[i] += vector_a[i-1];
                vector_b[i] += vector_a[i];
                vector_b[i] += vector_a[i+1];
                vector_b[i] += vector_a[i+elem];
                vector_b[i] *= mul[i];
            }

            #pragma omp for schedule (static)
            for (int i = remainder+elem+64; i< v_size; i++){
                vector_b[i] += vector_a[i-elem];
                vector_b[i] += vector_a[i-1];
                vector_b[i] += vector_a[i];
                if (i+1 < v_size) vector_b[i] += vector_a[i+1];
                if (i+elem < v_size) vector_b[i] += vector_a[i+elem];
                vector_b[i] *= mul[i];
            }
        }
        ORCS_tracing_stop();

        printf ("%f\n", vector_b[v_size-1]);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}
