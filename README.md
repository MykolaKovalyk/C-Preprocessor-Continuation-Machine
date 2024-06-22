# Summary

This project implements a simple continuation machine - a macro that 
utilizes recursion to iteratively expand macros, allowing for functions
like [map](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/map) 
and [reduce](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/reduce)
in js, but for C preprocessor tokens. Other use cases for this are
preprocessor for loops, simplified boilerplate declarations, 
CCP-only imlementations of compilers and interpreters, advanced
metaprogramming problems, etc.

Code is documented, structured and named so that it is easily accessible
for learning.

# Inspiration

The original inspiration for this project is a variant of a
continuation machine [here](https://web.archive.org/web/20220807225104/https://github.com/camel-cdr/bfcpp/blob/main/TUTORIAL.md#user-content-141-the-continuation-machine=),
used for a preprocessor-only Brainfuck interpreter. 

Ability to write interpreters/compilers using C macros suggests that
CPP is pseudo Turing-complete, i.e. even though recursion achieved 
can only be finite, you can mitigate this by implementing continuation
machine in such a way as to support exponentially high number of
iterations.

Writing interpreters in pure CPP is like building CPUs in minecraft - 
impractical, but fun. It's also a great learning experience. But
continuation machine has use cases highly practical even for
day-to-day use. An example of such is a `FOREACH` macro, included
in `helper_macros.h`.

The other inspiration for this project is [PPMP Iceberg](https://web.archive.org/web/20240306133415/https://jadlevesque.github.io/PPMP-Iceberg/).

# Try it 

To see the output, run:
```bash
gcc -E example.c > example.i
```
And examine output file `example.i`. Scroll to the end for generated symbols.