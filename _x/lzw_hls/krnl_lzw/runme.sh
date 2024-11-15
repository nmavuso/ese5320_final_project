#!/bin/sh

# 
# v++(TM)
# runme.sh: a v++-generated Runs Script for UNIX
# Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/mnt/pollux/software/xilinx/2020.2/Vitis_HLS/2020.2/bin:/mnt/pollux/software/xilinx/2020.2/Vitis/2020.2/bin:/mnt/pollux/software/xilinx/2020.2/Vitis/2020.2/bin
else
  PATH=/mnt/pollux/software/xilinx/2020.2/Vitis_HLS/2020.2/bin:/mnt/pollux/software/xilinx/2020.2/Vitis/2020.2/bin:/mnt/pollux/software/xilinx/2020.2/Vitis/2020.2/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=
else
  LD_LIBRARY_PATH=:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/mnt/castor/seas_home/j/joaquinr/ese5320_final_project/_x/lzw_hls/krnl_lzw'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

EAStep vitis_hls -f krnl_lzw.tcl -messageDb vitis_hls.pb
