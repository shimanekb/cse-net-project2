CSE Networks Project 2
----------------------

For code snippets, I start them with the ">" symbol. When copy pasting them,
exclude said ">" symbol in the bash terminal or text editor.

How to Install
--------------
To run this simulation you need ns3 built on your machine, I used
ns version 3.37. But, 3.29 should work as well.

How to Run (with Run.sh script)
-------------------------------
If this the first run, update within the script (use a plain text editor or ide)
the variable NS_PATH to the built ns3 install location. See example below:

> NS_PATH="../ns3/bake/source/ns-3.37"

Then run the run script in a bash command shell as below:

> ./run.sh

The animation file "test.xml" and the throughput csv file 
"throughput_vs_time.csv" will be in folder named "results"
in the same location that you ran the run.sh script.


How to Run (Manually, ie no run script)
---------------------------------------
If you are running this simulation manually, then:
1. Copy the file simulation.cc into your built ns3'
   scratch directory
2. At the build ns3 root folder run the command below:

> ./ns3 run scratch/simulation.cc

3. The animation file "test.xml" and the throughput csv file 
   "throughput_vs_time.csv" will be in the built ns3 root
    directory.


How to Generate Animation of Topology
-------------------------------------
1. Make sure net anim is installed for ns3.
2. Run the simulation (refer to How to Run)
3. Open net anim
4. Open test.xml generated from running the simulation in net anim.
   NOTE: click play in net anim and wait a second and the topology will look like
   what it should. The nodes at second 0, will later shift to their correct
   locations.

How to Generate Throughput vs Time Graph
----------------------------------------
For the graph I used matplotlib in python.

1. Make sure matplotlib is installed in python 3
2. execute the generate_throughput_graph.py script in the bash terminal, using
   the command below

> python3 generate_throughput_graph.py

3. Generated graph will be in the results directory of this project, and named
   throughput_vs_time.png