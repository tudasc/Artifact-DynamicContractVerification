/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: Invalid Datatype: Datatype is freed before it is actually used

  Version of MPI: 3.0

  Category: RMA

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR InvalidParam
  | InvalidParam
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

  MPI_Win mpi_win_0;
  int *winbuf;
  MPI_Win_allocate(10 * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD,
                   &winbuf, &mpi_win_0);
  MPI_Win_fence(0, mpi_win_0);
  if (rank == 0) {
    MPI_Datatype type;
    MPI_Type_contiguous(2, MPI_INT, &type);
    MPI_Type_commit(&type);
    MPI_Type_free(&type);
    /*MBBERROR_BEGIN*/ MPI_Get(buf, 10, MPI_INT, 1, 0, 10, type,
                               mpi_win_0); /*MBBERROR_END*/
  }
  MPI_Win_fence(0, mpi_win_0);
  MPI_Win_free(&mpi_win_0);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
