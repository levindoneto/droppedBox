int writeToSocket(string socketDesc, char* buffer,  int bufferSize) {
  printf(">>> ");
  /*
  bzero(buffer, bufferSize);
  fgets(buffer, bufferSize, stdin);

  status = sendto(
    socketDesc,
    buffer,
    strlen(buffer),
    0,
    (const struct sockaddr *) &serverAddress,
    sizeof(struct sockaddr_in)
  );
  if (status < 0) {
    throwError("Error on sending message");
  }

  lenSckAddr = sizeof(struct sockaddr_in);
  status = recvfrom(
    socketDesc,
    buffer,
    BUFFER_SIZE,
    0,
    (struct sockaddr *) &from,
    &lenSckAddr
  );

  if (status < 0) {
    throwError("Error on receive ack");
  }

  cout << "Got an ack: " << buffer << endl;
  */
  return 0;
}

int readFromSocket(string socketDesc, char* buffer) {
  return 0;
}
