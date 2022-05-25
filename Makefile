
CUR_DIR=$(shell pwd)
INCLUDE_DIRS=include ./ third_party/OpenCL-Headers third_party/fmt/include third_party/json11 address parcrypt_lib gpu logger platform secp256k1 util

INCLUDE = $(foreach d, $(INCLUDE_DIRS), -I$(CUR_DIR)/$d)

LIBDIR=$(CUR_DIR)/lib
BINDIR=$(CUR_DIR)/bin
LIBS+=-L$(LIBDIR)

PYTHON=python3

# C++ options
CXX=g++
CXXFLAGS=-O2 -std=c++2a

MAKEOTPS=-j4

# CUDA variables
#COMPUTE_CAP=30
#NVCC=nvcc
#NVCCFLAGS=-std=c++11 -gencode=arch=compute_${COMPUTE_CAP},code=\"sm_${COMPUTE_CAP}\" -Xptxas="-v" -Xcompiler "${CXXFLAGS}"
#CUDA_HOME=/usr/local/cuda
#CUDA_LIB=${CUDA_HOME}/lib64
#CUDA_INCLUDE=${CUDA_HOME}/include
#CUDA_MATH=$(CUR_DIR)/cudaMath

# OpenCL variables
#OPENCL_LIB=${CUDA_LIB}
#OPENCL_INCLUDE=${CUDA_INCLUDE}
OPENCL_INCLUDE=${CUR_DIR}/third_party/OpenCL-Headers
OPENCL_VERSION=110
BUILD_OPENCL=1

export INCLUDE
export LIBDIR
export BINDIR
export NVCC
export NVCCFLAGS
export LIBS
export CXX
export CXXFLAGS
export CUDA_LIB
export CUDA_INCLUDE
export CUDA_MATH
export OPENCL_LIB
export OPENCL_INCLUDE
export BUILD_OPENCL
export BUILD_CUDA
export PYTHON

DEPS=dir_json11
TARGETS=dir_address dir_parcrypt_lib dir_gpulib dir_logger dir_modules dir_platform dir_secp256k1 dir_util dir_parcrypt

# Use multiple cores to speed up build
main:
	$(MAKE) -j8 all

all:	${TARGETS}

unittests:	dir_unittests
	$(MAKE) -j8 dir_unittests

dir_fmt:
	$(MAKE) --directory third_party/fmt

dir_json11:
	$(MAKE) --directory third_party/json11

dir_util:
	$(MAKE) --directory util

dir_address:
	$(MAKE) --directory address

dir_logger:
	$(MAKE) --directory logger

dir_platform:
	$(MAKE) --directory platform

dir_secp256k1:
	$(MAKE) --directory secp256k1

dir_parcrypt_lib:
	$(MAKE) --directory parcrypt_lib

dir_gpulib:
	$(MAKE) --directory gpu

dir_modules:
	$(MAKE) --directory modules

dir_parcrypt: dir_json11 dir_fmt dir_gpulib dir_address dir_parcrypt_lib dir_logger dir_platform dir_secp256k1 dir_util dir_modules
	$(MAKE) --directory parcrypt

dir_unittests: dir_json11 dir_fmt dir_gpulib dir_address dir_parcrypt_lib dir_logger dir_platform dir_secp256k1 dir_util dir_modules
	$(MAKE) --directory unit_tests

clean:
	$(MAKE) --directory util clean
	$(MAKE) --directory address clean
	$(MAKE) --directory logger clean
	$(MAKE) --directory platform clean
	$(MAKE) --directory secp256k1 clean
	$(MAKE) --directory gpu clean
	$(MAKE) --directory parcrypt clean
	$(MAKE) --directory parcrypt_lib clean
	$(MAKE) --directory unit_tests clean
	$(MAKE) --directory modules clean
	$(MAKE) --directory third_party/fmt clean
	$(MAKE) --directory third_party/json11 clean
	rm -rf ${LIBDIR}
	rm -rf ${BINDIR}
