/*Tomé Maseda Dorado 34283930S
  Julián Barcia Facal 46294725A*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

#define DEBUG 0

/* Translation of the DNA bases
	A -> 0
	C -> 1
	G -> 2
	T -> 3
	N -> 4*/


#define M  1000 // Number of sequences
#define N  200000  // Number of bases per sequence

// The distance between two bases
int base_distance(int base1, int base2){

	if((base1 == 4) || (base2 == 4)){
		return 3;
	}

	if(base1 == base2) {
		return 0;
	}

	if((base1 == 0) && (base2 == 3)) {
		return 1;
	}

	if((base2 == 0) && (base1 == 3)) {
		return 1;
	}

	if((base1 == 1) && (base2 == 2)) {
		return 1;
	}

	if((base2 == 2) && (base1 == 1)) {
		return 1;
	}

	return 2;
}

int main(int argc, char *argv[] ) {

	int i, j, rank, numprocs, posLocal;
	int *data1, *data2, *data1local, *data2local;
	int *result, *resultLocal;
	int *computingTime, *communicationTime;
	struct timeval  tv1, tv2;
  
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int tam_block = ceil((float) M/ (float) numprocs);
		
	data1 = (int *) malloc(tam_block*numprocs*N*sizeof(int));
	data2 = (int *) malloc(tam_block*numprocs*N*sizeof(int));
	result = (int *) malloc(M*sizeof(int));
	/*Si el tamaño de la matriz no es divisible entre el numero de 
	 * procesos completamos la matriz con posiciones (que no se van 
	 * a usar) de relleno hasta que el tamaño de esta sea múltiplo.*/
		
	
	data1local = (int *) malloc(tam_block*N*sizeof(int));
	data2local = (int *) malloc(tam_block*N*sizeof(int));
	resultLocal = (int *) malloc(tam_block*sizeof(int));
	computingTime = (int *) malloc(numprocs*sizeof(int));
	communicationTime = (int *) malloc(numprocs*sizeof(int));
	
	if (rank == 0) {
		/* Initialize Matrices */
		for(i=0;i<M;i++) {
			for(j=0;j<N;j++) {
			data1[i*N+j] = (i+j)%5;
			data2[i*N+j] = ((i-j)*(i-j))%5;
			}
		}
	}
	
	gettimeofday(&tv1, NULL);
	MPI_Scatter(data1, tam_block*N, MPI_INT, data1local, tam_block*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(data2, tam_block*N, MPI_INT, data2local, tam_block*N, MPI_INT, 0, MPI_COMM_WORLD);
	gettimeofday(&tv2, NULL); 
	int t1_COMM = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);
	
	int begin = rank*tam_block;
	int end = (rank+1)*tam_block;
	if (rank == (numprocs-1))
		end = M;
		
	gettimeofday(&tv1, NULL);	
	for(i=begin;i<end;i++) {
		posLocal = i%tam_block;
		resultLocal[posLocal]=0;
		for(j=0;j<N;j++) {
			resultLocal[posLocal] += base_distance(data1local[posLocal*N+j], data2local[posLocal*N+j]);
		}		
	}
	gettimeofday(&tv2, NULL); 
	int computingTimeLocal = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);
	
	
	gettimeofday(&tv1, NULL);
	MPI_Gather(resultLocal, tam_block, MPI_INT, result, tam_block, MPI_INT, 0, MPI_COMM_WORLD);
	gettimeofday(&tv2, NULL); 
	int t2_COMM = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);
	
	int communicationTimeLocal = t1_COMM+t2_COMM;
	
	MPI_Gather(&computingTimeLocal, 1, MPI_INT, computingTime, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Gather(&communicationTimeLocal, 1, MPI_INT, communicationTime, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		/*Display result */
		if (DEBUG){
			for(i = 0; i < M; i++) {
				printf(" %d \t ", result[i]);
			}
		} else {
			for(i = 0; i < numprocs; i++) {
				printf("Computing time (seconds) from process %d = %lf\n", i, (double) computingTime[i]/1E6);
				printf("Communication time (seconds) from process %d = %lf\n", i, (double) communicationTime[i]/1E6);
			}
		}
	}   

	free(data1); free(data2); free(data1local); free(data2local);
	free(result); free(resultLocal); 
	free(computingTime); free(communicationTime);
	
	MPI_Finalize();

	return 0;
}
