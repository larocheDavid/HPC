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

// intialise U et assigne les valeurs aux bords
double** init_grid(int max_I, int max_J) {

    double** U = (double**) malloc(max_I*sizeof(double*));

    for (int i = 0; i < max_I; i++)
        U[i] = calloc(max_J, sizeof(double));
        
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

void grid_evol2(double **U, double** U_temp, int myStart, int myEnd, int max_J, int max_T) {

    //double** U_temp = (double**) malloc(max_I*sizeof(double*));

    //for (int i = 0; i < max_I; i++)band_size
    //    U_temp[i] = (double*) malloc(max_J*sizeof(double));

    for (int t=0; t<max_T; t++) 
    {
        for (int i=myStart+1; i<myEnd; i++)
        {   
            for (int j=1; j<max_J-1; j++)
                U_temp[i][j] = 0.25*(U[i-1][j] + U[i+1][j] + U[i][j-1] + U[i][j+1]);
        } 
        for (int i=myStart+1; i<myEnd; i++) {
            for (int j=1; j<max_J-1; j++)
                U[i][j] = U_temp[i][j];
        }
    }
    //free(U_temp);
}

void compute_band(double **band, double** band_temp, int band_size, int max_J, int max_T) {

    for (int t=0; t<max_T; t++) 
    {
        for (int i=0; i<band_size; i++)
        {
            for (int j=1; j<max_J-1; j++)
                band_temp[i][j] = 0.25*(band[i-1][j] + band[i+1][j] + band[i][j-1] + band[i][j+1]);
        } 
        for (int i=0; i<band_size; i++) {
            for (int j=1; j<max_J-1; j++)
                band[i][j] = band_temp[i][j];
        }
    }
}

void get_band_sz(int* band_sz, int nProc, int size) {

    for (int rank=0; rank<nProc; rank++) {

        if (nProc>1) {
            band_sz[rank]= size / nProc;
            
            if (rank == ROOT || rank == nProc-1) {// add 1 row for processes on edge's domain otherwise 2
                band_sz[rank] += 1; 
            }
            else  band_sz[rank] += 2;

            if (rank < size % nProc) // Distribute remaining rows to process if can't divide
                band_sz[rank] += 1;
        }
        else band_sz[rank] = size;
    }
}

void compute_scatterv_params(int *band_size, int *sendcounts, int* displs, int nProc, int max_J) {

    int offset = 0;
    for (int rank = 0; rank < nProc; rank++) { 
        
        sendcounts[rank] =  band_size[rank] * max_J;
        offset += band_size[rank] * max_J;
        displs[rank] = offset; // -1 for overlaping row on precedent band
        //printf("myRank %d, band_sz %d\n", rank, band_size[rank]);
    }
    displs[ROOT] = 0;
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

    double** U = NULL;

    int band_size[nProc];
    int sendcounts[nProc];
    int displs[nProc];

    if(myRank == ROOT) { // Root create grid
        U = init_grid(max_I, max_J); // Helvetii
        grid_evol(U, max_I, max_J, max_T);
        print_grid(U, max_I, max_J);

        get_band_sz(band_size, nProc, max_I);
        compute_scatterv_params(band_size, sendcounts, displs, nProc, max_J);
        for (int i=0; i<nProc; i++) { printf("rank %d, dsipl %d, band_size %d, sendcounts %d\n", i, displs[i]/max_J, band_size[i], sendcounts[i]);}
    }
    MPI_Barrier(MPI_COMM_WORLD);

    double** recv_band = (double**)malloc(band_size[myRank] * sizeof(double*));
    for (int i = 0; i < band_size[myRank]; i++) {
        recv_band[i] = (double*)malloc(max_J * sizeof(double));
    }

    MPI_Scatterv(&(U[0][0]), sendcounts, displs, MPI_DOUBLE, &(recv_band[0][0]), sendcounts[myRank], MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    //print_grid(band, band_size[myRank] , max_J);

    for (int i = 0; i<band_size[myRank]; i++) {
        for (int j=0; j<max_J; j++) {
            //printf("%.2f ", recv_band[i*(max_J-1) + j]);
            printf("%.2f ", recv_band[i][j]);
        }
        printf("\n");
    }


    /*
    int band_size = max_I / nProc; // Divide domain in bands of rows
    if (nProc>1) {
        if (myRank == ROOT || myRank == nProc-1) {// Remove 1 row for processes on edge's domain.
            band_size += 1; 
        }
        else  band_size += 2;
    }
    

    int band_sz[nProc];
    int rem = max_I % nProc;
    for (int i=0; i<nProc; i++) {

        band_sz[i] = max_I / nProc;
        
        if (nProc>1) {
            if (myRank == ROOT || myRank == nProc-1) {// add 1 row for processes on edge's domain otherwise 2
                band_sz[i] += 1; 
            }
            else  band_sz[i] += 2;

            if (myRank<rem) // Distribute remaining rows to process if can't divide
                band_sz[i] += 1;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i=0; i<nProc && myRank == 0; i++)
            printf("myRank %d, band_sz %d\n\n", myRank, band_sz[i]);

    

    int sendcounts[nProc];
    int displs[nProc];

    for (int i = 0; i < nProc; i++) { 
        
        sendcounts[i] = band_size * max_J;
        displs[i] = i * (band_size -1) * max_J; // -1 for overlaping row on precedent band
        
        //if (i == ROOT) {displs[i] += band_size;}

        MPI_Barrier(MPI_COMM_WORLD);
        printf("myRank %d,", myRank);
        printf(" sendcounts[i] %d,", sendcounts[i]);
        printf(" band_size %d,", band_size);
        printf(" displs[i] %d\n", displs[i]);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    double band[band_size][max_J];
    //double* band = (double *)malloc(band_size * max_J * sizeof(double));
    MPI_Scatterv(U, sendcounts, displs, MPI_DOUBLE, band, band_size * max_J, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    double* band_temp[band_size][max_J];

    //int myStart =  myRank * (band_size -1);
    //int myEnd = myStart + band_size;

    //grid_evol2(band, band_temp, myStart, myEnd, max_J, max_T);
    //compute_grid(U, U_temp, max_I, max_J, max_T, myRank, nProc, displs[myRank], displs[myRank]+);
    //compute_grid(U, U_temp, max_I, max_J, max_T, myRank, nProc, myStart, myEnd-1);
    //printf("myrank is %d, band sz %d, myStart %d, myEnd %d\n", myRank, band_size, myStart, myEnd);

    //clock_t start = clock();
    //compute_grid(U, U_temp, max_I, max_J, max_T, myRank, nProc, myStart, myEnd);
    //clock_t end = clock();

    

    if (myRank == ROOT) {
        free(U);
        free(U_temp);
    }*/
    //printf("Computing time %f[s]\n", ((double) (end - start)) / CLOCKS_PER_SEC);
    MPI_Finalize();
}