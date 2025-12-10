/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: not allocated send buffer

  Version of MPI: 1.0

  Category: P2P

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR InvalidParam
  | InvalidParam-Buffer-mpi_sendrecv
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

  int *not_allocated;

  int *buf = (int *)calloc(10, sizeof(int));

  int *recv_buf = (int *)calloc(10, sizeof(int));

  if (rank == 0) {
    /*MBBERROR_BEGIN*/ MPI_Sendrecv(not_allocated, 10, MPI_INT, 1, 0, recv_buf,
                                    10, MPI_INT, 1, 0, MPI_COMM_WORLD,
                                    MPI_STATUS_IGNORE); /*MBBERROR_END*/
  }
  if (rank == 1) {
    /*MBBERROR_BEGIN*/ MPI_Sendrecv(not_allocated, 10, MPI_INT, 0, 0, recv_buf,
                                    10, MPI_INT, 0, 0, MPI_COMM_WORLD,
                                    MPI_STATUS_IGNORE); /*MBBERROR_END*/
  }
  free(buf);
  free(recv_buf);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
