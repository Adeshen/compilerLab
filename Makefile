VERSION=lab2
BRANCH=main
MES=design 

OBJS = parser.o driver.o token.o main.o semantic_symbol_struct.o semantic.o
LLVMCONFIG = llvm-config
CPPFLAGS =  -std=c++11 -fpermissive   -Wall -c
LDFLAGS =  -lpthread -ldl -lz -lncurses -rdynamic
LIBS = ` `
TEST_PATH=./Test
WARN_FLAG= --no-warnings
CASE=$(find $(TEST_PATH) -name *.cmm)
MAIN_CASE=test1.cmm

parser: $(OBJS)
	g++ -o $@ $(OBJS)  $(LDFLAGS) $(WARN_FLAG)


clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp location.hh position.hh  stack.hh $(OBJS) token.cpp
	   
parser.cpp:parser.y
	bison -d -o $@ $^ 

parser.hpp:parser.cpp

token.cpp:token.l parser.hpp
	flex -o $@ $^ 

$.o: $.cpp
	g++ -c $(CPPFLAGS) -o $@ $<


build:clean  parser

all: build

$(CASE):
	

test:  parser 
	@echo ;echo
	@echo _________TEST $(MAIN_CASE) TEST_____________
	@cat $(TEST_PATH)/$(MAIN_CASE)
	@echo ;echo

	@./parser $(TEST_PATH)/$(MAIN_CASE)


seman:
	g++ -o semantic.o semantic.cpp -c
	g++ -o semantic_symbol_struct.o semantic_symbol_struct.cpp -c
	g++ -o seman.o  semantic_symbol_struct.o  semantic.o
git:
	git add .
	git commit -m "$(MES)"
	git push github $(BRANCH) 

.PHONY: clean  all test



