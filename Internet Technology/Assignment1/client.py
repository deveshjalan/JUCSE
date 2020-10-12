#!usr/bin/env python

import socket
import sys
import pickle

#to_read
ip_add=sys.argv[1]
port=int(sys.argv[2])

#create port
clientSocket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
clientSocket.connect((ip_add,port))

#get username
username=input("Enter Username : ")
clientSocket.sendall(username.encode())

#send requests
command=sys.argv[3:]
clientSocket.sendall(pickle.dumps(command))

#waiting for response
response =clientSocket.recv(1024).decode()
if(response!='results'):
    password=input(response)
    clientSocket.sendall(password.encode())
    dummy =clientSocket.recv(1024).decode()

#receiveing results

result=clientSocket.recv(10000)
result=pickle.loads(result)
for line in result:
    print(line)

print("----------Disconnected-----------")
