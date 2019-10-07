PRACTICE_PATH = src/practice.c
PACKET_IMPLEM_PATH = src/packet_implem.c
PACKET_INTERFACE_PATH = src/packet_interface.h
EXEC_NAME = Ex
ERR_FILE = src/stderr.txt

TEST_PATH = tests/test.c
TEST_NAME = Test

default : exec clean
	@./$(EXEC_NAME) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;

test : test_ex clean
	@./$(TEST_NAME) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;



exec : practice.o packet.o
	@gcc -o $(EXEC_NAME) practice.o packet.o

test_ex : test.o packet.o
	@gcc -o $(TEST_NAME) test.o packet.o


practice.o : $(PRACTICE_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(PRACTICE_PATH) -o practice.o

packet.o : $(PACKET_IMPLEM_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(PACKET_IMPLEM_PATH) -o packet.o

test.o : $(TEST_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(TEST_PATH) -o test.o

clean :
	@rm *.o
