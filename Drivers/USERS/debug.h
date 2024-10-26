#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_BUF_MAX_LENGTH 128
#define USE_DMA_THOL 16

void lv_log_binding_uart(void);
void U_LOG(const char * fmt, ...);

	
#ifdef __cplusplus
}
#endif	
#endif
