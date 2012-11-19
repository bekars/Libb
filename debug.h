/**
 * libx debug implements
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement debug function.
 *
 */

#ifndef __LIBX_DEBUG_H__
#define __LIBX_DEBUG_H__

#define PREX "RFW"

#define STATIC_INLINE static inline

/* global debugging logging facilities */
#ifdef __X_DEBUG__

#define xprintf(X, ARG...) do {\
	                printf("%s:%s(%d): ", PREX, __FUNCTION__, __LINE__);\
	                printf((X), ##ARG);\
	        } while (0)

#define xerror(X, ARG...) do {\
	                printf("!ERR! %s:(%s:%s:%d - %s): ", PREX, __FILE__, __FUNCTION__, __LINE__, strerror(errno));\
	                printf((X), ##ARG);\
			exit(-1);\
	        } while (0)

#define xsyslog(X, ARG...) syslog(LOG_INFO, (X), ##ARG)

#else

#define xprintf(X, ARG...)
#define xerror(X, ARG...) syslog(LOG_ERR, (X), ##ARG)
#define xsyslog(X, ARG...) syslog(LOG_INFO, (X), ##ARG)

#endif /* __X_DEBUG__ */

#define DUMP_BUF(__str, __pbuf, __len, __msg...) do {                   \
	int _i, _j = 0;                                           	\
	printf((__str), ##__msg);             			  	\
	for (_i=0; _i<((__len)/16); ++_i) {                            	\
		printf("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x\n",\
				((unsigned char *)(__pbuf))[_j],     	\
				((unsigned char *)(__pbuf))[_j+1],   	\
				((unsigned char *)(__pbuf))[_j+2],   	\
				((unsigned char *)(__pbuf))[_j+3],   	\
				((unsigned char *)(__pbuf))[_j+4],   	\
				((unsigned char *)(__pbuf))[_j+5],   	\
				((unsigned char *)(__pbuf))[_j+6],   	\
				((unsigned char *)(__pbuf))[_j+7],   	\
				((unsigned char *)(__pbuf))[_j+8],   	\
				((unsigned char *)(__pbuf))[_j+9],   	\
				((unsigned char *)(__pbuf))[_j+10],  	\
				((unsigned char *)(__pbuf))[_j+11],  	\
				((unsigned char *)(__pbuf))[_j+12],  	\
				((unsigned char *)(__pbuf))[_j+13],  	\
				((unsigned char *)(__pbuf))[_j+14],  	\
				((unsigned char *)(__pbuf))[_j+15]); 	\
		_j += 16;                                          	\
	}                                                         	\
	for (_i=0; _i<((__len)%16); ++_i) {                           	\
		printf("%02x ", ((unsigned char *)(__pbuf))[_j++]);  	\
		if ( 7 == _i )                                     	\
		printf("  ");    					\
	}                                                         	\
	if ( __len % 16 ) {                                         	\
		printf("\n");            				\
	}								\
} while (0)

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#define MAC_ADDR(__m) \
	((u8 *)(__m))[0], \
	((u8 *)(__m))[1], \
	((u8 *)(__m))[2], \
	((u8 *)(__m))[3], \
	((u8 *)(__m))[4], \
	((u8 *)(__m))[5]

#define IP_FMT "%d.%d.%d.%d"

#define IP_ADDR(__m) \
	((u8 *)(&__m))[0], \
	((u8 *)(&__m))[1], \
	((u8 *)(&__m))[2], \
	((u8 *)(&__m))[3]

#endif /* __LIBX_DEBUG_H__ */

