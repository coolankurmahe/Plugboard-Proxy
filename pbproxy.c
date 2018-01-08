// Server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<ctype.h>
#include<unistd.h>
#include<pthread.h>


#include <openssl/aes.h>
#include <openssl/rand.h> 
#include <openssl/hmac.h>
#include <openssl/buffer.h>

#include <math.h>
#include<fcntl.h>
#define PORT 8080

struct ctr_state 
{ 
	unsigned char ivec[AES_BLOCK_SIZE];	 
	unsigned int num; 
	unsigned char ecount[AES_BLOCK_SIZE]; 
}; 

AES_KEY key; 

int bytes_read, bytes_written;	 
unsigned char indata[10000]; 
unsigned char indata1[10000];
unsigned char outdata[10000];
unsigned char comboutdata[10000];
unsigned char iv[AES_BLOCK_SIZE];
unsigned char keydata[AES_BLOCK_SIZE];



unsigned char buffer2[10000] ;

struct ctr_state state;	 

int valread;

int getLength(char *st)
{
	int i;
	int len = 0;
	for(i=0;st[i]!='\0';i++)
	{	
		len++;
	}
	return len;
}
int init_ctr(struct ctr_state *state, const unsigned char iv[16])
{		 
	/* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
     * first call. */
    state->num = 0;
    memset(state->ecount, 0, AES_BLOCK_SIZE);

    /* Initialise counter in 'ivec' to 0 */
    memset(state->ivec + 8, 0, 8);

    /* Copy IV into 'ivec' */
    memcpy(state->ivec, iv, 8);
}



void fencrypt(unsigned char *st,const unsigned char* enc_key,int readbytes)
{ 
    if(!RAND_bytes(iv, AES_BLOCK_SIZE))
    {
        fprintf(stderr, "Could not create random bytes.");
        exit(1);    
    }
      
	//Initializing the encryption KEY
    if (AES_set_encrypt_key(enc_key, 128, &key) < 0)
    {
        fprintf(stderr, "Could not set encryption key.");
        exit(1); 
    }

	init_ctr(&state, iv); //Counter call
	//Encrypting Blocks of 16 bytes and writing the output.txt with ciphertext

	/*char st[1024];
		   
        fgets(st,1024,stdin);*/

	
	int i,j;
	
     /*	for(i=0;i< readbytes;i++)
	{	
		indata[i]= (unsigned char)st[i];
		
	}*/
//	indata[i] = '\0';

//	printf("\n indata is %s\n",indata);

//	printf("readbytes is %d",readbytes);
	int startind = 0;
	int lastind = AES_BLOCK_SIZE -1 ;

	int encbytes = AES_BLOCK_SIZE;

	if((lastind + 1) >readbytes)
	{
		lastind = readbytes  -1;
		encbytes = readbytes - startind;
	}

	while(startind < readbytes)
	{
	//	printf("\ninside while : startind is %d\n",startind);
	//	printf("\ninside while : lastind is %d\n",lastind);

		unsigned char localindata[AES_BLOCK_SIZE]; 
		unsigned char localoutdata[AES_BLOCK_SIZE];
		
		for (i= startind,j=0;i<=lastind;i++,j++)
		{
			localindata[j] = indata[i];
		}
		//localindata[j] = '\0';
		
		AES_ctr128_encrypt(localindata, localoutdata, encbytes, &key, state.ivec, state.ecount, &state.num);

	        for (i= startind,j=0;i<=lastind;i++,j++)
		{
			outdata[i] = localoutdata[j];
		}
		
		startind = startind + AES_BLOCK_SIZE;
		lastind = startind + AES_BLOCK_SIZE -1;
		encbytes = AES_BLOCK_SIZE;

		if((lastind + 1) >readbytes)
	        {
			lastind = readbytes  -1;
			encbytes = readbytes - startind;
		}
	}
/*
printf("\n printing outdata \n");
		for(i=0;i<readbytes;i++)
		{	
			printf("%u",outdata[i]);
		}

printf("\n printing iv \n");
	for(i=0;i < AES_BLOCK_SIZE;i++)
		{	
			printf("%u",iv[i]);
		}*/

     

}
	
void fdecrypt(unsigned char *outdataptr, const unsigned char* enc_key,int len)
{	
	int i,j;
	
/*	for(i=0;i< AES_BLOCK_SIZE;i++)
	{
		iv[i] = finaloutdata[i];
		
	}
	for(i=0;i<len;i++)
	{
		outdata[i] = finaloutdata[AES_BLOCK_SIZE + i];
	}*/

	//Initializing the encryption KEY
    if (AES_set_encrypt_key(enc_key, 128, &key) < 0)
    {
        fprintf(stderr, "Could not set decryption key.");
        exit(1);
    }

	init_ctr(&state, iv);//Counter call

      
	int startind = 0;
	int lastind = AES_BLOCK_SIZE -1 ;

	int encbytes = AES_BLOCK_SIZE;

	if((lastind + 1) >len)
	{
		lastind = len  -1;
		encbytes = len - startind;
	}

//printf("inside decrpting 11111");

	while(startind < len)
	{
		unsigned char localindata1[AES_BLOCK_SIZE]; 
		unsigned char localoutdata[AES_BLOCK_SIZE];

		for (i= startind,j=0;i<=lastind;i++,j++)
		{
			localoutdata[j] = outdata[i];
		}
		
		AES_ctr128_encrypt(localoutdata,localindata1, encbytes, &key, state.ivec, state.ecount, &state.num);

	        for (i= startind,j=0;i<=lastind;i++,j++)
		{
			indata1[i] = localindata1[j];
		}

		startind = startind + AES_BLOCK_SIZE;
		lastind = startind + AES_BLOCK_SIZE -1;

		encbytes = AES_BLOCK_SIZE;

		if((lastind + 1) >len)
		{
			lastind = len  -1;
			encbytes = len - startind;
		}
	}



	//AES_ctr128_encrypt(outdata, indata1,len, &key, state.ivec, state.ecount, &state.num);
	
	printf("\n indata1 inside decrpyt after decrption is ");
	for(i=0;i<len;i++)
	{	
		printf("%c",indata1[i]);
	}

}


int    clientcallatproxyserver(int portno,char* msg,int isconnection,int readbytes)
{
	
	struct sockaddr_in address;
	static int sock = 0; 
//	int valread;
	struct sockaddr_in serv_addr;
	if(isconnection == 0)
	{
// 		printf("CLIENT STARTED BY PROXY SERVER");
		
	    	//char *hello = "Hello from client";
	    	
	    	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    	{
			printf("\n Socket creation error \n");
			return -1;
	    	}
	  
	//	 int flags = fcntl(sock,F_GETFL,0);
	    	

	    	memset(&serv_addr, '0', sizeof(serv_addr));
	  
	    	serv_addr.sin_family = AF_INET;
	    	serv_addr.sin_port = htons(portno);
	      
	    	// Convert IPv4 and IPv6 addresses from text to binary form
	    	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	    	{
			printf("\nInvalid address/ Address not supported \n");
			return -1;
	    	}
	  
	    	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    	{
			printf("\nConnection Failed \n");
			return -1;
	    	}
		fcntl(sock,F_SETFL, O_NONBLOCK);
	}
	else
	{
		if(readbytes >0)
		{
/**/			printf("proxy : before message sent from proxy to actual server is msg is %s, port is %d  \n",msg,portno);
			send(sock , msg , readbytes , 0 );
		}

		
/**/	//	printf("proxy : after message is sent from proxy to actual server   \n");

		//callactualserver(portno,1);


		valread = read( sock , buffer2, 10000);

		if(valread > 0)
		{
			int i;
/**/			printf("\nproxy : msg received from actual server is  :\n" );
			for(i=0;i<valread;i++)
			{	
				printf("%c",buffer2[i]);
			}
		}
	//	return buffer;
		
		//return NULL;
		return 1;
	}
	//memset(buffer,0,sizeof(buffer));


	

}
/*
//void callactualserver(int portn,int isconnection)
void * callactualserver(void *portn)
{

	    int *portnoptr = (int *)portn;
	    static int isconnection = 0;
	    int server_fd, new_socket, valread;
	    struct sockaddr_in address;
	    int opt = 1;
	    int addrlen = sizeof(address);
	    char buffer[1024] = {0};
	//   if(isconnection == 0)
	 //  {

		    printf("ACTUAL  SERVER STARTED\n");
		   // int portn = atoi(portno);
		      


		   // char *hello = "Hello from server";
		      
		    // Creating socket file descriptor
		    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		    {
			perror("socket failed");
			exit(EXIT_FAILURE);
		    }
		      
		    // Forcefully attaching socket to the port 8080
		    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				                                  &opt, sizeof(opt)))
		    {
			perror("setsockopt");
			exit(EXIT_FAILURE);
		    }
		    address.sin_family = AF_INET;
		    address.sin_addr.s_addr = INADDR_ANY;
		    address.sin_port = htons(portn);
		      
		    // Forcefully attaching socket to the port portn
		    if (bind(server_fd, (struct sockaddr *)&address,
				                 sizeof(address))<0)
		    {
			perror("bind failed");
			exit(EXIT_FAILURE);
		    }
		    if (listen(server_fd, 3) < 0)
		    {
			perror("listen");
			exit(EXIT_FAILURE);
		    }
		    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
				       (socklen_t*)&addrlen))<0)
		    {
			perror("accept");
			exit(EXIT_FAILURE);
		    }
		    isconnection = 1;
		    printf("ACTUAL  SERVER STARTED--222222222222\n");
		     
	   // }
	   // else
	   // {
		     printf("inside main server");
		     while(1)
		    {
			   printf("inside main server 11111111");
			    valread = read( new_socket , buffer, 1024);
			    printf("\n\nReceived at Actual Server -->> %s\n",buffer );
			    send(new_socket , buffer , strlen(buffer) , 0 );
			    printf("server message sent to proxy server is % \n",buffer);
			    //printf("sizeof buffer is %d\n",sizeof(buffer));
			    memset(buffer,0,sizeof(buffer));
		    }
	   // }
}
*/
int main(int argc, char const *argv[])
{


     
    int c;
    char* portno =NULL;
    char * keyfile = NULL;
    char *expression= NULL;
    char * actualportno = NULL;
    char * host = NULL;
    while ((c = getopt (argc, argv, "l:k:")) != -1)
    switch (c)
      {
      case 'l':
        portno = optarg;
        break;
      case 'k':
        keyfile = optarg;
        break;
      case '?':
        if (optopt == 'l')
          fprintf (stderr, "Option -%i requires an argument.\n", optopt);
        else if (optopt == 'k')
          fprintf (stderr, "Option -%r requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
//printf("22 \n");
	//char host[1024],portnumber[1024];
	int index;
	for (index = optind; index < argc; index++)
	{
		if(index == optind)
			host = argv[index];
		else
			actualportno = argv[index];
		//expression = argv[index];
	}
	FILE *readFile= NULL;

	if(keyfile!= NULL)
	readFile=fopen(keyfile,"rb");

	if(readFile)
	bytes_read = fread(keydata, 1, AES_BLOCK_SIZE, readFile); 

/*	printf("\nkey is \n");
	int i;
	for(i=0;i<AES_BLOCK_SIZE;i++)
	{
		printf("%d",keydata[i]);
	}*/

//	printf("host is %s, port is %s",host,actualportno);

       if(portno != NULL)
	{
	     int portn = atoi(portno);
/**/	    printf("SERVER STARTED\n");
	   
	int actualportn = atoi(actualportno);
	      
	

	    int server_fd, new_socket, valread1,valread2,valread4;
	    struct sockaddr_in address;
	    int opt = 1;
	    int addrlen = sizeof(address);
	    char buffer[10000] = {0};
	    char *hello = "Hello from server";
	    
	    // Creating socket file descriptor
	    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	    {
		perror("socket failed");
		exit(EXIT_FAILURE);
	    }

	 //  int flags = fcntl(server_fd,F_GETFL,0);
	  

//	      printf("SERVER STARTED  11111\n");
	    // Forcefully attaching socket to the port 8080
	    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		                                          &opt, sizeof(opt)))
	    {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	    }
	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = INADDR_ANY;
	    address.sin_port = htons(portn);
	      
	    // Forcefully attaching socket to the port portn
	    if (bind(server_fd, (struct sockaddr *)&address,
		                         sizeof(address))<0)
	    {
		perror("bind failed");
		exit(EXIT_FAILURE);
	    }

	    if (listen(server_fd, 3) < 0)
	    {
		perror("listen");
		exit(EXIT_FAILURE);
	    }

	    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
		               (socklen_t*)&addrlen))<0)
	    {
		perror("accept");
		exit(EXIT_FAILURE);
	    }

	     fcntl(server_fd,F_SETFL, O_NONBLOCK);
	    
	     fcntl(new_socket,F_SETFL, O_NONBLOCK);
	   // callactualserver(actualportn,0);

	    pthread_t tid;
         /*   printf("before thread");
	    pthread_create(&tid,NULL,callactualserver,(void*)actualportn);
	    printf("after thread");
	    sleep(5);*/
	  int i;

	    clientcallatproxyserver(actualportn,NULL,0,0);
	    while(1)
	    {

		    valread4 = read( new_socket , comboutdata, 10000);
		//    valread1 = read( new_socket , iv, AES_BLOCK_SIZE);
		 //   valread2 = read( new_socket , outdata, 1024);
	
		
		    if(valread4 > 0)
		    {

			int i,j;
			 for(i=0;i<AES_BLOCK_SIZE;i++)
			{
				iv[i]  = comboutdata[i];
			}
			valread1 = AES_BLOCK_SIZE;

			for(i=AES_BLOCK_SIZE,j=0 ;i<valread4;i++,j++)
			{
				outdata[j] = comboutdata[i];
			}
			valread2 = valread4 - AES_BLOCK_SIZE ;

			printf("\noutdata received at proxy is for valread2 = %d ",valread2);
			for(i=0;i<valread2;i++)
			{	
				printf("%u",outdata[i]);
			}
			printf("\n");
	
		printf("\iv received at proxy is for valread1 = %d ",valread1);
			for(i=0;i<valread1;i++)
			{	
				printf("%u",iv[i]);
			}
			printf("\n");


		    //	fdecrypt(outdata,(unsigned const char*)"1234567812345678",valread2);
			fdecrypt(outdata,(unsigned const char*)keydata,valread2);
		 	printf("\nproxy: data received and decrypted by proxy server is %s\n",indata1 );
			printf("\n");
		    }		

		//    printf("\n\nReceived at server -->> " );
		    
		//   for(i=0;indata1[i]!='\0';i++)
		 //  {	
		//	printf("%c",indata1[i]);
		 //  }
			       

		    //clientcallatproxyserver(actualportn,buffer,1);
	
//11		   int readbytes = read( new_socket , buffer, 10000);




		  clientcallatproxyserver(actualportn,indata1,1,valread2);
		   valread2 = 0;

//11		 clientcallatproxyserver(actualportn,buffer,1,readbytes);


		  //  send(new_socket , recbuffer , strlen(recbuffer) , 0 );

		     
		   
		/*     send(new_socket , indata1 , valread2 , 0 );
		    printf("server message sent to client \n");*/

		if(valread > 0)
		{
			printf("\n message sent from proxy to client is ");
			
			for(i=0;i<valread;i++)
			{	
				printf("%c",buffer2[i]);
			}
		     	send(new_socket , buffer2 ,valread , 0 );
		}
//		    printf("server message sent to client \n");
		   
	/*	    memset(iv,0,sizeof(iv));
		    memset(outdata,0,sizeof(outdata));
		    memset(indata1,0,sizeof(indata1));
		    memset(buffer2,0,sizeof(buffer2));
		    memset(buffer,0,sizeof(buffer));
		    memset(comboutdata,0,sizeof(comboutdata));*/
	    }
    }
    else
    {
//	   printf("CLIENT STARTED");
	int actualportn = atoi(actualportno);
	    struct sockaddr_in address;
	    int sock = 0, valread;
	    struct sockaddr_in serv_addr;
	    char *hello = "Hello from client";
	    char buffer[10000] = {0};
	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    {
		printf("\n Socket creation error \n");
		return -1;
	    }
	  
	//    int flags = fcntl(sock,F_GETFL,0);
	   


	    memset(&serv_addr, '0', sizeof(serv_addr));
	  
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(actualportn);
	      
	    // Convert IPv4 and IPv6 addresses from text to binary form
	    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	    {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	    }
	  
	    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    {
		printf("\nConnection Failed \n");
		return -1;
	    }

	    fcntl(sock,F_SETFL, O_NONBLOCK);

int check = 0;
fcntl(0,F_SETFL, O_NONBLOCK);
 // char st[10000];
char* write1 = "log.txt";
FILE *writeFile;
		writeFile=fopen(write1,"wb");  
	    while(1)
	    {

	//	printf("client 11111");
		   
		    
		//    printf("client 2222");
			
		  //   fgets(st,10000,stdin);
		
		  int readbytes = read(0,indata,10000);
//printf("client 3333");
	//	     int readbytes = read(0,st,1024);
		     

			//int len = getLength(st)+AES_BLOCK_SIZE;

		   if(readbytes > 0)
		   {	
		//	printf("sending from client >> %s \n",st);

		    //	fencrypt(indata, (unsigned const char*)"1234567812345678",readbytes);
			fencrypt(indata, (unsigned const char*)keydata,readbytes);
			int i,j;
			 for(i=0;i<AES_BLOCK_SIZE;i++)
			{
				comboutdata[i] = iv[i];
			}
		
			for(i=0,j=AES_BLOCK_SIZE ;i<readbytes;i++,j++)
			{
				comboutdata[j] = outdata[i];
			}
			int totalbytes = AES_BLOCK_SIZE + readbytes;
			send(sock , comboutdata , totalbytes , 0 );

		   // 	send(sock , iv , AES_BLOCK_SIZE , 0 );

		   //	send(sock , outdata , readbytes , 0 );
		   } 

/*11		   if(readbytes > 0)
		   send(sock , indata ,readbytes , 0 ); */

		 //    send(sock , st , readbytes , 0 );

		   // write(sock , st , strlen(st)  );
		    //printf("stdin message sent\n");
		   // char *st



		  
		    //ioctl(sock,FIONBIO,&iMode);
		    valread = read( sock , buffer, 10000);
//printf("client 66666");
		//	printf("valread is %d",valread);

		    if(valread > 0)
		    {
		    	//printf("\ndata from server is %s\n",buffer );
			//int bytes_written = fwrite(buffer, 1, valread, writeFile); 
			write(1, buffer, valread);    
		}
//printf("client 77777");

	//	    memset(st,0,sizeof(st));
	/*	    memset(iv,0,sizeof(iv));
		    memset(outdata,0,sizeof(outdata));
		     memset(indata,0,sizeof(indata));
		   memset(buffer,0,sizeof(buffer));
		   memset(comboutdata,0,sizeof(comboutdata));*/
	    }
    }
    //pthread_exit(NULL);
    return 0;
}
