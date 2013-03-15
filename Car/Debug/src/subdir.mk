################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Arduino.cpp \
../src/Car.cpp \
../src/Server.cpp \
../src/Stereo.cpp \
../src/main.cpp 

OBJS += \
./src/Arduino.o \
./src/Car.o \
./src/Server.o \
./src/Stereo.o \
./src/main.o 

CPP_DEPS += \
./src/Arduino.d \
./src/Car.d \
./src/Server.d \
./src/Stereo.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/opencv -I/usr/local/include/cvblobs -O3 -g3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


