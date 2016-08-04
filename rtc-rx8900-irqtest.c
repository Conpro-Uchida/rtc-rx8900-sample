/* 
-- rtc-rx8900-irqtest.c  rx8900 interrupt test program
-- Copyright (c) 2016 Syunsuke Okamoto
--
-- This software is released under the MIT License.
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.
-- 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>

typedef unsigned char u8;
typedef unsigned int u16;

// RX-8900 Basic Time and Calendar Register definitions
#define RX8900_BTC_SEC					0x00
#define RX8900_BTC_MIN					0x01
#define RX8900_BTC_HOUR					0x02
#define RX8900_BTC_WEEK					0x03
#define RX8900_BTC_DAY					0x04
#define RX8900_BTC_MONTH				0x05
#define RX8900_BTC_YEAR					0x06
#define RX8900_BTC_RAM					0x07
#define RX8900_BTC_ALARM_MIN			0x08
#define RX8900_BTC_ALARM_HOUR			0x09
#define RX8900_BTC_ALARM_WEEK_OR_DAY	0x0A
#define RX8900_BTC_TIMER_CNT_0			0x0B
#define RX8900_BTC_TIMER_CNT_1			0x0C
#define RX8900_BTC_EXT					0x0D
#define RX8900_BTC_FLAG					0x0E
#define RX8900_BTC_CTRL					0x0F

// RX-8900 Extension Register 1 definitions
#define RX8900_EXT_SEC					0x10
#define RX8900_EXT_MIN					0x11
#define RX8900_EXT_HOUR					0x12
#define RX8900_EXT_WEEK					0x13
#define RX8900_EXT_DAY					0x14
#define RX8900_EXT_MONTH				0x15
#define RX8900_EXT_YEAR					0x16
#define RX8900_EXT_TEMP					0x17
#define RX8900_EXT_BACKUP				0x18

#define RX8900_EXT_TIMER_CNT_0			0x1B
#define RX8900_EXT_TIMER_CNT_1			0x1C
#define RX8900_EXT_EXT					0x1D
#define RX8900_EXT_FLAG					0x1E
#define RX8900_EXT_CTRL					0x1F


// Flag RX8900_BTC_EXT Register bit positions
#define RX8900_BTC_EXT_TSEL0		(1 << 0)
#define RX8900_BTC_EXT_TSEL1		(1 << 1)
#define RX8900_BTC_EXT_FSEL0		(1 << 2)
#define RX8900_BTC_EXT_FSEL1		(1 << 3) 
#define RX8900_BTC_EXT_TE 			(1 << 4)
#define RX8900_BTC_EXT_USEL			(1 << 5) 
#define RX8900_BTC_EXT_WADA			(1 << 6)
#define RX8900_BTC_EXT_TEST			(1 << 7)

// Flag RX8900_BTC_FLAG Register bit positions
#define RX8900_BTC_FLAG_VDET 		(1 << 0)
#define RX8900_BTC_FLAG_VLF 		(1 << 1)

#define RX8900_BTC_FLAG_AF 			(1 << 3)
#define RX8900_BTC_FLAG_TF 			(1 << 4)
#define RX8900_BTC_FLAG_UF 			(1 << 5)

// Flag RX8900_BTC_FLAG Register bit positions
#define RX8900_BTC_CTRL_RESET 		(1 << 0)


#define RX8900_BTC_CTRL_AIE 		(1 << 3)
#define RX8900_BTC_CTRL_TIE 		(1 << 4)
#define RX8900_BTC_CTRL_UIE 		(1 << 5)
#define RX8900_BTC_CTRL_CSEL0 		(1 << 6)
#define RX8900_BTC_CTRL_CSEL1		(1 << 7)

typedef struct {
	u8 number;
	u8 value;
}reg_data;

#define SE_RTC_REG_READ		_IOWR('p', 0x20, reg_data)		
#define SE_RTC_REG_WRITE	_IOW('p',  0x21, reg_data)	

// 2016.06.07 Init Battery Set/Unset Check
#define SE_RTC_REG_READ_BATTERY_SETTING		_IOR('p', 0x22, reg_data) // VLF Read

#if DEBUG
#define rx8900_dbg_print(fmt...)	printf(fmt)	
#else
#define rx8900_dbg_print(fmt...)	
#endif

/**
	@param fd : file dispcriptor
	@param addr : RX8900 Address
	@param value : value
	@return io control 
**/
int rx8900_read( int fd, u8 addr, u8 *value )
{ 
	int iRet;
	reg_data reg;

	reg.number = addr;
	iRet = ioctl( fd, SE_RTC_REG_READ, &reg );
	*value = reg.value;
	rx8900_dbg_print( "rx8900 read : Addr %x , value %x\n", addr, *value );
	return iRet;
}

/**
	@param fd : file dispcriptor
	@param addr : RX8900 Address
	@param value : value
	@return io control 
**/
int rx8900_write( int fd, u8 addr, u8 value )
{ 
	int iRet;
	reg_data reg;

	reg.number = addr;
	reg.value = value;

	iRet = ioctl( fd, SE_RTC_REG_WRITE, &reg );
	
	rx8900_dbg_print( "rx8900 write : Addr %x , value %x\n", addr, value );

	return iRet;
}

int rx8900_set_time_value( int fd, u16 value )
{
	int iRet;

	rx8900_write( fd, RX8900_BTC_TIMER_CNT_0, (u8)(value % 256 ) );
	rx8900_write( fd, RX8900_BTC_TIMER_CNT_1, (u8)(value / 256 ) );
	
	return 0;
}

int main( int argc, char* argv[] )
{
	u16 isWrite;
	u16 tim;
	u8 tim_mode;

	int fd;
	u8 tmp;

	if( argc < 2 ){
		printf("rtc-rx8900-irqtest Ver.1.0.0 \n");
		printf("timer interrupt test program \n\n");
		printf("  ./rtc-rx8900-irqtest 0 :\n");
		printf("      timer interrupt disable \n");
		printf("  ./rtc-rx8900-irqtest 1 sec 10\n");
		printf("      timer interrupt enable 10 seconds \n");
		printf("  ./rtc-rx8900-irqtest 1 min 2\n");
		printf("      timer interrupt enable 2 minutes.\n");				 
		return 0;
	}

	sscanf(argv[1], "%d", &isWrite );

	if( isWrite ){
		if( argc == 4 ){
			if( strcmp(argv[2], "sec" ) == 0 ){
				tim_mode = 2;	// second update
			}
			else if( strcmp(argv[2], "min" ) == 0 ){
				tim_mode = 3;	// minute update
			}else{
				printf("error parameter.\n");
				return 0;
			}
			sscanf(argv[3], "%d", &tim );	
		}else{
			printf("error parameter.\n");
			return 0;
		}
	}

	fd = open("/dev/rtc0",O_RDWR);

	if( fd < 0 ){
		printf("rtc0 cannot open.\n");
		return 1;
	}

	if( isWrite ){
		rx8900_read(fd, RX8900_BTC_EXT, &tmp );
		tmp = (tmp & 0xEC) | (tim_mode & 0x03 );
		rx8900_write( fd, RX8900_BTC_EXT, tmp );

		rx8900_set_time_value( fd, tim );

		rx8900_write( fd, RX8900_BTC_CTRL, RX8900_BTC_CTRL_TIE );

		rx8900_write ( fd, RX8900_BTC_EXT, (tmp | RX8900_BTC_EXT_TE ) );

	}else{
		rx8900_read(fd, RX8900_BTC_EXT, &tmp );
		rx8900_write ( fd, RX8900_BTC_EXT, (tmp & ~RX8900_BTC_EXT_TE ) );
	}

	close(fd);

	return 0;
}
