################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/hw/driver/button.c \
../App/hw/driver/can.c \
../App/hw/driver/cdc.c \
../App/hw/driver/fram.c \
../App/hw/driver/gpio.c \
../App/hw/driver/i2c.c \
../App/hw/driver/led.c \
../App/hw/driver/log.c \
../App/hw/driver/pwm.c \
../App/hw/driver/rtc.c \
../App/hw/driver/swtimer.c \
../App/hw/driver/uart.c \
../App/hw/driver/ws2812.c 

OBJS += \
./App/hw/driver/button.o \
./App/hw/driver/can.o \
./App/hw/driver/cdc.o \
./App/hw/driver/fram.o \
./App/hw/driver/gpio.o \
./App/hw/driver/i2c.o \
./App/hw/driver/led.o \
./App/hw/driver/log.o \
./App/hw/driver/pwm.o \
./App/hw/driver/rtc.o \
./App/hw/driver/swtimer.o \
./App/hw/driver/uart.o \
./App/hw/driver/ws2812.o 

C_DEPS += \
./App/hw/driver/button.d \
./App/hw/driver/can.d \
./App/hw/driver/cdc.d \
./App/hw/driver/fram.d \
./App/hw/driver/gpio.d \
./App/hw/driver/i2c.d \
./App/hw/driver/led.d \
./App/hw/driver/log.d \
./App/hw/driver/pwm.d \
./App/hw/driver/rtc.d \
./App/hw/driver/swtimer.d \
./App/hw/driver/uart.d \
./App/hw/driver/ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
App/hw/driver/%.o App/hw/driver/%.su App/hw/driver/%.cyclo: ../App/hw/driver/%.c App/hw/driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/bsp" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/hw" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/hw/driver/usb" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common/core" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common/hw" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/common/hw/include" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/App/ap" -I"C:/Users/user/Desktop/Project/STM32/2024/stm32g4-auto/USB_Device" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-hw-2f-driver

clean-App-2f-hw-2f-driver:
	-$(RM) ./App/hw/driver/button.cyclo ./App/hw/driver/button.d ./App/hw/driver/button.o ./App/hw/driver/button.su ./App/hw/driver/can.cyclo ./App/hw/driver/can.d ./App/hw/driver/can.o ./App/hw/driver/can.su ./App/hw/driver/cdc.cyclo ./App/hw/driver/cdc.d ./App/hw/driver/cdc.o ./App/hw/driver/cdc.su ./App/hw/driver/fram.cyclo ./App/hw/driver/fram.d ./App/hw/driver/fram.o ./App/hw/driver/fram.su ./App/hw/driver/gpio.cyclo ./App/hw/driver/gpio.d ./App/hw/driver/gpio.o ./App/hw/driver/gpio.su ./App/hw/driver/i2c.cyclo ./App/hw/driver/i2c.d ./App/hw/driver/i2c.o ./App/hw/driver/i2c.su ./App/hw/driver/led.cyclo ./App/hw/driver/led.d ./App/hw/driver/led.o ./App/hw/driver/led.su ./App/hw/driver/log.cyclo ./App/hw/driver/log.d ./App/hw/driver/log.o ./App/hw/driver/log.su ./App/hw/driver/pwm.cyclo ./App/hw/driver/pwm.d ./App/hw/driver/pwm.o ./App/hw/driver/pwm.su ./App/hw/driver/rtc.cyclo ./App/hw/driver/rtc.d ./App/hw/driver/rtc.o ./App/hw/driver/rtc.su ./App/hw/driver/swtimer.cyclo ./App/hw/driver/swtimer.d ./App/hw/driver/swtimer.o ./App/hw/driver/swtimer.su ./App/hw/driver/uart.cyclo ./App/hw/driver/uart.d ./App/hw/driver/uart.o ./App/hw/driver/uart.su ./App/hw/driver/ws2812.cyclo ./App/hw/driver/ws2812.d ./App/hw/driver/ws2812.o ./App/hw/driver/ws2812.su

.PHONY: clean-App-2f-hw-2f-driver

