# DroppedBox

Dropbox-like platform for sharing and automatic synchronization of files among different users with 2 or less devices.

## Used Technologies

### Main programming language
* C++

### Main libraries
* pthread.h
* arpa/inet.h
* sys/socket.h

## Organization of the Project

* Client
* Server
* Settings
* Utils

## How to deploy it

```terminal
$ make dropboxDP
```

Being *DP* Client or Server.

## How to use it

### Client side

```terminal
$ ./dropboxClient <username> <host> <port>
```

### Server side

```terminal
$ ./dropboxServer <port> --x
```

Being *x* **p** for primary servers or **b** for the backups.


See also the full list of [contributors](https://github.com/levindoneto/droppedBox/graphs/contributors) who participated in this project.
