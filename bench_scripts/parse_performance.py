import pandas as pd
import os
import sys

if len(sys.argv) < 2:
    print(f"Insufficient arguments!\nUsage: {os.path.basename(__file__)} <JUBE output csv>")
    exit(1)

# Read in the CSV
raw_data = pd.read_csv(sys.argv[1])

proxy_apps = raw_data["Proxy App"].unique()
tool_configurations = raw_data["Tool"].unique()
for proxy_app in proxy_apps:
    tmpdata = {}
    proxy_data = raw_data[raw_data["Proxy App"] == proxy_app]
    new_df = pd.DataFrame(columns=tool_configurations)
    new_df.index.name = "Processes"

    for num_tasks in proxy_data["Processes"].unique():
        num_tasks = num_tasks.item()
        row_df = proxy_data[proxy_data["Processes"] == num_tasks]

        for tool in tool_configurations:
            cur_df = row_df[row_df["Tool"] == tool]
            if num_tasks not in tmpdata: tmpdata[num_tasks] = {}
            times = cur_df["Time (Avg)"]
            tmpdata[num_tasks][tool] = times.item() if len(times) == 1 else pd.NA

        new_df.loc[num_tasks] = [tmpdata[num_tasks][tool] for tool in tool_configurations]

    new_df.sort_index(inplace=True)
    new_df.to_csv(f"{proxy_app}_runtimes.csv", sep=";")
    print(f"### Results for {proxy_app} ###")
    print(new_df, end="\n\n")
