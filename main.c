#include "colorize.h"

void main() {
    char *msg = colorize("Colorize 'demo' version 1.0");
    printf ("%s\n", msg);
    free (msg);

    msg = colorize("Here's an $ENVIRONMENT_VARIABLE");;
    printf ("%s\n", msg);
    free (msg);

    msg = colorize("Percentage signs are handled 100% correctly");;
    printf ("%s\n", msg);
    free (msg);

    msg = colorize("Numbers after \"letters\" are not colorized: x64_86");;
    printf ("%s\n", msg);
    free (msg);    
}