################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ClientList.c \
../CommandLineArguments.c \
../ProcedureList.c \
../RCClient.c \
../RCClientFactory.c \
../ServoControllerData.c \
../main.c 

OBJS += \
./ClientList.o \
./CommandLineArguments.o \
./ProcedureList.o \
./RCClient.o \
./RCClientFactory.o \
./ServoControllerData.o \
./main.o 

C_DEPS += \
./ClientList.d \
./CommandLineArguments.d \
./ProcedureList.d \
./RCClient.d \
./RCClientFactory.d \
./ServoControllerData.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/lorenz/git/servocontroller" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


