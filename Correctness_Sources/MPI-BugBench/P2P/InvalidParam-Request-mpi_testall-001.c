/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: Invalid Request Array: the same request is not allowed to be in
the array multiple times

  Version of MPI: 1.0

  Category: P2P

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR InvalidParam
  | InvalidParam-Request-mpi_testall
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
  MPI_Request mpi_request_0 = MPI_REQUEST_NULL;
  int int_0 = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (nprocs < 2)
    printf(
        "MBB ERROR: This test needs at least 2 processes to produce a bug!\n");

  int *buf = (int *)calloc(10, sizeof(int));

  if (rank == 0) {
    MPI_Irecv(buf, 10, MPI_INT, 1, 0, MPI_COMM_WORLD, &mpi_request_0);
    while (!int_0) {
      MPI_Request req_array[2];
      req_array[0] = mpi_request_0;
      req_array[1] = mpi_request_0;
      /*MBBERROR_BEGIN*/ MPI_Testall(1, req_array, &int_0,
                                     MPI_STATUSES_IGNORE); /*MBBERROR_END*/
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
