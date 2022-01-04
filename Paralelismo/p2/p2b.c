/*Tomé Maseda Dorado 34283930S
  Julián Barcia Facal 46294725A*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int MPI_FlattreeColectiva(void *buff, void *recvbuff, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
	int i, j, rank, numprocs;
	int* buffer = buff;
	int* recvbuffer = recvbuff;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if ((buffer == NULL) || (recvbuffer == NULL))
		return MPI_ERR_BUFFER;
	if (comm == NULL)
		return MPI_ERR_COMM;
	if (count < 0)
		return MPI_ERR_COUNT;
	
	for (i = 0; i < count; i++) {	
		if (rank == root) {
			recvbuffer[i] = 0;
			recvbuffer[i] += buffer[i];
			for (j = 1; j < numprocs; j++) {
				MPI_Recv(&buffer[i], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				recvbuffer[i] += buffer[i];
			} 
		}else{
			MPI_Send(&buffer[i], 1, MPI_INT, root, 0, MPI_COMM_WORLD);
		}
	}
	return MPI_SUCCESS;	
}

int MPI_BinomialColectiva (void * buf, int count, MPI_Datatype datatype, MPI_Comm comm){
	int i, rank, numprocs;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (buf == NULL)
		return MPI_ERR_BUFFER;
	if (comm == NULL)
		return MPI_ERR_COMM;
	if (count < 0)
		return MPI_ERR_COUNT;
	
	if (rank != 0)
		MPI_Recv(buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
	for (i = 1; (pow(2, i-1)+rank) < numprocs; i++) {
		if (rank < pow(2, i-1))
			MPI_Send(buf, 1, MPI_INT, pow(2, i-1)+rank, 0, MPI_COMM_WORLD);
	}
	return MPI_SUCCESS;
}

int main(int argc, char *argv[])
{
    int i, j, prime, done = 0, n, count, rank, numprocs, suma;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!done)
    {
        if (rank == 0) {
			printf("Enter the maximum number to check for primes: (0 quits) \n");
			scanf("%d",&n);
		}
		
		//MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_BinomialColectiva(&n, 1, MPI_INT, MPI_COMM_WORLD);
		
        if (n == 0) break;

        count = 0;  

        for (i = 2 + rank; i < n; i += numprocs) {
            prime = 1;

            // Check if any number lower than i is multiple
            for (j = 2; j < i; j++) {
                if((i%j) == 0) {
                   prime = 0;
                   break;
                }
            }
            count += prime;
        }
        
        //MPI_Reduce(&count, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); 
        MPI_FlattreeColectiva(&count, &suma, 1, MPI_INT, 0, MPI_COMM_WORLD); 
        
		if (rank == 0) {
			printf("The number of primes lower than %d is %d\n", n, suma);
		}
    }
    
    MPI_Finalize();
}
