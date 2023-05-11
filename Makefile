VERSION=lab2
BRANCH=main
MES=design 

OBJS = parser.o driver.o token.o main.o semantic_symbol_struct.o semantic.o inter_struct.o inter.o 
LLVMCONFIG = llvm-config
CPPFLAGS =  -std=c++11 -fpermissive   -Wall -c -g
LDFLAGS =  -lpthread -ldl -lz -lncurses -rdynamic
LIBS = ` `
TEST_PATH=./Test2
WARN_FLAG= --no-warnings
CASE=$(find $(TEST_PATH) -name *.cmm)
MAIN_CASE=test1.cmm

parser: $(OBJS)
	g++ -o $@ $(OBJS)  $(LDFLAGS) $(WARN_FLAG) 

gdb:parser
	gdb ./parser -x command.gdb
# run ./Test/test3.cmm

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp location.hh position.hh  stack.hh $(OBJS) token.cpp
	   
parser.cpp:parser.y
	bison -d -o $@ $^ 

parser.hpp:parser.cpp

token.cpp:token.l parser.hpp
	flex -o $@ $^ 

$.o: $.cpp
	g++  $(CPPFLAGS) -o $@ $< 


build:clean  parser

all: build


test_lab3:parser 
	@echo ;echo
	@echo _________TEST $(MAIN_CASE) TEST_____________
	@cat Test3/$(MAIN_CASE)
	@echo ;echo

	@./parser Test3/$(MAIN_CASE) test.out
	

test_lab2:  parser 
	@echo ;echo
	@echo _________TEST $(MAIN_CASE) TEST_____________
	@cat $(TEST_PATH)/$(MAIN_CASE)
	@echo ;echo
	@./parser $(TEST_PATH)/$(MAIN_CASE)


testCode:semantic_symbol_struct.o  TestCode/semantic_test.o
	g++ -o TestCode/semantic TestCode/semantic_test.o  semantic_symbol_struct.o
	gdb ./TestCode/semantic
	
git:
	git add .
	git commit -m "$(MES)"
	git push github $(BRANCH) 

.PHONY: clean  all test_lab2 test_lab3  git  gdb



