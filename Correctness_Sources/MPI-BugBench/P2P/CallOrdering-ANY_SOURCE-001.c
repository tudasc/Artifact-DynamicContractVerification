/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: order of messages is indeterministic, may lead to a deadlock

  Version of MPI: 1.0

 This testcase can result in a Deadlock


  Category: P2P

BEGIN_MBB_TESTS
  $ mpirun -np 3 ${EXE}
  | ERROR CallOrdering
  | CallOrdering-ANY_SOURCE
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
  if (nprocs < 3)
    printf(
        "MBB ERROR: This test needs at least 3 processes to produce a bug!\n");

  int *buf = (int *)calloc(10, sizeof(int));

  if (rank != 0) {
    buf[0] = rank;
    MPI_Send(buf, 10, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  if (rank == 0) {
    for (int i = 1; i < nprocs; ++i) {
      MPI_Recv(buf, 10, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      if (buf[0] != i) {
        /*MBBERROR_BEGIN*/ MPI_Recv(buf, 10, MPI_INT, 1, 0, MPI_COMM_WORLD,
                                    MPI_STATUS_IGNORE); /*MBBERROR_END*/
      }
    }
  }
  if (rank == 1) {
    MPI_Send(buf, 10, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  free(buf);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
