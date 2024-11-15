// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// lzw_fpga_control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 4  - ap_continue (Read/Write/SC)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - enable ap_done interrupt (Read/Write)
//        bit 1  - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - ap_done (COR/TOW)
//        bit 1  - ap_ready (COR/TOW)
//        others - reserved
// 0x10 : Data signal of ap_return
//        bit 31~0 - ap_return[31:0] (Read)
// 0x18 : Data signal of s
//        bit 31~0 - s[31:0] (Read/Write)
// 0x1c : Data signal of s
//        bit 31~0 - s[63:32] (Read/Write)
// 0x20 : reserved
// 0x24 : Data signal of output_code
//        bit 31~0 - output_code[31:0] (Read/Write)
// 0x28 : Data signal of output_code
//        bit 31~0 - output_code[63:32] (Read/Write)
// 0x2c : reserved
// 0x30 : Data signal of output_size
//        bit 31~0 - output_size[31:0] (Read/Write)
// 0x34 : Data signal of output_size
//        bit 31~0 - output_size[63:32] (Read/Write)
// 0x38 : reserved
// 0x3c : Data signal of output_r
//        bit 31~0 - output_r[31:0] (Read/Write)
// 0x40 : Data signal of output_r
//        bit 31~0 - output_r[63:32] (Read/Write)
// 0x44 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL          0x00
#define XMAIN_LZW_FPGA_CONTROL_ADDR_GIE              0x04
#define XMAIN_LZW_FPGA_CONTROL_ADDR_IER              0x08
#define XMAIN_LZW_FPGA_CONTROL_ADDR_ISR              0x0c
#define XMAIN_LZW_FPGA_CONTROL_ADDR_AP_RETURN        0x10
#define XMAIN_LZW_FPGA_CONTROL_BITS_AP_RETURN        32
#define XMAIN_LZW_FPGA_CONTROL_ADDR_S_DATA           0x18
#define XMAIN_LZW_FPGA_CONTROL_BITS_S_DATA           64
#define XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_CODE_DATA 0x24
#define XMAIN_LZW_FPGA_CONTROL_BITS_OUTPUT_CODE_DATA 64
#define XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_SIZE_DATA 0x30
#define XMAIN_LZW_FPGA_CONTROL_BITS_OUTPUT_SIZE_DATA 64
#define XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_R_DATA    0x3c
#define XMAIN_LZW_FPGA_CONTROL_BITS_OUTPUT_R_DATA    64

