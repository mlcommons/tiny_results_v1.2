/*
Copyright (C) EEMBC(R). All Rights Reserved
All EEMBC Benchmark Software are products of EEMBC and are provided under the
terms of the EEMBC Benchmark License Agreements. The EEMBC Benchmark Software
are proprietary intellectual properties of EEMBC and its Members and is
protected under all applicable laws, including all applicable copyright laws.
If you received this EEMBC Benchmark Software without having a currently
effective EEMBC Benchmark License Agreement, you must discontinue use.
Copyright 2020 The MLPerf Authors. All Rights Reserved.
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
/// \brief C implementations of submitter_implemented.h

#include "api/submitter_implemented.h"
#include "api/internally_implemented.h"
#include "onnc_main.h"
#include "micro_model_setting.h"

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

#include <time.h>
#include <stdio.h>
#include <termios.h>

typedef struct {
  void *in_buf;
  size_t in_bytes;
  void *out_buf;
  size_t out_bytes;
} InOutDescriptor;


static int8_t g_result[kOutputSize];
static int8_t input_data[kInputSize];
InOutDescriptor in_out_desc;

void load_tensor() {
  // copy data from gp_buff into input_float
  ee_get_buffer(reinterpret_cast<uint8_t *>(in_out_desc.in_buf),
                kInputSize * sizeof(uint8_t));
}

void results() {
  th_printf("m-results-[");
  size_t tmp = (12 < in_out_desc.out_bytes) ? 12:in_out_desc.out_bytes;
  for (size_t i = 0; i < tmp; ++i) {
    th_printf("%d", static_cast<int8_t *>(in_out_desc.out_buf)[i]);
    if (i != (tmp-1))
      th_printf(",");
  }
  th_printf("]\r\n");
}

void infer() {
  onnc_input_tensor_t in_buf = onnc_get_input_tensor();
  memcpy(in_buf.data, input_data, in_buf.size);
  onnc_output_tensor_t result = onnc_main();
  memcpy(g_result, result.data, result.size);
}

void final_initialize() {
  onnc_input_tensor_t in_buf = onnc_get_input_tensor();
  onnc_output_tensor_t out_buf = onnc_main();
  // Setup the descriptor for the testbench.
  in_out_desc.in_buf = input_data;
  in_out_desc.in_bytes = kInputSize;
  in_out_desc.out_buf = g_result;
  in_out_desc.out_bytes = out_buf.size;
}

#define UART_DEFAULT_BAUDRATE  115200
static const struct device* g_uart;

// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  load_tensor();
}


// Add to this method to return real inference results.
void th_results() {
  results();
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() {
  infer();
}

/// \brief optional API.
void th_final_initialize(void) {
  final_initialize();
}

void th_pre() {}
void th_post() {}

void th_command_ready(char volatile* p_command) {
  p_command = p_command;
  ee_serial_command_parser_callback((char*)p_command);
}

// th_libc implementations.
int th_strncmp(const char* str1, const char* str2, size_t n) { return strncmp(str1, str2, n); }

char* th_strncpy(char* dest, const char* src, size_t n) { return strncpy(dest, src, n); }

size_t th_strnlen(const char* str, size_t maxlen) { return strlen(str); }

char* th_strcat(char* dest, const char* src) { return strcat(dest, src); }

char* th_strtok(char* str1, const char* sep) { return strtok(str1, sep); }

int th_atoi(const char* str) { return atoi(str); }

void* th_memset(void* b, int c, size_t len) { return memset(b, c, len); }

void* th_memcpy(void* dst, const void* src, size_t n) { return memcpy(dst, src, n); }

/* N.B.: Many embedded *printf SDKs do not support all format specifiers. */
int th_vprintf(const char* format, va_list ap) { return vprintf(format, ap); }


void th_printf(const char* p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  th_vprintf(p_fmt, args);
  va_end(args);
}

char th_getchar() {
  return getchar();
}

void th_serialport_initialize(void) {
    int c;
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void th_timestamp(void) {
  /* USER CODE 2 BEGIN */
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  uint64_t microSeconds = tv.tv_sec*(uint64_t)1000000+tv.tv_nsec/1000;
  /* USER CODE 2 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP, microSeconds);
}

void th_timestamp_initialize(void) {
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP_MODE);
  /* Always call the timestamp on initialize so that the open-drain output
     is set to "1" (so that we catch a falling edge) */
  th_timestamp();
}
