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

#ifndef UNIX
#define UNIX

#include <string>
#include <sys/un.h>
#include <vector>

#define FAIL    0
#define SUCCESS 1
#define TIMEOUT 2

#define SERVER_PATH "/tmp/boblightd.socket"

class CUnixSocket //base class
{
  public:
    CUnixSocket();
    ~CUnixSocket();

    virtual int Open(int usectimeout = -1);
    void Close();
    bool IsOpen() { return m_sock != -1; }
    std::string GetError()   { return m_error; }
    std::string GetAddress() { return m_address; }
    int         GetPort()    { return 19444; }
    int         GetSock()    { return m_sock; }

    int         SetSockOptions();
    void        SetTimeout(int usectimeout) { m_usectimeout = usectimeout; }
    
  protected:
    std::string m_address;
    std::string m_error;

    int m_sock;
    int m_usectimeout;
    int SetKeepalive();
    int WaitForSocket(bool write, std::string timeoutstr);
};

class CUnixClientSocket : public CUnixSocket
{
  public:
    int Open(int usectimeout = -1);
    int Read(CData& data);
    int Write(CData& data);
};

class CUnixServerSocket : public CUnixSocket
{
  public:
    int Open(int usectimeout = -1);
    int Accept(CUnixClientSocket& socket);
};
#endif //Unix
