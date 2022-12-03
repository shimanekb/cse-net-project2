#!/bin/bash

NS_PATH=../ns3/ns-allinone-3.37/ns-3.37/
SCRATCH_PATH=$NS_PATH/scratch

cp simulation.cc $SCRATCH_PATH
cd $NS_PATH

./ns3 run scratch/simulation.cc