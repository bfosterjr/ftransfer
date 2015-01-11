
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ftransfer.h"

#define MAX_TEMP_PATH   MAX_PATH - 16
static
ULONG
_handle_client
(
SOCKET  clientSocket
)
{
    ULONG       retVal                      = FTRAN_SUCCESS;
    INT         bytesRecv                   = 0;
    HANDLE      hFile                       = NULL;
    CHAR        filePath[MAX_PATH]          = { 0 };
    DWORD       fileSize                    = 0;
    CHAR        tempPath[MAX_TEMP_PATH]     = { 0 };
    CHAR        tempFile[MAX_PATH]          = { 0 };
    BYTE        data[MAX_FILE_CHUNK]        = { 0 };
    DWORD       bytesWritten                = 0;
    DWORD       totalBytes                  = 0;

   
    __try
    {
        //ignoring network byte order here...
        if (sizeof(fileSize) != (bytesRecv = recv(clientSocket, (PCHAR)&fileSize, sizeof(fileSize), 0)))
        {
            retVal = FTRAN_ERR_FILE_RECV;
        }
        else if (0 >= (bytesRecv = recv(clientSocket, (PCHAR)filePath, sizeof(filePath), 0)))
        {
            retVal = FTRAN_ERR_FILE_RECV;
        }
        /*
        else if (0 >= GetTempPathA(sizeof(tempPath), tempPath))
        {

        }
        else if (0 == GetTempFileNameA(tempPath, "ft", 0, tempFile))
        {

        }
        */
        else if (INVALID_HANDLE_VALUE == (hFile = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)))
        {
            retVal = FTRAN_ERR_FILE_CREATE;
        }

        //ignoring network byte order here...
        if (sizeof(retVal) != send(clientSocket, (PCHAR)&retVal, sizeof(retVal), 0))
        {
            retVal = FTRAN_ERR_SOCKET;
        }
        else if (FTRAN_SUCCESS == retVal)
        {
            do
            {
                if (0 == (bytesRecv = recv(clientSocket, (PCHAR)&data, sizeof(data), 0)))
                {

                }
                else if (0 > bytesRecv)
                {
                    retVal = FTRAN_ERR_SOCKET;
                }
                //got data .. write it
                else if (!WriteFile(hFile,data,bytesRecv,&bytesWritten,NULL))
                {
                    retVal = FTRAN_ERR_FILE_IO;
                }
                else
                {
                    totalBytes += bytesWritten;
                }

            } while (bytesRecv > 0 && FTRAN_SUCCESS == retVal);
        }
    }
    __finally
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            CloseHandle(hFile);
            if (totalBytes != fileSize)
            {
                (void)DeleteFileA(filePath);
            }
        }

        (void)shutdown(clientSocket, SD_SEND);
        closesocket(clientSocket);
    }

    return retVal;
}

static
ULONG
_do_accept
(
    SOCKET          listenSocket
)
{
    ULONG   retVal = FTRAN_SUCCESS;
    SOCKET  clientSocket = INVALID_SOCKET;

    do
    {
        if (INVALID_SOCKET == (clientSocket = accept(listenSocket, NULL, NULL)))
        {
            retVal = FTRAN_ERR_INVALID_SOCKET;
        }
        else
        {
            retVal = _handle_client(clientSocket);
        }
    } while (FTRAN_SUCCESS == retVal);

    return retVal;
}



ULONG
FTRAN_API
ftran_server_start
(
PCHAR  port
)
{
    ULONG           retVal = FTRAN_SUCCESS;
    WSADATA         wsaData = { 0 };
    SOCKET          listenSocket = INVALID_SOCKET;
    BOOL            startup = FALSE;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    __try
    {
        if (NULL == port)
        {
            retVal = FTRAN_ERR_ARGS;
        }
        else if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData) ||
            ((startup = TRUE), FALSE))
        {
            retVal = FTRAN_ERR_WSA;
        }
        else if (0 != getaddrinfo(NULL, port, &hints, &result))
        {
            retVal = FTRAN_ERR_ADDR;
        }
        else if (INVALID_SOCKET == (listenSocket = socket(result->ai_family,
            result->ai_socktype, result->ai_protocol)))
        {
            retVal = FTRAN_ERR_SOCKET_CREATE;
        }
        else if (SOCKET_ERROR == bind(listenSocket, result->ai_addr, (int)result->ai_addrlen))
        {
            retVal = FTRAN_ERR_SOCKET_BIND;
        }
        else if (SOCKET_ERROR == listen(listenSocket, SOMAXCONN))
        {
            retVal = FTRAN_ERR_SOCKET_LISTEN;
        }
        else
        {
            retVal = _do_accept(listenSocket);
        }
    }
    __finally
    {

        if (NULL != result)
        {
            freeaddrinfo(result);
            result = NULL;
        }

        if (INVALID_SOCKET != listenSocket)
        {
            closesocket(listenSocket);
            listenSocket = INVALID_SOCKET;
        }

        if (startup)
        {
            (void)WSACleanup();
        }
    }
    return retVal;
}
