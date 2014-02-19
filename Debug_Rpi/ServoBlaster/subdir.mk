################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ServoBlaster/servod.c 

OBJS += \
./ServoBlaster/servod.o 

C_DEPS += \
./ServoBlaster/servod.d 


# Each subdirectory must supply rules for building sources it contributes
ServoBlaster/%.o: ../ServoBlaster/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Wpointer-to-int-cast
	@echo 'Finished building: $<'
	@echo ' '


