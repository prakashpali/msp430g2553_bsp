################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Documents/MSP430G2xx3_Code_Examples/Assembly_CCS/%.obj: ../Documents/MSP430G2xx3_Code_Examples/Assembly_CCS/%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.3.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccsv8/ccs_base/msp430/include" --include_path="C:/Users/ppali/ccs_workspace_v8/msp430g2553_bsp" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.3.LTS/include" --advice:power=all --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="Documents/MSP430G2xx3_Code_Examples/Assembly_CCS/$(basename $(<F)).d_raw" --obj_directory="Documents/MSP430G2xx3_Code_Examples/Assembly_CCS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


