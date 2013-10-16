################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Connection.c \
../ConnectionContainer.c \
../ControlCommandRxSocket.c \
../TransmitTelemetryTimerHandler.c \
../main.c 

OBJS += \
./Connection.o \
./ConnectionContainer.o \
./ControlCommandRxSocket.o \
./TransmitTelemetryTimerHandler.o \
./main.o 

C_DEPS += \
./Connection.d \
./ConnectionContainer.d \
./ControlCommandRxSocket.d \
./TransmitTelemetryTimerHandler.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


