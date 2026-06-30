# Auxiliary data for the benchmark suite

- submit.job.in: Input sbatch script, which gets configured by JUBE
- tea.{in, problems}: Input and Verification file for TeaLeaf
- shmem.h: Copy of the Sandia OpenSHMEM 1.5.3 header.
  - Included to avoid hardcoding the header path to a system directory for CoVer, as no CMake configure file for OpenSHMEM exists to allow for dynamic discovery (e.g. FindMPI).
- no_err_on_exit.patch: MUST ordinarily prefixes error reports AND "no errors to report" with MUST-REPORT. For easier parsing, this patch modifies the prefix of the latter.
- must_filtered_compiler.sh: A compiler wrapper that runs CoVer to generate a detection report JSON, then the MUST compiler wrapper using that json for filtered instrumentation