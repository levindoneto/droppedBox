# DroppedBox

Dropbox-like platform for sharing and automatic synchronization of files among different devices.

## Used Technologies

### Main programming language
* C++

### Main libraries
* I-Notify
* pthread.h
* socket.h

## Organization of the Project

* Client
* Server
* Settings
* Utils

## How to deploy it

```terminal
$ make
$ .dropboxDP
```

Being DP Client or Server.

## How to use it

### Client side

```terminal
$ ./dropboxClient <username> <host> <port>
```

### Server side

```terminal
$ ./dropboxServer <port>
```

See also the full list of [contributors](https://github.com/levindoneto/droppedBox/graphs/contributors) who participated in this project.
