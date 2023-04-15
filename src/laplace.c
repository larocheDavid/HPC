#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

#define FIRST_COL 0.0
#define LAST_COL  1.0
#define FIRST_ROW 1.0
#define LAST_ROW  0.0

#define ROOT 0

void init_grid_test(double* U, int max_I, int max_J) {
    
    for (int i=0; i<max_I; i++) {
        for (int j=0; j<max_J; j++)
            U[i*max_J+j] = i;
    }
}

void init_temp(double* U, int max_I, int max_J) {
        
    for (int i=0; i<max_I; i++)
    {
        U[i*max_J] = FIRST_COL; 
        U[(i+1)*max_J-1] = LAST_COL;
    }

    for (int j=0; j<max_J; j++)
    {
        U[j] = FIRST_ROW;
        U[max_I*(max_J-1)+j] = LAST_ROW;
    }
}

void update_row(double *old_row, double *new_row, int max_J) {
    for (int i=0; i<max_J; i++)
        old_row[i] = new_row[i];
}

void mat_evol(double* mat, int max_I, int max_J) {
    double* mat_temp = (double*) malloc(max_I * max_J * sizeof(double));

    for (int i = 1; i < max_I - 1; i++) {
        for (int j = 1; j < max_J - 1; j++) {
            int index = i * max_J + j;
            mat_temp[index] = 0.25 * (mat[(i - 1) * max_J + j] + mat[(i + 1) * max_J + j] +
                mat[i * max_J + j - 1] + mat[i * max_J + j + 1]);
        }
    }
    for (int i = 1; i < max_I - 1; i++) {
        for (int j = 1; j < max_J - 1; j++) {
            int index = i * max_J + j;
            mat[index] = mat_temp[index];
        }
    }
    free(mat_temp);
}

void get_band_sz(int* band_sz, int nProc, int size) {
    int q = size/nProc;
    int rem = size%nProc;

    for (int rank = 0; rank < nProc; rank++) {

        if(nProc == 1) {
            band_sz[rank] = q;
        }
        else if ((rank == ROOT || rank == nProc-1)) {// add 1 row for processes on edge's domain otherwise 2
            band_sz[rank] = q + 1; 
        }
        else  band_sz[rank] = q + 2;

        if (rank < rem) // Distribute remaining rows to process if can't divide
            band_sz[rank] += 1;
    }
}

void compute_scatterv_params(int *myRows, int *sendcounts, int* displ, int nProc, int max_J) {

    for (int rank = 0; rank < nProc; rank++) { 
        sendcounts[rank] =  myRows[rank] * max_J;

        if (rank == ROOT) {
            displ[ROOT] = 0;
        }
        else displ[rank] = displ[rank-1] + sendcounts[rank-1] -max_J * 2;
    }
}

void output_mat(double* mat, int rows, int cols) {

    FILE* fp = fopen("laplace.txt", "w");

    if (fp == NULL)
        perror("fopen\n");

    for (int i = 0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            fprintf(fp, "%.2f ", mat[i*cols+j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void print_mat(double* mat, int rows, int cols) 
{   
    for (int i = 0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            printf("%.2f ", mat[i*cols+j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_mat_ref(int max_I, int max_J, int max_T) {
    double *U_test = calloc(max_I*max_J, sizeof(double));
    init_temp(U_test, max_I, max_J);

    for (int t = 0; t < max_T; t++)
        mat_evol(U_test, max_I, max_J);

    printf("mat ref:\n");
    print_mat(U_test, max_I, max_J);
    free(U_test);
}

int main(int argc, char** argv) {
	(void)argc;
    
    int myRank, nProc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProc);    
    
    int max_I = atoi(argv[1]); // lignes
    int max_J = atoi(argv[2]); // colonnes
    int max_T = atoi(argv[3]); // temps discret

    double *U = NULL;

    if(myRank == ROOT) { // Root create grid
        U = calloc(max_I*max_J, sizeof(double));
        init_temp(U, max_I, max_J);
        //print_mat(U, max_I, max_J);
        //print_mat_ref(max_I, max_J, max_T);
    }
    
    int myRows[nProc];
    int sendcounts[nProc];
    int displs[nProc];

    get_band_sz(myRows, nProc, max_I); // get number of rows per process
    compute_scatterv_params(myRows, sendcounts, displs, nProc, max_J);

    double* myBand = malloc(sendcounts[myRank] * sizeof(double));
    
    MPI_Scatterv(U, sendcounts, displs, MPI_DOUBLE, myBand, sendcounts[myRank], MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    double *send_top_row;
    double *send_bottom_row;
    double *recv_top_row = malloc(max_J* sizeof(double)); 
    double *recv_bottom_row = malloc(max_J* sizeof(double));

    clock_t start_time = clock();
    for(int t=0; t<max_T; t++) {

        MPI_Barrier(MPI_COMM_WORLD);
        mat_evol(myBand, myRows[myRank], max_J);
        MPI_Barrier(MPI_COMM_WORLD);

        send_top_row = myBand + max_J;
        send_bottom_row = myBand + (myRows[myRank] -2) * max_J;

        if (myRank%2 == 0) {
            if (myRank != nProc-1) {
                MPI_Send(send_bottom_row, max_J, MPI_DOUBLE, myRank +1, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_bottom_row, max_J, MPI_DOUBLE, myRank +1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                update_row(myBand + (myRows[myRank] -1) * max_J, recv_bottom_row, max_J);
            }
            if (myRank != ROOT) {
                MPI_Send(send_top_row, max_J, MPI_DOUBLE, myRank -1, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_top_row, max_J, MPI_DOUBLE, myRank -1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                update_row(myBand, recv_top_row, max_J);
            }
        } else
        {
            if (myRank != nProc-1) {
                MPI_Recv(recv_bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(send_bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
                update_row(myBand + (myRows[myRank] -1) * max_J, recv_bottom_row, max_J);
            }
            if (myRank != ROOT) {
                MPI_Recv(recv_top_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(send_top_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD);
                update_row(myBand, recv_top_row, max_J);
            }
        }
    }
    clock_t end_time = clock();
    MPI_Gatherv(myBand, sendcounts[myRank], MPI_DOUBLE, U, sendcounts, displs, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    
    if(myRank == ROOT) {
        //printf("Result\n");
        //print_mat(U, max_I, max_J);
        //output_mat(U, max_I, max_J);
        printf("Elapsed time: %.3f seconds\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);
        free(U);
    }
    free(recv_top_row);
    free(recv_bottom_row);
    free(myBand);
    MPI_Finalize();
}