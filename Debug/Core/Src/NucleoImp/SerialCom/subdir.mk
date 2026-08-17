################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/NucleoImp/SerialCom/COBS.cpp \
../Core/Src/NucleoImp/SerialCom/SerialFrame.cpp \
../Core/Src/NucleoImp/SerialCom/UART.cpp 

OBJS += \
./Core/Src/NucleoImp/SerialCom/COBS.o \
./Core/Src/NucleoImp/SerialCom/SerialFrame.o \
./Core/Src/NucleoImp/SerialCom/UART.o 

CPP_DEPS += \
./Core/Src/NucleoImp/SerialCom/COBS.d \
./Core/Src/NucleoImp/SerialCom/SerialFrame.d \
./Core/Src/NucleoImp/SerialCom/UART.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/NucleoImp/SerialCom/%.o Core/Src/NucleoImp/SerialCom/%.su Core/Src/NucleoImp/SerialCom/%.cyclo: ../Core/Src/NucleoImp/SerialCom/%.cpp Core/Src/NucleoImp/SerialCom/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-NucleoImp-2f-SerialCom

clean-Core-2f-Src-2f-NucleoImp-2f-SerialCom:
	-$(RM) ./Core/Src/NucleoImp/SerialCom/COBS.cyclo ./Core/Src/NucleoImp/SerialCom/COBS.d ./Core/Src/NucleoImp/SerialCom/COBS.o ./Core/Src/NucleoImp/SerialCom/COBS.su ./Core/Src/NucleoImp/SerialCom/SerialFrame.cyclo ./Core/Src/NucleoImp/SerialCom/SerialFrame.d ./Core/Src/NucleoImp/SerialCom/SerialFrame.o ./Core/Src/NucleoImp/SerialCom/SerialFrame.su ./Core/Src/NucleoImp/SerialCom/UART.cyclo ./Core/Src/NucleoImp/SerialCom/UART.d ./Core/Src/NucleoImp/SerialCom/UART.o ./Core/Src/NucleoImp/SerialCom/UART.su

.PHONY: clean-Core-2f-Src-2f-NucleoImp-2f-SerialCom

