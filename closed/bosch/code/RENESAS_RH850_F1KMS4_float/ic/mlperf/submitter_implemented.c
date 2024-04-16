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

#include "submitter_implemented.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internally_implemented.h"

#include "r_device.h"
#include "uart/r_uart.h"
#include "ostm/r_ostm.h"
#include "system/r_system.h"

#include "neural_network/include/in_out.h"
#include "neural_network/include/call.h"

float buffer[BUFFER_SIZE_32BIT];
nnInOut in_out;

void init_submitter(){
    in_out = get_nnInOut_for(buffer);
}
void invoke() {
    nn_call(buffer);
}
void set_input(const float *custom_input) {
	memcpy(in_out.input, custom_input, sizeof(float)*3072);
}
float *get_output() {
	return in_out.output;
}


void th_load_tensor() {
    uint8_t input_quantized[kInputSize];
    float input_asfloat[kInputSize];
    size_t bytes = ee_get_buffer((uint8_t *)(input_quantized),
                                 kInputSize * sizeof(uint8_t));

    if (bytes / sizeof(uint8_t) != kInputSize) {
        th_printf("Input db has %d elements, expected %d\n", bytes / sizeof(uint8_t),
                  kInputSize);
        return;
    }
    for (uint16_t i = 0; i < kInputSize; i++) {
        input_asfloat[i] = (float)input_quantized[i];
    }
    set_input(input_asfloat);
}

void th_results() {
    float *output = get_output();

    th_printf("m-results-[");

    for (int i = 0; i < kNumClasses; i++) {
        th_printf("%0.3f", output[i]);
        if (i < kNumClasses - 1) {
            th_printf(",");
        }
    }
    th_printf("]\r\n");
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() { invoke(); }

void th_final_initialize(void) {}

void th_pre() {}

void th_post() {}

void th_command_ready(char volatile *p_command) {
    p_command = p_command;
    ee_serial_command_parser_callback((char *) p_command);
}

// th_libc implementations.
int th_strncmp(const char *str1, const char *str2, size_t n) {
    return strncmp(str1, str2, n);
}

char *th_strncpy(char *dest, const char *src, size_t n) {
    return strncpy(dest, src, n);
}

size_t th_strnlen(const char *str, size_t maxlen) {
    return strnlen(str, maxlen);
}

char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

int th_atoi(const char *str) { return atoi(str); }

void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

void *th_memcpy(void *dst, const void *src, size_t n) {
    return memcpy(dst, src, n);
}

/* N.B.: Many embedded *printf SDKs do not support all format specifiers. */
int th_vprintf(const char *format, va_list ap) {
    char tmp[1000];
    vsprintf(tmp, format, ap);
    R_UART_SendString(tmp);
    return 0;
}

void th_printf(const char *p_fmt, ...) {
    va_list args;
    va_start(args, p_fmt);
    (void) th_vprintf(p_fmt, args); /* ignore return */
    va_end(args);
}

char th_getchar() { return R_UART_ReceiveChar(); }

void th_serialport_initialize(void) {}

void th_timestamp(void) {
#if EE_CFG_ENERGY_MODE == 1
    timestampPin = 0;
    for (int i = 0; i < 100'000; ++i) {
      asm("nop");
    }
    timestampPin = 1;
#else
    unsigned long microSeconds = 0ul;
    microSeconds = R_OSTM0_GetUS();
    th_printf(EE_MSG_TIMESTAMP, microSeconds);
#endif
}

void th_timestamp_initialize(void) {
    __DI();
    R_OSTM0_Init();
    R_SYSTEM_usDelayInit();
    R_SYSTEM_usDelay(100);
    R_OSTM0_SetCompareValue(60000000);
    R_SYSTEM_usDelay(100);
    __EI();
    R_OSTM0_Start();
    th_printf(EE_MSG_TIMESTAMP_MODE);
    th_timestamp();
}
