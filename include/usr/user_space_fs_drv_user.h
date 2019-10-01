
#ifndef USER_SPACE_FS_DRV_USER_H
#define USER_SPACE_FS_DRV_USER_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

#define USER_FS_FS_DIR_NAME_IN_DIR	"fs"
#define USER_FS_DEVICE_ENTRY_NAME	"user_space_fs"
#define	USER_FS_DEVICE_ENTRY_DIR	"user_space_fs_dir"
#define	USER_FS_DEVICE_ENTRY_PATH	"/dev/"	USER_FS_DEVICE_ENTRY_DIR "/" USER_FS_DEVICE_ENTRY_NAME


#define USER_SPACE_FC_IOC			'0'
#define USER_SPACE_FC_ADD_NEW_IOC	_IOWR(USER_SPACE_FC_IOC, 10, char*)
#define USER_SPACE_FC_REMOVE_IOC	_IO(USER_SPACE_FC_IOC, 11)



#endif  // #ifndef USER_SPACE_FS_DRV_USER_H
