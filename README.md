# Proc-C-mity
A chatroom made using C lang and websockets


## Usage:

-   
    ``` git clone https://github.com/nsankethreddy/Proc-C-mity.git ```

- To compile the code for server and client
    ``` chmod +x compile.sh ```
    ``` ./compile.sh ```

- To start the server
  
    ``` ./server ```

    Choose an IP address in the list of IP adresses presented, to start the server in the respective network.

- To start a client and connect to a chat room
  
    ``` ./client <IP where the server was hosted> ```

## Features:

- Can support group chats.
- Anyone in the same network can start/join chatroom.
- A client can exit and re-connect to a chat-room multiple times.
- A server can host multiple chatrooms, but only one chat-room per network interface.
- When the server disconnects, all the participants wil be forced to exit.
- Type 'quit' to leave the chat-room.
