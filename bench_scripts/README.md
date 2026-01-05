This folder contains files used to parse the results gained during the evaluation.

The scripts here expect a csv input file gained from JUBE.
They can be generated using the `-s csv` parameter when calling `jube result` on the finished benchmark folder.

- `analyse_correctness.py`: Using the output JUBE csv of the correctness benchmarks, this creates formatted tables and calculates the accuracy metric.
- `parse_performance.py`: Using the output JUBE csv of the performance benchmark, this creates separate csvs per proxy app, which were used to generate the tables in the paper.
