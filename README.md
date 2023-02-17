# tcp-square-mean

TCP server which calculates an avg of squares of recieved numbers. 
For every new client connection server creates a new `Session`, they are 
processed in multiple threads if it's possible on current machiene. 
Both server and client print logs to cmd and to their log files. 
Server also dumps numbers already received from clients to a dump 
file periodically in a separate thread. Server logger runs in a separate thread as well.


There are different log levels:

* Err - print error level logs only,
* Warning - print errors and warnings,
* Info - print errors, warnings and info logs,
* All - print all the logs, including debug level logs.

The default level set in the server is `INFO`.

## How to build

Build it with Visual Studio. There is a solution file: `tcp-mean/tcp-mean.sln`.

Dependency: Boost.
`Boost_DIR` should be installed

## How to run 

There is a script `run.py` which runs a single server and 10 clients. One of these clients sends values bigger than server allows.

You can this script like this:

```
python run.py -p <path/to/executables/folder> -ls <serverLogName> -lc <clientLogName> -d <dumfName> -mn <maxNumver>
```

Or run server and clients separately from command line.

Run server:
```
tcp-mean-server.exe <port> <logFilename> <dumpFilename> <maxNumber>
```

Run client:
```
tcp-mean-client.exe <host> <port> <logFilename> <maxNumber>
```

To terminate, press `ctrl+c`.
