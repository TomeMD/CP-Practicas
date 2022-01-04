/*Tomé Maseda Dorado 34283930S
  Julián Barcia Facal 46294725A*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int i, j, prime, done = 0, n, count, rank, numprocs, suma;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!done)
    {
        if (rank == 0) {
			printf("Enter the maximum number to check for primes: (0 quits) \n");
			scanf("%d",&n);
		}
		
		if (rank == 0) {
			for (i = 1; i < numprocs; i++)
				MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}else{
			MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}
		
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
        
        suma = 0;
        if (rank == 0) {
			suma += count;
			for (i = 1; i < numprocs; i++) {
				MPI_Recv(&count, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				suma += count;
			} 
		}else{
			MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}

        
        
		if (rank == 0) {
			printf("The number of primes lower than %d is %d\n", n, suma);
		}
    }
    
    MPI_Finalize();
}
