################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
analysis.obj: ../analysis.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="analysis.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bitManipulation.obj: ../bitManipulation.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="bitManipulation.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

dequantize.obj: ../dequantize.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="dequantize.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

globals.obj: ../globals.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="globals.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

quantize.obj: ../quantize.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="quantize.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

synthesis.obj: ../synthesis.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="synthesis.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

wavpcm_io.obj: ../wavpcm_io.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000/bin/cl6x" -mv6740 -O2 --include_path="C:/ti/ccsv5/tools/compiler/c6000/include" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="wavpcm_io.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


