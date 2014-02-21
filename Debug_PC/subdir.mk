################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CommandLineArguments.c \
../Connection.c \
../ConnectionContainer.c \
../ControlCommandRxSocket.c \
../MessageSinkCdc.c \
../ServoDriver.c \
../ServoDriverRPi.c \
../TransmitTelemetryTimerHandler.c \
../main.c 

OBJS += \
./CommandLineArguments.o \
./Connection.o \
./ConnectionContainer.o \
./ControlCommandRxSocket.o \
./MessageSinkCdc.o \
./ServoDriver.o \
./ServoDriverRPi.o \
./TransmitTelemetryTimerHandler.o \
./main.o 

C_DEPS += \
./CommandLineArguments.d \
./Connection.d \
./ConnectionContainer.d \
./ControlCommandRxSocket.d \
./MessageSinkCdc.d \
./ServoDriver.d \
./ServoDriverRPi.d \
./TransmitTelemetryTimerHandler.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Wpointer-to-int-cast
	@echo 'Finished building: $<'
	@echo ' '


