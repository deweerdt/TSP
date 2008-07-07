#ifndef __BB_CORE_K_H__
#define __BB_CORE_K_H__

#define BB_DEV_MAX 128

#ifdef __KERNEL__
#include <linux/fs.h>
#include <linux/cdev.h>
#include "bb_core.h"

/** ioctls for the bb */
#define BBKGETSIZE 1 /* gets the shared memory size */
#define BBKSETSIZE 2 /* sets the shared memory size */


extern struct file_operations bb_fops;
extern int bb_major;
extern int bb_minor;
extern S_BB_T *present_bbs[BB_DEV_MAX];
extern DECLARE_BITMAP(present_devices, BB_DEV_MAX);
extern spinlock_t pdeviceslock;

extern struct class *bb_dev_class;

struct bb_device {
	struct S_BB *bb;
	/* represents the char device, useless in user space */
	struct cdev cdev;
	dev_t devno;
	char *name;
};


#endif /* __KERNEL__ */


#endif /* __BB_CORE_K_H__ */
