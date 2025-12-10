/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: use after free for buffers

  Version of MPI: 1.0

  Category: P2P

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | OK
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

  int *buf = (int *)calloc(10, sizeof(int));

  int *recv_buf = (int *)calloc(10, sizeof(int));

  free(buf);
  free(recv_buf);
  if (rank == 0) {
    MPI_Sendrecv(buf, 10, MPI_INT, 1, 0, recv_buf, 10, MPI_INT, 1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  if (rank == 1) {
    MPI_Sendrecv(buf, 10, MPI_INT, 0, 0, recv_buf, 10, MPI_INT, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
