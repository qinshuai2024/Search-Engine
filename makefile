parser: parser.cc
	g++ -o $@ $^ -std=c++11 -lboost_system -lboost_filesystem 

.PHONY:clean
clean:
	rm -f parser