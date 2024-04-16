# MLPerf Tiny benchmark v1.2 - Closed Division

This document provides an overview of our submission to the MLPerf Tiny benchmark v1.2. The benchmarks were recorded on the **EK-U1-VCU128-G** and **LC236R5E176_BOARD_V2.0**. We run the **vww** benchmark using both **EK-U1-VCU128-G** and **LC236R5E176_BOARD_V2.0**, and run the **ic** benchmark only using **LC236R5E176_BOARD_V2.0**. The submission contains performance results. Our solutions are as below：



## LC236R5E176_BOARD_V2.0

### Generate quantized model data

Use our model parsing tool for model parsing, model quantization, and model serialization to generate quantized model data files. The model parsing tool is placed in ```code/vww/VCU128/graph_quantization```. These model data files are placed in ```code/vww/VCU128/graph_quantization/temps```, including model architecture, weight data, quantization information, and other data.

### Integrate into inference engine

Integrating the C model data files into our inference engine, and compiled into binary files.

In the **ic** benchmark, the C model data files are placed in the ```code/vww/graph_quantization/temps/ic``` folder. The binary file is the  ```code/ic/LC236R5E176_BOARD_V2.0/resnet.elf```.

In the **vww** benchmark, the model data files are placed in the ```code/vww/graph_quantization/temps/vww``` folder. The binary file is the  ```code/vww/LC236R5E176_BOARD_V2.0/mobilenet.elf```.

The can be downloaded to the **LC236R5E176_BOARD_V2.0** board with flash programmer tools, which is located in ```code/vww/LC236R5E176_BOARD_V2.0/FlashProgrammer.rar```.

### Prepare for testing

1. Open the flash programmer and add an ELF Target Objects in the **File** tab. Add the *.elf path to the **File Path** box.
2. Select the **Advance** tab and add the path of e906_flash_if.elf to the **Program Algorithm File** path. Select the **Erase Type** as **Chip Erase**.
3. Connect the board through the **JTAG** port **U76**. Connect the **TTL** console port **U115**. Turn the **SW1** to **eflash boot** mode.
4. Power on the board and click **Start** to program the board.



## EK-U1-VCU128-G

### Generate quantized model data

Use our model parsing tool for model parsing, model quantization, and model serialization to generate quantized model data files. The model parsing tool is placed in ```code/vww/VCU128/graph_quantization```. These model data files are placed in ```code/vww/VCU128/graph_quantization/temps```, including model architecture, weight data, quantization information, and other data.

### Generate ANPU hardware execution control instructions

Based on the model architecture data, we dissect the model manually and generate **ANPU** hardware execution control instructions, which is located at ```code/vww/VCU128/hardware/Hardware_Exucution_Control_Instruction_for_ANPU.xlsx```. 

### Generate model data used for ANPU

Use scripts to rearrange weight data, quantization data, and quantization offset data in the model data files to generate weight data, quantization data, and quantization offset data used for **ANPU**. These scripts are placed in ```code/vww/VCU128/hardware/convert_to_anpu_model_data```, and their outputs are placed in ```code/vww/VCU128/hardware/convert_to_anpu_model_data/anpu_model_data```.

### Integrate into an executable software program

Integrate **ANPU**'s hardware execution control instructions, weight data, quantization data, and quantization offset data into an executable software program.

### Prepare for testing

1. Power on the **EK-U1-VCU128-G** board; use a **JTAG** downloader to write the **SoC+ANPU** BIT file, which is located at ```code/vww/VCU128/hardware/hw.bit```. Then check if the **SoC** is working properly using **CDK** IDE, and flash the executable software program onto the **SoC**. Resources occupied by hardware_design is shown in ```code/vww/VCU128/hardware/Resources_Occupied_By_Hardware_Design.png```
2. The executable software program sends **ANPU**'s hardware execution control instructions, weight data, quantization data, and quantization offset data to **ANPU**.
3. The host reads an image from the dataset and sends it to the **SoC** via a serial port. When the **SoC** receives it, it sends the image to **ANPU** through **DMA**. **ANPU** caches the image and all intermediate calculation results in the accelerator memory and sends the calculation results to the **SoC** after the computation is done.
4. The **SoC** performs data post-processing and sends it to the host via a serial port, completing the computation process for a single image.