################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cr_startup_lpc8xx.c \
../src/crp.c \
../src/display.c \
../src/game.c \
../src/hardware.c \
../src/mtb.c \
../src/pong.c \
../src/tvout.c 

OBJS += \
./src/cr_startup_lpc8xx.o \
./src/crp.o \
./src/display.o \
./src/game.o \
./src/hardware.o \
./src/mtb.o \
./src/pong.o \
./src/tvout.o 

C_DEPS += \
./src/cr_startup_lpc8xx.d \
./src/crp.d \
./src/display.d \
./src/game.d \
./src/hardware.d \
./src/mtb.d \
./src/pong.d \
./src/tvout.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -DCORE_M0PLUS -D__MTB_DISABLE -D__MTB_BUFFER_SIZE=256 -D__USE_CMSIS=CMSIS_CORE_LPC8xx -D__LPC8XX__ -I"C:\Users\Jacques\Documents\LPCXpresso_7.0.2_102\workspace\CMSIS_CORE_LPC8xx\inc" -O2 -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wa,-a,-ad="$*.lst" -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


