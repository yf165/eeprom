#include "eeprom.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define DEVICE "/dev/i2c-0"
#define DEVADDR 0x50


int main(int argc,char **argv)
{
	unsigned char writeBuf[256] ={0};
	unsigned char readBuf[256] = {0};	

	int fd = openEep(DEVICE);
	if (argc == 2){
		memset(writeBuf,0,sizeof(writeBuf));
		memcpy(writeBuf,*(argv+1),strlen(*(argv+1)));
		writeEep(fd,DEVADDR,128,writeBuf, strlen(writeBuf));		
	}

	readEep(fd,DEVADDR,128,64,readBuf);
	printBuf(readBuf,128,64);

	closeEep(fd);
	return 0;
}