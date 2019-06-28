# openssl-sha3
example program which uses openssl implementation of SHA-3.

SHA-3 (Secure Hash Algorithm 3) is the latest member of the Secure Hash Algorithm family of standards, released by NIST.

This program reads data from stdin and on EOF produces the SHA-3 digest.

It has a command line parameter (-t) which allows to specify the digest lenght in bits. Default value is 256. Valid values are: 224, 256, 384, 512.

Can be opened as an Eclipse IDE project.

Or can be built in this way:

gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -o "openssl-sha3-example.o" openssl-sha3/openssl-sha3-example.c 
gcc -o "openssl-sha3"  ./openssl-sha3-example.o   -lcrypto

