/* ///////////////////////// The MPI Bug Bench ////////////////////////

  Description: Missmatching message tags in iteration 10

  Version of MPI: 0.0

 This testcase can result in a Deadlock


  Category: P2P

BEGIN_MBB_TESTS
  $ mpirun -np 2 ${EXE}
  | ERROR GlobalParameterMissmatch
  | GlobalParameterMissmatch-tag-mpi_send
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

  /*MBBERROR_BEGIN*/
#define MSG_TAG_A 124523
#define N 10
#define EVEN 0

  int buffer[N];
  int i;

  MPI_Request request;
  MPI_Status status;

  int countEvenNumbers = 0;

  for (i = 0; i < 10; i++) {
    if (rank == 0) {
      int tag_sender = i * N;
      MPI_Isend(buffer, 1, MPI_INT, 1, tag_sender, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }

    else if (rank == 1) {
      int tag_receiver = i * N;

      if (i % 2 == EVEN) {
        (countEvenNumbers)++;
      }

      if ((countEvenNumbers) == (N / 2)) {
        tag_receiver++; // mismatch
      }

      printf("Count Even Numbers: %d \n", countEvenNumbers);
      MPI_Irecv(buffer, 1, MPI_INT, 0, tag_receiver, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }
  }
  /*MBBERROR_END*/

  MPI_Finalize();
  printf("Rank %d finished normally\n", rank);
  return 0;
}
