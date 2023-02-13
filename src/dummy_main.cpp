#include <stdio.h>
#include "add_function.h"


int main() {
    // Example library usage
    const int x = add(4, 5);

    printf("Calling library function...\n");
    printf("The result of adding 4 and 5 is %i\n", x);
    return 0;
}