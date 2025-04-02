#include "lib.h"

SSL_CTX *init_ssl() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    if (SSL_CTX_use_certificate_file(ctx, "ssl/cert.pem", SSL_FILETYPE_PEM) <= 0 
    || SSL_CTX_use_PrivateKey_file(ctx, "ssl/key", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}