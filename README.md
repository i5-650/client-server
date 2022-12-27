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
The JSON messages are following a specific format for not single textual informations.
```JSON
{
    "code": "msg", // the code is always on exactly 3 characters without capital letters
    "values": [3, 1, 2, 3] // the values are always in an array. The first value MUST be the number of values that will be sent. The only exception is the 'cal' code. 
}
```
If the message is a single text information, the `value` array will only contain one value: the string sent/received.
### Examples
```JSON
{
    "code": "okk",
    "values": ["Something saying everything is okay"]
}
```
```JSON
{
    "code": "err",
    "values": ["Something saying everything what happened"]
}
```
```JSON
{
    "code": "msg",
    "values": ["Hello World"]
}
```
```JSON
{
    "code": "nam",
    "values": ["hostname"]
}
```
```JSON
{
    "code": "col",
    "values": [4, "0xffffff", "0xffffff", "0xffffff", "0xffffff"]
}
```
```JSON
{
    "code": "anl",
    "values": [4, "0xffffff", "0xffffff", "0xffffff", "0xffffff"]
}
```
```JSON
{
    "code": "min",
    "values": [4, 10, 11, 12, 13]
}
```
```JSON
{
    "code": "max",
    "values": [4, 10, 11, 12, 13]
}
```
```JSON
{
    "code": "avg",
    "values": [4, 10, 11, 12, 13]
}
```
```JSON
{
    "code": "ect",
    "values": [4, 10, 11, 12, 13]
}
```
```JSON
{
    "code": "tag",
    "values": [3, "tag1", "tag2", "tag3"]
}
```
### The exception
```JSON
{
    "code": "CAL",
    "values": ["+", 2.14, 3] // the first value is the operator. The second and third values are the numbers to operate on.
}
```

## Mac and Linux difference
If you are willing to run this code on MacOs, it will work fine. We encountered a problem using semaphores on Mac. Indeed, Mac doesn't support a non-deprecated version of posix semaphores. So we had to use an other version of it, specific to mac.
Do not worry, if you run this code on Linux it will use the posix semaphores.