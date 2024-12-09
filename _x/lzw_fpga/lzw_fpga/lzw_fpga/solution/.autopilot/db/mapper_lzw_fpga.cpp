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
extern "C" void lzw_fpga(char*, int*, int*, char*, int, int, int, int, int, int);
extern "C" void apatb_lzw_fpga_hw(volatile void * __xlx_apatb_param_input, int __xlx_apatb_param_input_size, volatile void * __xlx_apatb_param_output_code, volatile void * __xlx_apatb_param_output_size, volatile void * __xlx_apatb_param_output, volatile void * __xlx_apatb_param_output_length) {
  // Collect __xlx_input__tmp_vec
  vector<sc_bv<8> >__xlx_input__tmp_vec;
  for (int j = 0, e = 256; j != e; ++j) {
    __xlx_input__tmp_vec.push_back(((char*)__xlx_apatb_param_input)[j]);
  }
  int __xlx_size_param_input = 256;
  int __xlx_offset_param_input = 0;
  int __xlx_offset_byte_param_input = 0*1;
  char* __xlx_input__input_buffer= new char[__xlx_input__tmp_vec.size()];
  for (int i = 0; i < __xlx_input__tmp_vec.size(); ++i) {
    __xlx_input__input_buffer[i] = __xlx_input__tmp_vec[i].range(7, 0).to_uint64();
  }
  // Collect __xlx_output_code__tmp_vec
  vector<sc_bv<32> >__xlx_output_code__tmp_vec;
  for (int j = 0, e = 256; j != e; ++j) {
    __xlx_output_code__tmp_vec.push_back(((int*)__xlx_apatb_param_output_code)[j]);
  }
  int __xlx_size_param_output_code = 256;
  int __xlx_offset_param_output_code = 0;
  int __xlx_offset_byte_param_output_code = 0*4;
  int* __xlx_output_code__input_buffer= new int[__xlx_output_code__tmp_vec.size()];
  for (int i = 0; i < __xlx_output_code__tmp_vec.size(); ++i) {
    __xlx_output_code__input_buffer[i] = __xlx_output_code__tmp_vec[i].range(31, 0).to_uint64();
  }
  // Collect __xlx_output_size_output_length__tmp_vec
  vector<sc_bv<32> >__xlx_output_size_output_length__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_output_size_output_length__tmp_vec.push_back(((int*)__xlx_apatb_param_output_size)[j]);
  }
  int __xlx_size_param_output_size = 1;
  int __xlx_offset_param_output_size = 0;
  int __xlx_offset_byte_param_output_size = 0*4;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_output_size_output_length__tmp_vec.push_back(((int*)__xlx_apatb_param_output_length)[j]);
  }
  int __xlx_size_param_output_length = 1;
  int __xlx_offset_param_output_length = 1;
  int __xlx_offset_byte_param_output_length = 1*4;
  int* __xlx_output_size_output_length__input_buffer= new int[__xlx_output_size_output_length__tmp_vec.size()];
  for (int i = 0; i < __xlx_output_size_output_length__tmp_vec.size(); ++i) {
    __xlx_output_size_output_length__input_buffer[i] = __xlx_output_size_output_length__tmp_vec[i].range(31, 0).to_uint64();
  }
  // Collect __xlx_output__tmp_vec
  vector<sc_bv<8> >__xlx_output__tmp_vec;
  for (int j = 0, e = 256; j != e; ++j) {
    __xlx_output__tmp_vec.push_back(((char*)__xlx_apatb_param_output)[j]);
  }
  int __xlx_size_param_output = 256;
  int __xlx_offset_param_output = 0;
  int __xlx_offset_byte_param_output = 0*1;
  char* __xlx_output__input_buffer= new char[__xlx_output__tmp_vec.size()];
  for (int i = 0; i < __xlx_output__tmp_vec.size(); ++i) {
    __xlx_output__input_buffer[i] = __xlx_output__tmp_vec[i].range(7, 0).to_uint64();
  }
  // DUT call
  lzw_fpga(__xlx_input__input_buffer, __xlx_output_code__input_buffer, __xlx_output_size_output_length__input_buffer, __xlx_output__input_buffer, __xlx_offset_byte_param_input, __xlx_apatb_param_input_size, __xlx_offset_byte_param_output_code, __xlx_offset_byte_param_output_size, __xlx_offset_byte_param_output, __xlx_offset_byte_param_output_length);
// print __xlx_apatb_param_input
  sc_bv<8>*__xlx_input_output_buffer = new sc_bv<8>[__xlx_size_param_input];
  for (int i = 0; i < __xlx_size_param_input; ++i) {
    __xlx_input_output_buffer[i] = __xlx_input__input_buffer[i+__xlx_offset_param_input];
  }
  for (int i = 0; i < __xlx_size_param_input; ++i) {
    ((char*)__xlx_apatb_param_input)[i] = __xlx_input_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output_code
  sc_bv<32>*__xlx_output_code_output_buffer = new sc_bv<32>[__xlx_size_param_output_code];
  for (int i = 0; i < __xlx_size_param_output_code; ++i) {
    __xlx_output_code_output_buffer[i] = __xlx_output_code__input_buffer[i+__xlx_offset_param_output_code];
  }
  for (int i = 0; i < __xlx_size_param_output_code; ++i) {
    ((int*)__xlx_apatb_param_output_code)[i] = __xlx_output_code_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output_size
  sc_bv<32>*__xlx_output_size_output_buffer = new sc_bv<32>[__xlx_size_param_output_size];
  for (int i = 0; i < __xlx_size_param_output_size; ++i) {
    __xlx_output_size_output_buffer[i] = __xlx_output_size_output_length__input_buffer[i+__xlx_offset_param_output_size];
  }
  for (int i = 0; i < __xlx_size_param_output_size; ++i) {
    ((int*)__xlx_apatb_param_output_size)[i] = __xlx_output_size_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output_length
  sc_bv<32>*__xlx_output_length_output_buffer = new sc_bv<32>[__xlx_size_param_output_length];
  for (int i = 0; i < __xlx_size_param_output_length; ++i) {
    __xlx_output_length_output_buffer[i] = __xlx_output_size_output_length__input_buffer[i+__xlx_offset_param_output_length];
  }
  for (int i = 0; i < __xlx_size_param_output_length; ++i) {
    ((int*)__xlx_apatb_param_output_length)[i] = __xlx_output_length_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_output
  sc_bv<8>*__xlx_output_output_buffer = new sc_bv<8>[__xlx_size_param_output];
  for (int i = 0; i < __xlx_size_param_output; ++i) {
    __xlx_output_output_buffer[i] = __xlx_output__input_buffer[i+__xlx_offset_param_output];
  }
  for (int i = 0; i < __xlx_size_param_output; ++i) {
    ((char*)__xlx_apatb_param_output)[i] = __xlx_output_output_buffer[i].to_uint64();
  }
}
