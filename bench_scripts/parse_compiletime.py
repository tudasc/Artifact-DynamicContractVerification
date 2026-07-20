#!/usr/bin/env python3
"""Split results.csv into per-proxy-app compile-time CSVs.

Reads results.csv and writes one <app>_compiletime.csv per proxy app,
e.g. lulesh_compiletime.csv, stencil_compiletime.csv.
"""

import csv
import sys
from collections import OrderedDict

INPUT = sys.argv[1] if len(sys.argv) > 1 else "results.csv"

OUT_HEADER = [
    "app",
    "tool",
    "base_compilation",
    "contract_parsing",
    "precall",
    "postcall",
    "release",
    "instrumentation",
    "interaction_effects",
    "must_total",
]


def num(value):
    """Parse a cell into a float, treating blanks as 0."""
    value = (value or "").strip()
    return float(value) if value else 0.0


def main():
    # Read rows grouped by proxy app, preserving input order.
    apps = OrderedDict()
    with open(INPUT, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            app = (row.get("Proxy App") or "").strip()
            if not app:
                continue  # skip trailing blank lines
            apps.setdefault(app, []).append(row)

    for app, rows in apps.items():
        # base_compilation is the "base" tool's total time for this app.
        base_compilation = 0.0
        for row in rows:
            if (row.get("Tool") or "").strip() == "base":
                base_compilation = num(row["Total Time"])
                break

        out_rows = []
        for row in rows:
            tool = (row.get("Tool") or "").strip()
            ssa_arg = (row.get("Static Analysis Arg") or "").strip()
            # cover with static analysis skipped is reported as "covssa".
            if tool == "cover" and ssa_arg == "--skip-static-analysis":
                tool = "covssa"

            contract = num(row.get("Contract Parsing"))
            precall = num(row.get("PreCall Time"))
            postcall = num(row.get("PostCall Time"))
            release = num(row.get("Release Time"))
            instrumentation = num(row.get("Instrumentation Time"))
            total = num(row.get("Total Time"))

            interaction_effects = total - base_compilation - (
                contract + precall + postcall + release + instrumentation
            )

            out_rows.append([
                app,
                tool,
                base_compilation,
                contract,
                precall,
                postcall,
                release,
                instrumentation,
                interaction_effects,
                0,
            ])

        out_path = f"{app}_compiletime.csv"
        with open(out_path, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(OUT_HEADER)
            writer.writerows(out_rows)
        print(f"wrote {out_path} ({len(out_rows)} rows)")


if __name__ == "__main__":
    main()
