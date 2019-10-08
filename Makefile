PACKET_IMPLEM_PATH = src/packet_implem.c
PACKET_INTERFACE_PATH = src/packet_interface.h
PRACTICE_PATH = src/practice.c
TEST_PATH = tests/test.c
RECIEVE_PATH = src/receive.c
MAIN_PATH = src/main.c

ERR_FILE = src/stderr.txt

PRACTICE_NAME = practice
TEST_NAME = test
RECIEVE_NAME = receive
MAIN_NAME = main

VAR = default


default : practice_exec
	@./$(PRACTICE_NAME) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;

test : test_exec
	@./$(TEST_NAME) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;

# UTILISER 'make main VAR=""' en mettant les arguments de la main dans les crochets de VAR
main : main_exec
	@./$(MAIN_NAME) $(VAR) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;

receive : receive_exec
	@./$(RECIEVE_NAME)
#	@./$(RECIEVE_NAME) 2> $(ERR_FILE)
#	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;

main_exec : main.o packet.o
	@gcc -o $(MAIN_NAME) main.o packet.o

receive_exec : $(RECIEVE_PATH)
	@gcc -o $(RECIEVE_NAME) $(RECIEVE_PATH) -lm

practice_exec : practice.o packet.o
	@gcc -o $(PRACTICE_NAME) practice.o packet.o

test_exec : test.o packet.o
	@gcc -o $(TEST_NAME) test.o packet.o

main.o : $(MAIN_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(MAIN_PATH) -o main.o

practice.o : $(PRACTICE_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(PRACTICE_PATH) -o practice.o

packet.o : $(PACKET_IMPLEM_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(PACKET_IMPLEM_PATH) -o packet.o

test.o : $(TEST_PATH) $(PACKET_INTERFACE_PATH)
	@gcc -c $(TEST_PATH) -o test.o

clean :
	@rm -f *.o
	@rm -f $(PRACTICE_NAME) $(TEST_NAME) $(RECIEVE_NAME) $(MAIN_NAME)
