NAME=gpu
SRC=$(wildcard *.cpp)
OBJS=$(SRC:.cpp=.o)

ifeq ($(BUILD_OPENCL), 1)
SRC := $(SRC) $(wildcard opencl/*.cpp)
endif

all:    ${SRC}
	for file in ${SRC} ; do\
		${CXX} -c $$file ${INCLUDE} -I${OPENCL_INCLUDE} ${CXXFLAGS};\
	done
	mkdir -p ${LIBDIR}
	ar rvs ${LIBDIR}/lib$(NAME).a *.o

clean:
	rm -rf *.o
