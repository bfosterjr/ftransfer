
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "ftransfer.h"


static
ULONG
_send_file
(
    SOCKET  connectedSocket,
    PCHAR   inputFile,
    PCHAR   destFile
)
{
    ULONG retVal = -1;
    HANDLE hFile = NULL;
    BYTE data[MAX_FILE_CHUNK] = { 0 };
    DWORD bytesRead = 0;
    DWORD fileSize = 0;
    ULONG srvStatus = 0;
    INT bytesSent = 0;
    DWORD totalBytesSent = 0;

    __try
    {
        if (INVALID_HANDLE_VALUE == (hFile = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ,
                                        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
        {

        }
        else if (0 == (fileSize = GetFileSize(hFile, NULL)))
        {

        }
        //ignoring network byte order here...
        else if (SOCKET_ERROR == send(connectedSocket, (PCHAR)&fileSize, sizeof(fileSize), 0) ||
                SOCKET_ERROR == send(connectedSocket, (PCHAR)destFile, (int)strlen(destFile), 0) )
        {

        }
        //ignoring network byte order here...
        else if (sizeof(srvStatus) != recv(connectedSocket, (PCHAR)&srvStatus, sizeof(srvStatus), 0) ||
                 FTRAN_SUCCESS != srvStatus)
        {

        }
        else
        {
            while (ReadFile(hFile, data, sizeof(data), &bytesRead, NULL))
            {
                if (0 == bytesRead)
                {
                    break;
                }
                else if (SOCKET_ERROR == (bytesSent = send(connectedSocket, data, bytesRead, 0)) ||
                    bytesSent != bytesRead)
                {
                    break;
                }
                else
                {
                    totalBytesSent += bytesRead;
                }
            }
        }
    }
    __finally
    {
        CloseHandle(hFile);

        if (totalBytesSent == fileSize)
        {
            retVal = 0;
        }
    }
    return retVal;
}


static
SOCKET
_connect_server
(
    struct addrinfo*  srvinfo
)
{
    SOCKET      connectSocket = INVALID_SOCKET;


    for (; srvinfo != NULL; srvinfo = srvinfo->ai_next) {

        // Create a SOCKET for connecting to server
        if (INVALID_SOCKET == (connectSocket = socket(srvinfo->ai_family,
            srvinfo->ai_socktype, srvinfo->ai_protocol)))
        {
            break;
        }
        else if (SOCKET_ERROR == connect(connectSocket, srvinfo->ai_addr,
            (int)srvinfo->ai_addrlen))
        {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
        }
        else
        {
            break;
        }
    }

    return connectSocket;
}

FTRAN_EXPORT
ULONG
FTRAN_API
ftran_client_send
(
    PCHAR   serverIp,
    PCHAR   port,
    PCHAR   inputFile,
    PCHAR   destFile
)
{
    ULONG           retVal = FTRAN_SUCCESS;
    WSADATA         wsaData = { 0 };
    SOCKET          connectSocket = INVALID_SOCKET;
    BOOL            startup = FALSE;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    __try
    {
        if (NULL == serverIp || 0 == port ||
            NULL == inputFile || NULL == destFile)
        {
            retVal = FTRAN_ERR_ARGS;
        }
        else if (MAX_PATH < strlen(destFile))
        {
            retVal = FTRAN_ERR_ARGS;
        }
        else if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData) ||
            ((startup = TRUE), FALSE))
        {
            retVal = FTRAN_ERR_WSA;
        }
        else if (0 != getaddrinfo(serverIp, port, &hints, &result))
        {
            retVal = FTRAN_ERR_ADDR;
        }
        else if (INVALID_SOCKET == (connectSocket = _connect_server(result)))
        {
            retVal = FTRAN_ERR_CONNECT;
        }
        else
        {
            retVal = _send_file(connectSocket, inputFile, destFile);
        }

    }
    __finally
    {
        if (INVALID_SOCKET != connectSocket)
        {
            shutdown(connectSocket, SD_SEND);
            closesocket(connectSocket);
        }
        if (startup)
        {
            WSACleanup();
        }
    }
    return retVal;
}