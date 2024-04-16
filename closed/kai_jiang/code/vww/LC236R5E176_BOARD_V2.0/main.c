#include <stdint.h>
#include <soc.h>
#include <stdio.h>
#include <string.h>
#include <drv_usart.h>
#include <board_init.h>
#include <bsp_common.h>
#include "drv_pmu.h"
#include "pmu.h"
#include "internally_implemented.h"

extern usart_handle_t console_handle;
extern int quant_mbv1_int8(void);
extern int quant_resnet_uint8(void);
uint64_t g_time_start = 0;
uint32_t syscalc = 1000;
#define CH_LEN 2048
char g_recvbuf[CH_LEN];

uint64_t g_microSeconds_start;
uint64_t g_microSeconds_end;


void my_memcpy(void * to, void  * from, size_t size)
{
    int32_t *dest = (int *)to;
	int32_t *src = (int *)from;
	int32_t word_num = (size>>2) - 1;
	int32_t slice = size%4 - 1;
	while(word_num >= 0)
	{
		*dest++ = *src++;
		word_num--;
	}
	while(slice >= 0)
	{
		*((char *)dest + slice) = *((char *)src + slice);
		slice--;
	}
}

uint64_t get_current_clock(void)
{
	uint64_t ntime;
	
	ntime = SysTimer_GetLoadValue();
	if(ntime > g_time_start)
	{
		return((ntime - g_time_start)/syscalc);
	}
	return 0;
}
void readdata(void)
{
	uint32_t recv_len = 0;
	uint32_t r_pos = csi_usart_get_read_pos(console_handle);
	uint32_t w_pos = csi_usart_get_write_pos(console_handle);
		
	while(r_pos != w_pos)
	{
		ee_serial_callback(g_recvbuf[r_pos]);
		r_pos = ((r_pos < CH_LEN - 1) ? (r_pos + 1): 0);
		recv_len++;
	}
	csi_usart_set_read_pos(console_handle, r_pos);
}

int main(void)
{
	pmu_handle_t handle;
	mdelay(100);
	handle = csi_pmu_initialize(0, NULL);
	csi_sysclock_switchCLK(handle, PMU_PLL_INPUT_EHS, 9.2, false);
	csi_set_apb_clockdiv_mux3(handle,PMU_APB_DIV2,SYS_SW3); 
	mdelay(100);
	console_handle = csi_usart_initialize(3, usart_event_callback);
    csi_usart_config(console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
    csi_usart_receive(console_handle, g_recvbuf, sizeof(g_recvbuf));
	
	syscalc = drv_get_sys_freq()/1000000;//us
	g_time_start = SysTimer_GetLoadValue();
	ee_benchmark_initialize();
   
	while(1)
	{
		readdata();
	}
	
    return 0;
}