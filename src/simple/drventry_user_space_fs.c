

#include <linux/module.h>  // For module_init, module_exit
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/device.h>  // for class_create
#include <usr/user_space_fs_drv_user.h>
#include <kernel/user_space_fs_drv_kernel.h>
#include <linux/atomic.h>						// for atomic_inc_return()
#include <asm/uaccess.h>				// for strncpy_from_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DAVIT KALANTARYAN");

#define MINOR_NUMBER_FOR_MAIN_FS	0


static int		SimpleOpen (struct inode * a_in, struct file * a_fp);
static int		UserFsRelease (struct inode * a_in, struct file * a_fp);
static ssize_t	SimpleRead(struct file * a_fp, char __user * a_userBuffer, size_t a_size, loff_t * a_pOff);
static ssize_t	SimpleWrite (struct file * a_fp, const char __user * a_userBuffer, size_t a_size, loff_t * a_pOff);
static long		UserFsIoctl (struct file * a_fp, unsigned int a_code, unsigned long a_agument);

struct class*			g_class = NULL;
static dev_t			s_firstDevNumber = -1;
static struct cdev		s_cdev;
static struct device*	s_pDevice = NULL;
static int				s_cdevAdded = 0;

static const struct file_operations s_file_operations = {
	.owner			= THIS_MODULE,
	.read			= SimpleRead,
	.write			= SimpleWrite,
	.unlocked_ioctl	= UserFsIoctl,
	.compat_ioctl	= UserFsIoctl,
	.open			= SimpleOpen,
	.release		= UserFsRelease,
};


static void CleanupAllOpenUserFS(void)
{
	//
}


static int SimpleOpen (struct inode * a_in, struct file * a_fp)
{
	pr_notice("inode=%p, fp=%p, i_private=%p\n",a_in,a_fp,a_in->i_private);
	a_fp->private_data = NULL;
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


static int UserFsRelease (struct inode * a_in, struct file * a_fp)
{
	pr_notice("inode=%p, fp=%p\n",a_in,a_fp);
	if(likely(a_fp->private_data)){
		struct SUserFsDeviceStruct* pData =(struct SUserFsDeviceStruct*)a_fp->private_data;
		DestroyUserFs(pData);
		a_fp->private_data = NULL;
	}
	return 0;
}


static long UserFsIoctl (struct file * a_fp, unsigned int a_code, unsigned long a_agument)
{
	pr_notice("fp=%p, code=%ud, argument=%lud, privateData=%p\n",a_fp,a_code,a_agument,a_fp->private_data)		;
	
	switch(a_code){
	case USER_SPACE_FC_ADD_NEW_IOC:{
		if(unlikely(a_fp->private_data)){
			return 1;
		}
		else{
			long ullStrLen;
			char vcBuffer[128];
			struct SUserFsDeviceStruct* pData;
			
			ullStrLen = strncpy_from_user(vcBuffer,(const char*)a_agument,127);
			if(ullStrLen<0){
				pr_err("Unable to get string from user!\n");
				return -1;
			}
			vcBuffer[ullStrLen]=0;
			pData = CreateNewUserFs(vcBuffer);
			pr_notice("pData=%p, name=%s\n",pData,vcBuffer);
			a_fp->private_data = pData;
		}
		
	}break;
	case USER_SPACE_FC_REMOVE_IOC:
		if(likely(a_fp->private_data)){
			struct SUserFsDeviceStruct* pData =(struct SUserFsDeviceStruct*)a_fp->private_data;
			DestroyUserFs(pData);
			a_fp->private_data = NULL;
		}
		break;
	default:
	return -ENOTTY;
	}
	
	return 0;
}


static void CleanupModulePrivate(void)
{
	CleanupAllOpenUserFS();
	CleanFsFunctions();
	
	if(s_pDevice && (!IS_ERR(s_pDevice))){
		device_destroy(g_class, s_firstDevNumber);
		//put_device(s_pDevice);
		//device_unregister(s_pDevice);
		s_pDevice = NULL;
	}
	
	if(s_cdevAdded){
		cdev_del(&s_cdev);
		s_cdevAdded = 0;
	}
	
	if(g_class && (!IS_ERR(g_class))){
		class_destroy(g_class);
		g_class = NULL;
	}
	
	if(s_firstDevNumber>=0){
		//unregister_chrdev(g_nMajorNumber,USER_FS_DRIVER_NAME);
		unregister_chrdev_region(s_firstDevNumber,1);
		s_firstDevNumber = -1;
	}
}


static int __init hello_world_test_init_module(void)
{
	int result;
	dev_t firstdev;
	pr_notice("user_space_fs: version 1. Initing module \n");
	
	// s_nMajorNumber = register_chrdev(0,DRV_AND_DEVICE_ENTRY_NAME,&s_file_operations);
	result = alloc_chrdev_region(&firstdev,MINOR_NUMBER_FOR_MAIN_FS,1,USER_FS_DRIVER_NAME);
	if (result < 0) {
		pr_err( "scull: can't get major %d\n",result);
		return 1;
	}
	s_firstDevNumber = firstdev;
	pr_notice("Major=%u, Minor=%u\n",MAJOR(s_firstDevNumber),MINOR(s_firstDevNumber));
	
	g_class = class_create(THIS_MODULE, USER_FS_DRIVER_NAME);	
	if(IS_ERR(g_class)){
		CleanupModulePrivate();
		return 2;
	}	
	
	cdev_init(&s_cdev,&s_file_operations);	
	
	result = cdev_add(&s_cdev,s_firstDevNumber,1);
	if (result){
		pr_err("Adding error(%d) devno:%d for slot:%dd\n", result, (int)s_firstDevNumber, MINOR_NUMBER_FOR_MAIN_FS);
		CleanupModulePrivate();
		return 3;
	}
	s_cdevAdded = 1;
	
	s_pDevice = device_create(g_class, NULL, s_firstDevNumber,(void*)0x1,USER_FS_DEVICE_ENTRY_DIR "!" USER_FS_DEVICE_ENTRY_NAME);
	pr_notice("!!!!!!!!!!!!!!!!! s_pDevice=%p \n",s_pDevice);
	if (IS_ERR(s_pDevice)){
		pr_err("Device creation error!\n");
		CleanupModulePrivate();
		return 4;
	}
	//device_create_file()
	
	if(InitFsFunctions()){
		pr_err("Unable to init FS functions!\n");
		CleanupModulePrivate();
		return 5;
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
