#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "life.h"
#include <time.h>

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Usage:\n"
               "  %s STEPS INPUT_FILE [THREADS] [OUTPUT_FILE]\n", argv[0]);
        return 1;
    }

    int steps = atoi(argv[1]);
    char input_file[256];
    strcpy(input_file, argv[2]);

    int threads = 1;  
    threads = atoi(argv[3]);
    char output_file[256] = "output_serial.txt";  // 默认输出文件
    strcpy(output_file, argv[4]);


    FILE* in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error opening input file: %s\n", input_file);
        return 1;
    }

    LifeBoard* board = malloc(sizeof(LifeBoard));
    read_life_board(in, board);
    fclose(in);

    
    if (threads <= 1) {
        simulate_life_serial(board, steps);
    } else {
        simulate_life_parallel(threads, board, steps);
    }

    FILE* out = fopen(output_file, "w");
    if (out) {
        print_life_board(board, out);
        fclose(out);
    } else {
        fprintf(stderr, "Error: Cannot open output file: %s\n", output_file);
    }

    destroy_life_board(board);
    return 0;
}