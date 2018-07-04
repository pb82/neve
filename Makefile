BIN = neve
TESTBIN = unit
LIB = -I/usr/include/libbson-1.0 -luv -llua -lbson-1.0

.PHONY: first
first:
	@echo "Run 'make all' to build the project"

.PHONY: check
check:
	cat -e -t -v  Makefile

vendor/http_parser.o: vendor/http_parser.c vendor/http_parser.h
	$(CXX) -g -c -fPIC vendor/http_parser.c -o vendor/http_parser.o

loop/loop.o: loop/loop.cpp loop/loop.hpp
	$(CXX) -g -c -fPIC loop/loop.cpp -o loop/loop.o

http/path.o: http/path.cpp http/path.hpp
	$(CXX) -g -c -fPIC http/path.cpp -o http/path.o

http/router.o: http/router.cpp http/router.hpp
	$(CXX) -g -c -fPIC http/router.cpp -o http/router.o

http/response.o: http/response.cpp http/response.hpp
	$(CXX) -g -c -fPIC http/response.cpp -o http/response.o

logger/logger.o: logger/logger.cpp logger/logger.hpp
	$(CXX) -g -c -fPIC logger/logger.cpp -o logger/logger.o

json/value.o: json/value.cpp json/value.hpp
	$(CXX) -g -c -fPIC json/value.cpp -o json/value.o

json/parser.o: json/parser.cpp json/parser.hpp
	$(CXX) -g -c -fPIC json/parser.cpp -o json/parser.o

json/printer.o: json/printer.cpp json/printer.hpp
	$(CXX) -g -c -fPIC json/printer.cpp -o json/printer.o

config/config.o: config/config.cpp config/config.hpp
	$(CXX) -g -c -fPIC config/config.cpp -o config/config.o

actions/compiler.o: actions/compiler.cpp actions/compiler.hpp
	$(CXX) -g -c -fPIC actions/compiler.cpp -o actions/compiler.o

actions/sandbox.o: actions/sandbox.cpp actions/sandbox.hpp
	$(CXX) -g -c -fPIC actions/sandbox.cpp -o actions/sandbox.o

jobs/ping.o: jobs/ping.cpp jobs/ping.hpp
	$(CXX) -g -c -fPIC jobs/ping.cpp -o jobs/ping.o

jobs/create.o: jobs/create.cpp jobs/create.hpp
	$(CXX) -g -c -fPIC jobs/create.cpp -o jobs/create.o

jobs/run.o: jobs/run.cpp jobs/run.hpp
	$(CXX) -g -c -fPIC jobs/run.cpp -o jobs/run.o

jobs/get.o: jobs/get.cpp jobs/get.hpp
	$(CXX) -g -c -fPIC jobs/get.cpp -o jobs/get.o

jobs/delete.o: jobs/delete.cpp jobs/delete.hpp
	$(CXX) -g -c -fPIC jobs/delete.cpp -o jobs/delete.o

jobs/update.o: jobs/update.cpp jobs/update.hpp
	$(CXX) -g -c -fPIC jobs/update.cpp -o jobs/update.o

jobs/uuid.o: jobs/uuid.cpp jobs/uuid.hpp
	$(CXX) -g -c -fPIC jobs/uuid.cpp -o jobs/uuid.o

persistence/cache.o: persistence/cache.cpp persistence/cache.hpp
	$(CXX) -g -c -fPIC persistence/cache.cpp -o persistence/cache.o

plugins/registry.o: plugins/registry.cpp plugins/registry.hpp plugins/plugin.hpp
	$(CXX) -g -c -fPIC plugins/registry.cpp -o plugins/registry.o

all: json/printer.o json/parser.o logger/logger.o http/path.o \
		loop/loop.o vendor/http_parser.o http/router.o http/response.o \
		json/value.o actions/compiler.o jobs/ping.o jobs/create.o config/config.o \
		jobs/uuid.o jobs/list.o jobs/run.o persistence/cache.o actions/sandbox.o \
		jobs/get.o jobs/delete.o jobs/update.o plugins/registry.o main.o
	$(CXX) $(LIB) json/printer.o json/parser.o logger/logger.o \
		http/path.o http/router.o http/response.o loop/loop.o vendor/http_parser.o \
		json/value.o actions/compiler.o jobs/ping.o jobs/create.o config/config.o \
		jobs/list.o jobs/run.o jobs/delete.o persistence/cache.o actions/sandbox.o \
		jobs/uuid.o jobs/get.o jobs/update.o plugins/registry.o \
		main.o -pipe -g -Wall -W -fPIC -o $(BIN)

# =============
# Tests section
# =============
tests/main.o: tests/main.cpp
	$(CXX) -c tests/main.cpp -o tests/main.o

.PHONY: tests
tests: tests/main.o tests/t_http_path.cpp http/path.o
	$(CXX) $(LIB) -pipe -g -Wall -W -fPIC \
		http/path.o \
		json/value.o \
		tests/t_http_path.cpp \
		tests/main.o -o $(TESTBIN)
	@./$(TESTBIN)
# =================
# End tests section
# =================

# ===============
# Plugins section
# ===============

MONGO_PLUGIN_CXX_ARGS = -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -lmongoc-1.0 -lbson-1.0

plugins/default/mongo/plugin_mongo.so: plugins/default/mongo/plugin_mongo.hpp \
	plugins/default/mongo/plugin_mongo.cpp json/printer.o json/value.o \
	plugins/default/mongo/intents/intent.hpp plugins/default/mongo/intents/create.hpp plugins/default/mongo/intents/create.cpp \
	plugins/default/mongo/intents/list.hpp plugins/default/mongo/intents/list.cpp \
	plugins/default/mongo/intents/read.hpp plugins/default/mongo/intents/read.cpp \
	plugins/default/mongo/intents/delete.hpp plugins/default/mongo/intents/delete.cpp
	$(CXX) -g -pipe -shared -fPIC $(MONGO_PLUGIN_CXX_ARGS) \
	plugins/default/mongo/plugin_mongo.cpp json/printer.o json/value.o \
	plugins/default/mongo/intents/create.cpp \
	plugins/default/mongo/intents/list.cpp \
	plugins/default/mongo/intents/read.cpp \
	plugins/default/mongo/intents/delete.cpp \
	plugins/default/mongo/intents/update.cpp \
	-o plugins/default/mongo/plugin_mongo.so

plugins/default/skeleton/plugin_skeleton.so: plugins/default/skeleton/plugin_skeleton.hpp plugins/default/skeleton/plugin_skeleton.cpp
	$(CXX) -g -pipe -shared -fPIC \
	plugins/default/skeleton/plugin_skeleton.cpp \
	-o plugins/default/skeleton/plugin_skeleton.so

.PHONY: plugins
plugins: plugins/default/mongo/plugin_mongo.so \
	plugins/default/skeleton/plugin_skeleton.so
	@echo "All plugins built"

# ===================
# End Plugins section
# ===================

.PHONY: clean
clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f **/*.o
