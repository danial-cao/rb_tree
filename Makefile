

rb_tree: main.o rb_tree.o
	gcc main.o rb_tree.o rb_tree.h -o rb_tree 

main.o: main.c rb_tree.h
	gcc -c -o main.o main.c

rb_tree.o: rb_tree.c rb_tree.h
	gcc -c -o rb_tree.o rb_tree.c

clean:
	rm -f *.o
	rm -f rb_tree

