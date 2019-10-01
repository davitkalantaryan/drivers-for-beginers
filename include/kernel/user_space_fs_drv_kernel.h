
#ifndef USER_SPACE_FS_DRV_KERNEL_H
#define USER_SPACE_FS_DRV_KERNEL_H

#include <linux/cdev.h>						// for struct cdev
#include <linux/device.h>					// for struct class
#include <usr/user_space_fs_drv_user.h>

#define USER_FS_DRIVER_NAME		USER_FS_DEVICE_ENTRY_NAME

struct SUserFsDeviceStruct{
	struct cdev			cdev; // todo: better to have one cdev
	struct device*		pDevice;
	dev_t				deviceNumber;
	pid_t				creatorProcess;
	unsigned int		cdevAdded : 1;
};

extern struct class*			g_class;

int							InitFsFunctions(void);
void						CleanFsFunctions(void);
struct SUserFsDeviceStruct*	CreateNewUserFs(const char* a_nEntryName);
void						DestroyUserFs(struct SUserFsDeviceStruct* a_pUserFs);

#endif  // #ifndef USER_SPACE_FS_DRV_KERNEL_H
