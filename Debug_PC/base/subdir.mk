################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../base/DBG.c \
../base/GEN.c \
../base/MON.c \
../base/Reactor.c \
../base/TRC.c \
../base/TRC_File.c \
../base/TimerFd.c \
../base/avltree.c \
../base/btree.c \
../base/deque.c \
../base/dlist.c \
../base/heap.c \
../base/linklist.c \
../base/memchunk.c 

OBJS += \
./base/DBG.o \
./base/GEN.o \
./base/MON.o \
./base/Reactor.o \
./base/TRC.o \
./base/TRC_File.o \
./base/TimerFd.o \
./base/avltree.o \
./base/btree.o \
./base/deque.o \
./base/dlist.o \
./base/heap.o \
./base/linklist.o \
./base/memchunk.o 

C_DEPS += \
./base/DBG.d \
./base/GEN.d \
./base/MON.d \
./base/Reactor.d \
./base/TRC.d \
./base/TRC_File.d \
./base/TimerFd.d \
./base/avltree.d \
./base/btree.d \
./base/deque.d \
./base/dlist.d \
./base/heap.d \
./base/linklist.d \
./base/memchunk.d 


# Each subdirectory must supply rules for building sources it contributes
base/%.o: ../base/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Wpointer-to-int-cast
	@echo 'Finished building: $<'
	@echo ' '


