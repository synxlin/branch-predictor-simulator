#pragma once

#define CORRECT 0
#define WRONG 1

#define _error_exit(fun) { perror(fun); exit(EXIT_FAILURE); }

#define _output_error_exit(fun) { printf(fun); exit(EXIT_FAILURE);}

#define pow_2(num) ((uint32_t)(1 << ((int)num)))

typedef char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

