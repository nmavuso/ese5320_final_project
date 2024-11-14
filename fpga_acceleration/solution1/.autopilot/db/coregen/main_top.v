// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1 ns / 1 ps
module main_top (
aresetn,
aclk
);

parameter RESET_ACTIVE_LOW = 1;
input aresetn ;

input aclk ;

wire aresetn;


wire sig_main_ap_rst_n;



main main_U(
    .ap_rst_n(sig_main_ap_rst_n),
    .ap_clk(aclk)
);

main_ap_rst_n_if #(
    .RESET_ACTIVE_LOW(RESET_ACTIVE_LOW))
ap_rst_n_if_U(
    .dout(sig_main_ap_rst_n),
    .din(aresetn));

endmodule
