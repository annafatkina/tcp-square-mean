import subprocess
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-p', '--path') # path to exe folder
args = parser.parse_args()
FNULL = open(os.devnull, 'w')  

host = "localhost"
port = "1234"
server = args.path + "tcp-mean-server.exe "
client = args.path + "tcp-mean-client.exe "
subprocess.Popen(args = [server, port])
for i in range(2):
    subprocess.Popen(args = [client, host, port])

