# ==============================================================
# Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
# Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
# ==============================================================
proc generate {drv_handle} {
    xdefine_include_file $drv_handle "xparameters.h" "XMain" \
        "NUM_INSTANCES" \
        "DEVICE_ID" \
        "C_S_AXI_LZW_FPGA_CONTROL_BASEADDR" \
        "C_S_AXI_LZW_FPGA_CONTROL_HIGHADDR"

    xdefine_config_file $drv_handle "xmain_g.c" "XMain" \
        "DEVICE_ID" \
        "C_S_AXI_LZW_FPGA_CONTROL_BASEADDR"

    xdefine_canonical_xpars $drv_handle "xparameters.h" "XMain" \
        "DEVICE_ID" \
        "C_S_AXI_LZW_FPGA_CONTROL_BASEADDR" \
        "C_S_AXI_LZW_FPGA_CONTROL_HIGHADDR"
}

