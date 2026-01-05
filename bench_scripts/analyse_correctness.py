import os
import sys
import re
import pandas as pd

if len(sys.argv) < 2:
    print(f"Insufficient arguments!\nUsage: {os.path.basename(__file__)} <JUBE output csv> [\"shmem\" if analysing shmem tests]")
    exit(1)

# Read in the CSV
raw_data = pd.read_csv(sys.argv[1])

# Check mode
mode = "mpi"
if len(sys.argv) > 2:
    if sys.argv[2] == "shmem": mode = sys.argv[2]

explicit_allow = [
    # Tests detectable without DL
    "CallOrdering-unmatched-mpi_allgather-mpi_iscatter-001",
    "CallOrdering-unmatched-mpi_allgather-mpi_iscan-001",
    # Tests with DL but detectable using static-dynamic coupling
    "CallOrdering-unmatched-mpi_allgather-mpi_igather-001",
    "CallOrdering-unmatched-mpi_allgather-mpi_ibarrier-001",
    "CallOrdering-unmatched-mpi_allgather-mpi_ibcast-001",
    "CallOrdering-unmatched-mpi_allgather-mpi_ireduce-001",
    "CallOrdering-unmatched-mpi_allgather-mpi_iallreduce-001",
    "CallOrdering-unmatched-mpi_allgather-mpi_ialltoall-001",
    # Double request use
    "InvalidParam-Request-mpi_start-002",
    "InvalidParam-Request-mpi_startall-001",
]
explicit_deny = [ # Easier than denying whole category
    # Deadlock detection - receive before send etc.
    "CallOrdering-mpi_send-mpi_recv-001",
    "CallOrdering-ANY_TAG-001",
    "CallOrdering-ANY_SOURCE-001",
    # Missing corresponding call
    "MissingCall-mpi_send-001",
    # Collective not called by all ranks
    "EpochLifeCycle-003",
]
broken = [
    # Uses barrier instead of fence. Leads to missing epoch and inflight call at free errors
    "040-MPI-conflict-put-put-same-array-remote-no",
]

# Check test capability
def check_test_useful(test_name: str):
    if test_name in explicit_deny: return False
    if test_name in explicit_allow: return True

    # Check mode
    if mode == "shmem":
        if "shmem" not in test_name: return False
    else:
        if "shmem" in test_name: return False

    if "hybrid" in test_name: return False # None of the tools support it

    # CoVer tool capability
    if "remote" in test_name or "GlobalConcurrency" in test_name: return False # Remote Data Races
    if "InvalidParam" in test_name: return False # Wrong parameter (NULL checks etc.)
    if "Type" in test_name or "Dtype" in test_name: return False # Data type mismatches
    if "GlobalParameterMissmatch" in test_name: return False # Mismatching op, comm
    if "CallOrdering" in test_name: return False # Deadlock tests

    return True

# Create a dict for each tool
parsed_data = {}
possible_classifications = ["TN", "TP", "NC-TP", "FN", "FP", "NC-FP", "TO"]
for analysis_tool in raw_data["Tool"].unique():
    parsed_data[analysis_tool] = {}
    parsed_data[analysis_tool]["excluded"] = {}
    for classification in possible_classifications:
        parsed_data[analysis_tool][classification] = {}
        parsed_data[analysis_tool][classification]["num"] = 0
        parsed_data[analysis_tool][classification]["tests"] = []
        parsed_data[analysis_tool]["excluded"][classification] = {}
        parsed_data[analysis_tool]["excluded"][classification]["num"] = 0
        parsed_data[analysis_tool]["excluded"][classification]["tests"] = []

def add_test_classification(analysis_tool: str, test_name: str, classification: str):
    if check_test_useful(test_name):
        parsed_data[analysis_tool][classification]["num"] += 1
        parsed_data[analysis_tool][classification]["tests"].append(test_name)
    else:
        parsed_data[analysis_tool]["excluded"][classification]["num"] += 1
        parsed_data[analysis_tool]["excluded"][classification]["tests"].append(test_name)

# Parse the data
for idx, row in raw_data.iterrows():
    analysis_tool = row["Tool"]
    test_kind = row["Expect"]
    tool_result = row["Result"]
    test_name = row["Test Case"]

    if test_name in broken: continue

    if tool_result == "TIMEOUT": add_test_classification(analysis_tool, test_name, "TO")

    if tool_result == "OK" and test_kind == "OK": add_test_classification(analysis_tool, test_name, "TN"); continue
    elif tool_result == "OK": add_test_classification(analysis_tool, test_name, "FN"); continue

    if tool_result.startswith("REPORT") and test_kind == "ERR": add_test_classification(analysis_tool, test_name, "TP" if tool_result == "REPORT" else "NC-TP"); continue
    elif tool_result.startswith("REPORT"): add_test_classification(analysis_tool, test_name, "FP" if tool_result == "REPORT" else "NC-FP"); continue

# Compute Metrics
for analysis_tool, tool_data in parsed_data.items():
    pos = tool_data["TN"]["num"] + tool_data["TP"]["num"] + tool_data["NC-TP"]["num"]
    neg = tool_data["FN"]["num"] + tool_data["FP"]["num"] + tool_data["NC-FP"]["num"]
    parsed_data[analysis_tool]["acc"] = pos / (pos + neg)
    

# Pretty print summary
column_size = 15
print("### Result Summary ###")
print(f"Tool".rjust(20), end="")
print(f"TP (+NC-TP)".rjust(column_size), end="")
print(f"FN".rjust(column_size), end="")
print(f"FP (+NC-FP)".rjust(column_size), end="")
print(f"TO".rjust(column_size), end="")
print(f"Acc".rjust(column_size))
for analysis_tool, tool_data in parsed_data.items():
    print(analysis_tool.rjust(20), end="")
    print(f"{tool_data["TN"]["num"]}".rjust(column_size), end="")
    print(f"{tool_data["TP"]["num"]} (+{tool_data["NC-TP"]["num"]})".rjust(column_size), end="")
    print(f"{tool_data["FN"]["num"]}".rjust(column_size), end="")
    print(f"{tool_data["FP"]["num"]} (+{tool_data["NC-FP"]["num"]})".rjust(column_size), end="")
    print(f"{tool_data["TO"]["num"]}".rjust(column_size), end="")
    print("{:.2f}".format(tool_data["acc"]).rjust(column_size))
print(f"Sanity check: Were all tests run using all tools?")
sum_tests = {}
prev = -1
for analysis_tool, tool_data in parsed_data.items():
    sum_tests[analysis_tool] = 0
    for classification in possible_classifications:
        sum_tests[analysis_tool] += tool_data[classification]["num"]
    if prev == -1: prev = sum_tests[analysis_tool]
    if prev != sum_tests[analysis_tool]:
        print(f"SANITY CHECK ERROR: NUMBER OF TESTS DIFFER BETWEEN TOOLS! {analysis_tool} was run on {sum_tests[analysis_tool]} tests, but previous on {prev}")
        exit(1)
print(f"Sanity check complete, no errors.")

exit(0)

# Output for LaTex
for analysis_tool, tool_data in parsed_data.items():
    print(analysis_tool, end=" & ")
    print(f"{tool_data["TP"]["num"]}", end=" & ")
    print(f"{tool_data["TN"]["num"]}", end=" & ")
    print(f"{tool_data["FP"]["num"]}", end=" & ")
    print(f"{tool_data["FN"]["num"]}", end=" & ")
    print(f"{tool_data["NC-TP"]["num"]}", end=" & ")
    print(f"{tool_data["NC-FP"]["num"]}", end=" & ")
    print(f"{tool_data["TO"]["num"]}", end=" & ")
    print("{:.2f}".format(tool_data["acc"]), end=" \\\\\n")