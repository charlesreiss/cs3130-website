#include <stdint.h>

typedef uint64_t cycles_type;

extern cycles_type measure_function(void (*function)(), void (*setup)(), void (*teardown)());
