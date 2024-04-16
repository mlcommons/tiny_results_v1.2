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

#ifndef __NDS_NN_BASIC_OPERATION_H__
#define __NDS_NN_BASIC_OPERATION_H__

#ifdef __cplusplus
extern    "C"
{
#endif

// function name backward compatibility
#include "riscv_nn_basic.h"
#define nds_nn_ew_add_s8        riscv_nn_ew_add_s8_asym
#define nds_nn_ew_mul_s8        riscv_nn_ew_mul_s8_asym
#define nds_nn_add_q7_2sft      riscv_nn_add_s8_sym

#ifdef __cplusplus
}
#endif

#endif
