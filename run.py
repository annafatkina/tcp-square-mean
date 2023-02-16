import subprocess
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-p', '--path') # path to exe folder
parser.add_argument('-ls', '--logserver') # path to exe folder
parser.add_argument('-lc', '--logclient') # path to exe folder
parser.add_argument('-d', '--dumpfile') # path to exe folder
parser.add_argument('-mn', '--maxnumber') # path to exe folder

args = parser.parse_args()
FNULL = open(os.devnull, 'w')  

host = "localhost"
port = "1234"
server = args.path + "tcp-mean-server.exe "
client = args.path + "tcp-mean-client.exe "

# Run the server
subprocess.Popen(args = [server, port, args.logserver, args.dumpfile, args.maxnumber])

for i in range(9):
    # Run some clients
    subprocess.Popen(args = [client, host, port, str(i) + args.logclient, args.maxnumber])

# Let's add a client who sends incorrect values
subprocess.Popen(args = [client, host, port, '10' + args.logclient, str(int(args.maxnumber)+10)])

