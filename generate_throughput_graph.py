#!/usr/bin/env python3
import csv
import matplotlib.pyplot as plt


class Flow:
    def __init__(self, flow_id):
        self.x = []
        self.y = []
        self.flow_id = flow_id

flows = {}
with open('./results/throughput_vs_time.csv') as csv_file:
    reader = csv.DictReader(csv_file)
    for row in reader:
        flow_id = row['Flow']
        x_time = row['Time (s)']
        y_throughput = row['Throughput (Mbps)']

        if flow_id not in flows:
            flows[flow_id] = Flow(flow_id)

        flows[flow_id].x.append(float(x_time))
        flows[flow_id].y.append(float(y_throughput))

for flow_id in flows:
    flow = flows[flow_id]
    plt.plot(flow.x, flow.y, label = flow.flow_id)

plt.legend()
plt.show()