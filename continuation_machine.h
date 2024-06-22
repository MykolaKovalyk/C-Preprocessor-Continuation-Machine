/**
 * @file continuation_machine.h
 * @brief Continuation machine for iterative macro expansion.
 *
 * @section cm_usage Usage
 * Use it to iteratively apply a macro to a variadic list of arguments. Example:
 *
 * @code
 * #include "continuation_machine.h"
 * #define CM_REMOVE_COMMAS(p, f, state, current_arg, ...)               \
 *   (, IF(IS_EMPTY(__VA_ARGS__))(RETURN, f), (EXPAND state current_arg),\
 *    __VA_ARGS__)
 *
 * CM(REMOVE_COMMAS, CM_NO_STATE, 1, 2, 3, 4, 5) // expands to 1 2 3 4 5
 * @endcode
 *
 * @note Number of iterations shall be finite (currently not more than 2046).
 *
 * Arguments:
 * - `f`: **Transition function** - a map of the form `(prefix, current_f,
 * state, args...) -> (prefix, next_f, new_state, new_args...)`, where both
 * input and output is a form of *machine state* (see below). `f` is applied to
 * an input state, and produces an output state, which is fed back into `f` at
 * the next iteration. Passed macros shall be prefixed with `CM_`. To use
 * `CM_MY_MACRO`, pass `MY_MACRO`.
 * - `initial_state`: If none, use `CM_NO_STATE` (expands to `()`).
 * - `...`: Variadic list of arguments for machine to process.
 *
 * **Machine state** is a list of the form  (p, f, state, args...), which is
 * used to represent state at each iteration.
 * Contains:
 * - `p`: Concatenation prefix.
 * - `f`: Transition function to use at the next iteration.
 * - `state`: Current iteration state.
 * - `args...`: Variadic list of arguments for machine to process.
 *
 * @note Since tokens concatenated with ## operator are not macro-expanded,
 * concatentaion with `p` is used to prevent excessive preprocessing. While
 * iterating, argument `p` of transition function `f` is always empty.
 * Regardless of what `p` is in the output state, it will become empty in the
 * next iteration's input state.
 *
 * Iteration can terminate in 3 ways:
 * - `CM_EXIT(...)`: Terminates iteration, whole expression expands to nothing.
 * - `CM_RETURN(p, f, state, ...)`: Terminates iteration, whole expression
 * expands to `state` (without encapsulating parentheses).
 * - `CM_ABORT_ITER(x)`: Iteration limit reached. Preprocessing fails.
 *
 * @section cm_how_it_works How it works
 * `CM_EXEC_N` applies exponential number of rescans, so that `f` can
 * repeatedly be invoked on the state. By itself, it is heavy on preprocessor
 * (`CM_EXEC_9` is equivalent to ~1024 nested `CM_EXEC_0`s). To mitigate this,
 * `CM_CONT_N` is used. Iteration begins from `CM_CONT_0` - starting small with
 * only 1 rescan. Only after `CM_EXEC_N` is completely expanded, `CM_CONT_N`
 * steps up number of rescans by 2 times if iteration is still not finished.
 * Stepping up number of rescans gradually allows for fast expansion and low
 * memory usage when number of iterations is small.
 *
 * Error-free termination involves enclosing remaining `CM_EXEC_N` and
 * `CM_CONT_N` into `DISCARD` macro. Notice `CM_LPAREN` and `CM_RPAREN`.
 * `CM_LPAREN` at the beginning, when paired with `CM_RPAREN` placed by
 * `CM_EXIT` at the end, will substitute an argument list for `DISCRAD`, thus
 * invoking it and removing leftover `CM_XXX`'s. `CM_RETURN` calls `CM_EXIT`
 * internally, and puts `state` at the end of the replacement list so the whole
 * expression expands to `state`.
 *
 * If number of iterations exceeds implementation limit, `CM_ABORT_ITER` is
 * called, which invokes `CM_ERROR_ITERATION_LIMIT_REACHED` with wrong number of
 * arguments, to intentionally fail preprocessing and display an error message.
 *
 * To avoid `f` macro not being replaced because of disabling context, its name
 * is not put into replacement list directly until the moment it is invoked.
 * This is why `CM_##f(...)` is used instead of `f(...)`, and why CM argument
 * `f` shall be of a form `MY_MACRO`, not `CM_MY_MACRO`.
 *
 * @note Measured time complexity of this macro is at most `O(n * m)`, and space
 * complexity - at most `O(n + m)`, where `n` is the number of iterations and
 * `m` is time/space complexity of a single iteration (including number of
 * characters in the replacement list, number of macro calls, etc.).
 *
 * @note This macro is compliant with C99 and C11 standards.
 *
 * @note Ignoring the restriction of finite iteration count, and physical
 * memory limitations, this macro is Turing-complete.
 */
#pragma once
#include "macro_helpers.h"

#define CM(f, initial_state, ...)                                              \
  EXPAND(DISCARD CM_LPAREN CM_CONT_0(, f, initial_state, __VA_ARGS__))

/* clang-format off */
#define CM_EXEC_0(p, f, ...)                           CM_##f(, p##f, p##__VA_ARGS__)
#define CM_EXEC_1(p, f, ...) CM_EXECUTE_0(CM_EXECUTE_0(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_2(p, f, ...) CM_EXECUTE_1(CM_EXECUTE_1(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_3(p, f, ...) CM_EXECUTE_2(CM_EXECUTE_2(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_4(p, f, ...) CM_EXECUTE_3(CM_EXECUTE_3(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_5(p, f, ...) CM_EXECUTE_4(CM_EXECUTE_4(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_6(p, f, ...) CM_EXECUTE_5(CM_EXECUTE_5(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_7(p, f, ...) CM_EXECUTE_6(CM_EXECUTE_6(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_8(p, f, ...) CM_EXECUTE_7(CM_EXECUTE_7(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_EXEC_9(p, f, ...) CM_EXECUTE_8(CM_EXECUTE_8(CM_##f(, p##f, p##__VA_ARGS__)))

#define CM_CONT_0(p, f, ...) CM_CONTINUE_1(CM_EXECUTE_0(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_1(p, f, ...) CM_CONTINUE_2(CM_EXECUTE_1(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_2(p, f, ...) CM_CONTINUE_3(CM_EXECUTE_2(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_3(p, f, ...) CM_CONTINUE_4(CM_EXECUTE_3(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_4(p, f, ...) CM_CONTINUE_5(CM_EXECUTE_4(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_5(p, f, ...) CM_CONTINUE_6(CM_EXECUTE_5(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_6(p, f, ...) CM_CONTINUE_7(CM_EXECUTE_6(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_7(p, f, ...) CM_CONTINUE_8(CM_EXECUTE_7(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_8(p, f, ...) CM_CONTINUE_9(CM_EXECUTE_8(CM_##f(, p##f, p##__VA_ARGS__)))
#define CM_CONT_9(p, f, ...) CM_ABORT_ITER(CM_EXECUTE_9(CM_##f(, p##f, p##__VA_ARGS__)))

#define CM_EXECUTE_0(x)  CM_EXEC_0 x
#define CM_EXECUTE_1(x)  CM_EXEC_1 x
#define CM_EXECUTE_2(x)  CM_EXEC_2 x
#define CM_EXECUTE_3(x)  CM_EXEC_3 x
#define CM_EXECUTE_4(x)  CM_EXEC_4 x
#define CM_EXECUTE_5(x)  CM_EXEC_5 x
#define CM_EXECUTE_6(x)  CM_EXEC_6 x
#define CM_EXECUTE_7(x)  CM_EXEC_7 x
#define CM_EXECUTE_8(x)  CM_EXEC_8 x
#define CM_EXECUTE_9(x)  CM_EXEC_9 x

#define CM_CONTINUE_1(x)  CM_CONT_1 x
#define CM_CONTINUE_2(x)  CM_CONT_2 x
#define CM_CONTINUE_3(x)  CM_CONT_3 x
#define CM_CONTINUE_4(x)  CM_CONT_4 x
#define CM_CONTINUE_5(x)  CM_CONT_5 x
#define CM_CONTINUE_6(x)  CM_CONT_6 x
#define CM_CONTINUE_7(x)  CM_CONT_7 x
#define CM_CONTINUE_8(x)  CM_CONT_8 x
#define CM_CONTINUE_9(x)  CM_CONT_9 x
#define CM_ABORT_ITER(x)  CM_ERROR_ITERATION_LIMIT_REACHED x /* intentionally wrong number of arguments to cause preprocessing error */

#define CM_ERROR_ITERATION_LIMIT_REACHED() /* NOTE: if you see this in your error output, you're likely over the cm's iteration limit. */
/* clang-format on */

#define CM_LPAREN (
#define CM_RPAREN )

#define CM_NO_STATE ()
#define CM_EXIT(...) CM_RPAREN
#define CM_RETURN(p, f, state, ...) CM_EXIT() EXPAND state
