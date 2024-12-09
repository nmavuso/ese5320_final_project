set moduleName lzw_fpga_entry11
set isTopModule 0
set isCombinational 0
set isDatapathOnly 0
set isPipelined 0
set pipeline_type none
set FunctionProtocol ap_ctrl_hs
set isOneStateSeq 1
set ProfileFlag 1
set StallSigGenFlag 1
set isEnableWaveformDebug 1
set C_modelName {lzw_fpga.entry11}
set C_modelType { void 0 }
set C_modelArgList {
	{ input_r int 64 regular  }
	{ input_size int 32 regular  }
	{ output_code int 64 regular  }
	{ output_size int 64 regular  }
	{ output_r int 64 regular  }
	{ output_length int 64 regular  }
	{ input_out int 64 regular {fifo 1}  }
	{ input_size_out int 32 regular {fifo 1}  }
	{ input_size_out1 int 32 regular {fifo 1}  }
	{ output_code_out int 64 regular {fifo 1}  }
	{ output_size_out int 64 regular {fifo 1}  }
	{ output_out int 64 regular {fifo 1}  }
	{ output_length_out int 64 regular {fifo 1}  }
}
set C_modelArgMapList {[ 
	{ "Name" : "input_r", "interface" : "wire", "bitwidth" : 64, "direction" : "READONLY"} , 
 	{ "Name" : "input_size", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY"} , 
 	{ "Name" : "output_code", "interface" : "wire", "bitwidth" : 64, "direction" : "READONLY"} , 
 	{ "Name" : "output_size", "interface" : "wire", "bitwidth" : 64, "direction" : "READONLY"} , 
 	{ "Name" : "output_r", "interface" : "wire", "bitwidth" : 64, "direction" : "READONLY"} , 
 	{ "Name" : "output_length", "interface" : "wire", "bitwidth" : 64, "direction" : "READONLY"} , 
 	{ "Name" : "input_out", "interface" : "fifo", "bitwidth" : 64, "direction" : "WRITEONLY"} , 
 	{ "Name" : "input_size_out", "interface" : "fifo", "bitwidth" : 32, "direction" : "WRITEONLY"} , 
 	{ "Name" : "input_size_out1", "interface" : "fifo", "bitwidth" : 32, "direction" : "WRITEONLY"} , 
 	{ "Name" : "output_code_out", "interface" : "fifo", "bitwidth" : 64, "direction" : "WRITEONLY"} , 
 	{ "Name" : "output_size_out", "interface" : "fifo", "bitwidth" : 64, "direction" : "WRITEONLY"} , 
 	{ "Name" : "output_out", "interface" : "fifo", "bitwidth" : 64, "direction" : "WRITEONLY"} , 
 	{ "Name" : "output_length_out", "interface" : "fifo", "bitwidth" : 64, "direction" : "WRITEONLY"} ]}
# RTL Port declarations: 
set portNum 40
set portList { 
	{ ap_clk sc_in sc_logic 1 clock -1 } 
	{ ap_rst sc_in sc_logic 1 reset -1 active_high_sync } 
	{ ap_start sc_in sc_logic 1 start -1 } 
	{ start_full_n sc_in sc_logic 1 signal -1 } 
	{ ap_done sc_out sc_logic 1 predone -1 } 
	{ ap_continue sc_in sc_logic 1 continue -1 } 
	{ ap_idle sc_out sc_logic 1 done -1 } 
	{ ap_ready sc_out sc_logic 1 ready -1 } 
	{ start_out sc_out sc_logic 1 signal -1 } 
	{ start_write sc_out sc_logic 1 signal -1 } 
	{ input_r sc_in sc_lv 64 signal 0 } 
	{ input_size sc_in sc_lv 32 signal 1 } 
	{ output_code sc_in sc_lv 64 signal 2 } 
	{ output_size sc_in sc_lv 64 signal 3 } 
	{ output_r sc_in sc_lv 64 signal 4 } 
	{ output_length sc_in sc_lv 64 signal 5 } 
	{ input_out_din sc_out sc_lv 64 signal 6 } 
	{ input_out_full_n sc_in sc_logic 1 signal 6 } 
	{ input_out_write sc_out sc_logic 1 signal 6 } 
	{ input_size_out_din sc_out sc_lv 32 signal 7 } 
	{ input_size_out_full_n sc_in sc_logic 1 signal 7 } 
	{ input_size_out_write sc_out sc_logic 1 signal 7 } 
	{ input_size_out1_din sc_out sc_lv 32 signal 8 } 
	{ input_size_out1_full_n sc_in sc_logic 1 signal 8 } 
	{ input_size_out1_write sc_out sc_logic 1 signal 8 } 
	{ output_code_out_din sc_out sc_lv 64 signal 9 } 
	{ output_code_out_full_n sc_in sc_logic 1 signal 9 } 
	{ output_code_out_write sc_out sc_logic 1 signal 9 } 
	{ output_size_out_din sc_out sc_lv 64 signal 10 } 
	{ output_size_out_full_n sc_in sc_logic 1 signal 10 } 
	{ output_size_out_write sc_out sc_logic 1 signal 10 } 
	{ output_out_din sc_out sc_lv 64 signal 11 } 
	{ output_out_full_n sc_in sc_logic 1 signal 11 } 
	{ output_out_write sc_out sc_logic 1 signal 11 } 
	{ output_length_out_din sc_out sc_lv 64 signal 12 } 
	{ output_length_out_full_n sc_in sc_logic 1 signal 12 } 
	{ output_length_out_write sc_out sc_logic 1 signal 12 } 
	{ ap_ext_blocking_n sc_out sc_logic 1 signal -1 } 
	{ ap_str_blocking_n sc_out sc_logic 1 signal -1 } 
	{ ap_int_blocking_n sc_out sc_logic 1 signal -1 } 
}
set NewPortList {[ 
	{ "name": "ap_clk", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "clock", "bundle":{"name": "ap_clk", "role": "default" }} , 
 	{ "name": "ap_rst", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "reset", "bundle":{"name": "ap_rst", "role": "default" }} , 
 	{ "name": "ap_start", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "start", "bundle":{"name": "ap_start", "role": "default" }} , 
 	{ "name": "start_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "start_full_n", "role": "default" }} , 
 	{ "name": "ap_done", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "predone", "bundle":{"name": "ap_done", "role": "default" }} , 
 	{ "name": "ap_continue", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "continue", "bundle":{"name": "ap_continue", "role": "default" }} , 
 	{ "name": "ap_idle", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "done", "bundle":{"name": "ap_idle", "role": "default" }} , 
 	{ "name": "ap_ready", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "ready", "bundle":{"name": "ap_ready", "role": "default" }} , 
 	{ "name": "start_out", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "start_out", "role": "default" }} , 
 	{ "name": "start_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "start_write", "role": "default" }} , 
 	{ "name": "input_r", "direction": "in", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "input_r", "role": "default" }} , 
 	{ "name": "input_size", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "input_size", "role": "default" }} , 
 	{ "name": "output_code", "direction": "in", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_code", "role": "default" }} , 
 	{ "name": "output_size", "direction": "in", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_size", "role": "default" }} , 
 	{ "name": "output_r", "direction": "in", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_r", "role": "default" }} , 
 	{ "name": "output_length", "direction": "in", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_length", "role": "default" }} , 
 	{ "name": "input_out_din", "direction": "out", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "input_out", "role": "din" }} , 
 	{ "name": "input_out_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_out", "role": "full_n" }} , 
 	{ "name": "input_out_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_out", "role": "write" }} , 
 	{ "name": "input_size_out_din", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "input_size_out", "role": "din" }} , 
 	{ "name": "input_size_out_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_size_out", "role": "full_n" }} , 
 	{ "name": "input_size_out_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_size_out", "role": "write" }} , 
 	{ "name": "input_size_out1_din", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "input_size_out1", "role": "din" }} , 
 	{ "name": "input_size_out1_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_size_out1", "role": "full_n" }} , 
 	{ "name": "input_size_out1_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_size_out1", "role": "write" }} , 
 	{ "name": "output_code_out_din", "direction": "out", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_code_out", "role": "din" }} , 
 	{ "name": "output_code_out_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_code_out", "role": "full_n" }} , 
 	{ "name": "output_code_out_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_code_out", "role": "write" }} , 
 	{ "name": "output_size_out_din", "direction": "out", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_size_out", "role": "din" }} , 
 	{ "name": "output_size_out_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_size_out", "role": "full_n" }} , 
 	{ "name": "output_size_out_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_size_out", "role": "write" }} , 
 	{ "name": "output_out_din", "direction": "out", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_out", "role": "din" }} , 
 	{ "name": "output_out_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_out", "role": "full_n" }} , 
 	{ "name": "output_out_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_out", "role": "write" }} , 
 	{ "name": "output_length_out_din", "direction": "out", "datatype": "sc_lv", "bitwidth":64, "type": "signal", "bundle":{"name": "output_length_out", "role": "din" }} , 
 	{ "name": "output_length_out_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_length_out", "role": "full_n" }} , 
 	{ "name": "output_length_out_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_length_out", "role": "write" }} , 
 	{ "name": "ap_ext_blocking_n", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "ap_ext_blocking_n", "role": "default" }} , 
 	{ "name": "ap_str_blocking_n", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "ap_str_blocking_n", "role": "default" }} , 
 	{ "name": "ap_int_blocking_n", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "ap_int_blocking_n", "role": "default" }}  ]}

set RtlHierarchyInfo {[
	{"ID" : "0", "Level" : "0", "Path" : "`AUTOTB_DUT_INST", "Parent" : "",
		"CDFG" : "lzw_fpga_entry11",
		"Protocol" : "ap_ctrl_hs",
		"ControlExist" : "1", "ap_start" : "1", "ap_ready" : "1", "ap_done" : "1", "ap_continue" : "1", "ap_idle" : "1", "real_start" : "1",
		"Pipeline" : "None", "UnalignedPipeline" : "0", "RewindPipeline" : "0", "ProcessNetwork" : "0",
		"II" : "1",
		"VariableLatency" : "0", "ExactLatency" : "0", "EstimateLatencyMin" : "0", "EstimateLatencyMax" : "0",
		"Combinational" : "0",
		"Datapath" : "0",
		"ClockEnable" : "0",
		"HasSubDataflow" : "0",
		"InDataflowNetwork" : "1",
		"HasNonBlockingOperation" : "0",
		"Port" : [
			{"Name" : "input_r", "Type" : "None", "Direction" : "I"},
			{"Name" : "input_size", "Type" : "None", "Direction" : "I"},
			{"Name" : "output_code", "Type" : "None", "Direction" : "I"},
			{"Name" : "output_size", "Type" : "None", "Direction" : "I"},
			{"Name" : "output_r", "Type" : "None", "Direction" : "I"},
			{"Name" : "output_length", "Type" : "None", "Direction" : "I"},
			{"Name" : "input_out", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "2", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "input_out_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "input_size_out", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "2", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "input_size_out_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "input_size_out1", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "3", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "input_size_out1_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "output_code_out", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "4", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "output_code_out_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "output_size_out", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "7", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "output_size_out_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "output_out", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "7", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "output_out_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "output_length_out", "Type" : "Fifo", "Direction" : "O", "DependentProc" : "0", "DependentChan" : "0", "DependentChanDepth" : "7", "DependentChanType" : "2",
				"BlockSignal" : [
					{"Name" : "output_length_out_blk_n", "Type" : "RtlSignal"}]}]}]}


set ArgLastReadFirstWriteLatency {
	lzw_fpga_entry11 {
		input_r {Type I LastRead 0 FirstWrite -1}
		input_size {Type I LastRead 0 FirstWrite -1}
		output_code {Type I LastRead 0 FirstWrite -1}
		output_size {Type I LastRead 0 FirstWrite -1}
		output_r {Type I LastRead 0 FirstWrite -1}
		output_length {Type I LastRead 0 FirstWrite -1}
		input_out {Type O LastRead -1 FirstWrite 0}
		input_size_out {Type O LastRead -1 FirstWrite 0}
		input_size_out1 {Type O LastRead -1 FirstWrite 0}
		output_code_out {Type O LastRead -1 FirstWrite 0}
		output_size_out {Type O LastRead -1 FirstWrite 0}
		output_out {Type O LastRead -1 FirstWrite 0}
		output_length_out {Type O LastRead -1 FirstWrite 0}}}

set hasDtUnsupportedChannel 0

set PerformanceInfo {[
	{"Name" : "Latency", "Min" : "0", "Max" : "0"}
	, {"Name" : "Interval", "Min" : "0", "Max" : "0"}
]}

set PipelineEnableSignalInfo {[
]}

set Spec2ImplPortList { 
	input_r { ap_none {  { input_r in_data 0 64 } } }
	input_size { ap_none {  { input_size in_data 0 32 } } }
	output_code { ap_none {  { output_code in_data 0 64 } } }
	output_size { ap_none {  { output_size in_data 0 64 } } }
	output_r { ap_none {  { output_r in_data 0 64 } } }
	output_length { ap_none {  { output_length in_data 0 64 } } }
	input_out { ap_fifo {  { input_out_din fifo_data 1 64 }  { input_out_full_n fifo_status 0 1 }  { input_out_write fifo_update 1 1 } } }
	input_size_out { ap_fifo {  { input_size_out_din fifo_data 1 32 }  { input_size_out_full_n fifo_status 0 1 }  { input_size_out_write fifo_update 1 1 } } }
	input_size_out1 { ap_fifo {  { input_size_out1_din fifo_data 1 32 }  { input_size_out1_full_n fifo_status 0 1 }  { input_size_out1_write fifo_update 1 1 } } }
	output_code_out { ap_fifo {  { output_code_out_din fifo_data 1 64 }  { output_code_out_full_n fifo_status 0 1 }  { output_code_out_write fifo_update 1 1 } } }
	output_size_out { ap_fifo {  { output_size_out_din fifo_data 1 64 }  { output_size_out_full_n fifo_status 0 1 }  { output_size_out_write fifo_update 1 1 } } }
	output_out { ap_fifo {  { output_out_din fifo_data 1 64 }  { output_out_full_n fifo_status 0 1 }  { output_out_write fifo_update 1 1 } } }
	output_length_out { ap_fifo {  { output_length_out_din fifo_data 1 64 }  { output_length_out_full_n fifo_status 0 1 }  { output_length_out_write fifo_update 1 1 } } }
}
