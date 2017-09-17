################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Kentec320x240x16_ssd2119_8bit.obj: ../Kentec320x240x16_ssd2119_8bit.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="C:/TI/TivaWare_C_Series-1.1" -g --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="Kentec320x240x16_ssd2119_8bit.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

grlib_demo.obj: ../grlib_demo.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="C:/TI/TivaWare_C_Series-1.1" -g --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="grlib_demo.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

images.obj: ../images.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="C:/TI/TivaWare_C_Series-1.1" -g --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="images.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="C:/TI/TivaWare_C_Series-1.1" -g --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

touch.obj: ../touch.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="C:/TI/TivaWare_C_Series-1.1" -g --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="touch.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ustdlib.obj: C:/TI/TivaWare_C_Series-1.1/utils/ustdlib.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="C:/TI/TivaWare_C_Series-1.1" -g --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="ustdlib.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


