################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CommandLineArguments.c \
../Connection.c \
../ConnectionContainer.c \
../ConnectionFsm.c \
../ConnectionFsmTest.c \
../MessageReceiver.c \
../MessageSinkCdc.c \
../ServoDriver.c \
../ServoDriverMock.c \
../ServoDriverRPi.c \
../SystemFsm.c \
../SystemFsmTest.c \
../TransmitTelemetry.c \
../main.c 

OBJS += \
./CommandLineArguments.o \
./Connection.o \
./ConnectionContainer.o \
./ConnectionFsm.o \
./ConnectionFsmTest.o \
./MessageReceiver.o \
./MessageSinkCdc.o \
./ServoDriver.o \
./ServoDriverMock.o \
./ServoDriverRPi.o \
./SystemFsm.o \
./SystemFsmTest.o \
./TransmitTelemetry.o \
./main.o 

C_DEPS += \
./CommandLineArguments.d \
./Connection.d \
./ConnectionContainer.d \
./ConnectionFsm.d \
./ConnectionFsmTest.d \
./MessageReceiver.d \
./MessageSinkCdc.d \
./ServoDriver.d \
./ServoDriverMock.d \
./ServoDriverRPi.d \
./SystemFsm.d \
./SystemFsmTest.d \
./TransmitTelemetry.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Wpointer-to-int-cast
	@echo 'Finished building: $<'
	@echo ' '


