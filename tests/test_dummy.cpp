#include <stdio.h>
#include "add_function.h"


int main() {
    if(add(1, 2) != 3){
        printf("Failed");
    }

    return 0;
}