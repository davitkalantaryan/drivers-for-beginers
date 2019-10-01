
#include <fcntl.h>
#include <stdio.h>
#include <user_space_fs_drv_user.h>
#include <sys/ioctl.h>
#include <unistd.h>

//#define DEVICE_ENTRY_NAME	"/dev/basic_file_ops_test"


int main()
{
	int fd = open(USER_FS_DEVICE_ENTRY_PATH,O_RDWR);
	
	if(fd<0){
		fprintf(stderr,"Unable to open the file \"%s\"\n",USER_FS_DEVICE_ENTRY_PATH);
		perror("\n");
		return 1;
	}
	
	ioctl(fd,USER_SPACE_FC_ADD_NEW_IOC,"Hi");
	ioctl(fd,USER_SPACE_FC_REMOVE_IOC);
	
	close(fd);
	
	
	return 0;
}
