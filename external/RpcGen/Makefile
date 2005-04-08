
CC = gcc

CFLAGS = -Dinline=inline_flag -DVXWORKS

TARGET=MyRpcGen

OBJECTS = rpc_clntout.o rpc_cout.o rpc_hout.o rpc_main.o rpc_parse.o \
	  rpc_scan.o rpc_svcout.o rpc_tblout.o rpc_util.o rpc_sample.o

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

build: $(TARGET)


destroy:
	rm -f *.o $(TARGET)


