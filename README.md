# Artifact for 'Dynamic Contract Verification for Parallel Programming Models'

To reproduce the results in the paper, the JUBE benchmarking system is required.
Additionally, the module structure may need to be adapted to the target system.

The artifact is structured as follows:
- `archive/`: This folder contains all results used in the paper
- `jube_common.xml`: Generic Setup of CoVer, MUST and module system for both the correctness and performance benchmarks
- `jube_performance.xml`: JUBE benchmark definitions for the performance analysis. See [the reproduction steps](#reproduction-steps---performance) on how to use it.
- `jube_correctness.xml`: JUBE benchmark definitions for the classification quality analysis. See [the reproduction steps](#reproduction-steps---correctness) on how to use it.
- `bench_data/`: Supporting files required to execute the benchmarks. See [here](bench_data/README.md) for details.
- `bench_scripts/`: Supporting files used to parse the results of the benchmarks. See [here](bench_scripts/README.md) for details.
- `Tool_Sources/`: Source Code for the MUST and CoVer versions used in this evaluation.
- `Correctness_Sources/`: Source Code for the classification quality test files from RMARaceBench and MPI-BugBench.
- `Performance_Sources/`: Source Code for the proxy applications used for the runtime overhead analysis.

## Reproduction Steps - Performance

## Reproduction Steps - Correctness
