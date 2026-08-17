################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Game/CombatMode.cpp \
../Core/Src/Game/MySnake.cpp \
../Core/Src/Game/SessionManager.cpp \
../Core/Src/Game/SnakeGame.cpp \
../Core/Src/Game/SnakePositionMessage.cpp \
../Core/Src/Game/UltrasonicSensor.cpp 

OBJS += \
./Core/Src/Game/CombatMode.o \
./Core/Src/Game/MySnake.o \
./Core/Src/Game/SessionManager.o \
./Core/Src/Game/SnakeGame.o \
./Core/Src/Game/SnakePositionMessage.o \
./Core/Src/Game/UltrasonicSensor.o 

CPP_DEPS += \
./Core/Src/Game/CombatMode.d \
./Core/Src/Game/MySnake.d \
./Core/Src/Game/SessionManager.d \
./Core/Src/Game/SnakeGame.d \
./Core/Src/Game/SnakePositionMessage.d \
./Core/Src/Game/UltrasonicSensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Game/%.o Core/Src/Game/%.su Core/Src/Game/%.cyclo: ../Core/Src/Game/%.cpp Core/Src/Game/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Game

clean-Core-2f-Src-2f-Game:
	-$(RM) ./Core/Src/Game/CombatMode.cyclo ./Core/Src/Game/CombatMode.d ./Core/Src/Game/CombatMode.o ./Core/Src/Game/CombatMode.su ./Core/Src/Game/MySnake.cyclo ./Core/Src/Game/MySnake.d ./Core/Src/Game/MySnake.o ./Core/Src/Game/MySnake.su ./Core/Src/Game/SessionManager.cyclo ./Core/Src/Game/SessionManager.d ./Core/Src/Game/SessionManager.o ./Core/Src/Game/SessionManager.su ./Core/Src/Game/SnakeGame.cyclo ./Core/Src/Game/SnakeGame.d ./Core/Src/Game/SnakeGame.o ./Core/Src/Game/SnakeGame.su ./Core/Src/Game/SnakePositionMessage.cyclo ./Core/Src/Game/SnakePositionMessage.d ./Core/Src/Game/SnakePositionMessage.o ./Core/Src/Game/SnakePositionMessage.su ./Core/Src/Game/UltrasonicSensor.cyclo ./Core/Src/Game/UltrasonicSensor.d ./Core/Src/Game/UltrasonicSensor.o ./Core/Src/Game/UltrasonicSensor.su

.PHONY: clean-Core-2f-Src-2f-Game

