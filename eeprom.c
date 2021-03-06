#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "eeprom.h"

int openEep(const char* device)
{
	int fd =open(device, O_RDWR);
    if (fd< 0) {
           perror("open");
    }
	return fd;
}
int readEep(int fd,int devAddr,int regAddr,int num,char* readBuf)
{
	int i = 0,j = 0;
	char c = 0;
	int ret = -1;
	struct i2c_msg  msg;
    struct i2c_at24_r rd = {0};
    struct i2c_rdwr_ioctl_data ioctl_data;
    struct i2c_msg msgs;

	ioctl_data.nmsgs= 1;
    ioctl_data.msgs= &msgs;

    // 写入要读的地址
    msgs.addr  = devAddr;
    msgs.flags = 0;
	rd.addr = regAddr;
    msgs.len   = sizeof(rd.addr);
    msgs.buf   = (unsigned char*)&rd.addr;
	usleep(INTERVALOFBLOCKWR);	
	ret = ioctl(fd, I2C_RDWR, &ioctl_data);
//	printf("ioctl write start register address fot read 0x%x, return :%d\n",regAddr,ret );
	if(ret < 0 ){
		perror("ioctl");
		return -1;
	}	
	usleep(INTERVALOFBLOCKWR);	
    // 连续读取num个byte
    msgs.addr   = devAddr;
    msgs.flags |= I2C_M_RD;
//    msgs.len    = sizeof(rd.rdata);
	msgs.len = num;
    msgs.buf    = (unsigned char*)&rd.rdata[0];
	ret = ioctl(fd, I2C_RDWR, &ioctl_data);
	if(ret < 0 ){
		perror("ioctl");
		return -1;
	}		
//    printf("ioctl read, return :%d\n", ioctl(fd, I2C_RDWR, &ioctl_data));
	memccpy(readBuf,ioctl_data.msgs->buf,num);
#if 0
	printf("    ");
    for ( i = 0; i < 16;i++) {
		printf("%2x ",i);	
	}
	printf("\n");	
    for ( i = 0; i < num/16;i++) {
		printf("%2x: ",i+regAddr/16);
		for (j = 0;j<16; j++){
			printf("%2x ",*((ioctl_data.msgs->buf)+i*16+j));			
		}
		printf(" ");
		for (j = 0;j<16; j++){
			c = *((ioctl_data.msgs->buf)+i*16+j);
			if((c == 0xd) || (c == 0xa)) 
			{
				printf(" ");
			}else{
				printf("%c ",c);					
			}	
		}
		printf("\n");
    }
	printf("\n");	
#endif
	return 0;
}
int printBuf(const char *buf,int regAddr,int num)
{	
	int i = 0,j = 0;
	char c = 0;
	printf("    ");
    for ( i = 0; i < 16;i++) {
		printf("%2x ",i);	
	}
	printf("\n");	
    for ( i = 0; i < num/16;i++) {
		printf("%2x: ",i+regAddr/16);
		for (j = 0;j<16; j++){
			printf("%2x ",*(buf+i*16+j));			
		}
		printf(" ");
		for (j = 0;j<16; j++){
			c = *(buf+i*16+j);
			if((c == 0xd) || (c == 0xa)) 
			{
				printf(" ");
			}else{
				printf("%c ",c);					
			}	
		}
		printf("\n");
    }
	printf("\n");		
}
int writeEep(int fd,int devAddr,int regAddr,const unsigned char *writeContent, int num)
{
	int i = 0,j = 0;
	int cnt =0;
	int ret = -1;
    struct i2c_msg  msg;
    struct i2c_at24_w wd = {0};
    struct i2c_rdwr_ioctl_data ioctl_data;
    struct i2c_msg msgs;

    // 要写入的消息
    ioctl_data.nmsgs= 1;
    ioctl_data.msgs= &msgs;

	i = 0;
	usleep(INTERVALOFBLOCKWR);	
	do{
		if(i==(num+MAXWRITENUMONCE-1)/MAXWRITENUMONCE-1){
			cnt = num - i*4;
		}else{
			cnt = MAXWRITENUMONCE;
		}
		for ( j = 0; j < cnt;j++) {
			wd.wdata[j] = *(writeContent+i*MAXWRITENUMONCE+j);
		}
		wd.addr    = regAddr+i*MAXWRITENUMONCE;
		msgs.addr  = 0x50;
		msgs.flags = 0;
		msgs.len   = sizeof(wd.addr)+cnt;
		msgs.buf   = (unsigned char*)&wd;
		ret = ioctl(fd, I2C_RDWR, &ioctl_data);
		if(ret < 0){
			perror("ioctl");
			return -1;
		}
//		printf("ioctl write addr %x, return :%d\n",wd.addr ,ioctl(fd, I2C_RDWR, &ioctl_data));
		usleep(INTERVALOFBLOCKWR);	
		i++;		
	}while(i<(num+MAXWRITENUMONCE-1)/MAXWRITENUMONCE);

}
int closeEep(int fd)
{
	close(fd);	
}

#if 0
int main()
{
	int i = 0,j = 0;
	char c = 0;
    int fd =open("/dev/i2c-0", O_RDWR);
    if (fd< 0) {
           printf("open /dev/i2c-0 failed\n");
           goto exit;
    }

    struct i2c_msg  msg;
    struct i2c_at24_r rd = {0};
    struct i2c_at24_w wd = {0};


    struct i2c_rdwr_ioctl_data ioctl_data;
    struct i2c_msg msgs;

    // 要写入的消息
    ioctl_data.nmsgs= 1;
    ioctl_data.msgs= &msgs;

    // 0地址写入8Byte 0x33，AT24C02一次最多能写入8byte
    for ( i = 0; i < 8;i++) {
        wd.wdata[i] = 0x33;
    }

    wd.addr    = 0x00;
    msgs.addr  = 0x50;
    msgs.flags = 0;
    msgs.len   = sizeof(struct i2c_at24_w);
    msgs.buf   = (unsigned char*)&wd;
 //   printf("ioctl write addr 0, return :%d\n", ioctl(fd, I2C_RDWR, &ioctl_data));
	ioctl_data.nmsgs= 1;
    ioctl_data.msgs= &msgs;

    // 写入要读的地址
    msgs.addr  = 0x50;
    msgs.flags = 0;
    msgs.len   = sizeof(rd.addr);
    msgs.buf   = (unsigned char*)&rd.addr;
	printf("ioctl write address, return :%d\n", ioctl(fd, I2C_RDWR, &ioctl_data));
    // 连续读取128byte
    msgs.addr   = 0x50;
    msgs.flags |= I2C_M_RD;
    msgs.len    = sizeof(rd.rdata);
    msgs.buf    = (unsigned char*)&rd.rdata[0];
    printf("ioctl read, return :%d\n", ioctl(fd, I2C_RDWR, &ioctl_data));
	printf("    ");
    for ( i = 0; i < 16;i++) {
		printf("%2x ",i);	
	}
	printf("\n");	
    for ( i = 0; i < 16;i++) {
		printf("%2x: ",i);
		for (j = 0;j<16; j++){
			printf("%2x ",*((ioctl_data.msgs->buf)+i*16+j));			
		}
		printf(" ");
		for (j = 0;j<16; j++){
			c = *((ioctl_data.msgs->buf)+i*16+j);
			if((c == 0xd) || (c == 0xa)) 
			{
				printf(" ");
			}else{
				printf("%c ",c);					
			}	
		}
		printf("\n");
//		printf("0x%x  %c \n",*((ioctl_data.msgs->buf)+i),*((ioctl_data.msgs->buf)+i));

    }
	printf("\n");
 exit:
	close(fd);
}
#endif
