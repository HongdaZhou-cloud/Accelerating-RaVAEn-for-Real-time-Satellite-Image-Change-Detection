############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
############################################################
open_project encoder_m_axi
set_top encoder_m_axi
add_files norm.cpp
add_files -tb output.dat 
add_files -tb out.gold.dat 
add_files -tb norm_test.cpp 

open_solution "solution1" 
set_part {xczu5ev-fbvb900-1-e}
create_clock -period 215MHz -name default
set_clock_uncertainty 12.5%
source "./directives.tcl"
exit

