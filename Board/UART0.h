#ifndef UART0_H
#define UART0_H
 
#include "sys_conf.h"

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

//add by jgs
#define uart_printf(fmt, ...)								\
	do{														\
		unsigned char tbuf[OUTPUT_BUF];						\
		unsigned char length;								\
		length = sprintf((char*)tbuf, fmt, ## __VA_ARGS__);	\
		if (length <= OUTPUT_BUF)							\
		{													\
		 	uart0SendStr(tbuf, length);						\
		}													\
	}while(0)


#ifdef DEBUG_EN
#define debug_printf	uart_printf
#else
#define debug_printf
#endif

extern uint32_t       SystemFrequency;                                 /* Clock Variable               */ 

void uart0Init (void);
uint8_t uart0GetByte (void);
void uart0GetStr (uint8_t *puiStr, uint32_t ulNum);
void uart0SendByte (uint8_t ucDat);
void uart0SendStr (uint8_t  const *puiStr, uint32_t len);

#endif
