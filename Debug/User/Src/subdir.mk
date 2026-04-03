################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Src/can_handler.c \
../User/Src/guidance_task.c \
../User/Src/sensor_task.c 

OBJS += \
./User/Src/can_handler.o \
./User/Src/guidance_task.o \
./User/Src/sensor_task.o 

C_DEPS += \
./User/Src/can_handler.d \
./User/Src/guidance_task.d \
./User/Src/sensor_task.d 


# Each subdirectory must supply rules for building sources it contributes
User/Src/%.o User/Src/%.su User/Src/%.cyclo: ../User/Src/%.c User/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/johnh/isr_pgm_project/isr_pgm/User/Inc" -I"C:/Users/johnh/isr_pgm_project/isr_pgm/User/Src" -I"C:/Users/johnh/isr_pgm_project/isr_pgm/ThirdParty/FreeRTOS" -I"C:/Users/johnh/isr_pgm_project/isr_pgm/ThirdParty/FreeRTOS/include" -I"C:/Users/johnh/isr_pgm_project/isr_pgm/ThirdParty/FreeRTOS/portable/GCC/ARM_CM3" -I"C:/Users/johnh/isr_pgm_project/isr_pgm/ThirdParty/FreeRTOS/portable/MemMang" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-Src

clean-User-2f-Src:
	-$(RM) ./User/Src/can_handler.cyclo ./User/Src/can_handler.d ./User/Src/can_handler.o ./User/Src/can_handler.su ./User/Src/guidance_task.cyclo ./User/Src/guidance_task.d ./User/Src/guidance_task.o ./User/Src/guidance_task.su ./User/Src/sensor_task.cyclo ./User/Src/sensor_task.d ./User/Src/sensor_task.o ./User/Src/sensor_task.su

.PHONY: clean-User-2f-Src

