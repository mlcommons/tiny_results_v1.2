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

#ifndef __NDS_NN_CONVOLUTION_H__
#define __NDS_NN_CONVOLUTION_H__

#ifdef __cplusplus
extern    "C"
{
#endif

// function name backward compatibility
#include "riscv_nn_convolution.h"
#define nds_nn_conv_1x1_HWC_q7_fast_any                 riscv_nn_conv_1x1_HWC_s8_s8_s8_sft_bias_fast_any
#define nds_nn_conv_1x1_HWC_q7_fast_any_2sft            riscv_nn_conv_1x1_HWC_s8_s8_s8_sym_fast_any
#define nds_nn_conv_1x1_HWC_q7_fast_any_bias_2sft       riscv_nn_conv_1x1_HWC_s8_s8_s8_sym_bias_fast_any
#define nds_nn_conv_1x1_HWC_q7_q15_fast_any_2sft        riscv_nn_conv_1x1_HWC_s8_s16_s8_sym_fast_any
#define nds_nn_conv_1x1_HWC_q7_q15_fast_any_bias_2sft   riscv_nn_conv_1x1_HWC_s8_s16_s8_sym_bias_fast_any
#define nds_nn_conv_1x1_HWC_u8_fast_any_2sft            riscv_nn_conv_1x1_HWC_u8_u8_s8_sym_fast_any
#define nds_nn_conv_1x1_HWC_u8_fast_any_bias_2sft       riscv_nn_conv_1x1_HWC_u8_u8_s8_sym_bias_fast_any
#define nds_nn_conv_1x1_HWC_u8_q7_fast_any_2sft         riscv_nn_conv_1x1_HWC_u8_s8_s8_sym_fast_any
#define nds_nn_conv_1x1_HWC_u8_q7_fast_any_bias_2sft    riscv_nn_conv_1x1_HWC_u8_s8_s8_sym_bias_fast_any
#define nds_nn_conv_1x1_HWC_u8_q15_fast_any_2sft        riscv_nn_conv_1x1_HWC_u8_s16_s8_sym_fast_any
#define nds_nn_conv_1x1_HWC_u8_q15_fast_any_bias_2sft   riscv_nn_conv_1x1_HWC_u8_s16_s8_sym_bias_fast_any
#define nds_nn_conv_1x1_HWC_s8_fast_any                 riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any
#define nds_nn_conv_1xn_HWC_s8_any                      riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any
#define nds_nn_conv_HWC_q15                             riscv_nn_conv_HWC_s16_s16_s16_sft_bias
#define nds_nn_conv_HWC_q15_fast                        riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast
#define nds_nn_conv_HWC_q15_fast_any                    riscv_nn_conv_HWC_s16_s16_s16_sft_bias_fast_any
#define nds_nn_conv_HWC_q7_RGB                          riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias
#define nds_nn_conv_HWC_q7_RGB_fast                     riscv_nn_conv_HWC_s8_s8_s8_RGB_sft_bias_fast
#define nds_nn_conv_HWC_q7_q15_RGB_fast_2sft            riscv_nn_conv_HWC_s8_s16_s8_RGB_sym_fast
#define nds_nn_conv_HWC_q7_q15_RGB_fast_bias_2sft       riscv_nn_conv_HWC_s8_s16_s8_RGB_sym_bias_fast
#define nds_nn_conv_HWC_q7_RGB_fast_2sft                riscv_nn_conv_HWC_s8_s8_s8_RGB_sym_fast
#define nds_nn_conv_HWC_q7_RGB_fast_bias_2sft           riscv_nn_conv_HWC_s8_s8_s8_RGB_sym_bias_fast
#define nds_nn_conv_HWC_u8_q7_RGB_fast_2sft             riscv_nn_conv_HWC_u8_s8_s8_RGB_sym_fast
#define nds_nn_conv_HWC_u8_q7_RGB_fast_bias_2sft        riscv_nn_conv_HWC_u8_s8_s8_RGB_sym_bias_fast
#define nds_nn_conv_HWC_u8_q15_RGB_fast_2sft            riscv_nn_conv_HWC_u8_s16_s8_RGB_sym_fast
#define nds_nn_conv_HWC_u8_q15_RGB_fast_bias_2sft       riscv_nn_conv_HWC_u8_s16_s8_RGB_sym_bias_fast
#define nds_nn_conv_HWC_u8_RGB_fast_2sft                riscv_nn_conv_HWC_u8_u8_s8_RGB_sym_fast
#define nds_nn_conv_HWC_u8_RGB_fast_bias_2sft           riscv_nn_conv_HWC_u8_u8_s8_RGB_sym_bias_fast
#define nds_nn_conv_HWC_q7                              riscv_nn_conv_HWC_s8_s8_s8_sft_bias
#define nds_nn_conv_HWC_q7_any                          riscv_nn_conv_HWC_s8_s8_s8_sft_bias_any
#define nds_nn_conv_HWC_q7_fast                         riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast
#define nds_nn_conv_HWC_q7_fast_2sft                    riscv_nn_conv_HWC_s8_s8_s8_sym_fast
#define nds_nn_conv_HWC_q7_fast_bias_2sft               riscv_nn_conv_HWC_s8_s8_s8_sym_bias_fast
#define nds_nn_conv_HWC_q7_q15_fast_2sft                riscv_nn_conv_HWC_s8_s16_s8_sym_fast
#define nds_nn_conv_HWC_q7_q15_fast_bias_2sft           riscv_nn_conv_HWC_s8_s16_s8_sym_bias_fast
#define nds_nn_conv_HWC_u8_fast_2sft                    riscv_nn_conv_HWC_u8_u8_s8_sym_fast
#define nds_nn_conv_HWC_u8_fast_bias_2sft               riscv_nn_conv_HWC_u8_u8_s8_sym_bias_fast
#define nds_nn_conv_HWC_u8_q7_fast_2sft                 riscv_nn_conv_HWC_u8_s8_s8_sym_fast
#define nds_nn_conv_HWC_u8_q7_fast_bias_2sft            riscv_nn_conv_HWC_u8_s8_s8_sym_bias_fast
#define nds_nn_conv_HWC_u8_q15_fast_2sft                riscv_nn_conv_HWC_u8_s16_s8_sym_fast
#define nds_nn_conv_HWC_u8_q15_fast_bias_2sft           riscv_nn_conv_HWC_u8_s16_s8_sym_bias_fast
#define nds_nn_conv_HWC_q7_fast_any                     riscv_nn_conv_HWC_s8_s8_s8_sft_bias_fast_any
#define nds_nn_conv_HWC_q7_fast_any_2sft                riscv_nn_conv_HWC_s8_s8_s8_sym_fast_any
#define nds_nn_conv_HWC_q7_fast_any_bias_2sft           riscv_nn_conv_HWC_s8_s8_s8_sym_bias_fast_any
#define nds_nn_conv_HWC_q7_q15_fast_any_2sft            riscv_nn_conv_HWC_s8_s16_s8_sym_fast_any
#define nds_nn_conv_HWC_q7_q15_fast_any_bias_2sft       riscv_nn_conv_HWC_s8_s16_s8_sym_bias_fast_any
#define nds_nn_conv_HWC_u8_fast_any_2sft                riscv_nn_conv_HWC_u8_u8_s8_sym_fast_any
#define nds_nn_conv_HWC_u8_fast_any_bias_2sft           riscv_nn_conv_HWC_u8_u8_s8_sym_bias_fast_any
#define nds_nn_conv_HWC_u8_q7_fast_any_2sft             riscv_nn_conv_HWC_u8_s8_s8_sym_fast_any
#define nds_nn_conv_HWC_u8_q7_fast_any_bias_2sft        riscv_nn_conv_HWC_u8_s8_s8_sym_bias_fast_any
#define nds_nn_conv_HWC_u8_q15_fast_any_2sft            riscv_nn_conv_HWC_u8_s16_s8_sym_fast_any
#define nds_nn_conv_HWC_u8_q15_fast_any_bias_2sft       riscv_nn_conv_HWC_u8_s16_s8_sym_bias_fast_any
#define nds_nn_conv_HWC_s8_any                          riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any
#define nds_nn_conv_dw_HWC_3x3_s8_any                   riscv_nn_conv_dw_HWC_3x3_s8_s8_s8_asym_bias_any
#define nds_nn_conv_dw_HWC_s8_any                       riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any
#define nds_nn_conv_dw_HWC_s8_fast_any                  riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any
#define nds_nn_conv_dw_HWC_u8_act_any                   riscv_nn_conv_dw_HWC_u8_u8_u8_asym_bias_any
#define nds_nn_conv_dw_HWC_q7                           riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias
#define nds_nn_conv_dw_HWC_q7_any                       riscv_nn_conv_dw_HWC_s8_s8_s8_sft_bias_any
#define nds_nn_conv_dw_HWC_q7_2sft                      riscv_nn_conv_dw_HWC_s8_s8_s8_sym
#define nds_nn_conv_dw_HWC_q7_bias_2sft                 riscv_nn_conv_dw_HWC_s8_s8_s8_sym_bias
#define nds_nn_conv_dw_HWC_q7_q15_2sft                  riscv_nn_conv_dw_HWC_s8_s16_s8_sym
#define nds_nn_conv_dw_HWC_q7_q15_bias_2sft             riscv_nn_conv_dw_HWC_s8_s16_s8_sym_bias
#define nds_nn_conv_dw_HWC_u8_2sft                      riscv_nn_conv_dw_HWC_u8_u8_s8_sym
#define nds_nn_conv_dw_HWC_u8_bias_2sft                 riscv_nn_conv_dw_HWC_u8_u8_s8_sym_bias
#define nds_nn_conv_dw_HWC_u8_q7_2sft                   riscv_nn_conv_dw_HWC_u8_s8_s8_sym
#define nds_nn_conv_dw_HWC_u8_q7_bias_2sft              riscv_nn_conv_dw_HWC_u8_s8_s8_sym_bias
#define nds_nn_conv_dw_HWC_u8_q15_2sft                  riscv_nn_conv_dw_HWC_u8_s16_s8_sym
#define nds_nn_conv_dw_HWC_u8_q15_bias_2sft             riscv_nn_conv_dw_HWC_u8_s16_s8_sym_bias
#define nds_nn_conv_dw_HWC_q7_any_2sft                  riscv_nn_conv_dw_HWC_s8_s8_s8_sym_any
#define nds_nn_conv_dw_HWC_q7_any_bias_2sft             riscv_nn_conv_dw_HWC_s8_s8_s8_sym_bias_any
#define nds_nn_conv_dw_HWC_q7_q15_any_2sft              riscv_nn_conv_dw_HWC_s8_s16_s8_sym_any
#define nds_nn_conv_dw_HWC_q7_q15_any_bias_2sft         riscv_nn_conv_dw_HWC_s8_s16_s8_sym_bias_any
#define nds_nn_conv_dw_HWC_u8_any_2sft                  riscv_nn_conv_dw_HWC_u8_u8_s8_sym_any
#define nds_nn_conv_dw_HWC_u8_any_bias_2sft             riscv_nn_conv_dw_HWC_u8_u8_s8_sym_bias_any
#define nds_nn_conv_dw_HWC_u8_q7_any_2sft               riscv_nn_conv_dw_HWC_u8_s8_s8_sym_any
#define nds_nn_conv_dw_HWC_u8_q7_any_bias_2sft          riscv_nn_conv_dw_HWC_u8_s8_s8_sym_bias_any
#define nds_nn_conv_dw_HWC_u8_q15_any_2sft              riscv_nn_conv_dw_HWC_u8_s16_s8_sym_any
#define nds_nn_conv_dw_HWC_u8_q15_any_bias_2sft         riscv_nn_conv_dw_HWC_u8_s16_s8_sym_bias_any
#define nds_nn_conv_1x1_HWC_s8_fast_any_get_buffer_size     riscv_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size
#define nds_nn_conv_1xn_HWC_s8_any_get_buffer_size          riscv_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size
#define nds_nn_conv_HWC_s8_any_get_buffer_size              riscv_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size
#define nds_nn_conv_dw_HWC_s8_fast_any_get_buffer_size      riscv_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size

#ifdef __cplusplus
}
#endif

#endif
