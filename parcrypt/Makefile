CPPSRC:=$(wildcard *.cpp)

all:
	${CXX} -o parcrypt.bin ${CPPSRC} ${INCLUDE} -I${OPENCL_INCLUDE} ${CXXFLAGS} ${LIBS} -L${OPENCL_LIB} -ljson11 -lfmt -lutil -lparcrypt -lmodules -lparcrypt -lmodules -laddress -lsecp256k1 -lpthread -lgpu -llogger -lplatform -lstdc++fs -ljson11 -lcurl -ldl
	mkdir -p $(BINDIR)
	cp parcrypt.bin $(BINDIR)/parcrypt

clean:
	rm -rf parcrypt.bin
