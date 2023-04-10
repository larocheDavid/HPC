#include <stdio.h>
#include <stdlib.h>

// intialise U et assigne les valeurs au bords
double** init_grid(int max_I, int max_J) {

    double** U = (double**) malloc(max_I*sizeof(double*));

    for (int i = 0; i < max_I; i++)
        U[i] = (double*) malloc(max_J*sizeof(double));

    for (int i=0; i<max_I; i++)
    {
        U[i][0] = 0.0; // première colonne
        U[i][max_J - 1] = 1.0; // dernière colonne
    }

    for (int j=0; j<max_J; j++)
    {
        U[0][j] = 1.0; // première rangée
        U[max_I - 1][j] = 0.0; // dernière rangée
    }
    return U;
}

void grid_evol(double **U, int max_I, int max_J, int max_T) {

    double** U_temp = (double**) malloc(max_I*sizeof(double*));

    for (int i = 0; i < max_I; i++)
        U_temp[i] = (double*) malloc(max_J*sizeof(double));

    for (int t=0; t<max_T; t++) 
    {
        for (int i=1; i<max_I-1; i++)
        {   
            for (int j=1; j<max_J-1; j++)
                U_temp[i][j] = 0.25*(U[i-1][j] + U[i+1][j] + U[i][j-1] + U[i][j+1]);
        } 
        for (int i=1; i<max_I-1; i++) {
            for (int j=1; j<max_J-1; j++)
                U[i][j] = U_temp[i][j];
        }
    }
    free(U_temp);
}

int print_grid(double **U, int max_I, int max_J) {

    FILE* fp = fopen("laplace.txt", "w");
    if (fp == NULL) {
        perror("fopen\n");
        return 1;
    }

    for (int i=0; i<max_I; i++) {
        for (int j=0; j < max_J; j++) {
            printf("%.2f ", U[i][j]);
            fprintf(fp, "%.2f ", U[i][j]);
        }
        printf("\n");
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("\n");
    return 0;
} 

int main(int argc, char** argv) { 

    int max_I = atoi(argv[1]);
    int max_J = atoi(argv[2]);
    int max_T = atoi(argv[3]);
    
    double** U = init_grid(max_I, max_J);

    grid_evol(U, max_I, max_J, max_T);
    print_grid(U, max_I, max_J);

    return 0;
}