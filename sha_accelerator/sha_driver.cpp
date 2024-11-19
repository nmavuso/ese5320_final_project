#include <linux/cacheflush.h>
#include <crypto/hash.h>
#include <crypto/internal/hash.h>
#include <crypto/sha3.h>
#include <linux/crypto.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/firmware/xlnx-zynqmp.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

// SETUP

// Flags for packet processsing
#define CONTINUE_PACKET BIT(31)
#define FIRST_PACKET BIT(30)
#define FINAL_PACKET 0

// Used for resetting operations
#define RESET 0

// DMA addressing capability (32-bit)
#define ZYNQMP_DMA_BIT_MASK 32U

// DMA Fixed siize allocations (4KB)
#define ZYNQMP_DMA_ALLOC_FIXED_SIZE 0x1000U

enum zynqmp_sha_op
{
    ZYNQMP_SHA3_INIT = 1,
    ZYNQMP_SHA3_UPDATE = 2,
    ZYNQMP_SHA3_FINAL = 4,
};

// Driver context: contains sha3-384 algorithm and device
struct xilinx_sha_drv_ctx
{
    struct shash_alg sha3_384;
    struct device *dev;
};

// Transform context: contains device & fallback information
struct zynqmp_sha_tfm_ctx
{
    struct device *dev;
    struct crypto_shash *fbk_tfm;
};

// Descriptor context: contains a fallback request
struct zynqmp_sha_desc_ctx
{
    struct shash_desc fbk_req;
};

static dma_addr_t update_dma_addr, final_dma_addr; // DMA address variables
static char *ubuf, *fbuf;                          // buffers for hashing ops

// CORE FUNCTIONS
static int zynqmp_sha_init(struct shash_desc *desc);                                              // initialize the hash state
static int zynqmp_sha_update(struct shash_desc *desc, const u8 *data, unsigned int length);       // process chunks of input data
static int zynqmp_sha_final(struct shash_desc *desc, u8 *out);                                    // finalize hash computation
static int zynqmp_sha_digest(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out); // complete hash operation in one call
