/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: Mixing fence with lock_all synchronization

  Version of MPI: 3.0

 This testcase can result in a Deadlock


  Category: RMA

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR EpochLifeCycle
  | EpochLifeCycle
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
  /*MBBERROR_BEGIN*/ MPI_Win_lock_all(0, mpi_win_0); /*MBBERROR_END*/
  if (rank == 0) {
    MPI_Get(buf, 10, MPI_INT, 1, 0, 10, MPI_INT, mpi_win_0);
  }
  MPI_Win_unlock_all(mpi_win_0);
  MPI_Win_fence(0, mpi_win_0);
  MPI_Win_free(&mpi_win_0);

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
