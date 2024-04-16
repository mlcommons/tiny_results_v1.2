/******************************************************************************
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.*
 * Copyright (C) 2018-2022 Andes Technology Corporation. All rights reserved. *
 *                                                                            *
 * SPDX-License-Identifier: Apache-2.0                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the License); you may      *
 * not use this file except in compliance with the License.                   *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 * www.apache.org/licenses/LICENSE-2.0                                        *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT    *
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.           *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/


/** @file*/

#ifndef __NDS_NN_FULLY_CONNECTED_H__
#define __NDS_NN_FULLY_CONNECTED_H__

#ifdef __cplusplus
extern    "C"
{
#endif

// function name backward compatibility
#include "riscv_nn_fully_connected.h"
#define nds_nn_fc_mat_q7_vec_q15            riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias
#define nds_nn_fc_mat_q7_vec_q15_fast       riscv_nn_fc_mat_vec_s16_s16_s8_sft_bias_fast
#define nds_nn_fc_q15                       riscv_nn_fc_s16_s16_s16_sft_bias
#define nds_nn_fc_q15_fast                  riscv_nn_fc_s16_s16_s16_sft_bias_fast
#define nds_nn_fc_q7                        riscv_nn_fc_s8_s8_s8_sft_bias
#define nds_nn_fc_q7_fast                   riscv_nn_fc_s8_s8_s8_sft_bias_fast
#define nds_nn_fc_s8                        riscv_nn_fc_s8_s8_s8_asym_bias
#define nds_nn_fc_q7_fast_2sft              riscv_nn_fc_s8_s8_s8_sym_fast
#define nds_nn_fc_q7_fast_bias_2sft         riscv_nn_fc_s8_s8_s8_sym_bias_fast
#define nds_nn_fc_q7_q15_fast_2sft          riscv_nn_fc_s8_s16_s8_sym_fast
#define nds_nn_fc_q7_q15_fast_bias_2sft     riscv_nn_fc_s8_s16_s8_sym_bias_fast
#define nds_nn_fc_u8_fast_2sft              riscv_nn_fc_u8_u8_s8_sym_fast
#define nds_nn_fc_u8_fast_bias_2sft         riscv_nn_fc_u8_u8_s8_sym_bias_fast
#define nds_nn_fc_u8_q7_fast_2sft           riscv_nn_fc_u8_s8_s8_sym_fast
#define nds_nn_fc_u8_q7_fast_bias_2sft      riscv_nn_fc_u8_s8_s8_sym_bias_fast
#define nds_nn_fc_u8_q15_fast_2sft          riscv_nn_fc_u8_s16_s8_sym_fast
#define nds_nn_fc_u8_q15_fast_bias_2sft     riscv_nn_fc_u8_s16_s8_sym_bias_fast
#define nds_nn_fc_s8_get_buffer_size        riscv_nn_fc_s8_s8_s8_asym_bias_get_buffer_size
#ifdef __cplusplus
}
#endif

#endif
