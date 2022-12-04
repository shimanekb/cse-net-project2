#!/bin/bash

NS_PATH="../ns3/bake/source/ns-3.37"

SCRATCH_PATH=$NS_PATH/scratch
SCRIPT_NAME=simulation.cc

cp $SCRIPT_NAME $SCRATCH_PATH
cd $NS_PATH

./ns3 run scratch/$SCRIPT_NAME
