PRACTICE_PATH = src/practice.c
PACKET_IMPLEM_PATH = src/packet_implem.c
PACKET_INTERFACE_PATH = src/packet_interface.h
EXEC_NAME = Ex

default : exec clean
	@./$(EXEC_NAME)


exec : practice.o packet.o
	@gcc -o $(EXEC_NAME) practice.o packet.o

practice.o : $(PRACTICE_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(PRACTICE_PATH) -o practice.o

packet.o : $(PACKET_IMPLEM_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(PACKET_IMPLEM_PATH) -o packet.o

clean :
	@rm *.o
