# This file was automatically generated by Vpl
if { ![info exists _is_init_cmds] } {
  set vivado_error_file vivado_error.txt
  set vivado_warn_file vivado_warning.txt

  source ../../../.local/ocl_util.tcl
  source ../../../.local/platform.tcl
  namespace import ocl_util::*

  set _is_init_cmds true
}

source ../../../.local/debug_profile_hooks.tcl


# generate cookie file for messaging
write_cookie_file_impl "lzw_hls"

# utilization reports
report_utilization_impl true "lzw_fpga" "routed" "" "../../../input" "../../../output"

# kernel service update
update_kernel_info ../../../../../link.steps.log /mnt/castor/seas_home/n/nmavuso/ese5320_final_project/_x/link/int

# update noc node information
update_profile_metadata_postroute /mnt/castor/seas_home/n/nmavuso/ese5320_final_project/_x/link/int
