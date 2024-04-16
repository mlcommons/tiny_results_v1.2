#include "eai_tiny_api.h"
#include "platform.h"
#include "eai_log.h"
#include <string.h>

#define ALIGN(n, alignment) ((n + alignment - 1) & (~(alignment - 1)))

static const char *data_type_string[] = {
    "",      "float32", "uint8", "int8",    "uint16",  "int16",  "int32",
    "int64", "",        "",      "float16", "float64", "uint32", "uint64",
};

static const char* hw_cfg_heap[] = {
    NULL,
    "CAM_LLCC_ISLAND1_POOL",
    "AUDIO_ISLAND_LPASS_TCM_POOL",
    "AUDIO_ISLAND_TCM_PHYSPOOL"
};

int load_model(struct eai_sample_context *context) {
    FILE *fp = NULL;
    int ret = 0;

    if (!context->model_name) {
        return -1;
    }

    EAI_LOG("Model : %s\n", context->model_name);
    context->model_size = 0;
    fp = fopen(context->model_name, "rb");
    if (fp == NULL) {
        EAI_LOG("Unable to open model file: %s\n", context->model_name);
        return -1;
    }
    else {
        EAI_LOG("opened model file: %s\n", context->model_name);
    }

    do {
        fseek(fp, 0, SEEK_END);
        context->model_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        EAI_LOG("opened model file: %zu\n", context->model_size);
        context->model_buffer = (uint8_t *)malloc_align(MODEL_BUFFER_ALIGNMENT, context->model_size);
        if (!context->model_buffer) {
            ret = -1;
            break;
        }

        if (!ret && fread(context->model_buffer, 1, context->model_size, fp) != context->model_size) {
            ret = -1;
            break;
        }
    } while (0);

    fclose(fp);
    return ret;
}

uint32_t get_eai_init_flags(struct eai_sample_context *context) {
    uint32_t eai_init_flags = 0x0;
  
    return eai_init_flags;
}

int init_eai(struct eai_sample_context *context)
{
    EAI_RESULT eai_ret = EAI_SUCCESS;
    eai_memory_info_t scratch_memory;

    uint32_t eai_init_flags = get_eai_init_flags(context);
    EAI_LOG("eai_init_flags %lu\n", eai_init_flags);

    if (eai_ret != EAI_SUCCESS) {
        EAI_LOG("eai_init_ex fail, result = %d\n", eai_ret);
        return -1;
    }
    
    if (!context->model_buffer)
    {
        return -1;
    }

	eai_ret = eai_init_ex(&context->eai_handle, context->model_buffer, context->model_size, eai_init_flags, hw_cfg_heap[context->hw_cfg_heap_type]); // flags contain user set flags such as enable/disable lpi mode
    if (eai_ret != EAI_SUCCESS) {
        EAI_LOG("eai_init_ex fail, result = %d\n", eai_ret);
        return -1;
    }

    eai_ret = eai_get_property(context->eai_handle, EAI_PROP_MODEL_META_INFO, &(context->eai_model_meta_info));
    if (eai_ret != EAI_SUCCESS) {
        EAI_LOG("eai_set_property(EAI_PROP_MODEL_META_INFO) fail, result = %d\n", eai_ret);
        return -1;
    }

    //model_type 0: fixed, 1: float
    bool is_enpu_supported = (context->eai_model_meta_info.model_type == 0 && context->eai_model_meta_info.enpu_ver >= 2);
    context->mla_usage = (context->use_enpu && is_enpu_supported) ? EAI_MLA_USAGE_TYPE_YES : EAI_MLA_USAGE_TYPE_NO;

    EAI_LOG("\neai: target enpu ver: %"PRIu32"\n", context->eai_model_meta_info.enpu_ver);
    EAI_LOG("\neai: enpu enable: %d\n", (int)context->mla_usage);


    if (context->mla_usage){
        // Needs to be set before MLA_USAGE, since it passes down to ENPU
        eai_client_perf_config_t client_perf_config = {0};
        client_perf_config.fps        = 100;
        client_perf_config.ftrt_ratio = 0x8000;
        client_perf_config.priority = context->priority;

        if ((eai_ret = eai_set_property(context->eai_handle, (EAI_PROP) EAI_PROP_CLIENT_PERF_CFG, &client_perf_config)) != EAI_SUCCESS){
            EAI_LOG("Failed to set property EAI_PROP_CLIENT_PERF_CFG : result = %d \n", eai_ret);
            return -1;
        }
    }

    eai_ret = eai_set_property(context->eai_handle, EAI_PROP_MLA_USAGE, &context->mla_usage);
    if (eai_ret != EAI_SUCCESS && eai_ret != EAI_MLA_NOT_AVAILABLE) {
        EAI_LOG("eai_set_property(EAI_PROP_MLA_USAGE) fail, result = %d\n", eai_ret);
        return -1;
    }

    if (context->mla_usage == EAI_MLA_USAGE_TYPE_YES) {
        eai_mla_affinity_t  client_affinity = {0};
        client_affinity.affinity = CORE_AFFINITY(context->affinity);
        client_affinity.core_selection= CORE_SELECTION(context->affinity);
        if ((eai_ret = eai_set_property(context->eai_handle, (EAI_PROP) EAI_PROP_MLA_AFFINITY, &client_affinity)) != EAI_SUCCESS){
            EAI_LOG("Failed to set property EAI_PROP_MLA_AFFINITY : result = %d \n", eai_ret);
            return -1;
        }
    }

    eai_ret = eai_preapply(context->eai_handle);
    if (eai_ret != EAI_SUCCESS) {
        EAI_LOG("eai_preapply fail, result = %d\n", eai_ret);
        return -1;
    }

    // get scratch buffer info
    eai_ret = eai_get_property(context->eai_handle, EAI_PROP_SCRATCH_MEM, &scratch_memory);
    if (eai_ret != EAI_SUCCESS)
    {
        EAI_LOG("eai_get_property(EAI_PROP_SCRATCH_MEM) FAIL. result = %d\n", eai_ret);
        return -1;
    }

    context->scratch_buffer_size = scratch_memory.memory_size;
    context->scratch_buffer = (uint8_t *)malloc_align(SCRATCH_BUFFER_ALIGNMENT, scratch_memory.memory_size);

    if (!context->scratch_buffer){
        EAI_LOG("Could not allocate memory for scratch buffer\n");
        return -1;
    }

    scratch_memory.addr = context->scratch_buffer;
    // set scratch buffer for eai api
    eai_ret = eai_set_property(context->eai_handle, EAI_PROP_SCRATCH_MEM, &scratch_memory);
    if (eai_ret != EAI_SUCCESS) {
        free_align(context->scratch_buffer);
        context->scratch_buffer = NULL;
        EAI_LOG("eai_set_property(EAI_PROP_SCRATCH_MEM) FAIL. result = %d\n", eai_ret);
        return -1;
    }

    eai_ret = eai_apply(context->eai_handle);
    if (eai_ret != EAI_SUCCESS) {
        EAI_LOG("eai_apply FAIL. result = %d\n", eai_ret);
        return -1;
    }

    for (int i = 0; i < 2; i ++){
        context->eai_buffers[i] = malloc_align(TENSOR_BUFFER_ALIGNMENT, sizeof(eai_buffer_info_t) * context->tensor_count[i]);
    }

    return eai_ret;
}

int get_model_io(struct eai_sample_context *context)
{
    EAI_RESULT eai_ret = EAI_SUCCESS;

    for (int i = 0; i < 2 && eai_ret == EAI_SUCCESS; i++)
    {
        eai_ports_info_t ports_info;
        ports_info.input_or_output = i;
        eai_ret = eai_get_property(context->eai_handle, EAI_PROP_PORTS_NUM, &ports_info);
        if (eai_ret != EAI_SUCCESS)
        {
            EAI_LOG("Failed eai_get_property(EAI_PROP_PORTS_NUM - inputs). result = %d\n", eai_ret);
            break;
        }
        context->tensor_count[i] = ports_info.size;
        context->tensors[i] = malloc_align(TENSOR_BUFFER_ALIGNMENT, sizeof(eai_tensor_info_t) * ports_info.size);
        if (context->tensors[i] == NULL){
            EAI_LOG("Failed to malloc tensor array\n");
            eai_ret = EAI_FAIL;
            break;
        }

        for (unsigned int j = 0; j < ports_info.size; j++)
        {
            context->tensors[i][j].index = j;
            context->tensors[i][j].input_or_output = i;

            eai_ret = eai_get_property(context->eai_handle, EAI_PROP_TENSOR_INFO, &(context->tensors[i][j]));
            EAI_LOG("user scratch pointer context->tensors[%d][%d]: %p\n",i,j,context->tensors[i][j].address)
            if (eai_ret != EAI_SUCCESS)
            {
                EAI_LOG("Failed eai_get_property(EAI_PROP_TENSOR_SIZE_INFO - inputs). result = %d\n", eai_ret);
                break;
            }
            if (!context->allocate_io_buf) {
                if (context->tensors[i][j].address == NULL) {
                    eai_ret = EAI_RESOURCE_FAILURE;
                    EAI_LOG("Input/Output not configured to use scratch, please use: -allocate_io\n");
                    break;
                }
            }
        }
    }
    
    if (eai_ret == EAI_SUCCESS && context->allocate_io_buf) {

        // get the hardware required alignment
        uint32_t buffer_alignment;
        eai_ret = eai_get_property(context->eai_handle, EAI_PROP_ALIGNMENT, &buffer_alignment);

        if (eai_ret != EAI_SUCCESS){
            EAI_LOG("Failed to retrieve hardware buffer alignment\n");
            return -1;
        }

        size_t aligned_tensor_size = 0;
        for (int i = 0; i < 2 && eai_ret == EAI_SUCCESS; i++) {
            for (int j = 0; j < context->tensor_count[i] && eai_ret == EAI_SUCCESS; j++) {
                aligned_tensor_size = context->tensors[i][j].tensor_size + buffer_alignment; // Allocate extra memory for alignment since this buffer is being allocated from the user side
                context->tensors[i][j].address = malloc_align(buffer_alignment, aligned_tensor_size);

                if (context->tensors[i][j].address == NULL) {
                    EAI_LOG("Failed to allocate buffer for I/O\n");
                    eai_ret = EAI_RESOURCE_FAILURE;
                    break;
                }

                //todo: register io buffer to the runtime if runtime is in the root pd
            }
        }
    }
    return (eai_ret == EAI_SUCCESS) ? 0 : -1;
}

void print_model_io(struct eai_sample_context *context)
{
    printf("print model io \r\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < context->tensor_count[i]; j++) {
            if (i == 0) {
                EAI_LOG("\ninput: ");
            }
            else {
                EAI_LOG("output: ");
            }
            eai_tensor_info_t *tensor = &(context->tensors[i][j]);
            EAI_LOG("data type: %s\n", data_type_string[tensor->element_type]);
            EAI_LOG("dimension:");
            for (unsigned int k = 0; k < tensor->num_dims; k++) {
                EAI_LOG(" %"PRIu32"", (tensor->dims[k]));
            }
            EAI_LOG("\n\n");
        }
    }
}

int fill_io_batch(struct eai_sample_context *context) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < context->tensor_count[i]; j++) {
            eai_tensor_info_t *tensor = &(context->tensors[i][j]);
            context->eai_buffers[i][j].index = j;
            context->eai_buffers[i][j].element_type = tensor->element_type;
            context->eai_buffers[i][j].addr = tensor->address;
            context->eai_buffers[i][j].buffer_size = tensor->tensor_size;
        }
    }
    context->eai_batch.num_inputs = context->tensor_count[0];
    context->eai_batch.num_outputs = context->tensor_count[1];
    context->eai_batch.inputs = &(context->eai_buffers[0][0]);
    context->eai_batch.outputs = &(context->eai_buffers[1][0]);
    return 0;
}

int deinit(struct eai_sample_context *context) {
    if (!context) {
        return -1;
    }

    if (!context->eai_handle) {
        return 0;
    }

    EAI_RESULT eai_ret = eai_deinit(context->eai_handle);
    if (eai_ret != EAI_SUCCESS) {
        EAI_LOG("fail to deinit eai");
    }

    free_align(context->model_buffer);
    context->model_buffer = NULL;

    //free io buffers if allocated
    if (context->allocate_io_buf) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < context->tensor_count[i]; j++) {
                if(context->tensors[i][j].address) {
                    free_align(context->tensors[i][j].address);
                    context->tensors[i][j].address = NULL;
                }
            }
        }
    }

    return 0;
}

int generate_output_file_name(struct eai_sample_context *context, char *full_path, int index) {
    char output_file_name[256];
    snprintf(output_file_name, 256, "output_%d.raw", index);
    if (context->output_path) {
        strcpy(full_path, context->output_path);
        if (context->output_path[strlen(context->output_path) - 1] != '/') {
            strcat(full_path, "/");
        }
    }
    strcat(full_path, output_file_name);
    return 0;
}

int initialize_o(struct eai_sample_context *context)
{
    int ret = 0;
    for (int i = 0; i < context->tensor_count[1]; i++)
    {
        context->output_name[i] = (char *)malloc(MAX_FILE_PATH_LENGTH);
        if(context->output_name[i] == NULL) {
            ret = -1;
            break;
        }
        context->output_name[i][0] = 0;
        generate_output_file_name(context, context->output_name[i], i);
        context->io_file[1][i] = fopen(context->output_name[i], "wb");
        if (context->io_file[1][i] == NULL)
        {
            EAI_LOG("fail to open output file %s\n", context->output_name[i]);
            ret = -1;
            break;
        }
    }
    return ret;
}

int initialize_i(struct eai_sample_context *context)
{
    int ret = 0;
    for (int i = 0; i < context->tensor_count[0]; i++)
    {
        context->io_file[0][i] = fopen(context->input_file[i], "rb");
        if (context->io_file[0][i] == NULL)
        {
            EAI_LOG("fail to open input file %s\n", context->input_file[i]);
            ret = -1;
            break;
        }
    }
    return ret;
}

int save_outputs(struct eai_sample_context *context)
{
    int ret = 0;
    for (int i = 0; i < context->tensor_count[1]; i++)
    {
        eai_tensor_info_t *tensor = &context->tensors[1][i];
        if (!tensor || !tensor->address) {
            EAI_LOG("invalid i/o tensor!\n");
            ret = -1;
            break;
        }
        size_t write_size = fwrite(tensor->address, 1, tensor->tensor_size, context->io_file[1][i]);

        if (write_size != tensor->tensor_size)
        {
            ret = -1;
            break;
        }
    }
    return ret;
}
