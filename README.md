# colorize
Pull your strings through one simple function to make them pretty!

## Getting started
Simply compile the project with:
```
gcc colorize.c -o colorize
```
and then run it with:
```
./colorize
```

## Embed it in your own project
You only need to call a single function to colorize your strings. Simply copy `colorize.c` in your project, and call it like this:

```c
char *msg = colorize("My 'pretty' colorized string");
printf("%s\n", msg);
free(msg);
```

## Example
![Example](https://github.com/SanderMertens/colorize/blob/master/example.png "Example")
