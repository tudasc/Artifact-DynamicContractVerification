# Auxiliary data for the benchmark suite

- submit.job.in: Input sbatch script, which gets configured by JUBE
- tea.{in, problems}: Input and Verification file for TeaLeaf
- shmem.h: Copy of the Sandia OpenSHMEM 1.5.3 header.
  - Included to avoid hardcoding the header path to a system directory for CoVer, as no CMake configure file for OpenSHMEM exists to allow for dynamic discovery (e.g. FindMPI).
- 425.patch: Compilation fix for MUST when disabling backward (needed for simultaneous p2p and rma race detection)