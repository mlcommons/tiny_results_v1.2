/**
This file was generated by
   _   _      _      _       _____
  | | | |    / \    | |     | ____|
  | |_| |   / _ \   | |     |  _|
  |  _  |  / ___ \  | |___  | |___
  |_| |_| /_/   \_\ |_____| |_____|
version: 1.0.174
Model: vww_96_int8.tflite
Target: CortexM4
Timestamp: 2024_03_05_18.53.51
SPDX-FileCopyrightText: Copyright © 2023 Robert Bosch GmbH
SPDX-License-Identifier: LicenseRef-Bosch-Proprietary
**/
#ifndef IN_OUT_H
#define IN_OUT_H

#include <math.h>
#include <stdint.h>


// Buffer size (4 byte for each element)
#define BUFFER_SIZE_32BIT 13824


// Input sizes
#define INPUT_1_INT8_NUM_ELEMS 27648

// Output sizes
#define IDENTITY_INT8_NUM_ELEMS 2


typedef struct {
    int8_t* input_1_int8;
    int8_t* Identity_int8;

    int8_t* input; // -> input_1_int8
    int8_t* output; // -> Identity_int8
} nnInOut;

/** 
 * Creates a nnInOut instance for the neural network nn.
 * This sets the pointers in the struct defined above to the correct places in the buffer
 * \param buffer Buffer that contains the memory the neural network operates on
 * return Struct containing pointers to the network input and output
 */
nnInOut get_nnInOut_for(int32_t* buffer);

/** 
 * Quantizes a single value from input input_1_int8 of network nn
 * param value Value to quantize
 * return Quantized value
 */
int8_t nn_input_1_int8_quantize_float_to_int8(float value);

/** 
 * Dequantizes a single value from output Identity_int8 of network nn
 * /param value Value to dequantize
 * return Dequantized value
 */
float nn_Identity_int8_dequantize_int8_to_float(int8_t value);

#endif // IN_OUT_H
