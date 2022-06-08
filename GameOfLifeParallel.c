#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int my_rank, p;
    MPI_Status status;
    FILE *input;
    FILE *output;
    char input_mode;
    clock_t start, end;
    double cpu_time_used;
    char no_step_output = 'n';
    char random_field = 'n';

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {
        start = clock();

        if (argc > 2) {
            input_mode = 'y';
            input = fopen(argv[1], "r");
            output = fopen(argv[2], "w");
            for (int i = 3; i < argc; i++) {
                if (strcmp(argv[i], "no") == 0) {
                   no_step_output = 'y'; 
                }
                else if (strcmp(argv[i], "rnd") == 0) {
                    random_field = 'y';
                }
            }
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
    }

    int test_cases;
    if (my_rank == 0) {
        fscanf(input, "%d", &test_cases);
    }
    MPI_Bcast(&test_cases, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int t = 0; t < test_cases; t++) {
        if (my_rank == 0) {
            fprintf(output, "Test case: %d\n\n", t + 1);
        }

        int num_of_iterations;
        if (my_rank == 0) {
            if (input_mode != 'y') {
                printf("Insert the number of simulation iterations:\n");
            }
            fscanf(input, "%d", &num_of_iterations);
        }
        MPI_Bcast(&num_of_iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int n, m, local_n;
        if (my_rank == 0) {
            if (input_mode != 'y') {
                printf("Insert the size of the field (nxm):\n");
            }
            fscanf(input, "%d %d", &n, &m);
            local_n = n / p;
        }

        if (my_rank == 0 && input_mode != 'y') {
            printf("Random alive cell distribution? (y/n)\n");
            do {
                scanf("%c", &random_field);
            } while (random_field == '\n' || random_field == '\n');
        }

        if (my_rank == 0 && input_mode != 'y') {
            printf("Print only last iteration? (y/n)\n");
            do {
                scanf("%c", &no_step_output);
            } while (no_step_output == '\n' || no_step_output == '\n');
        }

        MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&random_field, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

        int alive_cells;
        if (input_mode != 'y' && my_rank == 0 && random_field != 'y') {
            printf("Insert the number of alive cells:\n");
        }

        short int *field = (short int *) calloc(n * m, sizeof(short int));
        short int *local_field;
        if (my_rank == 0) {
            local_field = (short int *) calloc((local_n + n % p) * m, sizeof(short int));
        }
        else {
            local_field = (short int *) calloc(local_n * m, sizeof(short int));
        }
        short int *up_row = (short int *) calloc(m, sizeof(short int));
        short int *down_row = (short int *) calloc(m, sizeof(short int));
        int *send_counts = (int *) calloc(p, sizeof(int));
        int *displacements = (int *) calloc(p, sizeof(int));
        int zero_disp = local_n + n % p;
        *(send_counts) = zero_disp * m;
        *(displacements) = 0;
        int disp = zero_disp * m;
        for (int i = 1; i < p; i++) {
            *(send_counts + i) = local_n * m;
            *(displacements + i) = disp;
            disp += local_n * m;
        }
        if (my_rank == 0) {
            local_n = zero_disp;
        }
        int num_fields = local_n * m;
        if (random_field == 'y') {
            srand(time(NULL));
            for (int i = 0; i < num_fields; i++) {
                if (rand() % 2 == 0) {
                    *(local_field + i) = '-';
                }
                else {
                    *(local_field + i) = '#';
                } 
            }
        }
        else {
            for (int i = 0; i < num_fields; i++) {
                *(local_field + i) = '-';
            }
        }
        if (my_rank == 0) {
            for (int i = 0; i < m; i++) {
                *(up_row + i) = '-';
            }
        }
        if (my_rank == p-1) {
            for (int i = 0; i < m; i++) {
                *(down_row + i) = '-';
            }
        }

        MPI_Gatherv(local_field, local_n * m, MPI_SHORT, field, send_counts, displacements, MPI_SHORT, 0, MPI_COMM_WORLD);

        if (my_rank == 0 && random_field != 'y') {
            fscanf(input, "%d", &alive_cells);
            int x, y;
            for (int i = 0; i < alive_cells; i++) {
                if (input_mode != 'y') {
                    printf("Insert coordinates of an alive cell (x y):\n");
                }
                fscanf(input, "%d %d", &x, &y);
                *(field + x * m + y) = '#';
            }
        }

        if (my_rank == 0) {
            fprintf(output, "Iteration: 0\n");
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    fprintf(output, "%c", *(field + i * m + j));
                }
                fprintf(output, "\n");
            }
            fprintf(output, "\n\n");
        }

        MPI_Scatterv(field, send_counts, displacements, MPI_SHORT, local_field, local_n * m, MPI_SHORT, 0, MPI_COMM_WORLD);

        for (int iteration = 0; iteration < num_of_iterations; iteration++) {

             if (my_rank % 2 == 0) {
                 if (my_rank != p - 1) {
                    MPI_Sendrecv(local_field + (local_n - 1) * m, m, MPI_SHORT, my_rank + 1, 0, down_row, m, MPI_SHORT, my_rank + 1, 0, MPI_COMM_WORLD, &status);
                 }
                 if (my_rank != 0) {
                     MPI_Sendrecv(local_field, m, MPI_SHORT, my_rank - 1, 0, up_row, m, MPI_SHORT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
                 }
             }
             else {
                 MPI_Sendrecv(local_field, m, MPI_SHORT, my_rank - 1, 0, up_row, m, MPI_SHORT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
                 if (my_rank != p - 1) {
                     MPI_Sendrecv(local_field + (local_n - 1) * m, m, MPI_SHORT, my_rank + 1, 0, down_row, m, MPI_SHORT, my_rank + 1, 0, MPI_COMM_WORLD, &status);
                 }
             }    
           
            short int *next_field = (short int *) calloc(local_n * m, sizeof(short int));
            int neighbourhs_alive;
            for (int i = 0; i < local_n; i++) {
                for (int j = 0; j < m; j++) {
                    neighbourhs_alive = 0;
                    for (int x = -1; x < 2; x++) {
                        for (int y = -1; y < 2; y++) {
                            if (j + y < 0 || j + y >= m || x == 0 && y == 0) {
                                continue;
                            }
                            if (i + x < 0) {
                                if (*(up_row + j + y) == '#') {
                                    neighbourhs_alive++;
                                }
                                continue;
                            }
                            if (i + x == local_n) {
                                if (*(down_row + j + y) == '#') {
                                    neighbourhs_alive++;
                                }
                                continue;
                            }
                            if (*(local_field + (i + x) * m + j + y) == '#') {
                                neighbourhs_alive++;
                            }
                        }
                    }
                    if (*(local_field + i * m + j) == '#' && (neighbourhs_alive == 2 || neighbourhs_alive == 3) || *(local_field + i * m + j) == '-' && neighbourhs_alive == 3) {
                        *(next_field + i * m + j) = '#';
                    }
                    else {
                        *(next_field + i * m + j) = '-';
                    }
                }
            }
            free(local_field);
            local_field = next_field;

            MPI_Gatherv(local_field, local_n * m, MPI_SHORT, field, send_counts, displacements, MPI_SHORT, 0, MPI_COMM_WORLD);

            if (my_rank == 0 && (no_step_output != 'y' || no_step_output == 'y' && iteration == num_of_iterations - 1)) {
                fprintf(output, "Iteration: %d\n\n", iteration + 1);
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < m; j++) {
                        fprintf(output, "%c", *(field + i * m + j));
                    }
                    fprintf(output, "\n");
                }
                fprintf(output, "\n\n");
            }
        }
        free(field);
        free(local_field);
        free(send_counts);
        free(displacements);
        free(up_row);
        free(down_row);
    }
    if (my_rank == 0) {
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(output, "Program execution time: %f seconds\n", cpu_time_used);
        fprintf(stdout, "Program execution time: %f seconds\n", cpu_time_used);
        fclose(input);
        fclose(output);
    }

    MPI_Finalize();

	return 0;
}
