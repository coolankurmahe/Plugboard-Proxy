pbproxy: pbproxy.o
	gcc -w pbproxy.o -o pbproxy -lm -lcrypto -lssl

pbproxy.o: pbproxy.c
	gcc -c -w  pbproxy.c -o pbproxy.o

clean:
	-rm -f pbproxy.o
	-rm -f pbproxy
