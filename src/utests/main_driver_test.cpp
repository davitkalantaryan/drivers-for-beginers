
#include <fcntl.h>
#include <stdio.h>

#define DEVICE_ENTRY_NAME	"/dev/basic_file_ops_test"


int main()
{
	int fd = open(DEVICE_ENTRY_NAME,O_RDWR);
	
	if(fd<0){
		fprintf(stderr,"Unable to open the file \"%s\"\n",DEVICE_ENTRY_NAME);
		perror("\n");
		return 1;
	}
	
	
	return 0;
}
