/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: GlobalParameterMissmatch: mpi_comm_dup

  Version of MPI: 1.0

 This testcase can result in a Deadlock


  Category: P2P

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR GlobalParameterMissmatch
  | GlobalParameterMissmatch-Comm-mpi_recv
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

  MPI_Comm_dup(MPI_COMM_WORLD, &mpi_comm_0);

  int *buf = (int *)calloc(10, sizeof(int));

  if (rank == 0) {
    /*MBBERROR_BEGIN*/ MPI_Recv(buf, 10, MPI_INT, 1, 0, mpi_comm_0,
                                MPI_STATUS_IGNORE); /*MBBERROR_END*/
  }
  if (rank == 1) {
    MPI_Send(buf, 10, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  free(buf);
  MPI_Comm_free(&mpi_comm_0);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
