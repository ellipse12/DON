CC=g++
build=build
incl:=-Iinclude/ 
flags=-g -std=c++20 $(incl)
src=src
out=out
libs := 
files := $(subst src, build, $(patsubst %.cpp, %.o, $(wildcard $(src)/*.cpp)))
$(build)/%.o: $(src)/%.cpp
	$(CC) -c $(flags) -o $(build)/$*.o $(src)/$*.cpp $(libs)
$(build)/$(out): $(files) $(libs) $(build)
	$(CC) $(flags) -o $(build)/$(out) $(filter-out $(build), $^) 
	
$(build):
	mkdir build
.PHONY: clean
clean:
	rm -rf build
.PHONY: run
run:
	./$(build)/$(out)
