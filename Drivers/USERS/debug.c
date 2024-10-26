#include "debug.h"
#include "main.h"
#include "lvgl.h"
#include "cmsis_os.h"

volatile bool Log_Transfer_Fished = true;
char out_buf[LOG_BUF_MAX_LENGTH]={0};
char rem_cnt = 0;
short dma_cnt = 0;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (&debug_uart == huart)
	{
        HAL_UART_DMAStop(huart);
        if (rem_cnt)
        {
            HAL_UART_Transmit(&debug_uart, (const uint8_t *)out_buf+dma_cnt, rem_cnt, 10);
        }
		Log_Transfer_Fished = true;
	}
}

void lv_log_binding_uart(void)
{
	lv_log_register_print_cb(U_LOG);
}

void U_LOG(const char * fmt, ...)
{
	int bufer_len, time_len;
	va_list ap;

	while(!Log_Transfer_Fished);
    memset(out_buf, 0, LOG_BUF_MAX_LENGTH);
	time_len = sprintf(out_buf, "[%d]", HAL_GetTick());

	va_start(ap, fmt);
	bufer_len = vsnprintf(out_buf + time_len, LOG_BUF_MAX_LENGTH - time_len, fmt, ap);
	va_end(ap);
    out_buf[time_len + bufer_len] = '\r';
    out_buf[time_len + bufer_len +1] = '\n';
    rem_cnt = strlen(out_buf)%USE_DMA_THOL;
	dma_cnt = strlen(out_buf) - rem_cnt;
	if (dma_cnt)
	{
        Log_Transfer_Fished = false;
		HAL_UART_Transmit_DMA(&debug_uart, (const uint8_t *)out_buf, dma_cnt);
	}
	else
	{
		HAL_UART_Transmit(&debug_uart, (const uint8_t *)out_buf, rem_cnt, 10);
        Log_Transfer_Fished = true;
	}
}
