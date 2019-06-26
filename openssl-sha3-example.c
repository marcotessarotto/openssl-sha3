#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
/*
 * building on Debian requires libssl-dev package (sudo apt install libssl-dev)
 */

#define BUF_SIZE 1024

#define HANDLE_ERROR(msg) { fprintf(stderr, "%s\n", msg); exit(EXIT_FAILURE); }
#define HANDLE_ERROR2(msg, mdctx) { fprintf(stderr, "%s\n", msg); EVP_MD_CTX_destroy(mdctx); exit(EXIT_FAILURE); }

int main(int argc,  char * const argv[]) {

	int opt;
	char * endptr;
	char buffer[BUF_SIZE];
	int bytes_read;

	EVP_MD_CTX * mdctx;
	int val;
	unsigned char * digest;
	unsigned int digest_len;
	EVP_MD * algo = NULL;

    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
        case 't':
        	errno = 0;

        	val = strtol(optarg, &endptr, 10);

            if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                    || (errno != 0 && val == 0)) {
                perror("Wrong value for t parameter");
                exit(EXIT_FAILURE);
            }

            if (endptr == optarg) {
                fprintf(stderr, "No value was found for t parameter\n");
                exit(EXIT_FAILURE);
            }

            switch (val) {
			case 224:
				algo = EVP_sha3_224();
				break;
			case 256:
				algo = EVP_sha3_256();
				break;
			case 384:
				algo = EVP_sha3_384();
				break;
			case 512:
				algo = EVP_sha3_512();
				break;
			default:
				fprintf(stderr,"Wrong value for t parameter (valid values: 224, 256, 384, 512)");
				exit(EXIT_FAILURE);
            }

            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t sha3_size]\n"
            		"Example program which calculates SHA-3 hash of data read from stdin.\n"
            		"Uses openssl implementation of SHA-3 algorithm.\n"
            		"sha3_size can be: 224, 256, 384, 512. Default is 256.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (algo == NULL) {
    	algo = EVP_sha3_256();
    }


	if ((mdctx = EVP_MD_CTX_create()) == NULL) {
		HANDLE_ERROR("EVP_MD_CTX_create() error")
	}

	// initialize digest engine
	if (EVP_DigestInit_ex(mdctx, algo, NULL) != 1) { // returns 1 if successful
		HANDLE_ERROR2("EVP_DigestInit_ex() error", mdctx)
	}

	while ((bytes_read = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0) { // read returns 0 on EOF, -1 on error
		// provide data to digest engine
		if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) { // returns 1 if successful
			HANDLE_ERROR2("EVP_DigestUpdate() error", mdctx)
		}

	}

	if (bytes_read == -1) {
		perror("read error");
		exit(1);
	}

	digest_len = EVP_MD_size(algo);

	if ((digest = (unsigned char *)OPENSSL_malloc(digest_len)) == NULL) {
		HANDLE_ERROR2("OPENSSL_malloc() error", mdctx)
	}

	// produce digest
	if (EVP_DigestFinal_ex(mdctx, digest, &digest_len) != 1) { // returns 1 if successful
		OPENSSL_free(digest);
		HANDLE_ERROR2("EVP_DigestFinal_ex() error", mdctx)
	}

	for (int i = 0; i < digest_len; i++) {
		printf("%02x", digest[i]);
	}

	OPENSSL_free(digest);
	EVP_MD_CTX_destroy(mdctx);

	return 0;
}
