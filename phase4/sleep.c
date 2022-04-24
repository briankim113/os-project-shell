#include <stdio.h>
#include <stdlib.h> //strtol()
#include <unistd.h> //sleep

int main(int argc, char* argv[]){
    // if (argc != 2) {
    //     printf("Please provide exactly 2 arguments: program and n\n");
    //     return -1;
    // }

    char* p;
    long n = strtol(argv[1], &p, 10);
    printf("%ld\n", n);

    for (int i=0; i<n; i++) {
        printf("sleep: %d iteration\n", i);
        sleep(1);
    }

    printf("sleep: exit program\n");
    return 0;
}
