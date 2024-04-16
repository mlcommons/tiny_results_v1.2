/**
This file was generated by
   _   _      _      _       _____
  | | | |    / \    | |     | ____|
  | |_| |   / _ \   | |     |  _|
  |  _  |  / ___ \  | |___  | |___
  |_| |_| /_/   \_\ |_____| |_____|
version: 1.0.174
Model: kws_ref_model.tflite
Target: CortexM4
Timestamp: 2024_03_05_18.53.10
SPDX-FileCopyrightText: Copyright © 2023 Robert Bosch GmbH
SPDX-License-Identifier: LicenseRef-Bosch-Proprietary
**/
#ifndef CALL_H
#define CALL_H
#ifdef __cplusplus
extern "C"{
#endif
#include "in_out.h"

#include <stdint.h>

/** 
 * Call the neural network 'nn'
 * \param buffer Memory buffer to use for the neural network call
 */
void nn_call(int32_t buffer[BUFFER_SIZE_32BIT]);

#ifdef __cplusplus
}
#endif
#endif // CALL_H
