

#include <linux/module.h>  // For module_init, module_exit
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/cdev.h>   // for cdev_...
#include <linux/device.h>  // for class_create

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DAVIT KALANTARYAN");

#define DEVICE_MINOR_NUMBER				0
#define	DRV_AND_DEVICE_ENTRY_NAME		"basic_file_ops_test"

struct SDeviceStruct{
	struct cdev		m_cdev;
}static s_deviceStruct;

static int SimpleOpen (struct inode * a_in, struct file * a_fp);
static int SimpleOpenRelease (struct inode * a_in, struct file * a_fp);
static ssize_t SimpleRead(struct file * a_fp, char __user * a_userBuffer, size_t a_size, loff_t * a_pOff);
static ssize_t SimpleWrite (struct file * a_fp, const char __user * a_userBuffer, size_t a_size, loff_t * a_pOff);
static long SimpleIoctl (struct file * a_fp, unsigned int a_code, unsigned long a_agument);

static dev_t			s_deviceNumber = -1;
static struct class*	s_class = NULL;
static struct device*	s_pDevice = NULL;
static int				s_cdevAdded = 0;

static const struct file_operations s_file_operations = {
	.owner			= THIS_MODULE,
	.read			= SimpleRead,
	.write			= SimpleWrite,
	.unlocked_ioctl	= SimpleIoctl,
	.compat_ioctl	= SimpleIoctl,
	.open			= SimpleOpen,
	.release		= SimpleOpenRelease,
};


static int SimpleOpen (struct inode * a_in, struct file * a_fp)
{
	struct SDeviceStruct* dev = container_of(a_in->i_cdev, struct SDeviceStruct, m_cdev);
	pr_notice("inode=%p, fp=%p, privateData=%p, (&s_deviceStruct=%p, &cdev=%p)\n",a_in,a_fp,a_fp->private_data,dev,a_in->i_cdev);
	pr_notice("a_fp->f_version=%llu\n",a_fp->f_version);
	++a_fp->f_version;
	a_fp->private_data = dev;
	return 0;
}


static int SimpleOpenRelease (struct inode * a_in, struct file * a_fp)
{
	pr_notice("inode=%p, fp=%p, privateData=%p, drvData=%p\n",a_in,a_fp,a_fp->private_data,dev_get_drvdata(s_pDevice));
	return 0;
}


static ssize_t SimpleRead(struct file * a_fp, char __user * a_userBuffer, size_t a_size, loff_t * a_pOff)
{
	pr_notice("fp=%p, userBuf=%p, bufSize=%d, ofsetPtr=%p, privateData=%p\n",a_fp,a_userBuffer,(int)a_size,a_pOff,a_fp->private_data)		;
	return 0;
}


static ssize_t SimpleWrite (struct file * a_fp, const char __user * a_userBuffer, size_t a_size, loff_t * a_pOff)
{
	pr_notice("fp=%p, userBuf=%p, bufSize=%d, ofsetPtr=%p, privateData=%p\n",a_fp,a_userBuffer,(int)a_size,a_pOff,a_fp->private_data)		;
	return 0;
}


static long SimpleIoctl (struct file * a_fp, unsigned int a_code, unsigned long a_agument)
{
	pr_notice("fp=%p, code=%ud, argument=%lud, privateData=%p\n",a_fp,a_code,a_agument,a_fp->private_data)		;
	return 0;
}


static void CleanupModulePrivate(void)
{
	if(s_pDevice && (!IS_ERR(s_pDevice))){
		//device_destroy(s_class, MKDEV(s_nMajorNumber,DEVICE_MINOR_NUMBER)); // s_deviceNumber
		device_destroy(s_class, s_deviceNumber);
		//put_device(s_pDevice);
		//device_unregister(s_pDevice);
		s_pDevice = NULL;
	}
	
	if(s_cdevAdded){
		cdev_del(&s_deviceStruct.m_cdev);
		s_cdevAdded = 0;
	}
	
	if(s_class && (!IS_ERR(s_class))){
		class_destroy(s_class);
		s_class = NULL;
	}
	
	if(s_deviceNumber>=0){
		//unregister_chrdev(s_nMajorNumber,DRV_AND_DEVICE_ENTRY_NAME);
		unregister_chrdev_region(s_deviceNumber,1);
		s_deviceNumber = -1;
	}
}


static int __init hello_world_test_init_module(void)
{
	dev_t firstdev;
	int result;
	pr_notice("basic_file_ops_test: version 5. Initing module (&s_deviceStruct=%p, &cdev=%p)\n",&s_deviceStruct,&s_deviceStruct.m_cdev);
	
	//s_nMajorNumber = register_chrdev(0,DRV_AND_DEVICE_ENTRY_NAME,&s_file_operations);
	result = alloc_chrdev_region(&firstdev,DEVICE_MINOR_NUMBER,1,DRV_AND_DEVICE_ENTRY_NAME);
	if (result < 0) {
		pr_err( "scull: can't get major %d\n",result);
		return 1;
	}
	s_deviceNumber = firstdev;
	pr_notice("Major=%u, Minor=%u\n",MAJOR(s_deviceNumber),MINOR(s_deviceNumber));
	
	s_class = class_create(THIS_MODULE, DRV_AND_DEVICE_ENTRY_NAME);	
	if(IS_ERR(s_class)){
		CleanupModulePrivate();
		return 2;
	}
	
	cdev_init(&s_deviceStruct.m_cdev,&s_file_operations);	
	//result = cdev_add(&s_deviceStruct.m_cdev,MKDEV(s_nMajorNumber,DEVICE_MINOR_NUMBER),1);
	result = cdev_add(&s_deviceStruct.m_cdev,s_deviceNumber,1);
	if (result){
		pr_err("Adding error(%d) (Major=%u, Minor=%u) \n", result, MAJOR(s_deviceNumber),MINOR(s_deviceNumber));
		CleanupModulePrivate();
		return 3;
	}
	s_cdevAdded = 1;
	
	//s_pDevice = device_create(s_class, NULL, MKDEV(s_nMajorNumber,DEVICE_MINOR_NUMBER),(void*)0x1,DRV_AND_DEVICE_ENTRY_NAME);
	s_pDevice = device_create(s_class, NULL, s_deviceNumber,(void*)0x1,DRV_AND_DEVICE_ENTRY_NAME);
	if (IS_ERR(s_pDevice)){
		pr_err("Device creation error!\n");
		CleanupModulePrivate();
		return 4;
	}
	
	return 0;
}


static void __exit hello_world_test_cleanup_module(void)
{
	pr_notice("Cleaning moule!\n");
	CleanupModulePrivate();
}


module_init(hello_world_test_init_module);
module_exit(hello_world_test_cleanup_module);
