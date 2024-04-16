/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. The reporting logic
in th_results is copied from the original in EEMBC.
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h

#include "hal_data.h"

#include "api/submitter_implemented.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "api/internally_implemented.h"


// Extern app function api
extern bool app_setup();
extern void app_load_tensor(size_t (*get_buffer_func)(uint8_t* input, size_t size));
extern void app_infer();
extern float* app_get_results(uint32_t* output_size);

#define MAX_TX_BUFFER   (256)
char tx_buffer[MAX_TX_BUFFER];

#define TRANSFER_LENGTH (128)

uint8_t  g_dest[TRANSFER_LENGTH];
uint8_t  g_src[TRANSFER_LENGTH];
uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
uint32_t g_transfer_complete = 0;
uint32_t g_receive_complete  = 0;
uint32_t g_out_of_band_index = 0;

uint32_t rx_read_pos = 0;

uint64_t g_capture_overflows = 0U;



// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
    app_load_tensor(ee_get_buffer); 
}

// Add to this method to return real inference results.
void th_results() {
    uint32_t output_size;
    float* results = app_get_results(&output_size);
    th_printf("m-results-[");
    for (size_t i = 0; i < output_size; i++) {
	th_printf("%.3f", results[i]);
	if (i < (output_size - 1)) th_printf(",");
    }
    th_printf("]\r\n");
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() {
    app_infer();
}

// brief optional API.
void th_final_initialize(void) {
    bool rst = app_setup();
}
void th_pre() {}
void th_post() {}

void th_command_ready(char volatile *p_command) {
    p_command = p_command;
    ee_serial_command_parser_callback((char *)p_command);
}

// th_libc implementations.
int th_strncmp(const char *str1, const char *str2, size_t n) {
    return strncmp(str1, str2, n);
}

char *th_strncpy(char *dest, const char *src, size_t n) {
    return strncpy(dest, src, n);
}

size_t th_strnlen(const char * s, size_t maxlen) {     
    size_t i = 0;     
    for (; i < maxlen && s[i] != '\0'; ++i);     
    return i; 
}

char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

int th_atoi(const char *str) { return atoi(str); }

void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

void *th_memcpy(void *dst, const void *src, size_t n) {
    return memcpy(dst, src, n);
}

/* N.B.: Many embedded *printf SDKs do not support all format specifiers. */
int th_vprintf(const char *format, va_list ap)
{
    vsprintf(tx_buffer, format, ap);
    fsp_err_t err = R_SCI_B_UART_Write(&g_uart_ctrl, (uint8_t *)tx_buffer, strnlen(tx_buffer, MAX_TX_BUFFER));
    assert(FSP_SUCCESS == err);
    return 0;
}

void th_printf(const char *p_fmt, ...) {
    va_list args;


    while( g_transfer_complete  != 0)       // Wait for message send if need
    {
        __asm("nop");
    }

    va_start(args, p_fmt);
    g_transfer_complete  = 1;               // Message sending flag.
    (void)th_vprintf(p_fmt, args); /* ignore return */
    va_end(args);
}

void th_serialport_initialize(void){
#if EE_CFG_ENERGY_MODE == 1
/* In energy mode, we talk to the DUT through the IO Manager at 9600 baud */
    g_uart7_baud_setting.cks = 1;
    g_uart7_baud_setting.brr = 161;

    R_SCI_UART_Open(&g_uart7_ctrl, &g_uart7_cfg);

#else
/* In performance mode, we talk directly to the DUT at 115200 baud */
    R_SCI_B_UART_Open(&g_uart_ctrl, &g_uart_cfg);

#endif
}

#if 0
void reset_and_start_timer(){
    profile.Reset();
    profile.StartProfiling();
}
#endif

void th_timestamp(void) {
    # if EE_CFG_ENERGY_MODE==1
    // TODO
    #else
    uint64_t micro_seconds;
    timer_status_t status;
    R_GPT_StatusGet(&g_timer_ctrl, &status);

    micro_seconds = status.counter + UINT32_MAX * g_capture_overflows;

    micro_seconds = ((uint64_t)micro_seconds * 1000 * 1000) / BSP_STARTUP_PCLKD_HZ;



    th_printf(EE_MSG_TIMESTAMP, micro_seconds);
    #endif
}

void th_timestamp_initialize(void) {

    /* Initializes the module. */
    R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    /* Start the timer. */
    R_GPT_Start(&g_timer_ctrl);

    th_printf(EE_MSG_TIMESTAMP_MODE);
    th_timestamp();
}



void g_uart_callback (uart_callback_args_t * p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if (sizeof(g_out_of_band_received) > g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if (UART_DATA_BITS_8 >= g_uart_cfg.data_bits)
                {
                    g_out_of_band_received[g_out_of_band_index++] = (uint8_t) p_args->data;
                }
                else
                {
                    uint16_t * p_dest = (uint16_t *) &g_out_of_band_received[g_out_of_band_index];
                    *p_dest              = (uint16_t) p_args->data;
                    g_out_of_band_index += 2;
                }
                if( g_out_of_band_index >=TRANSFER_LENGTH)
                {
                    g_out_of_band_index = 0;
                }
            }
            break;
        }
        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_complete = 1;
            break;
        }
        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_complete = 0;
            break;
        }
        default:
        {
        }
    }
}


extern "C" uint8_t th_getchar(void)
{
    uint8_t ch = 0;

    if( g_out_of_band_index != rx_read_pos )
    {
        ch = g_out_of_band_received[rx_read_pos++];
    }

    if( TRANSFER_LENGTH <= rx_read_pos)
    {
        rx_read_pos = 0;
    }
    return ch;
}

void g_timer_overflow_callback (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* An overflow occurred during capture. This must be accounted for at the application layer. */
        g_capture_overflows++;
    }
}


