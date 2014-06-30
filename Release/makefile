################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: lpc810-pong.axf

# Tool invocations
lpc810-pong.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Jacques\Documents\LPCXpresso_7.0.2_102\workspace\CMSIS_CORE_LPC8xx\Release" -Xlinker -Map="lpc810-pong.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "pong_Release.ld" -o "lpc810-pong.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) lpc810-pong.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "lpc810-pong.axf" ; arm-none-eabi-objcopy -v -O ihex "lpc810-pong.axf" "lpc810-pong.hex" ; # checksum -p LPC810 -d "lpc810-pong.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
