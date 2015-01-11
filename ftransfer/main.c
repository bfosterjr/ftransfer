
#include <Windows.h>

#ifdef BUILD_DLL

BOOL 
WINAPI 
DllMain
(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#else


#ifndef BUILD_LIB

#include <stdio.h>
#include "Xgetopt.h"
#include "ftransfer.h"


static
void
_print_usage()
{
    printf("FTRANSFER Usage:\n\n");
    printf("\tServer:\n");
    printf("\t\tftransfer.exe -s -p <listen port>\n");
    printf("\tClient:\n");
    printf("\t\tftransfer.exe -c -a <server address> -p <port> -f <input file> -d <destination file>");
    printf("\n\n");
}

int main(int argc, char** argv)
{
    int     c           = 0;
    char*   port        = NULL;
    char*   address     = NULL;
    BOOL    server      = FALSE;
    char*   destFile    = NULL;
    char*   inputFile   = NULL;

    while ((c = getopt(argc, argv, "sca:p:u:d:f:")) != EOF)
    {
        switch (c)
        {
        case ('s') :
            server = TRUE;
            break;
        case ('c') :
            server = FALSE;
            break;
        case ('p') :
            port = optarg;
            break;
        case ('a') :
            address = optarg;
            break;
        case('d') :
            destFile = optarg;
            break;
        case('f') :
            inputFile = optarg;
            break;
        default:
            break;
        }
    }

    if (NULL == port)
    {
        _print_usage();
    }
    else if (server)
    {
        ftran_server_start(port);
    }
    else if (!server            &&
            NULL != address     &&
            NULL != inputFile   &&
            NULL != destFile)
    {
        ftran_client_send(address, port, inputFile, destFile);
    }
    else
    {
        _print_usage();
    }
}

#endif

#endif
