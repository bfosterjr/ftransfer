#include <Windows.h>

#pragma comment (lib, "Ws2_32.lib")

#ifdef BUILD_DLL
#define FTRAN_EXPORT             __declspec( dllexport ) 
#else
#define FTRAN_EXPORT
#endif



#define FTRAN_API                __cdecl

#define MAX_FILE_CHUNK           0x1000

#define FTRAN_SUCCESS            0x00000000
#define FTRAN_ERR_ARGS           0x00000001
#define FTRAN_ERR_INVALID_SOCKET 0x00000002
#define FTRAN_ERR_SOCKET         0x00000003
#define FTRAN_ERR_WSA            0x00000004
#define FTRAN_ERR_SOCKET_CREATE  0x00000005
#define FTRAN_ERR_SOCKET_BIND    0x00000006
#define FTRAN_ERR_SOCKET_LISTEN  0x00000007
#define FTRAN_ERR_ADDR           0x00000008
#define FTRAN_ERR_CONNECT        0x00000009
#define FTRAN_ERR_FILE_SEND      0x0000000A
#define FTRAN_ERR_FILE_RECV      0x0000000B
#define FTRAN_ERR_FILE_IO        0x0000000C
#define FTRAN_ERR_FILE_CREATE    0x0000000D


FTRAN_EXPORT
ULONG
FTRAN_API
ftran_server_start
(
    PCHAR  port
);

FTRAN_EXPORT
ULONG
FTRAN_API
ftran_client_send
(
    PCHAR   serverIp,
    PCHAR   port,
    PCHAR   inputFile,
    PCHAR   destFile
);