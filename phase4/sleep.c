#include <stdio.h>
#include <stdlib.h> //strtol()
#include <unistd.h> //sleep

int main(int argc, char* argv[]){
    // if (argc != 2) {
    //     printf("Please provide exactly 2 arguments: program and n\n");
    //     return -1;
    // }

    printf("\nExecuting program with socket Number: #%s!\n", argv[2]);

    char* p;
    long n = strtol(argv[1], &p, 10);
    int interactionsLeft = strtol(argv[3], &p, 10);
    printf("starting from %d iterations\n", interactionsLeft);
    for (int i=0; i<n; i++) {
        sleep(1);
        interactionsLeft--;
        printf("iteration left: %d \n", interactionsLeft);
    }
    // printf("iteration left: %d \n", interactionsLeft);

    // printf("sleep: exit program\n");
    return 0;
}
