#ifndef EEPROM_H
#define EEPROM_H
// д��Ľṹ��
struct i2c_at24_w
{
    unsigned char addr;
    unsigned char wdata[8];
};
// �����Ľṹ��
struct i2c_at24_r
{
    unsigned char addr;
    unsigned char rdata[256];
};


int openEep(const char*);
int readEep(int fd,int devAddr,int regAddr,int num);
int closeEep(int fd);
#endif
