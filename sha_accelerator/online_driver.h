// SPDX-License-Identifier: GPL-2.0
/*
 * Xilinx ZynqMP SHA Driver.
 * Copyright (c) 2022 Xilinx Inc.
 */
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

#define ZYNQMP_DMA_BIT_MASK 32U
#define ZYNQMP_DMA_ALLOC_FIXED_SIZE 0x1000U

enum zynqmp_sha_op
{
    ZYNQMP_SHA3_INIT = 1,
    ZYNQMP_SHA3_UPDATE = 2,
    ZYNQMP_SHA3_FINAL = 4,
};

struct zynqmp_sha_drv_ctx
{
    struct shash_alg sha3_384;
    struct device *dev;
};

struct zynqmp_sha_tfm_ctx
{
    struct device *dev;
    struct crypto_shash *fbk_tfm;
};

struct zynqmp_sha_desc_ctx
{
    struct shash_desc fbk_req;
};

static dma_addr_t update_dma_addr, final_dma_addr;
static char *ubuf, *fbuf;

static int zynqmp_sha_init_tfm(struct crypto_shash *hash);
static void zynqmp_sha_exit_tfm(struct crypto_shash *hash);

static int zynqmp_sha_init(struct shash_desc *desc);

static int zynqmp_sha_update(struct shash_desc *desc, const u8 *data, unsigned int length);
static int zynqmp_sha_final(struct shash_desc *desc, u8 *out);

static int zynqmp_sha_finup(struct shash_desc *desc, const u8 *data, unsigned int length, u8 *out);
static int zynqmp_sha_import(struct shash_desc *desc, const void *in);

static int zynqmp_sha_export(struct shash_desc *desc, void *out);
static int zynqmp_sha_digest(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out);

static struct zynqmp_sha_drv_ctx sha3_drv_ctx = {
    .sha3_384 = {
        .init = zynqmp_sha_init,
        .update = zynqmp_sha_update,
        .final = zynqmp_sha_final,
        .finup = zynqmp_sha_finup,
        .digest = zynqmp_sha_digest,
        .export = zynqmp_sha_export,
        .import = zynqmp_sha_import,
        .init_tfm = zynqmp_sha_init_tfm,
        .exit_tfm = zynqmp_sha_exit_tfm,
        .descsize = sizeof(struct zynqmp_sha_desc_ctx),
        .statesize = sizeof(struct sha3_state),
        .digestsize = SHA3_384_DIGEST_SIZE,
        .base = {
            .cra_name = "sha3-384",
            .cra_driver_name = "zynqmp-sha3-384",
            .cra_priority = 300,
            .cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY |
                         CRYPTO_ALG_ALLOCATES_MEMORY |
                         CRYPTO_ALG_NEED_FALLBACK,
            .cra_blocksize = SHA3_384_BLOCK_SIZE,
            .cra_ctxsize = sizeof(struct zynqmp_sha_tfm_ctx),
            .cra_module = THIS_MODULE,
        }}};

static int zynqmp_sha_probe(struct platform_device *pdev);

static void zynqmp_sha_remove(struct platform_device *pdev);
y static struct platform_driver zynqmp_sha_driver = {
    .probe = zynqmp_sha_probe,
    .remove_new = zynqmp_sha_remove,
    .driver = {
        .name = "zynqmp-sha3-384",
    },
};

module_platform_driver(zynqmp_sha_driver);
MODULE_DESCRIPTION("ZynqMP SHA3 hardware acceleration support.");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Harsha <harsha.harsha@xilinx.com>");