# Internet Technology
### Assignment 1 

## How to run

1. Open terminal and use : python3 server.py
1. Open another terminal : python3 client.py < ip address > < port > < commands >
1. Enter username to continue

## Usage - commands

Commands can be in form 

put < key > < value >
get < key >
makemanager
getuser < username > < key > 

All commands can be mixed and used together in the argruments.

Check output to see examples

## Manager criteria

A client can switch to manager authorization level and access key-value pairs of other users

A client can be a manager :
1. If earlier the client has been promoted to Manager
1. If the client username are pre-approved to be upgraded to manager when makemanager command is used
1. If the client provides an authorization key to be promoted to Manager level when makemanager command is used


## Memory Details

The memory files kept on the server side are :
1. Client Proifles : Every client's key-value pairs are stored in a csv file in the folder clientProfiles
1. Manager Profiles: Folder 'managerProfiles' keeps two lists :
1.1 managers.csv - a list of all client usernames who have manager authorization. Access mode of manager is provided whenever clients connect with server
1.1 allowed.csv - a list of all client username who can be promoted to manager if they request for the same using 'makemanager'

