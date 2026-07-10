################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/EXTRUDER_FSM.c \
../Core/Src/app_freertos.c \
../Core/Src/diameter_control.c \
../Core/Src/edu301d_driver.c \
../Core/Src/main.c \
../Core/Src/max31856_driver.c \
../Core/Src/motor_control.c \
../Core/Src/pid_controller.c \
../Core/Src/process_data.c \
../Core/Src/protocol_hmi.c \
../Core/Src/protocol_winder.c \
../Core/Src/safety.c \
../Core/Src/ssr50da_driver.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_hal_timebase_tim.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/temperature_control.c \
../Core/Src/uart_driver.c \
../Core/Src/wrapper_fsm.c 

OBJS += \
./Core/Src/EXTRUDER_FSM.o \
./Core/Src/app_freertos.o \
./Core/Src/diameter_control.o \
./Core/Src/edu301d_driver.o \
./Core/Src/main.o \
./Core/Src/max31856_driver.o \
./Core/Src/motor_control.o \
./Core/Src/pid_controller.o \
./Core/Src/process_data.o \
./Core/Src/protocol_hmi.o \
./Core/Src/protocol_winder.o \
./Core/Src/safety.o \
./Core/Src/ssr50da_driver.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_hal_timebase_tim.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/temperature_control.o \
./Core/Src/uart_driver.o \
./Core/Src/wrapper_fsm.o 

C_DEPS += \
./Core/Src/EXTRUDER_FSM.d \
./Core/Src/app_freertos.d \
./Core/Src/diameter_control.d \
./Core/Src/edu301d_driver.d \
./Core/Src/main.d \
./Core/Src/max31856_driver.d \
./Core/Src/motor_control.d \
./Core/Src/pid_controller.d \
./Core/Src/process_data.d \
./Core/Src/protocol_hmi.d \
./Core/Src/protocol_winder.d \
./Core/Src/safety.d \
./Core/Src/ssr50da_driver.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_hal_timebase_tim.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/temperature_control.d \
./Core/Src/uart_driver.d \
./Core/Src/wrapper_fsm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/EXTRUDER_FSM.cyclo ./Core/Src/EXTRUDER_FSM.d ./Core/Src/EXTRUDER_FSM.o ./Core/Src/EXTRUDER_FSM.su ./Core/Src/app_freertos.cyclo ./Core/Src/app_freertos.d ./Core/Src/app_freertos.o ./Core/Src/app_freertos.su ./Core/Src/diameter_control.cyclo ./Core/Src/diameter_control.d ./Core/Src/diameter_control.o ./Core/Src/diameter_control.su ./Core/Src/edu301d_driver.cyclo ./Core/Src/edu301d_driver.d ./Core/Src/edu301d_driver.o ./Core/Src/edu301d_driver.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/max31856_driver.cyclo ./Core/Src/max31856_driver.d ./Core/Src/max31856_driver.o ./Core/Src/max31856_driver.su ./Core/Src/motor_control.cyclo ./Core/Src/motor_control.d ./Core/Src/motor_control.o ./Core/Src/motor_control.su ./Core/Src/pid_controller.cyclo ./Core/Src/pid_controller.d ./Core/Src/pid_controller.o ./Core/Src/pid_controller.su ./Core/Src/process_data.cyclo ./Core/Src/process_data.d ./Core/Src/process_data.o ./Core/Src/process_data.su ./Core/Src/protocol_hmi.cyclo ./Core/Src/protocol_hmi.d ./Core/Src/protocol_hmi.o ./Core/Src/protocol_hmi.su ./Core/Src/protocol_winder.cyclo ./Core/Src/protocol_winder.d ./Core/Src/protocol_winder.o ./Core/Src/protocol_winder.su ./Core/Src/safety.cyclo ./Core/Src/safety.d ./Core/Src/safety.o ./Core/Src/safety.su ./Core/Src/ssr50da_driver.cyclo ./Core/Src/ssr50da_driver.d ./Core/Src/ssr50da_driver.o ./Core/Src/ssr50da_driver.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_hal_timebase_tim.cyclo ./Core/Src/stm32g4xx_hal_timebase_tim.d ./Core/Src/stm32g4xx_hal_timebase_tim.o ./Core/Src/stm32g4xx_hal_timebase_tim.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/temperature_control.cyclo ./Core/Src/temperature_control.d ./Core/Src/temperature_control.o ./Core/Src/temperature_control.su ./Core/Src/uart_driver.cyclo ./Core/Src/uart_driver.d ./Core/Src/uart_driver.o ./Core/Src/uart_driver.su ./Core/Src/wrapper_fsm.cyclo ./Core/Src/wrapper_fsm.d ./Core/Src/wrapper_fsm.o ./Core/Src/wrapper_fsm.su

.PHONY: clean-Core-2f-Src

