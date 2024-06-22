#pragma once
#include "continuation_machine.h"

#define DISCARD(...)

#define PARENTHESIZE(...) (__VA_ARGS__)

/* removes parentheses from passed argument */
#define EXPAND(...) __VA_ARGS__
#define EMPTY()
#define DEFER(...) __VA_ARGS__ EMPTY()
#define UNPARENTHESIZE(arg) EXPAND arg

#define FIRST_ARG(arg, ...) arg

/* counting number of elements in a comma separated token list */
#define PP_NARG(...) PP_NARG_(__VA_ARGS__ __VA_OPT__(, ) PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14,  \
                 _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26,   \
                 _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38,   \
                 _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,   \
                 _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62,   \
                 _63, N, ...)                                                  \
  N

#define PP_RSEQ_N()                                                            \
  63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,  \
      44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27,  \
      26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9,   \
      8, 7, 6, 5, 4, 3, 2, 1, 0
#define N_ARGS(...) PP_NARG(__VA_ARGS__)

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a##__VA_ARGS__

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0, )

#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 ~, 1,

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define BOOL(x) COMPL(NOT(x))

#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t

#define IF(c) IIF(BOOL(c))

#define IS_EMPTY(...) BOOL(__VA_OPT__(0))

#define CM_FOREACH_ITERATE(_prefix, _foreach, _state, _function, _head, ...)   \
  (, IF(IS_EMPTY(__VA_ARGS__))(RETURN, FOREACH_ITERATE),                       \
   (EXPAND _state _function(_head)), _function, __VA_ARGS__)

#define FOREACH(f, ...) CM(FOREACH_ITERATE, CM_NO_STATE, f, __VA_ARGS__)