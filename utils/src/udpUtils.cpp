#include "../headers/udpUtils.hpp"
#include "../headers/dropboxUtils.h"

UDPUtils::UDPUtils(int portComm) {
  port = portComm;
  id = socket(AF_INET, SOCK_DGRAM, INIT);
  if (id < EQUAL) {
    char error[ERROR_MSG_SIZE] = "Error on opening socket";
    throwError(error);
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  bzero(&(server_address.sin_zero), 8); // put all zeros in the end
  host = NULL;

  // Used by sendto and rcvfrom
  socklen = sizeof(struct sockaddr_in);

  // Set timeout
  timeout.tv_sec = TIMEO;
  timeout.tv_usec = INIT;
  setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
}

UDPUtils::~UDPUtils() {
  close(id);
}

void UDPUtils::bindServer() { // Every loginion
  set_timeout(0); // Never timeout
  server_address.sin_addr.s_addr = INADDR_ANY;
  if (bind(id, (struct sockaddr *)&server_address, socklen)) {
    throw runtime_error(strerror(errno));
  }
}

void UDPUtils::setIp(string hostname) {
  host = gethostbyname(hostname.c_str());
  if (host == NULL)
  {
    throw invalid_argument(strerror(errno));
  }
  server_address.sin_addr = *((struct in_addr *)host->h_addr);
}

void UDPUtils::send(string bytes) {
  struct sockaddr_in *target_address;
  if (dest_address.sin_port) {
    target_address = &dest_address;
  }
  else if (host) {
    target_address = &server_address;
  }
  else {
    return;
  }
  send_buffer = bytes.data();
  int n = sendto(id, send_buffer, bytes.length(), 0, (const struct sockaddr *)target_address, socklen);
  if (n < EQUAL) {
    throwError("ERROR in socket pra mandar");
  }
}

string UDPUtils::receive() {
  memset(receive_buffer, 0, sizeof(receive_buffer));
  int n = recvfrom(id, receive_buffer, SOCKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_address, &socklen);
  if (n < 0) {
    if (errno == EWOULDBLOCK) {
      throw timeout_exception();
    }
    throw runtime_error(strerror(errno));
  }
  return string(receive_buffer, n);
}

sockaddr_in UDPUtils::get_sender_address() {
  return sender_address;
}

void UDPUtils::set_timeout(int seconds) {
  timeout.tv_sec = seconds;
  setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
}

void UDPUtils::set_dest_address(sockaddr_in new_dest_address) {
  dest_address = new_dest_address;
}

UDPUtils* UDPUtils::get_answerer() {
  UDPUtils* answerer = new UDPUtils(port);
  answerer->dest_address = sender_address;
  return answerer;
}
