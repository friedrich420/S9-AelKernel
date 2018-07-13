/*
 * Samsung Exynos SoC series SCore driver
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/io.h>
#include <linux/dma-buf.h>
#include <linux/iommu.h>

#include "score_log.h"
#include "score_regs.h"
#include "score_system.h"
#include "score_mem_table.h"
#include "score_mmu.h"
#include "score_ion.h"
#include "score_memory.h"

int score_memory_kmap_packet(struct score_memory *mem,
		struct score_mmu_packet *packet)
{
	int ret = 0;
	int fd;
	struct dma_buf *dbuf;
	void *kvaddr;

	score_enter();
	fd = packet->fd;
	if (fd <= 0) {
		score_err("fd(%d) is invalid\n", fd);
		ret = -EINVAL;
		goto p_err;
	}

	dbuf = dma_buf_get(fd);
	if (IS_ERR_OR_NULL(dbuf)) {
		score_err("dma_buf is invalid (fd:%d)\n", fd);
		ret =  -EINVAL;
		goto p_err;
	}
	packet->dbuf = dbuf;

	ret = dma_buf_begin_cpu_access(dbuf, DMA_NONE);
	if (ret) {
		score_err("accessing at cpu failed (dbuf:%p)\n", dbuf);
		goto p_err_begin_cpu_access;
	}

	kvaddr = dma_buf_kmap(dbuf, 0);
	if (!kvaddr) {
		score_err("kmap failed (dbuf:%p)\n", dbuf);
		ret = -EFAULT;
		goto p_err_kmap;
	}

	packet->kvaddr = kvaddr;

	score_leave();
	return ret;
p_err_kmap:
	dma_buf_end_cpu_access(dbuf, 0);
p_err_begin_cpu_access:
	dma_buf_put(dbuf);
p_err:
	return ret;
}

void score_memory_kunmap_packet(struct score_memory *mem,
		struct score_mmu_packet *packet)
{
	struct dma_buf *dbuf;
	void *kvaddr;

	score_enter();
	dbuf = packet->dbuf;
	kvaddr = packet->kvaddr;

	dma_buf_kunmap(dbuf, 0, kvaddr);
	dma_buf_end_cpu_access(dbuf, DMA_NONE);
	dma_buf_put(dbuf);

	score_leave();
}

/**
 * score_memory_get_internal_mem_kvaddr - Get kvaddr of
 *      kernel space memory allocated for SCore device
 * @mem:        [in]    object about score_memory structure
 * @kvaddr:     [out]   kernel virtual address
 */
void *score_memory_get_internal_mem_kvaddr(struct score_memory *mem)
{
	score_enter();
	return mem->kvaddr;
}

/**
 * score_memory_get_internal_mem_dvaddr - Get dvaddr of
 *      kernel space memory allocated for SCore device
 * @mem:        [in]    object about score_memory structure
 * @dvaddr:     [out]   device virtual address
 */
dma_addr_t score_memory_get_internal_mem_dvaddr(struct score_memory *mem)
{
	score_check();
	return mem->dvaddr;
}

/**
 * score_memory_get_profiler_kvaddr - Get kvaddr of given core
 *      kernel space memory allocated for SCore device
 * @mem:        [in]    object about score_memory structure
 * @id:         [in]    id of requested core memory
 * @kvaddr:     [out]   kernel virtual address
 */
void *score_memory_get_profiler_kvaddr(struct score_memory *mem,
							unsigned int id)
{
	score_check();
	if (id == SCORE_MASTER)
		return mem->profiler_mc_kvaddr;
	else if (id == SCORE_KNIGHT1)
		return mem->profiler_kc1_kvaddr;
	else if (id == SCORE_KNIGHT2)
		return mem->profiler_kc2_kvaddr;
	return 0;
}

/**
 * score_memory_get_print_kvaddr - Get kvaddr of
 *      kernel space memory allocated for SCore device
 * @mem:        [in]    object about score_memory structure
 * @kvaddr:     [out]   kernel virtual address
 */
void *score_memory_get_print_kvaddr(struct score_memory *mem)
{
	score_check();
	return mem->print_kvaddr;
}

static int score_exynos_memory_map_dmabuf(struct score_mmu *mmu,
		struct score_mmu_buffer *buf)
{
	int ret = 0;
	struct score_memory *mem;
	struct dma_buf *dbuf;
	struct dma_buf_attachment *attachment;
	struct sg_table *sgt;
	dma_addr_t dvaddr;

	score_enter();
	mem = &mmu->mem;
	dbuf = buf->dbuf;

	attachment = dma_buf_attach(dbuf, mem->dev);
	if (IS_ERR(attachment)) {
		ret = PTR_ERR(attachment);
		score_err("failed to attach dmabuf (%d)\n", ret);
		goto p_err_attach;
	}
	buf->attachment = attachment;

	sgt = dma_buf_map_attachment(attachment, DMA_BIDIRECTIONAL);
	if (IS_ERR(sgt)) {
		ret = PTR_ERR(sgt);
		score_err("failed to map attachment (%d)\n", ret);
		goto p_err_map_attach;
	}
	buf->sgt = sgt;

	dvaddr = ion_iovmm_map(attachment, 0, buf->size, DMA_BIDIRECTIONAL,
			IOMMU_CACHE);
	if (IS_ERR_VALUE(dvaddr)) {
		ret = (int)dvaddr;
		score_err("failed to map iova (%d)\n", ret);
		goto p_err_iovmm_map;
	}
	buf->dvaddr = dvaddr;

	score_leave();
	return ret;
p_err_iovmm_map:
	dma_buf_unmap_attachment(attachment, sgt, DMA_BIDIRECTIONAL);
p_err_map_attach:
	dma_buf_detach(dbuf, attachment);
p_err_attach:
	return ret;
}

static int score_exynos_memory_unmap_dmabuf(struct score_mmu *mmu,
		struct score_mmu_buffer *buf)
{
	score_enter();
	ion_iovmm_unmap(buf->attachment, buf->dvaddr);
	dma_buf_unmap_attachment(buf->attachment, buf->sgt, DMA_BIDIRECTIONAL);
	dma_buf_detach(buf->dbuf, buf->attachment);
	buf->dvaddr = 0;

	score_leave();
	return 0;
}

const struct score_mmu_ops score_exynos_mmu_ops = {
	.map_dmabuf	= score_exynos_memory_map_dmabuf,
	.unmap_dmabuf	= score_exynos_memory_unmap_dmabuf,
};

static int __score_memory_alloc(struct score_memory *mem)
{
	int ret = 0;
	dma_addr_t dvaddr;
	void *kvaddr;

	score_enter();
	ret = score_ion_alloc(mem->ctx, SCORE_MEMORY_INTERNAL_SIZE,
			&dvaddr, &kvaddr);
	if (ret)
		goto p_err;

	mem->dvaddr = dvaddr;
	mem->kvaddr = kvaddr;

	mem->malloc_kvaddr = mem->kvaddr + SCORE_FW_SIZE;
	mem->malloc_dvaddr = mem->dvaddr + SCORE_FW_SIZE;

	mem->print_kvaddr = mem->malloc_kvaddr + SCORE_MALLOC_SIZE;
	mem->print_dvaddr = mem->malloc_dvaddr + SCORE_MALLOC_SIZE;

	mem->profiler_mc_dvaddr = mem->print_dvaddr + SCORE_PRINT_SIZE;
	mem->profiler_mc_kvaddr = mem->print_kvaddr + SCORE_PRINT_SIZE;
	mem->profiler_kc1_dvaddr =
			mem->profiler_mc_dvaddr + SCORE_MC_PROFILER_SIZE;
	mem->profiler_kc1_kvaddr =
			mem->profiler_mc_kvaddr + SCORE_MC_PROFILER_SIZE;
	mem->profiler_kc2_dvaddr =
			mem->profiler_kc1_dvaddr + SCORE_KC_PROFILER_SIZE;
	mem->profiler_kc2_kvaddr =
			mem->profiler_kc1_kvaddr + SCORE_KC_PROFILER_SIZE;

	score_leave();
	return ret;
p_err:
	return ret;
}

static void __score_memory_free(struct score_memory *mem)
{
	score_enter();
	score_ion_free(mem->ctx);
	score_leave();
}

void score_memory_init(struct score_memory *mem)
{
	void __iomem *sfr;
	dma_addr_t kc1_malloc, kc2_malloc;

	score_enter();
	sfr = mem->system->sfr;
	kc1_malloc = mem->malloc_dvaddr + SCORE_MC_MALLOC_SIZE;
	kc2_malloc = kc1_malloc + SCORE_KC_MALLOC_SIZE;

	writel(mem->malloc_dvaddr,   sfr + MC_MALLOC_BASE_ADDR);
	writel(SCORE_MC_MALLOC_SIZE, sfr + MC_MALLOC_SIZE);
	writel(kc1_malloc,           sfr + KC1_MALLOC_BASE_ADDR);
	writel(kc2_malloc,           sfr + KC2_MALLOC_BASE_ADDR);
	writel(SCORE_KC_MALLOC_SIZE, sfr + KC_MALLOC_SIZE);
	writel(mem->print_dvaddr,    sfr + PRINT_BASE_ADDR);
	writel(SCORE_PRINT_SIZE,     sfr + PRINT_SIZE);
	writel(SCORE_MC_PROFILER_SIZE,  sfr + PROFILER_MC_SIZE);
	writel(SCORE_KC_PROFILER_SIZE,  sfr + PROFILER_KC_SIZE);
	writel(mem->profiler_mc_dvaddr, sfr + PROFILER_MC_BASE_ADDR);
	writel(mem->profiler_kc1_dvaddr, sfr + PROFILER_KC1_BASE_ADDR);
	writel(mem->profiler_kc2_dvaddr, sfr + PROFILER_KC2_BASE_ADDR);

	score_leave();
}

int score_memory_open(struct score_memory *mem)
{
	int ret = 0;

	score_enter();
	ret = __score_memory_alloc(mem);
	score_leave();
	return ret;
}

void score_memory_close(struct score_memory *mem)
{
	score_enter();
	__score_memory_free(mem);
	score_leave();
}

int score_memory_probe(struct score_mmu *mmu)
{
	int ret = 0;
	struct score_memory *mem;

	score_enter();
	mem = &mmu->mem;
	mem->dev = mmu->system->dev;
	mem->system = mmu->system;

	ret = score_ion_create_context(mem);
	if (ret)
		return ret;

	mmu->mmu_ops = &score_exynos_mmu_ops;
	score_leave();
	return ret;
}

void score_memory_remove(struct score_memory *mem)
{
	score_enter();
	score_ion_destroy_context(mem->ctx);
	score_leave();
}
