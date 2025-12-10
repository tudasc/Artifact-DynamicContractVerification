/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: Invalid datatype: MPI_DATATYPE_NULL

  Version of MPI: 1.0

  Category: COLL

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR InvalidParam
  | InvalidParam-Type-mpi_bcast
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

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (nprocs < 2)
    printf(
        "MBB ERROR: This test needs at least 2 processes to produce a bug!\n");

  int *buf = (int *)calloc(10, sizeof(int));

  /*MBBERROR_BEGIN*/ MPI_Bcast(buf, 10, MPI_DATATYPE_NULL, 0,
                               MPI_COMM_WORLD); /*MBBERROR_END*/
  free(buf);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
