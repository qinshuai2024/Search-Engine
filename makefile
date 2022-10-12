SERVER=http_server
PARSER=parser
cc=g++
.PHONY:all
all:$(PARSER) $(SERVER)

$(PARSER): parser.cc
	$(cc) -o $@ $^ -std=c++11 -lboost_system -lboost_filesystem 
$(SERVER): http_server.cc
	$(cc) -o $@ $^ -std=c++11 -ljsoncpp -lpthread

.PHONY:clean
clean:
	rm -f $(PARSER) $(SERVER)