/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/

#include<stdio.h>      //printf/scanf/etc...
#include<string.h>     //strlen
#include<stdlib.h>     //strlen
#include<sys/socket.h> //sockets
#include<arpa/inet.h>  //inet_addr
#include<unistd.h>     //write
#include<pthread.h>    //for threading , link with lpthread

#define ERROR_STATE -1

#define NUM_FLIGHTS 2
#define NUM_PASSAGERS 5
#define MAX_NUM_PASSENGERS_PER_FLIGHT 50

//Flight/Passenger states
#define LANDED 0
#define STILL_FLYING 1
#define CRASHED 2
#define NOT_ONBOARD 3

//Passanger details Struct
typedef struct
{
    int ID;
    char * first_name;
    char * second_name;
    char * last_name;
} Passanger;

//Flight details Struct
typedef struct
{
    //Flight contains -> many passengers
    Passanger *passangers_array; 
    int ID;
    int has_landed_flag;
    char * destination;
    size_t used;
    size_t size;
} Flight;

//Airport details Struct
typedef struct
{
    //Airport contains -> many flights
    Flight *flights_array; 
    int ID;
    char * name;
    size_t used;
    size_t size;
} Airport;
Airport Sofia ={0,0};

//Communication message struct
typedef struct
{
    int id;
    int select;
    char message[100];
} Message;
Message msg={0,0};

//The thread function for execution
void *connection_handler(void *);

void initAirport(Airport *, size_t, int ,char *);
void initFlight(Flight *, size_t,int,int,char * );
void initPassenger(Passanger *, int, char*, char*, char*);
void assignPassengerToFlight(Flight *, Passanger );
void assignFlightToAirport(Airport *, Flight );
void freeFlight(Flight *);

//API funcs
int getPassangerStatusById(Airport* airport,int passangerId);

int main(int argc , char *argv[])
{
    int socket_desc , new_socket, c,*new_sock;
    struct sockaddr_in server , client;

    //Airport Sofia;
    Flight toBarcelona, toParis;
    Passanger pesho,gosho,ivan,dragan,atanas,
			  dragoi, blagoi, ico, moni, pepkata;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == ERROR_STATE)
    {
        printf("Could not create socket \n");
    }
    else
    {
        puts("Socket created successfully \n ");
    }
    

    //Initialize server to 0
    (void) memset((char*) &server, 0, sizeof(server));

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //Print the error message
        perror("Binding failed. Error");
        return ERROR_STATE;
    }
    puts("Binding successful");

    //Init airport
    initAirport(&Sofia,NUM_PASSAGERS,1,"Letishte Sofia");

    //Init flights
    initFlight(&toBarcelona,NUM_PASSAGERS,1,LANDED,"Barcelona");
    initFlight(&toParis,NUM_PASSAGERS,2,STILL_FLYING,"Paris");

    //Init passengers
    initPassenger(&pesho,1,"Petar","Petrov","Hadjigenchov");
    initPassenger(&gosho,2,"Georgi","Valentinov","Chelenkov");
    initPassenger(&ivan,3,"Ivan","Krasimirov","Botev");
    initPassenger(&dragan,4,"Dragan","Iordanov","Gospodinov");
    initPassenger(&atanas,5,"Atanas","Slaveikov","Troshanov");
    initPassenger(&dragoi,6,"Dragoi","Petrov","Dragoichev");
    initPassenger(&blagoi,7,"Blago","Blajev","Djiev");
    initPassenger(&ico,8,"Hristo","Ivanov","Stoichkov");
    initPassenger(&moni,9,"Moncho","Monev","Minev");
    initPassenger(&pepkata,10,"Pepi","Monev","Pepinchev");

    //Asigning passngers to flights
    assignPassengerToFlight(&toBarcelona, pesho);
    assignPassengerToFlight(&toBarcelona, gosho);
    assignPassengerToFlight(&toBarcelona, ivan);
    assignPassengerToFlight(&toBarcelona, dragan);
    assignPassengerToFlight(&toBarcelona, atanas);

    assignPassengerToFlight(&toParis, dragoi);
    assignPassengerToFlight(&toParis, blagoi);
    assignPassengerToFlight(&toParis, ico);
    assignPassengerToFlight(&toParis, moni);
    assignPassengerToFlight(&toParis, pepkata);

    assignFlightToAirport(&Sofia,toBarcelona);
    assignFlightToAirport(&Sofia,toParis);

    //Displaying flight passengers NFO
    printf("\n-----------FLIGHT PASSENGERS INFO--------------");
    int i,j,k;
    for(i = 0; i < Sofia.used; i++)
    {
        printf("\nDestination: %s\n",Sofia.flights_array[i].destination);

    	for(j = 0; j < Sofia.flights_array[i].used; j++)
    	{
    	    printf("\n");
       	    printf("%d ",Sofia.flights_array[i].passangers_array[j].ID);
            printf("%s ",Sofia.flights_array[i].passangers_array[j].first_name);
            printf("%s ",Sofia.flights_array[i].passangers_array[j].second_name);
            printf("%s ",Sofia.flights_array[i].passangers_array[j].last_name);
    	}
    }

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("\nWaiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

		int thread_desc = 0;
		thread_desc = pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock);
		
        if(thread_desc < 0)
        {
            perror("Could not create thread");
            return ERROR_STATE;
        }
        puts("Handler assigned");

		int detach = pthread_detach(thread_desc);
		if(detach != 0 )
		{
			puts("Freeing the thread resources failed!");
        }
    }

    if (new_socket < 0)
    {
        perror("Accept failed");
        return ERROR_STATE;
    }

    ///////////////Freeing memory
    freeFlight(&toBarcelona);

    	free(pesho.first_name);
    	free(pesho.second_name);
    	free(pesho.last_name);

    	free(gosho.first_name);
        free(gosho.second_name);
        free(gosho.last_name);

        free(ivan.first_name);
        free(ivan.second_name);
        free(ivan.last_name);

        free(dragan.first_name);
        free(dragan.second_name);
        free(dragan.last_name);

        free(atanas.first_name);
        free(atanas.second_name);
        free(atanas.last_name);
    ///////////////
    return 0;
}

/*
 * This will handle connection for each client
 * 
*/
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;

    while((read_size = read(sock , &msg , sizeof(Message))) > 0)
    {
        memset(msg.message, 0, sizeof msg.message);
		
    	if(msg.select == 1)
    	{
    	    msg.id = getPassangerStatusById(&Sofia,msg.id);

    	    switch(msg.id)
    	    {
    	        case 0:
            	    strcpy(msg.message,"The passanger has landed successfully\n");
					break;
    	        case 1:
					strcpy(msg.message,"The passanger is still flying\n");
					break;
    	        case 2:
					strcpy(msg.message,"Sorry but the plane has crashed ;(\n The passenger is dead\n");
					break;
    	        case 3:
					strcpy(msg.message,"Sorry, the passanger isn't using our services at the moment\n");
					break;
				default:
					break;
    	    }
			int send;
			send = write(sock, &msg, sizeof(Message));
        }
		//Add future possible cases here
        else
        {
    	    puts("Invalid command passed to server! \n");
    	}
    }
    if(read_size == 0)
    {
        puts("Client disconnected \n");
        fflush(stdout);
    }
    else if(read_size == ERROR_STATE)
    {
        perror("Receiving response from client failed! \n");
    }

    //Free the socket pointer
    free(socket_desc);
    return 0;
}

void initFlight(Flight *flight, size_t initialSize, int id, int has_landed_flag, char * dest)
{
    // Allocate initial space
    flight->passangers_array = (Passanger *)malloc(initialSize * sizeof(Passanger));

    flight->used = 0;           // no elements used
    flight->size = initialSize; // available number of elements

    // Initialize all values of the array to 0
    for(unsigned int i = 0; i<initialSize; i++)
    {
        memset(&flight->passangers_array[i],0,sizeof(Passanger));
    }

    flight->ID = id;
    flight->has_landed_flag = has_landed_flag;
    flight->destination=(char*)malloc(strlen(dest) + 1);
    strcpy(flight->destination,dest);

    printf("\nFlight Destination=%s, id =%d, has_landed_flag=%d created\n",
    		flight->destination,flight->ID,flight->has_landed_flag);
}

void initAirport(Airport *airport, size_t initialSize,int ID, char * name)
{
    // Allocate initial space
    airport->flights_array = (Flight *)malloc(initialSize * sizeof(Flight));

    airport->used = 0;           // no elements used
    airport->size = initialSize; // available number of elements

    // Initialize all values of the array to 0
    for(unsigned int i = 0; i<initialSize; i++)
    {
        memset(&airport->flights_array[i],0,sizeof(Flight));

        //Allocate initial space  passengers array sub member
        airport->flights_array[i].passangers_array = (Passanger *)malloc(MAX_NUM_PASSENGERS_PER_FLIGHT * sizeof(Passanger));

        //Initialize all values of the passengers array to 0
        for(unsigned int j=0; j < MAX_NUM_PASSENGERS_PER_FLIGHT; j++)
        {
            memset(&airport->flights_array[i].passangers_array[j],0,sizeof(Passanger));
        }
    }

    airport->ID = ID;
    airport->name=(char*)malloc(strlen(name) + 1);
    strcpy(airport->name,name);

    printf("\nAirport with ID =%d, Name=%s created\n",airport->ID,airport->name);
}

void assignFlightToAirport(Airport *airport, Flight flight )
{
    if (airport->used == airport->size)
    {
        airport->size *= 2;
    	airport->flights_array = (Flight *)realloc(airport->flights_array, airport->size * sizeof(Flight));
    }

    //Copy passengrs array
    //TO DO
    for(unsigned int i = 0; i < flight.used; i ++)
    {
    	//Copying passenger id
    	airport->flights_array[airport->used].passangers_array[i].ID = flight.passangers_array[i].ID;

    	//Copying passenger firstname
    	airport->flights_array[airport->used].passangers_array[i].first_name = (char*)malloc(strlen(flight.passangers_array[i].first_name) + 1);
    	strcpy(airport->flights_array[airport->used].passangers_array[i].first_name, flight.passangers_array[i].first_name);

        //Copying passenger secondname
    	airport->flights_array[airport->used].passangers_array[i].second_name = (char*)malloc(strlen(flight.passangers_array[i].second_name) + 1);
    	strcpy(airport->flights_array[airport->used].passangers_array[i].second_name, flight.passangers_array[i].second_name);

    	//Copying passenger lastname
    	airport->flights_array[airport->used].passangers_array[i].last_name = (char*)malloc(strlen(flight.passangers_array[i].last_name) + 1);
    	strcpy(airport->flights_array[airport->used].passangers_array[i].last_name, flight.passangers_array[i].last_name);

    	airport->flights_array[airport->used].used += 1;

    }

    // Copy destination
    airport->flights_array[airport->used].destination = (char*)malloc(strlen(flight.destination) + 1);
    strcpy(airport->flights_array[airport->used].destination, flight.destination);

    // Copy ID
    airport->flights_array[airport->used].ID=flight.ID;

    // Copy has_landed_flag
    airport->flights_array[airport->used].has_landed_flag=flight.has_landed_flag;

    airport->used += 1;
    printf("\nFlight with ID=%d assigned to Airport - %s",flight.ID,airport->name);
}

// Add element to array
void assignPassengerToFlight(Flight *flight, Passanger passenger)
{
    if (flight->used == flight->size)
    {
    	flight->size *= 2;
    	flight->passangers_array = (Passanger *)realloc(flight->passangers_array, flight->size * sizeof(Passanger));
    }

    // Copy firstname
    flight->passangers_array[flight->used].first_name = (char*)malloc(strlen(passenger.first_name) + 1);
    strcpy(flight->passangers_array[flight->used].first_name, passenger.first_name);

    // Copy secondname
    flight->passangers_array[flight->used].second_name = (char*)malloc(strlen(passenger.second_name) + 1);
    strcpy(flight->passangers_array[flight->used].second_name, passenger.second_name);

    // Copy lastname
    flight->passangers_array[flight->used].last_name = (char*)malloc(strlen(passenger.last_name) + 1);
    strcpy(flight->passangers_array[flight->used].last_name, passenger.last_name);

    // Copy ID
    flight->passangers_array[flight->used].ID=passenger.ID;

    flight->used += 1;
    printf("\nPassanger with ID=""%d"" assigned to flight-%s",passenger.ID,flight->destination);
}

void freeFlight(Flight *flight)
{
    // Free all name variables of each array element first
    for(int i=0; i<flight->used; i++)
    {
        free(flight->passangers_array[i].first_name);
        flight->passangers_array[i].first_name=NULL;

        free(flight->passangers_array[i].second_name);
        flight->passangers_array[i].second_name=NULL;

        free(flight->passangers_array[i].last_name);
        flight->passangers_array[i].last_name=NULL;
    }

    // Now free the array
    free(flight->passangers_array);
    flight->passangers_array = NULL;

    flight->used = 0;
    flight->size = 0;
}

void initPassenger(Passanger* passenger, int pass_id, char* first, char* second, char* last)
{
    passenger->ID = pass_id;
    
    passenger->first_name=(char*)malloc(strlen(first) + 1);
    strcpy(passenger->first_name,first);
    
    passenger->second_name=(char*)malloc(strlen(second) + 1);
    strcpy(passenger->second_name,second);
    
    passenger->last_name=(char*)malloc(strlen(last) + 1);
    strcpy(passenger->last_name,last);

    printf("\nPassanger with ID=%d FirstName=%s SecondName=%s LastName=%s created",passenger->ID,passenger->first_name, passenger->second_name,passenger->last_name);
}

//API funcs
int getPassangerStatusById(Airport* airport,int passangerId)
{
    int i,j;
    for(i = 0; i < airport->used; i++)
    {
        for(j = 0; j < airport->flights_array[i].used; j++)
		{
			if(airport->flights_array[i].passangers_array[j].ID == passangerId)
			{
				return(airport->flights_array[i].has_landed_flag);
			}
		}
    }

    return NOT_ONBOARD;
}
