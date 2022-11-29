# client-server
Client-server project for school

## What's possible now ? 
- Send a message to the server and receive it back
- Send your name or a choosen name to the server and receive it back
- Send an array of colors to the server and have it saved
- Send the informations about an image and save those informations on the server.
- Send multiple tags, up to 30, to the server and have them saved
- The server can handle multiple clients at the same time, up to 9 for now.
- Somes operations are possible such as: addition, substraction, division, multiplication, minimun, maximum, average and variance. 
- The server handle client disconnections and disappearances.
- The clients handle interruptions, it means that if you `ctrl-c` on the client: it sends a message to the server to inform it that the client is disconnected and the server will handle it.
- Clients and Server communicate with JSON messages. 

## The JSON messages
The JSON messages are following a specific format.
```JSON
{
    "code": "MSG", // the code is always on exactly 3 characters 
    "values": ["10", "5", "2"] // the values are always in an array and every thing is a string, always. Because is an array of string.
}
```

## Mac and Linux difference
If you are willing to run this code on MacOs, it will work fine. We encountered a problem using semaphores on Mac. Indeed, Mac doesn't support a non-deprecated version of posix semaphores. So we had to use an other version of it, specific to mac.
Do not worry, if you run this code on Linux it will use the posix semaphores.