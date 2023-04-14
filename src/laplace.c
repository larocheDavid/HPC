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

void init_test(double* U, int max_I, int max_J) {
    
    for (int i=0; i<max_I; i++)
    {
        for (int j=0; j<max_J; j++)
        {
            U[i*max_J+j] = i;
        }
    }
}   

/*
double** init_grid(int max_I, int max_J) {

    double** U = (double**) malloc(max_I*sizeof(double*));
    U[0] = (double*)calloc(max_I*max_J, sizeof(double));

    for (int i = 1; i < max_I; i++)
        U[i] = U[0] + i * max_J;
        
    for (int i=0; i<max_I; i++)
    {
        U[i][0] = FIRST_COL; 
        U[i][max_J-1] = LAST_COL;
    }

    for (int j=0; j<max_J; j++)
    {
        U[0][j] = FIRST_ROW;
        U[max_I-1][j] = LAST_ROW;
    }
    return U;
}


void init_grid(int max_I, int max_J, double U[max_I][max_J]) {

    for (int i=0; i<max_I; i++)
    {
        U[i][0] = FIRST_COL; 
        U[i][max_J-1] = LAST_COL;
    }

    for (int j=0; j<max_J; j++)
    {
        U[0][j] = FIRST_ROW;
        U[max_I-1][j] = LAST_ROW;
    }
}


double* init_grid2(int max_I, int max_J) {

    double* U = calloc(max_I*max_J, sizeof(double));
        
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
    return U;
}*/


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
    printf("\n\n");
    return 0;
}
/*
void compute_grid(double **U, double **U_temp, int max_I, int max_J, int max_T, int myRank, int nProc, int myStart, int myEnd) {

    double *top_data;
    double *bottom_data = (double*) malloc(max_J * sizeof(double));

    for (int t=0; t<max_T; t++) 
    {
        if(myRank%2==0) {
            if(myRank < nProc-1) {
                bottom_data = U[myEnd];
                MPI_Send(bottom_data, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
            } 
        } 
        else {
            if(myRank > 0) { 
                //top_data = U[myStart];
                MPI_Recv(bottom_data, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        
        for (int i = myStart + 1; i<=myEnd; i++)
        {
            if(i==myEnd && myRank < nProc-1) {
                 
            }
            for (int j=1; j<max_J-1; j++)
                U_temp[i][j] = 0.25*(U[i-1][j] + U[i+1][j] + U[i][j-1] + U[i][j+1]);
                
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i= myStart + 1; i<=myEnd; i++){ // actualise U
            for (int j=1; j<max_J-1; j++)
                U[i][j] = U_temp[i][j];
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    free(bottom_data);
}*/

void compute_grid(double **U, double **U_temp, int max_J, int max_T, int myRank, int nProc, int myStart, int myEnd) {

    //double *top_data;
    double *bottom_data = (double*) malloc(max_J * sizeof(double));

    for (int t=0; t<max_T; t++) 
    {
        /*if(myRank%2==0) {
            if(myRank < nProc-1) {
                bottom_data = U[myEnd];
                MPI_Send(bottom_data, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
            } 
        } 
        else {
            if(myRank > 0) { 
                //top_data = U[myStart];
                MPI_Recv(bottom_data, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }*/
        
        for (int i = myStart + 1; i<=myEnd; i++)
        {
            if(i==myEnd && myRank < nProc-1) {
                 
            }
            for (int j=1; j<max_J-1; j++)
                U_temp[i][j] = 0.25*(U[i-1][j] + U[i+1][j] + U[i][j-1] + U[i][j+1]);
                
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i= myStart + 1; i<=myEnd; i++){ // actualise U
            for (int j=1; j<max_J-1; j++)
                U[i][j] = U_temp[i][j];
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    free(bottom_data);
}

void grid_evol(double* U, int max_I, int max_J, int max_T) {
    double* U_temp = (double*) malloc(max_I * max_J * sizeof(double));
    for (int t = 0; t < max_T; t++) {
        for (int i = 1; i < max_I - 1; i++) {
            for (int j = 1; j < max_J - 1; j++) {
                int index = i * max_J + j;
                U_temp[index] = 0.25 * (U[(i - 1) * max_J + j] + U[(i + 1) * max_J + j] +
                    U[i * max_J + j - 1] + U[i * max_J + j + 1]);
            }
        }
        for (int i = 1; i < max_I - 1; i++) {
            for (int j = 1; j < max_J - 1; j++) {
                int index = i * max_J + j;
                U[index] = U_temp[index];
            }
        }
    }
    free(U_temp);
}

/*
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
}*/

/*
void grid_evol2(double **U, double** U_temp, int myStart, int myEnd, int max_J, int max_T) {

    double* U_temp = malloc(max_I*max_J*sizeof(double));

    //for (int i = 0; i < max_I; i++)band_size
    //    U_temp[i] = (double*) malloc(max_J*sizeof(double));

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
*/
/*
void compute_band(double **band, double** band_temp, int band_size, int max_J, int max_T) {

    for (int t=0; t<max_T; t++) 
    {
        for (int i=0; i<band_size; i++)
        {
            for (int j=1; j<max_J; j++)
                band_temp[i][j] = 0.25*(band[i-1][j] + band[i+1][j] + band[i][j-1] + band[i][j+1]);
        } 
        for (int i=0; i<band_size; i++) {
            for (int j=1; j<max_J; j++)
                band[i][j] = band_temp[i][j];
        }
    }
}

void compute_band(double* band, int band_size, int max_J) {

    double* band_temp = malloc(band_size*max_J *sizeof(double));

    for (int i = 0; i < band_size; i++) {
        for (int j = 1; j < max_J; j++) {
            int index = i * max_J + j;
            band_temp[index] = 0.25 * (band[(i - 1) * max_J + j] + band[(i + 1) * max_J + j] +
                band[i * max_J + j - 1] + band[i * max_J + j + 1]);
        }
    }
    for (int i = 0; i < band_size; i++) {
        for (int j = 1; j < max_J; j++) {
            int index = i * max_J + j;
            band[index] = band_temp[index];
        }
    }
}
*/
void band_evol(double* U, int max_I, int max_J) {
    double* U_temp = (double*) malloc(max_I * max_J * sizeof(double));

    for (int i = 1; i < max_I - 1; i++) {
        for (int j = 1; j < max_J - 1; j++) {
            int index = i * max_J + j;
            U_temp[index] = 0.25 * (U[(i - 1) * max_J + j] + U[(i + 1) * max_J + j] +
                U[i * max_J + j - 1] + U[i * max_J + j + 1]);
        }
    }
    for (int i = 1; i < max_I - 1; i++) {
        for (int j = 1; j < max_J - 1; j++) {
                int index = i * max_J + j;
                U[index] = U_temp[index];
            }
    }
    free(U_temp);
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

void compute_scatterv_params(int *band_size, int *sendcounts, int* displ, int nProc, int max_J) {

    for (int rank = 0; rank < nProc; rank++) { 
        
        sendcounts[rank] =  band_size[rank] * max_J;

        if (rank == ROOT) {
            displ[ROOT] = 0;
        }
        else displ[rank] = displ[rank-1] + sendcounts[rank-1] -max_J * 2;
    }
}

void update_row(double *old_row, double *new_row, int max_J) {
    for (int i=0; i<max_J; i++)
        old_row[i] = new_row[i];
}

void print_band(double* band, int nrows, int max_J) {
    for (int i = 0; i<nrows; i++) {
        for (int j=0; j<max_J; j++)
            printf("%.0f ", band[i*max_J+j]);

        printf("\n");
    }
    printf("\n");
}

void send_rows(double* band, int nProc, int band_size, int myRank, int max_J) {

    // names relatives to process
    double *send_top_row = band +  max_J;
    double *recv_top_row = malloc(max_J* sizeof(double)); 

    double *send_bottom_row = band + (band_size -2) * max_J;
    double *recv_bottom_row = malloc(max_J* sizeof(double));

    if (myRank == ROOT) {
        MPI_Send(send_bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
        //MPI_Recv(recv_bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Recv(recv_top_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        print_band(band, band_size, max_J);
        printf("\n");
        print_band(recv_top_row, 1, max_J);
       
        //MPI_Send(send_top_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD);
    }
    
    /*if (myRank%2 == 0 && nProc > 1) {

        if (myRank == ROOT) {
            MPI_Send(band + bottom_row_idx, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
            MPI_Recv(bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (myRank == nProc-1) {
            MPI_Send(band + displs[myRank], max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD);
            MPI_Recv(bottom_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else 
    {
        if (myRank == ROOT) {
            MPI_Recv(bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(band + bottom_row_idx, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
        }
        if (myRank == nProc-1) {
            MPI_Recv(bottom_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(band + displs[myRank], max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD);

            }
    }*/
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

    double* U = NULL;
    int band_size[nProc];
    int sendcounts[nProc];
    int displs[nProc];

    if(myRank == ROOT) { // Root create grid
        U = calloc(max_I*max_J, sizeof(double));
        init_test(U, max_I, max_J);
        //grid_evol(U, max_I, max_J, max_T);
        print_band(U, max_I, max_J);
    }
    
    get_band_sz(band_size, nProc, max_I);
    compute_scatterv_params(band_size, sendcounts, displs, nProc, max_J);
    
    MPI_Barrier(MPI_COMM_WORLD);
    double* recv_band = malloc(sendcounts[myRank]* sizeof(double));
    //for (int i=0; i<nProc; i++) { printf("rank %d, dsipl %d, band_size %d, sendcounts %d\n", i, displs[i], band_size[i], sendcounts[i]);}
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatterv(U, sendcounts, displs, MPI_DOUBLE, recv_band, sendcounts[myRank], MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    //band_evol(recv_band, band_size[myRank], max_J);
    MPI_Barrier(MPI_COMM_WORLD);

    //send_rows(recv_band, nProc, band_size[myRank], myRank, max_J);

    double *send_top_row = recv_band +  max_J;
    double *recv_top_row = malloc(max_J* sizeof(double)); 

    double *send_bottom_row = recv_band + (band_size[myRank] -2) * max_J;
    double *recv_bottom_row = malloc(max_J* sizeof(double));

    if (myRank == ROOT) {
        MPI_Send(send_bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD);
        //MPI_Recv(recv_bottom_row, max_J, MPI_DOUBLE, myRank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Recv(recv_top_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        print_band(recv_band, band_size, max_J);
        printf("\n");
        print_band(recv_top_row, 1, max_J);
        //MPI_Send(send_top_row, max_J, MPI_DOUBLE, myRank-1, 0, MPI_COMM_WORLD);
    }
    

    MPI_Barrier(MPI_COMM_WORLD);

    if (myRank == ROOT) {
        free(U);
        free(recv_band);
    }
    MPI_Finalize();
}