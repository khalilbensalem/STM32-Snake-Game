################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Interfaces/Communication/Message.cpp 

OBJS += \
./Core/Src/Interfaces/Communication/Message.o 

CPP_DEPS += \
./Core/Src/Interfaces/Communication/Message.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Interfaces/Communication/%.o Core/Src/Interfaces/Communication/%.su Core/Src/Interfaces/Communication/%.cyclo: ../Core/Src/Interfaces/Communication/%.cpp Core/Src/Interfaces/Communication/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Interfaces-2f-Communication

clean-Core-2f-Src-2f-Interfaces-2f-Communication:
	-$(RM) ./Core/Src/Interfaces/Communication/Message.cyclo ./Core/Src/Interfaces/Communication/Message.d ./Core/Src/Interfaces/Communication/Message.o ./Core/Src/Interfaces/Communication/Message.su

.PHONY: clean-Core-2f-Src-2f-Interfaces-2f-Communication

