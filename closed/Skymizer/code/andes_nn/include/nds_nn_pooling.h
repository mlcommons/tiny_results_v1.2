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

#ifndef __NDS_NN_POOLING_H__
#define __NDS_NN_POOLING_H__

#ifdef __cplusplus
extern    "C"
{
#endif

// function name backward compatibility
#include "riscv_nn_pooling.h"
#define nds_nn_maxpool_q7_HWC           riscv_nn_maxpool_HWC_s8
#define nds_nn_maxpool_s8_HWC_any       riscv_nn_maxpool_HWC_s8_any_act
#define nds_nn_avepool_q7_HWC           riscv_nn_avepool_HWC_s8
#define nds_nn_avepool_s8_HWC_any       riscv_nn_avepool_HWC_s8_any_act
#define nds_nn_avepool_q7_HWC_any       riscv_nn_avepool_HWC_s8_any
#define nds_nn_avepool_s8_HWC_any_get_buffer_size   riscv_nn_avepool_HWC_s8_any_act_get_buffer_size

#ifdef __cplusplus
}
#endif

#endif
