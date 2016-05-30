/*
    C ECHO client example using sockets
*/
#include<stdio.h> 		//printf
#include<string.h> 		//strlen
#include<sys/socket.h>  //socket
#include<arpa/inet.h> 	//inet_addr
#include<unistd.h> 		//read/write

#define ERROR_STATE -1

#define INET_ADDRESS "127.0.0.1"
#define PORT_NUMBER 8888
#define MESSAGE_LENGTH 500

//Menu commands
#define EXIT_COMMAND 0
#define PASSANGER_STATUS_BY_ID_COMMAND 1
#define FLIGHT_STATUS_BY_ID_COMMAND 2

//Flight/Passenger states
#define LANDED 0
#define STILL_FLYING 1
#define CRASHED 2
#define NOT_ONBOARD 3
#define FLIGHT_NOT_FOUND 4

//Communication message struct
typedef struct
{
    int id;
    int select;
    char message[MESSAGE_LENGTH];
} Message;
Message msg={0,0};

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == ERROR_STATE)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr(INET_ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons( PORT_NUMBER );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Connection failed. Error ");
        return ERROR_STATE;
    }

    puts("Connected\n");

    //Keep communicating with server until given an exit signal
    int signalExit = 0;
    while(!signalExit)
    {
        printf("\nChoose an option:\n");
    	printf("---------------------\n");
		printf("0. Exit\n");
    	printf("1. Search Passanger by id\n");
    	printf("2. Search Flight by id\n");

    	printf("Your choice: ");
    	scanf("%d", &msg.select);

		switch(msg.select)
		{
			case EXIT_COMMAND:
				printf("Exiting...\n");
				signalExit = 1;
				break;
			case PASSANGER_STATUS_BY_ID_COMMAND:
				printf("You have choosen to search passenger by ID.\n Enter passengers ID:");
				scanf("%d", &msg.id);

				int send;
				send = write(sock, &msg, sizeof(Message));
				if(send <= 0)
				{
					puts("Unable to send command to server");
				}

				//Reinititalize message struct
				msg.id = 0;
				msg.select = 0;
				memset(msg.message, 0, sizeof msg.message);

				int read_size;
				if((read_size = read(sock , &msg , sizeof(Message))) > 0)
				{
					printf("\n%s",msg.message);
				}
				break;
			case FLIGHT_STATUS_BY_ID_COMMAND:
				printf("You have choosen to search flight by ID.\n Enter flight's ID:");
				scanf("%d", &msg.id);

				send = write(sock, &msg, sizeof(Message));
				if(send <= 0)
				{
					puts("Unable to send command to server");
				}

				//Reinititalize message struct
				msg.id = 0;
				msg.select = 0;
				memset(msg.message, 0, sizeof msg.message);

				if((read_size = read(sock , &msg , sizeof(Message))) > 0)
				{
					printf("\n%s",msg.message);
				}
				break;
			default:
				printf("Invalid selection, please make another choice! \n");
				break;
		}
    }

    close(sock);
    return 0;
}
