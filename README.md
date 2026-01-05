# Artifact for 'Dynamic Contract Verification for Parallel Programming Models'

To reproduce the results in the paper, the JUBE benchmarking system is required.
Additionally, the module structure may need to be adapted to the target system.

The artifact is structured as follows:
- `archive.7z`: Contains all results used in the paper
- `jube_common.xml`: Generic Setup of CoVer, MUST and module system for both the correctness and performance benchmarks
- `jube_performance.xml`: JUBE benchmark definitions for the performance analysis. See [the reproduction steps](#reproduction-steps---performance) on how to use it.
- `jube_correctness.xml`: JUBE benchmark definitions for the classification quality analysis. See [the reproduction steps](#reproduction-steps---correctness) on how to use it.
- `bench_data/`: Supporting files required to execute the benchmarks. See [here](bench_data/README.md) for details.
- `bench_scripts/`: Supporting files used to parse the results of the benchmarks. See [here](bench_scripts/README.md) for details.
- `Tool_Sources/`: Source Code for the MUST and CoVer versions used in this evaluation.
- `Correctness_Sources/`: Source Code for the classification quality test files from RMARaceBench and MPI-BugBench.
- `Performance_Sources/`: Source Code for the proxy applications used for the runtime overhead analysis.

## Reproduction Steps

The benchmarks are designed to run on the Lichtenberg II cluster.
To adapt to a different environment, modify the job submission script in `bench_data/submit.job.in` and the module setup in `jube_common.xml` as needed.
Installation of the JUBE benchmarking framework is required. See [here](https://apps.fz-juelich.de/jsc/jube/docu/tutorial.html#installation) for details.

The benchmark system makes use of the JUBE tag system to specify options.
Some of them are shared across the performance and correctness benchmarks:
- `must(_filtered)`: Run the benchmark using MUST / MUST with filtered instrumentation
- `cover(_filtered)`: Run the benchmark using CoVer-Dynamic / CoVer-Dynamic with filtered instrumentation
- `base`: Run the benchmark without any analysis tools
- `ompi4`: Use OpenMPI 4.1.6 for this benchmark (if omitted, will use OpenMPI 5.0.5 instead, which may deadlock the MUST execution)
Specifying e.g. both `must` and `cover` will lead to both MUST and CoVer being tested for this benchmark execution.

### Reproduction Steps - Performance

To run the performance benchmarks, run `jube run jube_performance.xml -t must must_filtered cover cover_filtered base ompi4 submit`.
The tags given specify the options used during the original execution for the paper result data:
Additional options used:
- `submit`: Submit the benchmarks as jobs to SLURM. If omitted, the benchmarks will run locally (one-node execution, useful for testing)

### Reproduction Steps - Correctness

To run the correctness benchmarks, run `jube run jube_correctness.xml -t must must_filtered cover cover_filtered <RRB or MBB>`.
Specifying `RRB` will run the RMARaceBench tests, while `MBB` will run the MPI-BugBench tests.
However, specifying both will lead to an error. Run the command once for each benchmark suite instead.
