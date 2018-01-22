#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

// 写入的结构体
struct i2c_at24_w
{
    unsigned char addr;
    unsigned char wdata[8];
};
// 读出的结构体
struct i2c_at24_r
{
    unsigned char addr;
    unsigned char rdata[256];
};
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