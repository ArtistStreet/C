#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <mongoc/mongoc.h>
<<<<<<< HEAD

void handle_token(const char *name, const char *pass);
=======
#include <openssl/ssl.h>
#include <openssl/err.h>


void handle_token(const char *name, const char *pass);
SSL_CTX *init_ssl();
>>>>>>> 2f0e4a7 (update)
