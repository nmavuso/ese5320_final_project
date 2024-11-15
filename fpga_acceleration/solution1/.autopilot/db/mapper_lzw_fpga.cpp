#include <systemc>
#include <vector>
#include <iostream>
#include "hls_stream.h"
#include "ap_int.h"
#include "ap_fixed.h"
using namespace std;
using namespace sc_dt;
class AESL_RUNTIME_BC {
  public:
    AESL_RUNTIME_BC(const char* name) {
      file_token.open( name);
      if (!file_token.good()) {
        cout << "Failed to open tv file " << name << endl;
        exit (1);
      }
      file_token >> mName;//[[[runtime]]]
    }
    ~AESL_RUNTIME_BC() {
      file_token.close();
    }
    int read_size () {
      int size = 0;
      file_token >> mName;//[[transaction]]
      file_token >> mName;//transaction number
      file_token >> mName;//pop_size
      size = atoi(mName.c_str());
      file_token >> mName;//[[/transaction]]
      return size;
    }
  public:
    fstream file_token;
    string mName;
};
extern "C" int lzw_fpga(int*, int, int, int, int);
extern "C" int apatb_lzw_fpga_hw(volatile void * __xlx_apatb_param_s, volatile void * __xlx_apatb_param_output_code, volatile void * __xlx_apatb_param_output_size, volatile void * __xlx_apatb_param_output) {
  // Collect __xlx_s_output_code_output_size_output__tmp_vec
  vector<sc_bv<32> >__xlx_s_output_code_output_size_output__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s_output_code_output_size_output__tmp_vec.push_back(((int*)__xlx_apatb_param_s)[j]);
  }
  int __xlx_size_param_s = 1;
  int __xlx_offset_param_s = 0;
  int __xlx_offset_byte_param_s = 0*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s_output_code_output_size_output__tmp_vec.push_back(((int*)__xlx_apatb_param_output_code)[j]);
  }
  int __xlx_size_param_output_code = 1;
  int __xlx_offset_param_output_code = 1;
  int __xlx_offset_byte_param_output_code = 1*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s_output_code_output_size_output__tmp_vec.push_back(((int*)__xlx_apatb_param_output_size)[j]);
  }
  int __xlx_size_param_output_size = 1;
  int __xlx_offset_param_output_size = 2;
  int __xlx_offset_byte_param_output_size = 2*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_s_output_code_output_size_output__tmp_vec.push_back(((int*)__xlx_apatb_param_output)[j]);
  }
  int __xlx_size_param_output = 1;
  int __xlx_offset_param_output = 3;
  int __xlx_offset_byte_param_output = 3*4;
  int* __xlx_s_output_code_output_size_output__input_buffer= new int[__xlx_s_output_code_output_size_output__tmp_vec.size()];
  for (int i = 0; i < __xlx_s_output_code_output_size_output__tmp_vec.size(); ++i) {
    __xlx_s_output_code_output_size_output__input_buffer[i] = __xlx_s_output_code_output_size_output__tmp_vec[i].range(31, 0).to_uint64();
  }
  // DUT call
  int ap_return = lzw_fpga(__xlx_s_output_code_output_size_output__input_buffer, __xlx_offset_byte_param_s, __xlx_offset_byte_param_output_code, __xlx_offset_byte_param_output_size, __xlx_offset_byte_param_output);
// print __xlx_apatb_param_s
  sc_bv<32>*__xlx_s_output_buffer = new sc_bv<32>[__xlx_size_param_s];
  for (int i = 0; i < __xlx_size_param_s; ++i) {
    __xlx_s_output_buffer[i] = __xlx_s_output_code_output_size_output__input_buffer[i+__xlx_offset_param_s];
  }
  for (int i = 0; i < __xlx_size_param_s; ++i) {
    ((int*)__xlx_apatb_param_s)[i] = __xlx_s_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output_code
  sc_bv<32>*__xlx_output_code_output_buffer = new sc_bv<32>[__xlx_size_param_output_code];
  for (int i = 0; i < __xlx_size_param_output_code; ++i) {
    __xlx_output_code_output_buffer[i] = __xlx_s_output_code_output_size_output__input_buffer[i+__xlx_offset_param_output_code];
  }
  for (int i = 0; i < __xlx_size_param_output_code; ++i) {
    ((int*)__xlx_apatb_param_output_code)[i] = __xlx_output_code_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output_size
  sc_bv<32>*__xlx_output_size_output_buffer = new sc_bv<32>[__xlx_size_param_output_size];
  for (int i = 0; i < __xlx_size_param_output_size; ++i) {
    __xlx_output_size_output_buffer[i] = __xlx_s_output_code_output_size_output__input_buffer[i+__xlx_offset_param_output_size];
  }
  for (int i = 0; i < __xlx_size_param_output_size; ++i) {
    ((int*)__xlx_apatb_param_output_size)[i] = __xlx_output_size_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output
  sc_bv<32>*__xlx_output_output_buffer = new sc_bv<32>[__xlx_size_param_output];
  for (int i = 0; i < __xlx_size_param_output; ++i) {
    __xlx_output_output_buffer[i] = __xlx_s_output_code_output_size_output__input_buffer[i+__xlx_offset_param_output];
  }
  for (int i = 0; i < __xlx_size_param_output; ++i) {
    ((int*)__xlx_apatb_param_output)[i] = __xlx_output_output_buffer[i].to_uint64();
  }
return ap_return;
}
