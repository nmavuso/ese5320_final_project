// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xmain.h"

extern XMain_Config XMain_ConfigTable[];

XMain_Config *XMain_LookupConfig(u16 DeviceId) {
	XMain_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XMAIN_NUM_INSTANCES; Index++) {
		if (XMain_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XMain_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XMain_Initialize(XMain *InstancePtr, u16 DeviceId) {
	XMain_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XMain_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XMain_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

