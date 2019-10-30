C_MAIN     = src/main.c
C_PRACTICE = src/practice.c
C_TESTS    = tests/test.c
C_PACKET   = src/packet.c
H_PACKET   = src/packet.h
C_RECEIVE  = src/receive.c
H_RECEIVE  = src/receive.h
C_QUEUE    = src/queue.c
H_QUEUE    = src/queue.h

O_MAIN     = main.o
O_PRACTICE = practice.o
O_PACKET   = packet.o
O_QUEUE    = queue.o
O_RECEIVE  = receive.o
O_TESTS    = tests.o

EXEC_PRACTICE = prac
EXEC_RECEIVE  = receive
EXEC_MAIN     = receiver
EXEC_TESTS    = test

ERR_FILE = src/stderr.txt
VAR = ":: 8555"

COMP = @gcc
FLAGS = -lz -lm
WFLAGS = -Werror -Wall -std=c99

.PHONY : clean o_clean exec_clean


default : $(EXEC_MAIN)

tests : $(EXEC_TESTS)
	@./$(EXEC_TESTS) $(VAR) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;
	@make o_clean

practice : $(EXEC_PRACTICE)
	@./$(EXEC_PRACTICE) $(VAR) 2> $(ERR_FILE)
	@if [ -f $(ERR_FILE) ]; then if [ -s $(ERR_FILE) ]; then open $(ERR_FILE); fi ; fi;
	@make o_clean

zip : pdf
	rm -f projet1_Delcoigne_Graux.zip
	git log --stat > gitlog.stat
	zip -r projet1_Delcoigne_Graux Makefile src/ tests/ rapport.pdf gitlog.stat

clean : o_clean exec_clean

o_clean :
	rm -f *.aux *.log *.pdf
	rm -f *.o *.out gitlog.stat File*
exec_clean :
	rm -f $(EXEC_MAIN) $(EXEC_RECEIVE) $(EXEC_PRACTICE)

pdf :
	pdflatex Latex/rapport.tex
	open rapport.pdf
	@echo "prout"


$(EXEC_MAIN) : $(O_MAIN) $(O_RECEIVE) $(O_PACKET) $(O_QUEUE)
	$(COMP) -o $(EXEC_MAIN) $(O_MAIN) $(O_RECEIVE) $(O_PACKET) $(O_QUEUE) $(WFLAGS) $(FLAGS)

$(EXEC_RECEIVE) : $(O_RECEIVE) $(O_QUEUE) $(O_PACKET)
	$(COMP) -o $(EXEC_RECEIVE) $(O_RECEIVE) $(O_QUEUE) $(O_PACKET) $(FLAGS) $(WFLAGS)

$(EXEC_TESTS) : $(O_TESTS) $(O_PACKET) $(O_QUEUE) $(O_RECEIVE)
	$(COMP) -o $(EXEC_TESTS) $(O_TESTS) $(O_PACKET) $(O_QUEUE) $(O_RECEIVE) $(FLAGS) $(WFLAGS)

$(EXEC_PRACTICE) : $(O_PRACTICE) $(O_PACKET) $(O_QUEUE) $(O_RECEIVE)
	$(COMP) -o $(EXEC_PRACTICE) $(O_PRACTICE) $(O_PACKET) $(O_QUEUE) $(O_RECEIVE) $(FLAGS) $(WFLAGS)

$(O_MAIN) : $(C_MAIN) $(H_RECEIVE)
	$(COMP) -c $(C_MAIN) -o $(O_MAIN) $(WFLAGS)

$(O_RECEIVE) : $(C_RECEIVE) $(H_RECEIVE) $(H_QUEUE) $(H_PACKET)
	$(COMP) -c $(C_RECEIVE) -o $(O_RECEIVE) $(WFLAGS)

$(O_QUEUE) : $(C_QUEUE) $(H_QUEUE) $(H_PACKET)
	$(COMP) -c $(C_QUEUE) -o $(O_QUEUE) $(WFLAGS)

$(O_PACKET) : $(C_PACKET) $(H_PACKET)
	$(COMP) -c $(C_PACKET) -o $(O_PACKET) $(WFLAGS)

$(O_TESTS) : $(C_TESTS) $(H_PACKET) $(H_QUEUE) $(H_RECEIVE)
	$(COMP) -c $(C_TESTS) -o $(O_TESTS) $(WFLAGS)

$(O_PRACTICE) : $(C_PRACTICE) $(H_PACKET) $(H_QUEUE) $(H_RECEIVE)
	$(COMP) -c $(C_PRACTICE) -o $(O_PRACTICE) $(WFLAGS)
