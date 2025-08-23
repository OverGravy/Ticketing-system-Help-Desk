# Ticketing System using AF_INET

This university project uses **AF_INET socket communication** under Linux to create a **client-server architecture** for ticket creation, modification, and consultation.  
The system allows clients to send tickets to a central server, which stores them in its local database. Clients can request specific ticket information through field-based queries, and after authentication, they can modify ticket details.  

---

## Compile and Usage

Before compiling, make sure you have installed **SQLite3** for the database on the server side and **Raylib** for the graphical interface on the client side:

```bash
sudo apt install sqlite3
sudo apt install libraylib5-dev
```

- The system requires both a server and a client to run.  
- The **server** has no graphical interface; all updates and logs are displayed through terminal messages.  
- The **client** provides a simple graphical interface with multiple windows, an interactive menu, and forms for creating tickets and submitting queries.  

To compile the system, use the provided Makefile:

```bash
make
```

To start the server:

```bash
./server
```

To start a client:

```bash
./client <id_client>
```

where `<id_client>` is a unique number identifying the client.  

---

## Overview

The system provides an easy ticketing communication framework between clients and the server.  
The **server** handles ticket storage and querying using an SQLite database, as well as user login and authentication.  
The **client** allows end users to interact with the server through a simple UI to create, query, and (after registration) modify tickets.  

---

## Current Features

- The server forks a new process to handle each request.  
- A unified packet structure ensures consistent handling of different message types.  
- Raylib provides a lightweight and responsive graphical interface.  
- After compiling with the Makefile, example scripts can be used to generate multiple clients.  

---

## Packet Structure

To support socket communication, all messages are structured as packets.  
There are two main types of packets: **request packets** and **response packets**.  
Both share a common header containing the packet type.  

Request packets vary depending on the operation. They use a `union` structure to carry different types of data (e.g., new ticket, query, or modification request):

```c
typedef struct {
    RequestType type;
    int sender_id;
    union {
        Ticket new_ticket;                    
        TicketQuery client_query;             
        TicketQueryAndMod agent_query;        
    } data;
} RequestPacket;
```

Response packets are simpler: they contain only a response type and a message:

```c
typedef struct {
    ResponseType type;
    char message[MAX_RESP_MSG_LEN];           
} ResponsePacket;
```