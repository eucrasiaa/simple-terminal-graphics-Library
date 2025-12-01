# ================================
# | core flags
# ================================
CC       = gcc
CFLAGS   = -g -Wall -Wextra -pedantic -Iinclude
SAN_FLAG = -fsanitize=address
DEV_FLAG = -DENABLE_DEVKIT
FUCKING_EVIL_DASTARDLY = -Werror # fuck you Werror

# ================================
# | folder. its just folder
# ================================

SRC_DIR   = src
INC_DIR   = include
OBJ_DIR   = build
TESTS_DIR = tests # unused at moment

# ================================
# | sources & such
# ================================
CORE_SRC = $(SRC_DIR)/w_drawingTool.c $(SRC_DIR)/w_types.c
CORE_OBJ = $(CORE_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) #DAMNNNNN

# basically const unless specifically modified so can be lieral
TYPES_SRC    = $(SRC_DIR)/w_types.c
TYPES_OBJ    = $(OBJ_DIR)/w_types.o

DEVKIT_SRC = $(SRC_DIR)/w_devkit.c
DEVKIT_OBJ = $(OBJ_DIR)/w_devkit.o

DEMO_SRC = demo.c #just chillin in root for now
DEMO_OBJ = $(OBJ_DIR)/demo.o

WATCH_SRC = $(SRC_DIR)/watcher.c
WATCH_OBJ = $(OBJ_DIR)/watcher.o

OBJ_DEV = $(CORE_OBJ) $(TYPES_OBJ) $(DEVKIT_OBJ) $(DEMO_OBJ) $(WATCH_OBJ) #core tester requires: core, types, devkit, demo itself, and watcher
# ig? seems kinda overkill but im tired of edit this file brahhh

# ================================
# | targets or whatever they called
# ================================

all: demo watch

# # no devkit. do manual demo.o b/c flag stuff?
# demo-raw: $(CORE_OBJ) 
# 	$(CC) $(CFLAGS) $(DEMO_OBJ) $(CORE_OBJ) -o demo-raw

# devkit -> devkit + addressSanatizer
demo: $(DEVKIT_OBJ) $(CORE_OBJ) $(DEMO_OBJ)
	$(CC) $(CFLAGS) $(SAN_FLAG) $(DEV_FLAG) $(DEMO_OBJ) $(CORE_OBJ) $(DEVKIT_OBJ) -o demo-dev
#it gets to a point.


watch: $(WATCH_OBJ)
	$(CC) $(CFLAGS) $(WATCH_OBJ) -o wwatch

#build stuff cause folders now im moving up in this world
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
# .c -> .o into build
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/demo.o: demo.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEV_FLAG) -c $< -o $@
# demo is held in root and can just build there too for now

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean: 
# 	rm -f $(CORE_OBJ) $(DEVKIT_OBJ) $(DEMO_OBJ) $(WATCH_OBJ) wwatch.o 
# cleanf: clean
# 	rm -f demo-raw demo-dev wwatch

# clean:
# 	rm -rf $(OBJ_DIR) 
#short for make run demo devkit + run
rd: demo
	./demo-dev
#run watch
rw: watch
	./wwatch

cleanobj:
	rm -rf $(OBJ_DIR)
# DEMO_OBJS= demo.o w_drawingTool.o
# CC = gcc
# CFLAGS = -g -Wall -Wextra -pedantic
# CFLAG_SANITIZE = -fsanitize=address
# CFLAG_EVIL = -Werror # treat warnings as errors, only at end really
# DEVMODE = -DENABLE_DEVKIT


# # just the demo
# rawdemo: $(DEMO_OBJS)
# 	$(CC) $(CFLAGS) $(DEMO_OBJS) -o demo && ./demo

# demo.o: demo.c w_drawingTool.h
# 	$(CC) $(CFLAGS) -c demo.c -o $@
# w_drawingTool.o: w_drawingTool.c w_drawingTool.h
# 	$(CC) $(CFLAGS) -c w_drawingTool.c -o $@

# wwatch: watcher.c
# 	$(CC) $(CFLAGS) watcher.c -o wwatch

# wwatch.o: watcher.c
# 	$(CC) $(CFLAGS) -c watcher.c -o $@

# w_devkit.o: w_devkit.c w_devkit.h
# 	$(CC) $(CFLAGS) -c w_devkit.c -o $@
# clean:
# 	rm -f ${DEMO_OBJS} wwatch.o 
# # compile w_drawingTool
# # next, compile w_devkit 
# msD: w_devkit.o $(DEMO_OBJS)
# 	$(CC) $(CFLAGS) w_devkit.o -o w_devkit
# 	$(CC) $(CFLAGS) $(CFLAG_SANITIZE) $(DEVMODE) $(DEMO_OBJS) w_devkit.o -o msD

# #includes:
# # basically everything includes w_drawingTool.h, and w_drawingTool.h includes most of the standard library includes
# # note, is it worth moving structs to a seperate header maybe as opposed to w_drawingTool.h in everything? answer me
# # demo: w_drawingTool.h, (ifdef ENABLE_DEVKIT -> w_devkit.h)
# # w_drawingTool.o: w_drawingTool.h
# # w_devkit.o: w_devkit.h, w_drawingTool.h (includes)
# # watcher: w_drawingTool.h, w_devkit.h