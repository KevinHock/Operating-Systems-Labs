#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/pagemap.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <asm/mman.h>
#include "my_tlb.h"
#include "my_mmap.h"
#include "rootkit.h"

MODULE_LICENSE("Dual BSD/GPL");

//#define ROOTKIT_DEBUG	1

#if defined(ROOTKIT_DEBUG) && ROOTKIT_DEBUG == 1
# define DEBUG(...)		printk(KERN_INFO __VA_ARGS__)
#else
# define DEBUG(...)
#endif


static int rootkit_init(void)
{
	int rv = 0;
	void * __end = (void *) &unmap_page_range;

	/* Find the non-exported symbols.  'Cause you can't stop me. */
	unmap_page_range = (unmap_page_range_t)
		kallsyms_lookup_name("unmap_page_range");
	if ((!unmap_page_range) || (void *) unmap_page_range >= __end) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find important function unmap_page_range\n");
		return -ENOENT;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	my_tlb_gather_mmu = (tlb_gather_mmu_t)
		kallsyms_lookup_name("tlb_gather_mmu");
	printk(KERN_ERR "resolved symbol tlb_gather_mmu %p\n", my_tlb_gather_mmu);
	if (!my_tlb_gather_mmu) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function my_tlb_gather_mmu\n");
		return -ENOENT;
	}

	my_tlb_flush_mmu = (tlb_flush_mmu_t)
		kallsyms_lookup_name("tlb_flush_mmu");
	if (!my_tlb_flush_mmu) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function my_tlb_flush_mmu\n");
		return -ENOENT;
	}

	my_tlb_finish_mmu = (tlb_finish_mmu_t)
		kallsyms_lookup_name("tlb_finish_mmu");
	if (!my_tlb_finish_mmu) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function my_tlb_finish_mmu\n");
		return -ENOENT;
	}
#else
	pmmu_gathers = (struct mmu_gather *)
		kallsyms_lookup_name("mmu_gathers");
	if (!pmmu_gathers) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function mmu_gathers\n");
		return -ENOENT;
	}
#endif //kernel_version >< 3.2

	kern_free_pages_and_swap_cachep = (free_pages_and_swap_cache_t)
		kallsyms_lookup_name("free_pages_and_swap_cache");
	if (!kern_free_pages_and_swap_cachep) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function free_pages_and_swap_cache\n");
		return -ENOENT;
	}

	kern_flush_tlb_mm = (flush_tlb_mm_t)
		kallsyms_lookup_name("flush_tlb_mm");
	if (!kern_flush_tlb_mm) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function flush_tlb_mm\n");
		return -ENOENT;
	}

	kern_free_pgtables = (free_pgtables_t)
		kallsyms_lookup_name("free_pgtables");
	if (!kern_free_pgtables) {
		printk(KERN_ERR "Rootkit error: "
		       "can't find kernel function free_pgtables\n");
		return -ENOENT;
	}

	printk(KERN_ALERT "Rootkit: Hello, world\n");
	return rv;
}

static void rootkit_exit(void)
{
	printk(KERN_ALERT "Rootkit: Goodbye, cruel world\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
