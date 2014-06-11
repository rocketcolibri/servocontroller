################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include base/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: RCSimulator

dependents:
	-cd /home/lorenz/git/servocontroller/Debug_PC && $(MAKE) all

# Tool invocations
RCSimulator: $(OBJS) $(USER_OBJS) /home/lorenz/git/servocontroller/Debug_PC/ServoController
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "RCSimulator" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) RCSimulator
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/lorenz/git/servocontroller/Debug_PC/ServoController:

-include ../makefile.targets
