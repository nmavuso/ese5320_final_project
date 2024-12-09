# This script segment is generated automatically by AutoPilot

set axilite_register_dict [dict create]
set port_control {
input_r { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 16
	offset_end 27
}
input_size { 
	dir I
	width 32
	depth 1
	mode ap_none
	offset 28
	offset_end 35
}
output_code { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 36
	offset_end 47
}
output_size { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 48
	offset_end 59
}
output_r { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 60
	offset_end 71
}
output_length { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 72
	offset_end 83
}
ap_start { }
ap_done { }
ap_ready { }
ap_continue { }
ap_idle { }
event_start { }
}
dict set axilite_register_dict control $port_control


