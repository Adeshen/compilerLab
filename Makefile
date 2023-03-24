VERSION=lab2
BRANCH=main
MES=test git push

OBJS = parser.o driver.o token.o main.o 
LLVMCONFIG = llvm-config
CPPFLAGS =  -std=c++11 -fpermissive
LDFLAGS =  -lpthread -ldl -lz -lncurses -rdynamic
LIBS = ` `
TEST_PATH=./Test
WARN_FLAG= --no-warnings
CASE=$(find $(TEST_PATH) -name *.cmm)
MAIN_CASE=2.cmm

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
	

test: clean parser 
	@echo ;echo
	@echo _________TEST $(MAIN_CASE) TEST_____________
	@cat $(TEST_PATH)/$(MAIN_CASE)
	@echo ;echo

	@./parser $(TEST_PATH)/$(MAIN_CASE)
	

git:
	git add .
	git commit -m "$(MES)"
	git push github $(BRANCH) 

.PHONY: clean  all test



