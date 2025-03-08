#include "lib.h"

#define MX 1000007 
#define MOD 1000000007

int A[MX], p[MX];
char a[100][100], b[100][100]; 

__int32_t get_hash(char *str, __int8_t len) {
    __int32_t res = 0;

    for (size_t i = 0; i < len; i++) {  
        if (str[i] >= 'a' && str[i] <= 'z') {
            res = (res * 31 + str[i] - 'a' + 1) % MOD;
        }
    }

    return res;
}

__int32_t hash_for_input(char *str) {
    __int8_t len = strlen(str);
    return get_hash(str, len);
}

__int32_t hash_for_file(char *str, __int8_t len) {
    return get_hash(str, len);
} 