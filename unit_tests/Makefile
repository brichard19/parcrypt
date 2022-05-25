CPPSRC:=$(wildcard *.cpp)

all:
	${CXX} -o unittests.bin ${CPPSRC} ${INCLUDE} -I${OPENCL_INCLUDE} ${CXXFLAGS} ${LIBS} -L${OPENCL_LIB} -lfmt -lmodules -lfmt -lutil -lparcrypt -laddress -lsecp256k1 -lpthread -lgpu -llogger -lplatform -lstdc++fs -ljson11 -lcurl -ldl
	mkdir -p $(BINDIR)
	cp unittests.bin $(BINDIR)/unittests

clean:
	rm -rf unittests.bin
