#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    char output_file_name[256];
    printf("Insert the name of the output file:\n");
    fgets(output_file_name, sizeof(output_file_name), stdin);
    int len=strlen(output_file_name);
    if(output_file_name[len-1]=='\n')
        output_file_name[len-1]='\0';
    FILE *file = fopen(output_file_name, "w");
    printf("Insert number of test cases:\n");
    int test_cases;
    scanf("%d", &test_cases);
    fprintf(file, "%d\n\n", test_cases);
    for (int t = 0; t < test_cases; t++) {
        int num_of_iterations;
        printf("Insert the number of simulation iterations:\n");
        scanf("%d", &num_of_iterations);
        int n, m;
        printf("Insert the size of the field (nxm):\n");
        scanf("%d %d", &n, &m);
        int count = 0;
        int **coords = (int**) malloc(2*sizeof(int *));
        *(coords) = (int *) malloc(n*m*sizeof(int));
        *(coords + 1) = (int *) malloc(n*m*sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (rand() % 2 == 0) {
                    *(*(coords) + count) = i;
                    *(*(coords + 1) + count) = j;
                    count++;
                }
            }
        }
        fprintf(file, "%d\n", num_of_iterations);
        fprintf(file, "%d %d\n", n, m);
        fprintf(file, "%d\n", count);
        for (int i = 0; i < count; i++) {
            fprintf(file, "%d %d\n", *(*(coords) + i), *(*(coords + 1) + i));
        }
        fprintf(file, "\n\n");
        free(*(coords));
        free(*(coords + 1));
        free(coords);
    }
    fclose(file);
}