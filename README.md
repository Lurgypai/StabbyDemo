# Stabby
2d platforming fighting game

Arrow keys to move, 'x' to attack, 'c' to roll.

# Hosting a Server
Alter the port value in the 'settings' file to change hosted ported. This port needs to be forwarded for online use.
'disconectDelay' represents the amount of seconds a server will wait between receiving packets from the client before disconnecting them.
'forceDisconnectDelay' if by this time in seconds a server has not received a disconnection from the client, it will force close the connection.

# Running the Client
In order to start playing the game, the "startgame" command must be used.
Open the console by pressing the '~/\`' key located in the top left of your keyboard. Type the following (without quotes) to play offline
"/startgame false"

To play online type the following (without quotes)
"/startgame true ip_address port"
Where ip_address is the ip of the server in the form "0.0.0.0", and port is the port number.
