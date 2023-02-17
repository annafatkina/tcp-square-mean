# tcp-square-mean

TCP server which calculates an avg of squares of recieved numbers. Both server and client print logs to cmd and to their log files. Server also dumps numbers already received from clients to a dump file periodically.

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
