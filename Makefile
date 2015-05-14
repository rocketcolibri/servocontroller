LIBS = -lrt
CFLAGS = -g
include json-c/Makefile

all: servocontroller

OBJ = CommandLineArguments.o \
      Connection.o \
      ConnectionContainer.o \
      ConnectionFsm.o \
      ConnectionFsmTest.o \
      ITelemetrySource.o \
      main.o \
      MessageReceiver.o \
      MessageSinkCdc.o \
      ServoDriver.o \
      ServoDriverMock.o \
      ServoDriverLinino.o \
      ServoDriverRPi.o \
      SystemFsm.o \
      SystemFsmTest.o \
      TelemetrySourceVideoStream.o \
      TransmitTelemetry.o \
      base/avltree.o \
      base/btree.o \
      base/DBG.o \
      base/deque.o \
      base/dlist.o \
      base/GEN.o \
      base/heap.o \
      base/linklist.o \
      base/memchunk.o \
      base/MON.o \
      base/Reactor.o \
      base/TimerFd.o \
      base/TRC.o \
      base/TRC_File.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

servocontroller: $(OBJ) $(LIBJSON_C_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o
	rm -f $(LIBJSON_C_DIR)/*.o
	rm -f base/*.o
	rm -f servocontroller

