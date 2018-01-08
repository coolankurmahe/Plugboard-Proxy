

Description of Implementation:

I have developed the plugboard proxy  named pbproxy written in C using the OpenSSL  library
The user can specify options etc and arguments will be captured by getopt().
AES in Ctr mode encryption is used for to and fro communication  from client and proxy server .
SSH is working with all these functionalities.

Please start with the make command to execute makefile so that pbproxy executable gets created.
Make clean is used to clean all output files.

I have printed the packets in the exact format described by professor.

Example Commands and their outputs are as follows :

1) 
Client :
ssh -o "ProxyCommand ./pbproxy -k "enckey.txt" localhost 2222" localhost

Proxy Server:
./pbproxy -k "enckey.txt" -l 2222 localhost 22

The above commands will login ssh from client to server. The to and fro encryption from client to proxy server will  be done based on keyhole  mentioned after -k.


2) 
Proxy Server:
./pbproxy -k "enckey.txt" -l 2222 localhost 2240

Actual Server: 
nc -l -p 2240

Client :
./pbproxy -k "enckey.txt" localhost 2222


The above commands will complete the communication from client to pbproxy , pbproxy to actual server and then from actual server to pbproxy .
pbproxy to client.Any input on client can be seen be seen on actual server and vice-versa.


References Used:
http://www.gurutechnologies.net/blog/aes-ctr-encryption-in-c/

http://www.geeksforgeeks.org/socket-programming-cc/

http://www.thegeekstuff.com/2012/07/c-file-handling/

https://stackoverflow.com/questions/8107271/c-unix-sockets-non-blocking-read

