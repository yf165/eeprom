obj := eeprom

all:
	arm-none-linux-gnueabi-gcc $(obj).c -o $(obj).out

cp:
	cp $(obj).out /root/porting_x210/rootfs/rootfs/test/driver_test -f
.PHONY: clean	
clean:
	rm -rf $(obj).out