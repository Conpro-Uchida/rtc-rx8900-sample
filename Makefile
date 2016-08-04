#self build 
#GCC=gcc
#cross build
GCC=arm-linux-gnueabi-gcc

#EXTRA_CFLAGS	:= -DDEBUG=0  

SAMPLE_NAME1=rtc-rx8900-irqtest
SAMPLE_NAME2=rtc-rx8900-getBatteryCheck
SAMPLE_NAME3=rtc-rx8900-getTemperature

all: $(SAMPLE_NAME1).o $(SAMPLE_NAME2).o $(SAMPLE_NAME3).o

$(SAMPLE_NAME1).o :
	$(GCC) $(EXTRA_CFLAGS) -o  $(SAMPLE_NAME1) $(SAMPLE_NAME1).c
$(SAMPLE_NAME2).o :
	$(GCC) $(EXTRA_CFLAGS) -o  $(SAMPLE_NAME2) $(SAMPLE_NAME2).c
$(SAMPLE_NAME3).o :
	$(GCC) $(EXTRA_CFLAGS) -o  $(SAMPLE_NAME3) $(SAMPLE_NAME3).c
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions vtty Module.symvers modules.order 
