# ==============================================================
# Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
# Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
# ==============================================================
#
# Settings for Vivado implementation flow
#
set top_module main
set language vhdl
set family zynquplus
set device xczu3eg
set package -sbva484
set speed -1-i
set clock ap_clk
set fsm_ext "off"

# For customizing the implementation flow
set add_io_buffers false ;# true|false

set src_files [list \
    lzw_fpga_mux_1664_8_1_1.vhd \
    lzw_fpga_mux_1664_32_1_1.vhd \
    lzw_fpga_encoding_table_str_0.vhd \
    lzw_fpga_encoding_table_code_0.vhd \
    lzw_fpga_encoding_p.vhd \
    lzw_fpga_mux_1632_8_1_1.vhd \
    lzw_fpga_control_s_axi.vhd \
    lzw_fpga_gmem_m_axi.vhd \
    lzw_fpga_encoding.vhd \
    lzw_fpga_decoding.vhd \
    lzw_fpga_main.vhd \
]

set pcore_syn 1
set pcore_lang vhdl
