/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: A function tries to get cartesian information of NULL

  Version of MPI: 1.0

  Category: COLL

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR InvalidParam
  | InvalidParam-Comm-mpi_cart_create-mpi_cart_get
END_MBB_TESTS
//////////////////////       End of MBI headers        /////////////////// */

#include <mpi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int nprocs = -1;
  int rank = -1;
  MPI_Comm mpi_comm_0 = MPI_COMM_NULL;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (nprocs < 2)
    printf(
        "MBB ERROR: This test needs at least 2 processes to produce a bug!\n");

  int periods[2] = {1, 1};
  int dims[2] = {0, 0};
  int coords[2] = {0, 0};
  MPI_Dims_create(nprocs, 2, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &mpi_comm_0);
  /*MBBERROR_BEGIN*/ MPI_Cart_get(MPI_COMM_NULL, 2, dims, periods,
                                  coords); /*MBBERROR_END*/
  MPI_Comm_free(&mpi_comm_0);
  /*MBBERROR_BEGIN*/ MPI_Cart_get(NULL, 2, dims, periods,
                                  coords); /*MBBERROR_END*/
  MPI_Comm_free(&mpi_comm_0);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
