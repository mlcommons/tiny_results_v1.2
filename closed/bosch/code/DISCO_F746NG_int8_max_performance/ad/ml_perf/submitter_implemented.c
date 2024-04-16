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

#include "api/submitter_implemented.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api/internally_implemented.h"

#include "in_out.h"
#include "call.h"
#include "tim.h"

//UnbufferedSerial pc(USBTX, USBRX);


int timestampPin = 0;

int32_t buffer[BUFFER_SIZE_32BIT];
nnInOut in_out;

float input_float[kInputSize];


void init_submitter() {
	in_out = get_nnInOut_for(buffer);
    }

void invoke() {
	nn_call(buffer);

}

void set_input(const int8_t *custom_input) {
	memcpy(in_out.input, custom_input, 640);
}

int8_t *get_output() {
	return in_out.output;
}

float dequantize(const int8_t value) {
	return nn_Identity_dequantize_int8_to_float(value);
}


int8_t quantize(const float value) {
	return nn_input_1_quantize_float_to_int8(value);
}

void th_load_tensor() {
    int8_t input_quantized[kInputSize];
    size_t bytes = ee_get_buffer(
    		(uint8_t *)(input_float),
    		kInputSize * sizeof(float));
    if (bytes / sizeof(float) != kInputSize) {
        th_printf("Input db has %d elements, expected %d\n", bytes / sizeof(float),
                  kInputSize);
        return;
    }
    for (int i = 0; i < kInputSize; i++) {
        input_quantized[i] = quantize(input_float[i]);
    }

    set_input(input_quantized);
}


// Add to this method to return real inference results.
void th_results() {
    int8_t* output = get_output();

    th_printf("m-results-[");

    float diffsum = 0;
    for (size_t i = 0; i < kFeatureElementCount; i++) {
        float converted = dequantize(output[i]);
        float diff = converted - input_float[i];
        diffsum += diff * diff;
    }
    diffsum /= kFeatureElementCount;
    th_printf("%0.3f", diffsum);

    th_printf("]\r\n");

}

// Implement this method with the logic to perform one inference cycle.
void th_infer() { invoke(); }

/// \brief optional API.
void th_final_initialize(void) {

}

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
int th_vprintf(const char *format, va_list ap) { return vprintf(format, ap); }

void th_printf(const char *p_fmt, ...) {
    va_list args;
    va_start(args, p_fmt);
    (void) th_vprintf(p_fmt, args); /* ignore return */
    va_end(args);
}



char th_getchar() { return getchar(); }

void th_serialport_initialize(void) {
#if EE_CFG_ENERGY_MODE == 1
    pc.baud(9600);
#else
    //pc.baud(115200);
#endif
}

void th_timestamp(void) {
#if EE_CFG_ENERGY_MODE == 1
    timestampPin = 0;
    for (int i = 0; i < 100000; ++i) {
      asm("nop");
    }
    timestampPin = 1;
#else
    unsigned long microSeconds = 0ul;
    /* USER CODE 2 BEGIN */
    //microSeconds = us_ticker_read();
    microSeconds = __HAL_TIM_GET_COUNTER(&htim2);
    /* USER CODE 2 END */
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP, microSeconds);
#endif
}

void th_timestamp_initialize(void) {
    /* USER CODE 1 BEGIN */
    // Setting up BOTH perf and energy here
    /* USER CODE 1 END */
    /* This message must NOT be changed. */
    th_printf(EE_MSG_TIMESTAMP_MODE);
    /* Always call the timestamp on initialize so that the open-drain output
       is set to "1" (so that we catch a falling edge) */
    th_timestamp();
}
