/*
 * boblight UnixSocket
 * Copyright (C) Speedy1985
 * 
 * boblight is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * boblight is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "inclstdint.h"

#include <stdio.h>
#include <sys/socket.h>
#include <iostream> //debug
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/un.h>

#include "tcpsocket.h"
#include "unixsocket.h"

#include "misc.h"

using namespace std;


CUnixSocket::CUnixSocket()
{
  m_sock = -1;
}

CUnixSocket::~CUnixSocket()
{
  Close();
}

//can't open int the base class
int CUnixSocket::Open(int usectimeout)
{
  return FAIL;
}

void CUnixSocket::Close()
{
  if (m_sock != -1)
  {
    close(m_sock);
    m_sock = -1;
  }
}

int CUnixSocket::SetSockOptions()
{
  //set unix keepalive
  //SetKeepalive();

  return SUCCESS;
}


int CUnixSocket::SetKeepalive()
{
#if defined(SO_KEEPALIVE)

  int flag = 1;

  //turn keepalive on
  if (setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) == -1)
  {
    m_error = "SO_KEEPALIVE " + GetErrno();
    return FAIL;
  }

#else

#warning keepalive support not compiled in

#endif

  return SUCCESS;
}

//open a client socket
int CUnixClientSocket::Open(int usectimeout /*= -1*/)
{

  Close(); //close it if it was opened
  m_usectimeout = usectimeout;
  
  m_sock = socket(AF_UNIX, SOCK_STREAM, 0);

  if (m_sock == -1) //can't make socket
  {
    m_error = "socket() " + GetErrno();
    return FAIL;
  }
  
  struct sockaddr_un server; //where we connect to
  memset(&server, 0, sizeof(server));

  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, SERVER_PATH);

  if (connect(m_sock, reinterpret_cast<struct sockaddr*>(&server), sizeof(server)) < 0)
  {
    if (errno != EINPROGRESS) //because of the non blocking socket, this means we're still connecting
    {
      m_error = "connect() " + ToString(SERVER_PATH) + ": " + GetErrno();
      return FAIL;
    }
  }
  
  int returnv = WaitForSocket(true, "Connect");//wait for the socket to become writeable, that means the connection is established

  if (returnv == FAIL || returnv == TIMEOUT)
    return returnv;

  return SUCCESS;
}

int CUnixClientSocket::Read(CData& data)
{
  uint8_t buff[1000];
  
  if (m_sock == -1)
  {
    m_error = "socket closed";
    return FAIL;
  }
  
  int returnv = WaitForSocket(false, "Read");//wait until the socket has something to read
  if (returnv == FAIL || returnv == TIMEOUT)
    return returnv;

  //clear the data
  data.Clear();

  //loop until the socket has nothing more in its buffer
  while(1)
  {
    int size = recv(m_sock, buff, sizeof(buff), 0);

    if (errno == EAGAIN && size == -1) //we're done here, no more data, the call to WaitForSocket made sure there was at least some data to read
    {
      return SUCCESS;
    }    
    else if (size == -1) //socket had an error
    {
      m_error = "recv() " + ToString(SERVER_PATH) + ":" + GetErrno();
      return FAIL;
    }
    else if (size == 0 && data.GetSize() == 0) //socket closed and no data received
    {
      m_error = ToString(SERVER_PATH) + ":" + " Connection closed";
      return FAIL;
    }
    else if (size == 0) //socket closed but data received
    {
      return SUCCESS;
    }

    data.SetData(buff, size, true); //append the data
  }

  return SUCCESS;
}

int CUnixClientSocket::Write(CData& data)
{
  if (m_sock == -1)
  {
    m_error = "socket closed";
    return FAIL;
  }

  int bytestowrite = data.GetSize();
  int byteswritten = 0;

  //loop until we've written all bytes
  while (byteswritten < bytestowrite)
  {
    //wait until socket becomes writeable
    int returnv = WaitForSocket(true, "Write");

    if (returnv == FAIL || returnv == TIMEOUT)
      return returnv;
      
    int size = send(m_sock, data.GetData() + byteswritten, data.GetSize() - byteswritten, 0);
    
    if (size == -1)
    {
      m_error = "send() " + ToString(SERVER_PATH) + ":" + GetErrno();
      return FAIL;
    }

    byteswritten += size;
  }
  return SUCCESS;
}

int CUnixServerSocket::Open(int usectimeout)
{
  m_usectimeout = usectimeout;
  
  // Close old conncection if there is.
  Close();
  
  m_usectimeout = usectimeout;
  m_sock = socket(AF_UNIX, SOCK_STREAM, 0);

  if (m_sock == -1)
  {
    m_error = "socket() " + ToString(SERVER_PATH) + ":" + GetErrno();
    return FAIL;
  }
  
  int opt = 1;
  setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
      
   // Remove old file
  unlink(SERVER_PATH);
  
  struct sockaddr_un bindaddr;
  
  // bind the socket
  memset(&bindaddr, 0, sizeof(bindaddr));
  bindaddr.sun_family = AF_UNIX;
  strcpy(bindaddr.sun_path, SERVER_PATH);
  
  bind(m_sock, reinterpret_cast<struct sockaddr*>(&bindaddr), sizeof(bindaddr));

  if (listen(m_sock, SOMAXCONN) < 0)
  {
    m_error = "listen() " + ToString(SERVER_PATH) + ": " + GetErrno();
    return FAIL;
  }else{
    printf("Listen ok!\n");
  }
  return SUCCESS;
}

//wait until the socket becomes readable or writeable
int CUnixSocket::WaitForSocket(bool write, std::string timeoutstr)
{
  printf("CUnixSocket::WaitForSocket\n");  
  int returnv;
  fd_set rwsock;
  struct timeval *tv = NULL;

  //add the socket to the fd_set
  FD_ZERO(&rwsock);
  FD_SET(m_sock, &rwsock);

  //set the timeout
  struct timeval timeout;
  if (m_usectimeout > 0)
  {
    timeout.tv_sec = m_usectimeout / 1000000;
    timeout.tv_usec = m_usectimeout % 1000000;
    tv = &timeout;
  }

  if (write)
    returnv = select(m_sock + 1, NULL, &rwsock, NULL, tv);
  else
    returnv = select(m_sock + 1, &rwsock, NULL, NULL, tv);
  
  if (returnv == 0) //select timed out
  {
    m_error = ToString(SERVER_PATH) + ": " + timeoutstr + " timed out"; 
    return TIMEOUT;
  }
  else if (returnv == -1) //select had an error
  {
    m_error = "select() " + GetErrno();
    return FAIL;
  }

  //check if the socket had any errors, connection refused is a common one
  int sockstate, sockstatelen = sizeof(sockstate);
  returnv = getsockopt(m_sock, SOL_SOCKET, SO_ERROR, &sockstate, reinterpret_cast<socklen_t*>(&sockstatelen));
  
  if (returnv == -1) //getsockopt had an error
  {
    m_error = "getsockopt() " + GetErrno();
    return FAIL;
  }
  else if (sockstate) //socket had an error
  {
    m_error = "SO_ERROR " + ToString(SERVER_PATH) + ": " + GetErrno(sockstate);
    return FAIL;
  }

  return SUCCESS;
}

int CUnixServerSocket::Accept(CUnixClientSocket& socket)
{
  printf("CUnixSocket::Accept\n");  
  struct sockaddr_un client;
  socklen_t clientlen = sizeof(client);

  if (m_sock == -1)
  {
    m_error = "socket closed";
    return FAIL;
  }

  int returnv = WaitForSocket(false, "Accept");  //wait for socket to become readable
  if (returnv == FAIL || returnv == TIMEOUT)
    return returnv;
  
  int sock = accept(m_sock, reinterpret_cast<struct sockaddr*>(&client), &clientlen);
  if (sock < 0)
  {
    m_error = "accept() " + GetErrno();
    return FAIL;
  }

  /*if (socket.SetInfo(inet_ntoa(client.sin_addr), ntohs(client.sin_port), sock) != SUCCESS)
  {
    m_error = socket.GetError();
    return FAIL;
  }*/
  
  return SUCCESS;
}
