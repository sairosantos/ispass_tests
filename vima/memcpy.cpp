#include "../../intrinsics/vima/vima.hpp"

void __attribute__ ((noinline)) ORCS_tracing_start() {
    asm volatile ("nop");
}

void __attribute__ ((noinline)) ORCS_tracing_stop() {
    asm volatile ("nop");
}

__v32s main(__v32s argc, char const *argv[]) {
    __v32s size = atoi(argv[1]);
    __v32s VECTOR_SIZE = atoi (argv[2]);

    if (size != 0 && (size & (size - 1)) == 0){
        __v32s v_size = (1024 * 1024 * size) / sizeof(__v32s);
        __v32s *vector_a = (__v32s *) aligned_alloc(256, sizeof(__v32s) * v_size);
        __v32s *vector_b = (__v32s *) aligned_alloc(256, sizeof(__v32s) * v_size);
        if (VECTOR_SIZE == 2048){
            ORCS_tracing_start();
            for (__v32s i = 0; i < v_size; i += VECTOR_SIZE) {
                _vim2K_icpys(&vector_a[i], &vector_b[i]);
            }
            ORCS_tracing_stop();
        }

        if (VECTOR_SIZE == 64){
            ORCS_tracing_start();
            for (__v32s i = 0; i < v_size; i += VECTOR_SIZE) {
                _vim64_icpys(&vector_a[i], &vector_b[i]);
            }
            ORCS_tracing_stop();
        }

        printf ("%d\n", vector_b[v_size-1]);

        free (vector_a);
        free (vector_b);
    } else {
        printf("Error! Size is not power of two!\n");
        exit(1);
    }
    return 0;
}
