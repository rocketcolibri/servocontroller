################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../RtspServer/h264_v4l2_rtspserver.cpp 

OBJS += \
./RtspServer/h264_v4l2_rtspserver.o 

CPP_DEPS += \
./RtspServer/h264_v4l2_rtspserver.d 


# Each subdirectory must supply rules for building sources it contributes
RtspServer/%.o: ../RtspServer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


