#include <stdio.h>
#include <stdlib.h>

int global_variable = 1;

void test() {
    int function_variable = 5;
    printf("Address of function variable: %p\n", (void *)&function_variable);
}

int main() {

    int local_variable = 2;
    int local_variable_2 = 4;
    int *heap_variable = (int *)malloc(sizeof(int));
    *heap_variable = 3;

    printf("Address of global variable: %p\n", (void *)&global_variable);
    printf("Address of local variable: %p\n", (void *)&local_variable);
    printf("Address of local variable 2: %p\n", (void *)&local_variable_2);
    printf("Address of heap variable: %p\n", (void *)heap_variable);

    test();

    free(heap_variable);
    return 0;
}