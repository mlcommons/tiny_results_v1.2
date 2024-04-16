#include <math.h>
#include <stdint.h>
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

extern const uint8_t * get_model_pointer();
constexpr uint32_t model_input_size = (128 * 5);
constexpr uint32_t tensor_arena_size = (10 * 1024);
constexpr uint32_t max_result_buffer_size = 128;
constexpr uint32_t category_size = 640;

// notice sign
static float input_buffer[model_input_size];
float app_result_buffer[max_result_buffer_size];

namespace{
    const tflite::Model* model = nullptr;
    TfLiteTensor* model_input = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    uint8_t tensor_arena[tensor_arena_size] __attribute__ ((section (".dtcm_data")));
    TfLiteStatus invoke_status;
}  // namespace

bool app_setup(){
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const uint8_t* tflite_model = get_model_pointer();
    
    static tflite::MicroMutableOpResolver<3> micro_op_resolver;  // NOLINT
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddQuantize();
    micro_op_resolver.AddDequantize();

    static tflite::MicroInterpreter static_interpreter(
        tflite::GetModel(tflite_model), micro_op_resolver, tensor_arena, tensor_arena_size);
    interpreter = &static_interpreter;

    /* Allocate memory from the tensor_arena for the model's tensors. */
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        return false;
    }
    // Get information about the memory area to use for the model's input.
    model_input = interpreter->input(0);
    return true;
}

void app_load_tensor(size_t (*get_buffer_func)(uint8_t* input, size_t size)){
    size_t bytes = get_buffer_func(reinterpret_cast<uint8_t *>(input_buffer), model_input_size * sizeof(float));
    for (size_t i = 0; i < model_input_size; ++i){
	int32_t temp = round(input_buffer[i] / model_input->params.scale) + model_input->params.zero_point;
	if (temp < INT8_MIN) temp = INT8_MIN;
	if (temp > INT8_MAX) temp = INT8_MAX;
        model_input->data.int8[i] = (int8_t)temp;
    }
    assert(model_input->bytes == model_input_size);
}

void app_infer(){
    TfLiteStatus invoke_status = interpreter->Invoke();
    (void)invoke_status;
}

float* app_get_results(uint32_t* output_size){
    TfLiteTensor* output = interpreter->output(0);
    double diff_sum = 0.;
    for (size_t i = 0; i < category_size; ++i){
        float rst = (float)((int32_t)output->data.int8[i] - output->params.zero_point) * output->params.scale;
	float diff = rst - input_buffer[i];
	diff_sum += diff * diff;
    }
    diff_sum /= category_size;
    app_result_buffer[0] = (float)diff_sum;
    assert(output->bytes == category_size);
    *output_size = 1;
    return app_result_buffer;
}
