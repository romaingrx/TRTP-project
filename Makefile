C_MAIN     = src/main.c
C_PRACTICE =  src/practice.c
C_PACKET   = src/packet_implem.c
H_PACKET   = src/packet_interface.h
C_RECEIVE  = src/receive.c
H_RECEIVE  = src/receive.h
C_QUEUE    = src/queue.c
H_QUEUE    = src/queue.h

O_MAIN     = main.o
O_PRACTICE = practice.o
O_PACKET   = packet.o
O_QUEUE    = queue.o
O_RECEIVE  = receive.o

EXEC_PRACTICE = practice
EXEC_RECEIVE  = receive
EXEC_MAIN     = main

ERR_FILE = src/stderr.txt
VAR = "192.168.1.5 7903"

COMP = @gcc






default : $(EXEC_PRACTICE)
	@./$(EXEC_PRACTICE) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;

clean :
	@rm -f *.o
	@rm -f $(EXEC_MAIN) $(EXEC_RECEIVE) $(EXEC_PRACTICE) *.o


$(EXEC_MAIN) : $(O_MAIN) $(O_RECEIVE)
	$(COMP) -o $(EXEC_MAIN) $(O_MAIN) $(O_RECEIVE)

$(EXEC_RECEIVE) : $(O_RECEIVE) $(O_QUEUE)
	$(COMP) -o $(EXEC_RECEIVE) $(O_RECEIVE) $(O_QUEUE)

$(EXEC_PRACTICE) : $(O_PRACTICE) $(O_PACKET)
	$(COMP) -o $(EXEC_PRACTICE) $(O_PRACTICE) $(O_PACKET)

$(O_MAIN) : $(C_MAIN) $(H_RECEIVE)
	$(COMP) -c $(C_MAIN) -o $(O_MAIN)

$(O_RECEIVE) : $(C_RECEIVE) $(H_RECEIVE) $(H_QUEUE)
	$(COMP) -c $(C_RECEIVE) -o $(O_RECEIVE)

$(O_QUEUE) : $(C_QUEUE) $(H_QUEUE) $(H_PACKET)
	$(COMP) -c $(C_QUEUE) -o $(O_QUEUE)

$(O_PACKET) : $(C_PACKET) $(H_PACKET)
	$(COMP) -c $(C_PACKET) -o $(O_PACKET)

$(O_PRACTICE) : $(C_PRACTICE) $(H_PACKET) $(H_QUEUE) $(H_RECEIVE)
	$(COMP) -c $(C_PRACTICE) -o $(O_PRACTICE)
