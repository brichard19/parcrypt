NAME=modules

all: cl
	for file in $(shell ls btc_pub_key_hash/*.cpp) ; do\
		${CXX} -c $$file -I./ -Ibtc_pub_key_hash ${INCLUDE} ${CXXFLAGS};\
	done
	mkdir -p ${LIBDIR}
	ar rvs ${LIBDIR}/lib$(NAME).a *.o

cl:
	${PYTHON} ../tools/concat.py btc_pub_key_hash/secp256k1.cl btc_pub_key_hash/public_key_hash.cl > btc_pub_key_hash/btc_pub_key_hash.cl
	${PYTHON} ../tools/embed_string.py --input btc_pub_key_hash/btc_pub_key_hash.cl --symbol _btc_pub_key_hash_cl_source --output btc_pub_key_hash/btc_pub_key_hash_cl.cpp
clean:
	rm -rf *.o
