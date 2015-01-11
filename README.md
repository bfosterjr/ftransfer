# FTRANSER #

----------

FTRANSFER is a simple file transfer client/server application. Nothing sexy or secure.


## Platforms ##

FTRANSFER builds using VS2013 and is supported on x86 and x64 Windows versions (binaries provided)

## Usage ##

	FTRANSFER Usage:

        Server:
                ftransfer.exe -s -p <listen port>
        Client:
                ftransfer.exe -c -a <server address> -p <port> -f <input file> -d <destination file>




## Example ##

Start a server (server will remain running until killed):

    ftransfer.exe -s -p 777


Send a file the using client:

    ftransfer.exe -c -a localhost -p 777 -f c:\temp\test.txt -d c:\temp\test2.txt
