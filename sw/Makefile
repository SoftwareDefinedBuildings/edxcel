LIBS  = 
CFLAGS = -Wall -I../verilog/c_ref/src/ -I.
#GCC = arm-xilinx-linux-gnueabi-gcc
GCC = arm-linux-gnueabi-gcc
SRC=$(wildcard *.c ../verilog/c_ref/src/*.c)

run: bins

upload: bins
	cp edxcel /tftpboot
	
bins: $(SRC)
	$(GCC) -o edxcel $^ $(CFLAGS) $(LIBS)
