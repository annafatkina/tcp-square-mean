# tcp-square-mean

TCP server which calculates an avg of squares of recieved numbers.

## How to run 

There is a script `run.py` which runs a single server and 10 clients. One of these clients sends values bigger than server allows.

You can this script like this:

```
 python run.py -p tcp-mean/Debug/ -ls logserver.txt -lc logclient.txt -d dump.dat -mn 1023
```
