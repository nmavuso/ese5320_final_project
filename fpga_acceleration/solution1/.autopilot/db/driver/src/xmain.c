// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xmain.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XMain_CfgInitialize(XMain *InstancePtr, XMain_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Lzw_fpga_control_BaseAddress = ConfigPtr->Lzw_fpga_control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XMain_Start(XMain *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL) & 0x80;
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XMain_IsDone(XMain *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XMain_IsIdle(XMain *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XMain_IsReady(XMain *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XMain_Continue(XMain *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL) & 0x80;
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL, Data | 0x10);
}

void XMain_EnableAutoRestart(XMain *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XMain_DisableAutoRestart(XMain *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_CTRL, 0);
}

u32 XMain_Get_return(XMain *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_AP_RETURN);
    return Data;
}
void XMain_Set_s(XMain *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_S_DATA, (u32)(Data));
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_S_DATA + 4, (u32)(Data >> 32));
}

u64 XMain_Get_s(XMain *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_S_DATA);
    Data += (u64)XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_S_DATA + 4) << 32;
    return Data;
}

void XMain_Set_output_code(XMain *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_CODE_DATA, (u32)(Data));
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_CODE_DATA + 4, (u32)(Data >> 32));
}

u64 XMain_Get_output_code(XMain *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_CODE_DATA);
    Data += (u64)XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_CODE_DATA + 4) << 32;
    return Data;
}

void XMain_Set_output_size(XMain *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_SIZE_DATA, (u32)(Data));
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_SIZE_DATA + 4, (u32)(Data >> 32));
}

u64 XMain_Get_output_size(XMain *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_SIZE_DATA);
    Data += (u64)XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_SIZE_DATA + 4) << 32;
    return Data;
}

void XMain_Set_output_r(XMain *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_R_DATA, (u32)(Data));
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XMain_Get_output_r(XMain *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_R_DATA);
    Data += (u64)XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_OUTPUT_R_DATA + 4) << 32;
    return Data;
}

void XMain_InterruptGlobalEnable(XMain *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_GIE, 1);
}

void XMain_InterruptGlobalDisable(XMain *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_GIE, 0);
}

void XMain_InterruptEnable(XMain *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_IER);
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_IER, Register | Mask);
}

void XMain_InterruptDisable(XMain *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_IER);
    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_IER, Register & (~Mask));
}

void XMain_InterruptClear(XMain *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XMain_WriteReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_ISR, Mask);
}

u32 XMain_InterruptGetEnabled(XMain *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_IER);
}

u32 XMain_InterruptGetStatus(XMain *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XMain_ReadReg(InstancePtr->Lzw_fpga_control_BaseAddress, XMAIN_LZW_FPGA_CONTROL_ADDR_ISR);
}

