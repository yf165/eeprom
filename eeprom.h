#ifndef EEPROM_H
#define EEPROM_H
#define MAXWRITENUMONCE 4
#define INTERVALOFBLOCKWR 200*1000 //us
// 写入的结构体
struct i2c_at24_w
{
    unsigned char addr;
    unsigned char wdata[MAXWRITENUMONCE];
};
// 读出的结构体
struct i2c_at24_r
{
    unsigned char addr;
    unsigned char rdata[256];
};


int openEep(const char*);
int readEep(int fd,int devAddr,int regAddr,int num,char* readBuf);
int writeEep(int fd,int devAddr,int regAddr,const unsigned char *writeContent, int num);
int printBuf(const char *buf,int regAddr,int num);
int closeEep(int fd);
#endif
