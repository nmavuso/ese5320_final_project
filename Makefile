#######################################################################################
.PHONY: help
help:
	@echo "Makefile Usage:"
	@echo ""
	@echo "  make all"
	@echo "  	Command to build client, encoder, and decoder."
	@echo ""
	@echo "  make client"
	@echo "  	Command to build client."
	@echo ""
	@echo "  make encoder"
	@echo "  	Command to build encoder."
	@echo ""
	@echo "  make decoder"
	@echo "  	Command to build decoder."
	@echo ""
	@echo "  make fpga"
	@echo "  	Command to build the FPGA binary (kernel.xclbin)."
	@echo ""
	@echo "  make clean"
	@echo "  	Command to remove the generated files."
	@echo ""
#######################################################################################

# Compiler tools
HOST_CXX ?= aarch64-linux-gnu-g++
VPP ?= ${XILINX_VITIS}/bin/v++
RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM = u96v2_sbc_base
VITIS_PLATFORM_DIR = ${PLATFORM_REPO_PATHS}
VITIS_PLATFORM_PATH = $(VITIS_PLATFORM_DIR)/u96v2_sbc_base.xpfm

# Host compiler global settings
CXXFLAGS += -march=armv8-a+simd -mtune=cortex-a53 -std=c++11 -DVITIS_PLATFORM=$(VITIS_PLATFORM) -D__USE_XOPEN2K8 -I$(XILINX_VIVADO)/include/ -I$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux/usr/include/xrt/ -O3 -g -Wall -c -fmessage-length=0 --sysroot=$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux
LDFLAGS += -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ -L$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux/usr/lib/ --sysroot=$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux

# Hardware compiler shared settings
VPP_OPTS = --target hw

# OpenCL kernel files
XO := lzw_hls.xo
XCLBIN := lzw_hls.xclbin
ALL_MESSAGE_FILES = $(subst .xo,.mdb,$(XO)) $(subst .xclbin,.mdb,$(XCLBIN))

# Host files
CLIENT_SOURCES = Client/client.cpp
CLIENT_EXE = client

SERVER_SOURCES = Server/encoder.cpp Server/server.cpp Server/cdc_hw.cpp Server/cmd_hw.cpp Server/lzw_hw.cpp Server/sha3.cpp
SERVER_OBJECTS = $(SERVER_SOURCES:.cpp=.o)
SERVER_EXE = encoder

DECODER_SOURCES = Decoder/Decoder.cpp
DECODER_OBJECTS = $(DECODER_SOURCES:.cpp=.o)
DECODER_EXE = decoder

# Host files
# Host files
HOST_SOURCES = Host.cpp ./common/Utilities.cpp ./common/EventTimer.cpp hls/lzw_hls.cpp
HOST_OBJECTS = $(HOST_SOURCES:.cpp=.o)
HOST_EXE = host

# Host executable target
$(HOST_EXE): $(HOST_OBJECTS)
	$(HOST_CXX) -o "$@" $(HOST_OBJECTS) $(LDFLAGS)
	@mkdir -p package/sd_card
	@cp $(HOST_EXE) package/sd_card/

%.o: %.cpp
	$(HOST_CXX) $(CXXFLAGS) -c -I./common -I./hls -o"$@" "$<"

host: $(HOST_EXE)

# Build all executables
all: $(CLIENT_EXE) $(SERVER_EXE) $(DECODER_EXE)

$(CLIENT_EXE):
	g++ -O3 $(CLIENT_SOURCES) -o "$@"

$(SERVER_EXE): $(SERVER_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

$(DECODER_EXE): $(DECODER_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

.cpp.o:
	$(HOST_CXX) $(CXXFLAGS) -I./server -o "$@" "$<"

# FPGA targets
.PHONY: fpga
fpga: package/sd_card.img

$(XO): hls/lzw_hls.cpp
	-@$(RM) $@
	$(VPP) $(VPP_OPTS) -k lzw_fpga --compile -I"$(<D)" --config fpga_acceleration/design.cfg -o"$@" "$<"

$(XCLBIN): $(XO)
	$(VPP) $(VPP_OPTS) --link --config fpga_acceleration/design.cfg -o"$@" $(+)

package/sd_card.img: $(SERVER_EXE) $(XCLBIN) ./fpga_acceleration/xrt.ini
	$(VPP) --package $(VPP_OPTS) --config fpga_acceleration/package.cfg $(XCLBIN) \
    	--package.out_dir package \
    	--package.sd_file $(SERVER_EXE) \
    	--package.kernel_image $(PLATFORM_REPO_PATHS)/sw/$(VITIS_PLATFORM)/PetaLinux/image/image.ub \
    	--package.rootfs $(PLATFORM_REPO_PATHS)/sw/$(VITIS_PLATFORM)/PetaLinux/rootfs/rootfs.ext4 \
    	--package.sd_file $(XCLBIN) \
    	--package.sd_file ./fpga_acceleration/xrt.ini

# Clean commands
.NOTPARALLEL: clean
clean:
	-$(RM) $(SERVER_EXE) $(SERVER_OBJECTS) $(DECODER_EXE) $(DECODER_OBJECTS) $(CLIENT_EXE)
	-$(RM) $(XCLBIN) $(XO) $(ALL_MESSAGE_FILES)
	-${RMDIR} package package.build .Xil fpga/hls/proj_kernel
	-${RMDIR} _x .ipcache

# Testbench executable
TESTBENCH_SOURCES = hls/lzw_hls.cpp hls/Testbench.cpp
TESTBENCH_EXE = lzw_hls_testbench

.PHONY: lzw_hls
lzw_hls: $(TESTBENCH_EXE)

$(TESTBENCH_EXE): $(TESTBENCH_SOURCES)
	g++ -O3 -std=c++11 -I./hls -o "$@" $(TESTBENCH_SOURCES)


# $(XO): fpga/hls/MMult.cpp
# 	-@$(RM) $@
# 	$(VPP) $(VPP_OPTS) -k mmult_fpga --compile -I"$(<D)" --config fpga/design.cfg -o"$@" "$<"

# $(XCLBIN): $(XO)
# 	$(VPP) $(VPP_OPTS) --link --config fpga/design.cfg -o"$@" $(+)

# package/sd_card.img: $(HOST_EXE) $(XCLBIN) ./fpga/xrt.ini
# 	$(VPP) --package $(VPP_OPTS) --config fpga/package.cfg $(XCLBIN) \
#     	--package.out_dir package \
#     	--package.sd_file $(HOST_EXE) \
#     	--package.kernel_image $(PLATFORM_REPO_PATHS)/sw/$(VITIS_PLATFORM)/PetaLinux/image/image.ub \
#     	--package.rootfs $(PLATFORM_REPO_PATHS)/sw/$(VITIS_PLATFORM)/PetaLinux/rootfs/rootfs.ext4 \
#     	--package.sd_file $(XCLBIN) \
#     	--package.sd_file ./fpga/xrt.ini

