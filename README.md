# colorize
Just pull your strings through one simple function to make them pretty!

## Getting started
Simply compile the project with:
```
gcc *.c -o colorize
```
and then run it with:
```
./colorize
```

## Embed it in your own project
You only need to call a single function to colorize your strings. Simply copy the files `buffer.c`, `buffer.h`, `colorize.c`, colorize.h` and `base.h` in your project. Include `colorize.h` in the file where you want to use the function, then do this:

```
char *msg = colorize("My 'pretty' colorized string");
printf("%s\n", msg);
free(msg);
```

## About
The project contains a buffer implementation borrowed from https://www.github.com/cortoproject/base which makes it more efficient to do appending to an unbounded string. It also contains a file `base.h` borrowed from the same repository for some basic defines, like `TRUE` and `FALSE`.
