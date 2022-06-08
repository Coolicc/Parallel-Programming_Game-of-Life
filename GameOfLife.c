#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *input;
    FILE *output;
    char input_mode;
    if (argc == 3) {
        input_mode = 'y';
        input = fopen(argv[1], "r");
        output = fopen(argv[2], "w");
    }
    else {
        char output_file_name[256];
        printf("Insert the name of the output file:\n");
        fgets(output_file_name, sizeof(output_file_name), stdin);
        int len=strlen(output_file_name);
        if(output_file_name[len-1]=='\n')
            output_file_name[len-1]='\0';
        output = fopen(output_file_name, "w");
        printf("Read input from file? (y/n)\n");
        scanf("%c", &input_mode);
        if (input_mode == 'y' || input_mode == 'Y') {
            input_mode = 'y';
            char input_file_name[256];
            printf("Insert the name of the input file: \n");
            fgetc(stdin);
            fgets(input_file_name, sizeof(input_file_name), stdin);
            int len=strlen(input_file_name);
            if(input_file_name[len-1]=='\n')
                input_file_name[len-1]='\0';
            printf("%s", input_file_name);
            fflush(stdout);
            input = fopen(input_file_name, "r");
        }
        else {
            input = stdin;
            printf("Insert number of test cases:\n");
        }
    }
    int test_cases;
    fscanf(input, "%d", &test_cases);
    for (int t = 0; t < test_cases; t++) {
        fprintf(output, "Test case: %d\n\n", t + 1);
        int num_of_iterations;
        if (input_mode != 'y') {
            printf("Insert the number of simulation iterations:\n");
        }
        fscanf(input, "%d", &num_of_iterations);
        int n, m;
        if (input_mode != 'y') {
            printf("Insert the size of the field (nxm):\n");
        }
        fscanf(input, "%d %d", &n, &m);
        int alive_cells;
        if (input_mode != 'y') {
            printf("Insert the number of alive cells:\n");
        }
        short int *field = malloc(n * m * sizeof(short int));
        int num_fields = n * m;
        for (int i = 0; i < num_fields; i++) {
            *(field + i) = '-';
        }
        fscanf(input, "%d", &alive_cells);
        int x, y;
        for (int i = 0; i < alive_cells; i++) {
            if (input_mode != 'y') {
                printf("Insert coordinates of an alive cell (x y):\n");
            }
            fscanf(input, "%d %d", &x, &y);
            *(field + x * m + y) = '#';
        }
        fprintf(output, "Iteration: 0\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                fprintf(output, "%c", *(field + i * m + j));
            }
            fprintf(output, "\n");
        }
        fprintf(output, "\n\n");
        for (int iteration = 0; iteration < num_of_iterations; iteration++) {
            short int *next_field = malloc(n * m * sizeof(short int));
            int neighbourhs_alive;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    neighbourhs_alive = 0;
                    for (int x = -1; x < 2; x++) {
                        if (i + x < 0 || i + x >= n) {
                            continue;
                        }
                        for (int y = -1; y < 2; y++) {
                            if (j + y < 0 || j + y >= m || x == 0 && y == 0) {
                                continue;
                            }
                            if (*(field + (i + x) * m + j + y) == '#') {
                                neighbourhs_alive++;
                            }
                        }
                    }
                    fflush(stdout);
                    if (*(field + i * m + j) == '#' && (neighbourhs_alive == 2 || neighbourhs_alive == 3) || *(field + i * m + j) == '-' && neighbourhs_alive == 3) {
                        *(next_field + i * m + j) = '#';
                    }
                    else {
                        *(next_field + i * m + j) = '-';
                    }
                }
            }
            free(field);
            field = next_field;
            fprintf(output, "Iteration: %d\n\n", iteration + 1);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    fprintf(output, "%c", *(field + i * m + j));
                }
                fprintf(output, "\n");
            }
            fprintf(output, "\n\n");
        }
        free(field);
    }
    fclose(input);
    fclose(output);
}