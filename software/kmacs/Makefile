CFLAGS = -Isrc/include -Isrc/RMQ

all: 
		g++ $(CFLAGS) -O3 -fomit-frame-pointer -fprefetch-loop-arrays -DNDEBUG src/parser.cpp src/kmacs.cpp src/run.cpp src/sais.c src/RMQ/RMQ_succinct.cpp -o kmacs 
