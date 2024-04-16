#ifndef EAI_TINY_API_H
#define EAI_TINY_API_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>

#include "eai.h"

// Convert milliseconds to microseconds
#define MILLI_TO_MICRO(milli_) (1000 * milli_)

// Microseconds per second
#define MICRO_PER_SECOND (1000000)

#define MAX_IO_COUNT 10

//Flags Configuration
#define EAI_FLAGS_LPI_MODE                      0x00000001
#define MODEL_BUFFER_ALIGNMENT 256
#define SCRATCH_BUFFER_ALIGNMENT 16
#define TENSOR_BUFFER_ALIGNMENT 16

#define TENSOR_BUFF_SIZE 512
#define MAX_FILE_PATH_LENGTH 512

// core affinity is an 8 bit number, it's broken down into affinity (1 bits) | selection (2 bit), so 6 = 1 | 10 -> core 2 with AFFINITY_HARD
#define CORE_SELECTION(x) (x & 3)
#define CORE_AFFINITY(x) ( (x >> 2) & 1)

//copied from network_int.h, used for debugging purposes
#define EAI_INIT_FLAGS_DUMP_LINKED       0x00000004  /* Enable linked layer dump */

#define EAI_INIT_FLAGS_DISABLE_LINKING   0x00000008 /* Disable HAL Linking, this is an internal debugging feature*/

#define EAI_INIT_FLAGS_FORCE_WRITEBACK   0x00000010 /* Force enpu output writeback to DDR, this is an internal debugging feature*/

#define EAI_INIT_FLAGS_DISABLE_ASYNC    0x00000020 /* Disable asynchronous processing in HAL*/

#define EAI_INIT_FLAGS_DISABLE_CACHE_OPT   0x00000040 /* Disable Cache Optimization, this is an internal debugging feature*/
//Forward declaration
struct eai_sample_context;

// Function pointer for eai API calls
typedef int (*eai_sample_app_fptrs)(struct eai_sample_context *context);


struct eai_sample_context
{
    int hw_cfg_heap_type; // 0: DDR, 1: LLC, 2: LPI TCM, 3: Q6 TCM
    int buffer_heap_type; // 0: DDR, 1: LLC, 2: LPI TCM, 3: Q6 TCM
    int use_enpu;
    int allocate_io_buf;

    const char *model_name;
    const char *output_path;
    int input_file_count;
    const char *input_file[MAX_IO_COUNT];
    FILE *io_file[2][MAX_IO_COUNT];

    uint8_t *model_buffer;
    size_t model_size;
    uint8_t *scratch_buffer;
    size_t scratch_buffer_size;
    eaih_t eai_handle;
    int tensor_count[2]; // 0: input tensor, 1: output tensor
    eai_tensor_info_t * tensors[2];
    eai_buffer_info_t * eai_buffers[2];
    eai_batch_info_t eai_batch;

    char ** output_name;
    eai_model_meta_info_t eai_model_meta_info;
    uint32_t flags; //Eai config flags to be used during init (eg used for lpi mode etc)
/////////////////////////////////////////////////////////////////////
    EAI_MLA_USAGE_TYPE mla_usage;  //machine learning accelerator, determines if enpu can be used to offload ops
    uint8_t affinity; // core affinity
    uint8_t priority; // client priority
    uint8_t *cpd_last_input_address;

};

int load_model(struct eai_sample_context *context);
int init_eai(struct eai_sample_context *context);
int deinit(struct eai_sample_context *context);
int get_model_io(struct eai_sample_context *context);
void print_model_io(struct eai_sample_context *context);
int fill_io_batch(struct eai_sample_context *context);
int generate_output_file_name(struct eai_sample_context *context, char *full_path, int index);
int initialize_o(struct eai_sample_context *context);
int initialize_i(struct eai_sample_context *context);
int save_outputs(struct eai_sample_context *context);
#ifdef __cplusplus
}
#endif
#endif // EAI_TINY_API_H