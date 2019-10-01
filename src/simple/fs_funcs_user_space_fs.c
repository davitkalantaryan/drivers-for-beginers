

#include <kernel/user_space_fs_drv_kernel.h>
#include <linux/slab.h>							// for kzalloc
#include <linux/printk.h>						// for pr_* functions
#include <linux/fs.h>							// for struct file_operations
#include <linux/cdev.h>							// for struct cdev
#include <asm/string.h>							// for memset

#define FIRST_MINOR_NUMBER_FOR_FS	0
#define MAX_NUMBER_OF_UFS			256

static atomic_t						s_currentDevNumOffset = {FIRST_MINOR_NUMBER_FOR_FS};
//static struct cdev					s_cdev;  // todo: better to have one cdev
static dev_t						s_firstDevNumber = -1;
//static struct SUserFsDeviceStruct*	s_vectDevices[MAX_NUMBER_OF_UFS];

static int		SimpleOpenFs (struct inode * a_in, struct file * a_fp);
static int		UserFsReleaseFs (struct inode * a_in, struct file * a_fp);
static ssize_t	SimpleReadFs(struct file * a_fp, char __user * a_userBuffer, size_t a_size, loff_t * a_pOff);
static ssize_t	SimpleWriteFs (struct file * a_fp, const char __user * a_userBuffer, size_t a_size, loff_t * a_pOff);
static long		UserFsIoctlFs (struct file * a_fp, unsigned int a_code, unsigned long a_agument);

static const struct file_operations s_file_operations = {
	.owner			= THIS_MODULE,
	.read			= SimpleReadFs,
	.write			= SimpleWriteFs,
	.unlocked_ioctl	= UserFsIoctlFs,
	.compat_ioctl	= UserFsIoctlFs,
	.open			= SimpleOpenFs,
	.release		= UserFsReleaseFs,
};


static int SimpleOpenFs (struct inode * a_in, struct file * a_fp)
{
	struct SUserFsDeviceStruct* dev = container_of(a_in->i_cdev, struct SUserFsDeviceStruct, cdev);
	pr_notice("inode=%p, fp=%p, dev=%p\n",a_in,a_fp,dev);
	a_fp->private_data = dev;
	return 0;
}
static ssize_t SimpleReadFs(struct file * a_fp, char __user * a_userBuffer, size_t a_size, loff_t * a_pOff)
{
	struct SUserFsDeviceStruct* dev = (struct SUserFsDeviceStruct*)a_fp->private_data;
	pr_notice("fp=%p, userBuf=%p, bufSize=%d, ofsetPtr=%p, dev=%p\n",a_fp,a_userBuffer,(int)a_size,a_pOff,dev)		;
	return 0;
}
static ssize_t SimpleWriteFs (struct file * a_fp, const char __user * a_userBuffer, size_t a_size, loff_t * a_pOff)
{
	struct SUserFsDeviceStruct* dev = (struct SUserFsDeviceStruct*)a_fp->private_data;
	pr_notice("fp=%p, userBuf=%p, bufSize=%d, ofsetPtr=%p, dev=%p\n",a_fp,a_userBuffer,(int)a_size,a_pOff,dev)	;
	return 0;
}


static int UserFsReleaseFs (struct inode * a_in, struct file * a_fp)
{
	struct SUserFsDeviceStruct* dev = (struct SUserFsDeviceStruct*)a_fp->private_data;
	pr_notice("inode=%p, fp=%p, dev=%p\n",a_in,a_fp,dev);
	return 0;
}


static long UserFsIoctlFs (struct file * a_fp, unsigned int a_code, unsigned long a_agument)
{
	struct SUserFsDeviceStruct* dev = (struct SUserFsDeviceStruct*)a_fp->private_data;
	pr_notice("fp=%p, code=%ud, argument=%lud, dev=%p\n",a_fp,a_code,a_agument,dev)		;
	
	return 0;
}


struct SUserFsDeviceStruct* CreateNewUserFs(const char* a_nEntryName)
{
	int result;
	struct SUserFsDeviceStruct* pTmpStruct;
	int nCurrentOffset = atomic_inc_return(&s_currentDevNumOffset);
	
	if((--nCurrentOffset)>=MAX_NUMBER_OF_UFS){
		atomic_dec(&s_currentDevNumOffset);
		pr_warn("Maximum number of UFS is reached!\n");
		return NULL;
	}
	
	pTmpStruct = kzalloc(sizeof(struct SUserFsDeviceStruct), GFP_KERNEL);	
	pr_notice("CreateNewUserFs: currentOffset=%d,a_pUserFs=%p\n",nCurrentOffset,pTmpStruct);	
	if(!pTmpStruct){return pTmpStruct;}
	
	pTmpStruct->creatorProcess = current->group_leader->pid;
	
	//s_vectDevices[nCurrentOffset]=pTmpStruct;
	cdev_init(&pTmpStruct->cdev,&s_file_operations);
	pTmpStruct->deviceNumber =s_firstDevNumber+ nCurrentOffset;
	result = cdev_add(&pTmpStruct->cdev,pTmpStruct->deviceNumber,1);
	if (result){
		pr_err("Adding error(%d) devno:%d \n", result, (int)pTmpStruct->deviceNumber);
		DestroyUserFs(pTmpStruct);
		return NULL;
	}
	pTmpStruct->cdevAdded = 1;
	
	pTmpStruct->pDevice = device_create(g_class, NULL, pTmpStruct->deviceNumber,pTmpStruct,USER_FS_DEVICE_ENTRY_DIR "!" USER_FS_FS_DIR_NAME_IN_DIR "!%s",a_nEntryName);
	pr_notice("!!!!!!!!!!!!!!!!! pDevice=%p, pid=%d \n",pTmpStruct->pDevice,(int)pTmpStruct->creatorProcess);
	if (IS_ERR(pTmpStruct->pDevice)){
		pr_err("Device creation error!\n");
		DestroyUserFs(pTmpStruct);
		return NULL;
	}
	
	//pTmpStruct->minorNumber = a_minorNumber;
	
	return pTmpStruct;
}


void DestroyUserFs(struct SUserFsDeviceStruct* a_pUserFs)
{
	int nCurrentOffset;
	pr_notice("DestroyUserFs: a_pUserFs=%p\n",a_pUserFs);
	if(!a_pUserFs){return;}
	
	nCurrentOffset = a_pUserFs->deviceNumber - s_firstDevNumber;
	//s_vectDevices[nCurrentOffset]=NULL;
	pr_notice("DestroyUserFs: nCurrentOffset=%d\n",nCurrentOffset);
	
	if(a_pUserFs->pDevice && (!IS_ERR(a_pUserFs->pDevice))){
		device_destroy(g_class, a_pUserFs->deviceNumber);
		//put_device(s_pDevice);
		//device_unregister(s_pDevice);
		a_pUserFs->pDevice = NULL;
	}
	
	if(a_pUserFs->cdevAdded){
		cdev_del(&a_pUserFs->cdev);
		a_pUserFs->cdevAdded = 0;
	}
	
	kfree(a_pUserFs);
}


int InitFsFunctions(void)
{
	int result;
	dev_t firstdev;
		
	result = alloc_chrdev_region(&firstdev,FIRST_MINOR_NUMBER_FOR_FS,MAX_NUMBER_OF_UFS,USER_FS_DRIVER_NAME);
	if (result < 0) {
		CleanFsFunctions();
		pr_err( "scull: can't get major %d\n",result);
		return 1;
	}
	s_firstDevNumber = firstdev;
	
	//cdev_init(&s_cdev,&s_file_operations);
	//memset(s_vectDevices,0,sizeof(s_vectDevices));
	
	return 0;
}


void CleanFsFunctions(void)
{	
	if(s_firstDevNumber>=0){
		//unregister_chrdev(g_nMajorNumber,USER_FS_DRIVER_NAME);
		unregister_chrdev_region(s_firstDevNumber,MAX_NUMBER_OF_UFS);
		s_firstDevNumber = -1;
	}
}
