# ================================
# | core flags
# ================================
CC       = gcc
CFLAGS   = -g -Wall -Wextra -pedantic
SAN_FLAG = -fsanitize=address
DEV_FLAG = -DENABLE_DEVKIT
FUCKING_EVIL_DASTARDLY = -Werror # fuck you Werror

# ================================
# | sources & such
# ================================
CORE_SRC = w_drawingTool.c
CORE_OBJ = $(CORE_SRC:.c=.o)

DEVKIT_SRC = w_devkit.c
DEVKIT_OBJ = $(DEVKIT_SRC:.c=.o)

DEMO_SRC = demo.c
DEMO_OBJ = $(DEMO_SRC:.c=.o)

WATCH_SRC = watcher.c
WATCH_OBJ = $(WATCH_SRC:.c=.o)
# ig? seems kinda overkill but im tired of edit this file brahhh

# ================================
# | targets or whatever they called
# ================================

all: demo watch

# no devkit
demo-raw: $(CORE_OBJ) $(DEMO_OBJ)
	$(CC) $(CFLAGS) $(DEMO_OBJ) $(CORE_OBJ) -o demo-raw

# devkit -> devkit + addressSanatizer
demo: $(DEVKIT_OBJ) $(CORE_OBJ) $(DEMO_OBJ)
	$(CC) $(CFLAGS) $(SAN_FLAG) $(DEV_FLAG)  $(DEMO_OBJ) $(CORE_OBJ) $(DEVKIT_OBJ) -o demo-dev

watch: $(WATCH_OBJ)
	$(CC) $(CFLAGS) $(WATCH_OBJ) -o wwatch

#patterns?

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(CORE_OBJ) $(DEVKIT_OBJ) $(DEMO_OBJ) $(WATCH_OBJ) wwatch.o 
cleanf: clean
	rm -f demo-raw demo-dev wwatch

#short for make run demo devkit + run
rd: demo
	./demo-dev
#run watch
rw: watch
	./wwatch

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