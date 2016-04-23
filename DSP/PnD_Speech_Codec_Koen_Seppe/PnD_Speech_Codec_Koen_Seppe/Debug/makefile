################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./crypto/util.obj" \
"./crypto/sha3.obj" \
"./crypto/rand.obj" \
"./crypto/modular_arithmetic.obj" \
"./crypto/mac.obj" \
"./crypto/hash.obj" \
"./crypto/global.obj" \
"./crypto/format.obj" \
"./crypto/cryptoMain.obj" \
"./crypto/counter_block.obj" \
"./crypto/aes.obj" \
"./crypto/STS_protocol.obj" \
"./crypto/ISAAC.obj" \
"./crypto/CCM.obj" \
"./wavpcm_io.obj" \
"./synthesis.obj" \
"./quantize.obj" \
"./main.obj" \
"./globals.obj" \
"./dequantize.obj" \
"./bitManipulation.obj" \
"./analysis.obj" \
"../C6748.cmd" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include crypto/subdir_vars.mk
-include subdir_rules.mk
-include crypto/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: PnD_Speech_Codec_Koen_Seppe.out

# Tool invocations
PnD_Speech_Codec_Koen_Seppe.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 -g --display_error_number --diag_warning=225 --abi=coffabi -z -m"PnD_Speech_Codec_Koen_Seppe.map" --warn_sections -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --reread_libs --xml_link_info="PnD_Speech_Codec_Koen_Seppe_linkInfo.xml" --rom_model -o "PnD_Speech_Codec_Koen_Seppe.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QUOTED) "PnD_Speech_Codec_Koen_Seppe.out"
	-$(RM) "analysis.pp" "bitManipulation.pp" "dequantize.pp" "globals.pp" "main.pp" "quantize.pp" "synthesis.pp" "wavpcm_io.pp" "crypto\CCM.pp" "crypto\ISAAC.pp" "crypto\STS_protocol.pp" "crypto\aes.pp" "crypto\counter_block.pp" "crypto\cryptoMain.pp" "crypto\format.pp" "crypto\global.pp" "crypto\hash.pp" "crypto\mac.pp" "crypto\modular_arithmetic.pp" "crypto\rand.pp" "crypto\sha3.pp" "crypto\util.pp" 
	-$(RM) "analysis.obj" "bitManipulation.obj" "dequantize.obj" "globals.obj" "main.obj" "quantize.obj" "synthesis.obj" "wavpcm_io.obj" "crypto\CCM.obj" "crypto\ISAAC.obj" "crypto\STS_protocol.obj" "crypto\aes.obj" "crypto\counter_block.obj" "crypto\cryptoMain.obj" "crypto\format.obj" "crypto\global.obj" "crypto\hash.obj" "crypto\mac.obj" "crypto\modular_arithmetic.obj" "crypto\rand.obj" "crypto\sha3.obj" "crypto\util.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

