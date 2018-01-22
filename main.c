#include "eeprom.h"

#define DEVICE "/dev/i2c-0"
#define DEVADDR 0x50

int main()
{
	int fd = openEep(DEVICE);
//	readEep(fd,DEVADDR,0,256);
	readEep(fd,DEVADDR,128,64);
	closeEep(fd);
}