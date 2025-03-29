#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <mongoc/mongoc.h>

// Khai báo hàm xử lý token
void handle_token(const char *name, const char *pass);

#endif // LIB_H
