# Bosch Hardware-Aware Lowering Engine
The Bosch Hardware-Aware Lowering Engine (HALE) is a powerful commercial code generator designed specifically to 
generate optimized C code for microcontrollers.
HALE specifically supports Cortex-M-based architectures with and without DSP and boards of the RH850 automotive microcontroller family but other microcontroller architectures are also supported.
HALE's generated code offers superior performance compared to TensorFlow Lite Micro while producing identical numerical results.
HALE can optimize code for maximum performance, RAM usage, code size or a compromise of these properties.
It also supports the deployment and optimization of multiple networks simultaneously.
Both float32 and int8 networks are supported. 
In this submission, we present several variants of generated code.
For all boards we give results for int8 models.
We also include code for float models for those boards that are equipped with an FPU.
For the anomaly detection benchmark, performance can be maximized at the cost of runtime by putting weights into RAM.
On the other hand, for some models like the image classification benchmark, it is possible to greatly reduce RAM consumption at the cost of increased runtime.
We provide an example of this in this benchmark.
The generated code requires around 40% less RAM but runtime doubles.

What sets HALE apart is that it is developed specifically with automotive applications and other safety-critical applications in mind.
The generated code can be configured to fulfill coding standards as common in the automotive industry.
Parameter calibration support is currently in development.

# Run the benchmarks
In this submission we provide the code necessary to perform the MLPerf™ benchmarks and to show the API of HALE’s code.
We benchmarked the HALE-generated code on various Cortex-M architectures as well as on a Renesas RH850-F1KMS4 board.
If you wish to verify our results, you can flash the corresponding board with the binary/hex files that are included in our submission.