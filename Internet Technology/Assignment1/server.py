#!usr/bin/env python 

import socket
import sys
import pickle
import os
import csv 


#list of clients already manager
managers_list=[]
if os.path.isfile('./managerProfiles/managers'+'.csv'):
    mList = csv.reader(open('./managerProfiles/managers'+'.csv'))
    for user in mList:
        managers_list.append(user[0])
#list of clients pre-approved to be upgraded to Manager when requested
allowed_managers=[]
if os.path.isfile('./managerProfiles/allowed'+'.csv'):
    mList = csv.reader(open('./managerProfiles/allowed'+'.csv'))
    for user in mList:
        allowed_managers.append(user[0])
#updates the above two list whenever changes are made
def update_mList():
    with open('././managerProfiles/allowed'+'.csv', 'w') as f:
            for user in allowed_managers :
                f.write("%s\n"% (user))
    with open('././managerProfiles/managers'+'.csv', 'w') as f:
            for user in managers_list:
                f.write("%s\n"% (user))
    print("Updated")

#class to handle the requests received by clients
class clientProfile:
    def __init__(self,user_name,clientAddr):
        self.clientAddr=clientAddr
        self.username=user_name
        self.access_mode=self.get_access_mode()
        self.exists, self.key_value_dict=self.readProfile(self.username)
        self.clientResponse=[]
        if not self.exists:
            self.clientResponse.append("Prompt: New Client Profile Created")
        self.clientResponse.append("--------------------------------")
        self.requests=[]
        self.index=0
    #util helper
    def next_arg(self):
        nextarg=self.requests[self.index]
        self.index=self.index+1
        return nextarg
    #set default access mode
    def get_access_mode(self):
        if self.username in managers_list:
            return 'manager'
        else:
            return 'guest'
    #check whether client already exists in memory
    def check_client(self):
        if os.path.isfile('./clientProfiles/'+self.username+'.csv'):
            return True
        else :
            return False
    #reads and returns  client key value store if exists and new dict if not
    def readProfile(self,client_username):
        exists=self.check_client()
        key_value_dictionary={}
        if os.path.isfile('./clientProfiles/'+client_username+'.csv'):
            cProfile = csv.reader(open('./clientProfiles/'+client_username+'.csv'))
            for pair in cProfile:
                key_value_dictionary[pair[0]] = pair[1]
        return exists,key_value_dictionary
    #check if client is preapproved to be manager
    def check_upgrade(self):
        if (self.access_mode=='manager') :
            return True
        if self.username in allowed_managers:
            allowed_managers.remove(self.username)
            managers_list.append(self.username)
            update_mList()
            return True
        else :
            return False
    #store user key value pair
    def save_profile(self):
        with open('./clientProfiles/'+self.username+'.csv', 'w') as f:
            for key in self.key_value_dict.keys():
                f.write("%s,%s\n"% (key, self.key_value_dict[key]))
    #process request
    def process(self,requests):
        self.requests=requests

        while (self.index<len(requests)):
            #parses requests and executes
            requestCommand=self.next_arg().lower()
            #GET
            if(requestCommand=='get'):
                reqField=self.next_arg()
                response=self.key_value_dict.get(reqField)
                if response is None:
                    self.clientResponse.append("<blank>")
                    print("Request- GET - No value found for key :",reqField)
                else : 
                    self.clientResponse.append(response)
                    print("Request- GET - Key: ",reqField," Value :",response)
            #PUT
            elif (requestCommand=='put'):
                reqField=self.next_arg()
                dataField=self.next_arg()
                self.key_value_dict[reqField]=dataField
                print("Request- PUT- Key: ",reqField," Value :",dataField," = SUCCESS")
            #MAKEMANAGER -upgrade to manager
            elif (requestCommand=='makemanager'):
                isPermissable=self.check_upgrade()
                if(isPermissable):
                    self.access_mode='manager'
                    self.clientResponse.append("Upgarded to Authorization Level : Manager ")
                    print("Request- Make Manager = SUCCESS")
                else :
                    # Immediate manager upgrade via password
                    clientAddr.sendall("Enter password to upgarde authorization to Manager : ".encode())
                    password=clientAddr.recv(1024).decode()
                    if(password=='0000'):
                        self.access_mode='manager'
                        managers_list.append(self.username)
                        update_mList()
                        self.clientResponse.append("Upgarded to Authorization Level : Manager ")
                        print("Request- Make Manager = SUCCESS via password")
                    else:
                        print("Request- Make Manager = DENIED")
                        self.clientResponse.append("Incorrect Password : Manager authorization denied")
            #GETUSER - managers only
            elif (requestCommand=='getuser'):
                if(self.access_mode=='manager'):
                    reqUser=self.next_arg()
                    dummy,other_user_dict=self.readProfile(reqUser)
                    reqField=self.next_arg()
                    if (dummy) :
                        response=other_user_dict.get(reqField)
                        if response is None:
                            print("Request- GETUSER - Username :",reqUser,"Key: ",reqField," Value : <blank>")
                            self.clientResponse.append("User: "+reqUser+" Key : "+reqField+ " Value : <blank>")
                        else : 
                            print("Request- GETUSER - Username :",reqUser," Key: ",reqField," Value : ",response)
                            self.clientResponse.append("User: "+reqUser+" Key : "+reqField+ " Value : "+ response)
                    else : 
                        response= "User not available"
                        self.clientResponse.append("User: "+reqUser+" - Error 404 :User not found")
                        print("Request- GETUSER - Username :",reqUser," User Not Found")
                else :
                    print("Request- GETUSER - DENIED - Manager Authorization Required")
                    self.clientResponse.append("Access Denied : Manager Authorization Required. Command terminated.")
                    break
            else :
                print("")
                self.clientResponse.append("Invalid Command. Command terminated. Refer readMe.md")
                break
        self.save_profile()
        return self.clientResponse

#initailise server
serverPort=8080
serverSocket=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serverSocket.bind(('', serverPort))

#receive requests from clients
while True:
    serverSocket.listen(10)
    clientAddr, dummy = serverSocket.accept()
    username=clientAddr.recv(1024).decode()
    print("--------------------------------------")
    print('Connected to client user : ',username)
    print("--------------------------------------")
    client=clientProfile(username,clientAddr)
    requests=clientAddr.recv(1024)
    requests=pickle.loads(requests)
    reqResults=client.process(requests)
    clientAddr.sendall("results".encode())
    clientAddr.sendall(pickle.dumps(reqResults))
    print("")