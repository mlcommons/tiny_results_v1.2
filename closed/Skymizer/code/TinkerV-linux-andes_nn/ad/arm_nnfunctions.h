
#ifndef __ARM_NNFUNCTIONS_H__
#define __ARM_NNFUNCTIONS_H__

#ifdef __cplusplus
extern    "C"
{
#endif
#include <nds_nn_activation.h>
#include <nds_nn_basic.h>
#include <nds_nn_concatenation.h>
#include <nds_nn_convolution.h>
#include <nds_nn_fully_connected.h>
#include <nds_nn_pooling.h>
#include <nds_nn_softmax.h>
// #define __SSAT(a,b) (a) // ignore saturate
int32_t __SSAT(int32_t val, uint32_t sat)
{
  if ((sat >= 1U) && (sat <= 32U))
  {
    const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
    const int32_t min = -1 - max ;
    if (val > max)
    {
      return max;
    }
    else if (val < min)
    {
      return min;
    }
  }
  return val;
}

#define arm_convolve_1x1_HWC_q7_fast_nonsquare           nds_nn_conv_1x1_HWC_q7_fast_any
#define arm_convolve_HWC_q7_basic_nonsquare              nds_nn_conv_HWC_q7_any
#define arm_convolve_HWC_q7_fast_nonsquare               nds_nn_conv_HWC_q7_fast_any
#define arm_depthwise_separable_conv_HWC_q7_nonsquare    nds_nn_conv_dw_HWC_q7_any
#define arm_depthwise_separable_conv_HWC_q7              nds_nn_conv_dw_HWC_q7
#define arm_relu_q7                                      nds_nn_relu_q7
#define arm_avepool_q7_HWC                               nds_nn_avepool_q7_HWC
// #define arm_elementwise_add_s8                           nds_nn_ew_add_s8
#define arm_fully_connected_q7                           nds_nn_fc_q7
#define arm_softmax_q7                                   nds_nn_softmax_q7


// arm_elementwise_add_s8

#define Q31_MIN   ((q31_t)(0x80000000L))
#define Q31_MAX   ((q31_t)(0x7FFFFFFFL))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define SAT_INPUT(__INPUT, __MULT, __SHIFT)                                                                            \
    __INPUT = arm_nn_doubling_high_mult_no_sat(__INPUT, __MULT);                                                       \
    __INPUT = arm_nn_divide_by_power_of_two(__INPUT, -__SHIFT);

struct arm_nn_double
{
    uint32_t low;
    int32_t high;
};

union arm_nn_long_long
{
    int64_t long_long;
    struct arm_nn_double word;
};

q31_t arm_nn_doubling_high_mult(const q31_t m1, const q31_t m2)
{
    q31_t result = 0;
    // Rounding offset to add for a right shift of 31
    q63_t mult = 1 << 30;

    if ((m1 < 0) ^ (m2 < 0))
    {
        mult = 1 - mult;
    }
    // Gets resolved as a SMLAL instruction
    mult = mult + (q63_t)m1 * m2;

    // Utilize all of the upper 32 bits. This is the doubling step
    // as well.
    result = (int32_t)(mult / (1ll << 31));

    if ((m1 == m2) && (m1 == (int32_t)Q31_MIN))
    {
        result = Q31_MAX;
    }
    return result;
}

q31_t arm_nn_divide_by_power_of_two(const q31_t dividend, const q31_t exponent)
{
    q31_t result = 0;
    const q31_t remainder_mask = (1 << exponent) - 1;
    int32_t remainder = remainder_mask & dividend;

    // Basic division
    result = dividend >> exponent;

    // Adjust 'result' for rounding (mid point away from zero)
    q31_t threshold = remainder_mask >> 1;
    if (result < 0)
    {
        threshold++;
    }
    if (remainder > threshold)
    {
        result++;
    }

    return result;
}

q31_t arm_nn_doubling_high_mult_no_sat(const q31_t m1, const q31_t m2)
{
    q31_t result = 0;
    union arm_nn_long_long mult;

    // Rounding offset to add for a right shift of 31
    mult.word.low = 1 << 30;
    mult.word.high = 0;

    // Gets resolved as a SMLAL instruction
    mult.long_long = mult.long_long + (q63_t)m1 * m2;

    // Utilize all of the upper 32 bits. This is the doubling step
    // as well.
    result = (int32_t)(mult.long_long >> 31);

    return result;
}


int arm_elementwise_add_s8(const int8_t *input_1_vect,
                                  const int8_t *input_2_vect,
                                  const int32_t input_1_offset,
                                  const int32_t input_1_mult,
                                  const int32_t input_1_shift,
                                  const int32_t input_2_offset,
                                  const int32_t input_2_mult,
                                  const int32_t input_2_shift,
                                  const int32_t left_shift,
                                  int8_t *output,
                                  const int32_t out_offset,
                                  const int32_t out_mult,
                                  const int32_t out_shift,
                                  const int32_t out_activation_min,
                                  const int32_t out_activation_max,
                                  const uint32_t block_size)
{
    uint32_t loop_count;
    int32_t input_1;
    int32_t input_2;
    int32_t sum;

    loop_count = block_size;
    while (loop_count > 0U)
    {
        /* C = A + B */

        input_1 = (*input_1_vect++ + input_1_offset) << left_shift;
        input_2 = (*input_2_vect++ + input_2_offset) << left_shift;

        input_1 = arm_nn_doubling_high_mult(input_1, input_1_mult);
        input_1 = arm_nn_divide_by_power_of_two(input_1, -input_1_shift);

        input_2 = arm_nn_doubling_high_mult(input_2, input_2_mult);
        input_2 = arm_nn_divide_by_power_of_two(input_2, -input_2_shift);

        sum = input_1 + input_2;
        SAT_INPUT(sum, out_mult, out_shift);
        sum += out_offset;

        sum = MAX(sum, out_activation_min);
        sum = MIN(sum, out_activation_max);

        *output++ = (q7_t)sum;

        /* Decrement loop counter */
        loop_count--;
    }
    return 0;
}

// arm_elementwise_add_s8

#ifdef __cplusplus
}
#endif

#endif


