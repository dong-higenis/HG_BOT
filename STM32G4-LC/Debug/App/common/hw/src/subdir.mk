################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/common/hw/src/cli.c \
../App/common/hw/src/cli_gui.c 

OBJS += \
./App/common/hw/src/cli.o \
./App/common/hw/src/cli_gui.o 

C_DEPS += \
./App/common/hw/src/cli.d \
./App/common/hw/src/cli_gui.d 


# Each subdirectory must supply rules for building sources it contributes
App/common/hw/src/%.o App/common/hw/src/%.su App/common/hw/src/%.cyclo: ../App/common/hw/src/%.c App/common/hw/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/bsp" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/hw" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/hw/driver/usb" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common/core" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common/hw" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common/hw/include" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/ap" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/USB_Device" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-common-2f-hw-2f-src

clean-App-2f-common-2f-hw-2f-src:
	-$(RM) ./App/common/hw/src/cli.cyclo ./App/common/hw/src/cli.d ./App/common/hw/src/cli.o ./App/common/hw/src/cli.su ./App/common/hw/src/cli_gui.cyclo ./App/common/hw/src/cli_gui.d ./App/common/hw/src/cli_gui.o ./App/common/hw/src/cli_gui.su

.PHONY: clean-App-2f-common-2f-hw-2f-src

