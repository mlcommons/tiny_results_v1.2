/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (5)
#endif
/* ISR prototypes */
void sci_b_uart_rxi_isr(void);
void sci_b_uart_txi_isr(void);
void sci_b_uart_tei_isr(void);
void sci_b_uart_eri_isr(void);
void gpt_counter_overflow_isr(void);

#if __has_include("r_ioport.h")
        /* Vector table allocations */
        #define VECTOR_NUMBER_SCI3_RXI ((IRQn_Type) 0) /* SCI3 RXI (Receive data full) */
        #define SCI3_RXI_IRQn          ((IRQn_Type) 0) /* SCI3 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI3_TXI ((IRQn_Type) 1) /* SCI3 TXI (Transmit data empty) */
        #define SCI3_TXI_IRQn          ((IRQn_Type) 1) /* SCI3 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI3_TEI ((IRQn_Type) 2) /* SCI3 TEI (Transmit end) */
        #define SCI3_TEI_IRQn          ((IRQn_Type) 2) /* SCI3 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI3_ERI ((IRQn_Type) 3) /* SCI3 ERI (Receive error) */
        #define SCI3_ERI_IRQn          ((IRQn_Type) 3) /* SCI3 ERI (Receive error) */
        #define VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW ((IRQn_Type) 4) /* GPT0 COUNTER OVERFLOW (Overflow) */
        #define GPT0_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 4) /* GPT0 COUNTER OVERFLOW (Overflow) */
        #endif

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
