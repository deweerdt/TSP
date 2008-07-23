/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_module.c,v 1.6 2008-07-23 15:18:05 jaggy Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Frederik Deweerdt

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : Black board

-----------------------------------------------------------------------

Purpose   : Blackboard kernel module

-----------------------------------------------------------------------
 */


#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>

#include "bb_core.h"
#include "bb_core_k.h"
#include "bb_simple.h"
#include "bb_callback.h"

static int run_test = 0;
module_param(run_test, int, 0);

struct class *bb_dev_class = NULL;
static dev_t bb_devt;
int bb_major;
int bb_minor = 0;

static int bb_open(struct inode *inode, struct file *filp)
{
	struct bb_device *dev; 

	dev = container_of(inode->i_cdev, struct bb_device, cdev);
	filp->private_data = dev;
	return 0;
}

static int bb_release(struct inode *i, struct file *filp)
{
	return 0;
}

#define BBKGETSIZE 1 /* gets the shared memory size */
#define BBKSETSIZE 2 /* sets the shared memory size */

static int bb_ioctl(struct inode *i, struct file *filp,
		       unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct bb_device *dev;

	dev = container_of(i->i_cdev, struct bb_device, cdev);

	switch (cmd) {
	case BBKGETSIZE:
		if (copy_to_user ((int *) arg, &dev->bb->priv.k.shm_size, sizeof(dev->bb->priv.k.shm_size)))
			ret = -EFAULT;
		break;
	case BBKSETSIZE:
		if (copy_from_user (&dev->bb->priv.k.shm_size, (int *) arg, sizeof(dev->bb->priv.k.shm_size)))
			ret = -EFAULT;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static void msg_callback(struct S_BB *bb, void * context,
			 struct S_BB_MSG *msg);

static struct task_struct *test_thread;
static int test_running = 0;
#define MS_TO_JIFFIES(j) ((j * HZ) / 1000)
static int bb_test(void *data)
{
	int *v1, *v2;
	S_BB_T *test_bb1;
	S_BB_T *test_bb2;
	struct S_BB_MSG msg;


	bb_create(&test_bb1, "bb_test1", 2, 200);
	v1 = (int*)bb_simple_publish(test_bb1,"variable","bb_test1", -1, E_BB_INT32, sizeof(int), 1);

	bb_create(&test_bb2, "bb_test2", 2, 200);
	v2 = (int*)bb_simple_publish(test_bb2,"variable","bb_test2", -1, E_BB_INT32, sizeof(int), 1);
	*v1 = 0;
	*v2 = 0;

	msg.mtype = 10;
	bb_msg_subscribe(test_bb2, "Coucou bb2 10",  &msg, msg_callback);

	msg.mtype = 20;
	bb_msg_subscribe(test_bb1, "Coucou bb1 20",  &msg, msg_callback);


	while(!kthread_should_stop()) {
		test_running = 1;
		msleep(1000);
		*v1=*v1+1;
		*v2=*v2+1;

		bb_simple_synchro_go(test_bb1, 10);
		bb_simple_synchro_go(test_bb2, 20);
	}
	test_running = 0;
	bb_destroy(&test_bb1);
	bb_destroy(&test_bb2);
	return 0;
}

static void msg_callback(struct S_BB *bb, void* context, struct S_BB_MSG *msg)
{
	(void)bb;
	printk("BB : Received msg (bb [%s] context [%s] mtype [%ld])\n",
		bb->name, (char *)context, msg->mtype);
	return;
}

static int bb_mmap(struct file *filp, struct vm_area_struct *vma)
{

	unsigned long vsize = vma->vm_end - vma->vm_start;
	int ret;
	S_BB_T *bb;
	struct bb_device *dev = filp->private_data;

	bb = dev->bb;
	/* sanity check, assert that the user doesn't request more
	 * than available */
	if (vsize > (bb->priv.k.shm_size + 2 * PAGE_SIZE)) {
		printk("mmap requested more than available: %lu > %lu\n", 
			vsize, bb->priv.k.shm_size + 2 * PAGE_SIZE);
		return -EINVAL;
	}

	/* No offset allowed */
	if (vma->vm_pgoff != 0) {
		printk("bb->mmap doesn't support offsets\n");
		return -EINVAL;
	}

	/* In theory, this could be writeable, at least for root */
	/* Disable it for now, as this needs testing */
        if (vma->vm_flags & VM_WRITE)
		return -EPERM;


	/* remap pages to the user's address space */
	ret = remap_pfn_range(vma,
			      vma->vm_start,
			      virt_to_phys((void *) ((unsigned long) bb))
			      >> PAGE_SHIFT,
			      vma->vm_end - vma->vm_start,
			      PAGE_SHARED);
	if (ret != 0)
		return -EAGAIN;

	return 0;
}

struct file_operations bb_fops = {
	.owner = THIS_MODULE,
	.ioctl = bb_ioctl,
	.mmap = bb_mmap,
	.open = bb_open,
	.release = bb_release,
};

static int __init bb_init_module(void)
{
	int err;

	bb_dev_class = class_create(THIS_MODULE, "bb-dev");
	if (IS_ERR(bb_dev_class))
		return PTR_ERR(bb_dev_class);

	err = alloc_chrdev_region(&bb_devt, 0, BB_DEV_MAX, "bb");
	bb_minor = 0;
	bb_major = MAJOR(bb_devt);

	if (err < 0) {
		printk(KERN_ERR "%s: failed to allocate char dev region\n",
			__FILE__);
		goto err_destroy_class;
	}

	bitmap_zero(present_devices, BB_DEV_MAX);

	/* This is here for demo purposes, activate with the param */
	if (run_test)
		test_thread = kthread_run(bb_test, NULL, "bb_test");

	printk("Kernel black board loaded %d %d.\n", bb_major, bb_minor);
	printk("See http://savannah.nongnu.org/projects/tsp for details.\n");
	return 0;

err_destroy_class:
	class_destroy(bb_dev_class);

	return err;
}

static void __exit bb_cleanup_module(void)
{
	if (run_test && test_running) {
		kthread_stop(test_thread);
	}

	spin_lock(&pdeviceslock);
	while(!bitmap_empty(present_devices, BB_DEV_MAX)) {
		int i = find_first_bit(present_devices, BB_DEV_MAX);
		/* bb_destroy clears the bit in present_devices */
		bb_destroy(&present_bbs[i]);
	}
	spin_unlock(&pdeviceslock);
	unregister_chrdev_region(bb_devt, BB_DEV_MAX);
	class_destroy(bb_dev_class);
	printk("Kernel black board unloaded.\n");
}

module_init(bb_init_module);
module_exit(bb_cleanup_module);
MODULE_AUTHOR("frederik.deweerdt@gmail.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("In-kernel black board");
