#include "continuation_machine.h"
#include <stdio.h>

#undef CM_ERROR_ITERATION_LIMIT_REACHED
#define CM_ERROR_ITERATION_LIMIT_REACHED(p, f, state, ...)                        \
  CM_RETURN(, f, state, __VA_ARGS__)
#define CM_ADD(p, f, state, ...) (, f, (__COUNTER__), state)

int main() {
  printf("CM iteration count: %i\n", CM(ADD, CM_NO_STATE));
  return 0;
}