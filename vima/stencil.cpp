#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../intrinsics/vima/vima.hpp"

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

int main(__v32s argc, char const *argv[]) {
    __v32s size = atoi(argv[1]);
    __v32s VECTOR_SIZE = atoi (argv[2]);

    if (size != 0 && (size & (size - 1)) == 0){
        int i = 0;
        __v32u v_size = (1024 * 1024 * size) / sizeof(__v32f);
        __v32f *vector_a = (__v32f *) aligned_alloc(256, sizeof(__v32f) * v_size);
        __v32f *vector_b = (__v32f *) aligned_alloc(256, sizeof(__v32f) * v_size);
        __v32f *mul = (__v32f *)malloc(sizeof(__v32f) * v_size);
        if (VECTOR_SIZE == 2048){
            for (i = 0; i < v_size; i += VECTOR_SIZE) {
                _vim2K_fmovs(1, &vector_a[i]);
                _vim2K_fmovs(0, &vector_b[i]);
                _vim2K_fmovs(1, &mul[i]);
            }
        }
        if (VECTOR_SIZE == 64){
            for (i = 0; i < v_size; i += VECTOR_SIZE) {
                _vim64_fmovs(1, &vector_a[i]);
                _vim64_fmovs(0, &vector_b[i]);
                _vim64_fmovs(1, &mul[i]);
            }
        }
        srand (time(NULL));
        for (int x = 0; x < v_size; x++){
            vector_a[x] = 1;//rand() % 10 + 1;
            vector_b[x] = 0;//rand() % 10 + 1;
            mul[x] = 2;
        }
        int elem = sqrt (v_size);
        int remainder = 0;
        if (VECTOR_SIZE == 2048){
            ORCS_tracing_start();
            for (int i = elem; i+elem+VECTOR_SIZE < v_size; i += VECTOR_SIZE) {
                _vim2K_fadds(&vector_b[i], &vector_a[i-elem], &vector_b[i]);
                _vim2K_fadds(&vector_b[i], &vector_a[i], &vector_b[i]);
                _vim2K_fadds(&vector_b[i], &vector_a[i-1], &vector_b[i]);
                _vim2K_fadds(&vector_b[i], &vector_a[i+1], &vector_b[i]);
                _vim2K_fadds(&vector_b[i], &vector_a[i+elem], &vector_b[i]);
                _vim2K_fmuls(&vector_b[i], &mul[i], &vector_b[i]);
                remainder = i;
            }
            ORCS_tracing_stop();
        }

        if (VECTOR_SIZE == 64){
            ORCS_tracing_start();
            for (int i = elem; i+elem+VECTOR_SIZE < v_size; i += VECTOR_SIZE) {
                _vim64_fadds(&vector_b[i], &vector_a[i-elem], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i-1], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i+1], &vector_b[i]);
                _vim64_fadds(&vector_b[i], &vector_a[i+elem], &vector_b[i]);
                _vim64_fmuls(&vector_b[i], &mul[i], &vector_b[i]);
                remainder = i;
            }
            ORCS_tracing_stop();
        }
        printf ("elem: %d\n", elem);
        for (int x = 0; x < v_size; x++){
            if (x % elem == 0) printf ("\n");
            printf ("%.0lf ", vector_a[x]);
        }
        printf ("\n-------------------------\n");
        for (int x = 0; x < v_size; x++){
            if (x % elem == 0) printf ("\n");
            printf ("%.0lf ", vector_b[x]);
        }

        printf ("%f\n", vector_b[v_size-1]);

        free (vector_a);
        free (vector_b);
        free (mul);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}
