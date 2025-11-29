OBJS = demo.o w_drawingTool.o
ms: $(OBJS)
	gcc -g -Wall -Wextra -pedantic $(OBJS) -o ms && ./ms

msAdd: $(OBJS)
	gcc -g -Wall -Wextra -pedantic -fsanitize=address $(OBJS) -o msAdd && ./msAdd
demo.o: demo.c w_drawingTool.h
	gcc -g -Wall -Wextra -pedantic -c demo.c -o $@
w_drawingTool.o: w_drawingTool.c w_drawingTool.h
	gcc -g -Wall -Wextra -pedantic -c w_drawingTool.c -o $@

clean:
	rm -f ${OBJS}