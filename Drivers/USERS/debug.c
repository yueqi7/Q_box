#include "debug.h"
#include "main.h"
#include "lvgl.h"


void lv_log_binding_uart(void)
{
	lv_log_register_print_cb(U_LOG);
}

void U_LOG(const char * fmt, ...)
{
    char out_buf[PRINT_BUF_MAX_LENGTH]={0};
	int bufer_len, time_len;
	va_list ap;
    
	time_len = sprintf(out_buf, "[%d]", HAL_GetTick());

	va_start(ap, fmt);
	bufer_len = vsnprintf(out_buf + time_len, PRINT_BUF_MAX_LENGTH - time_len, fmt, ap);
	va_end(ap);
    out_buf[time_len + bufer_len] = '\r';
    out_buf[time_len + bufer_len +1] = '\n';
	HAL_UART_Transmit(&debug_uart, (const uint8_t *)out_buf, strlen(out_buf), 10);
}
