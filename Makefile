all: cmcli

cmcli: cmcli.o confmatrix.o ulog.o
	gcc -o cmcli cmcli.o confmatrix.o ulog.o

clean:
	rm -f *.o && rm -f cmcli
