#include "lib.h"

void handle_token(const char *name, const char *pass) {
    printf("Handling token: Name = %s, Password = %s\n", name, pass);

    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_t *doc;
    bson_error_t error;

    mongoc_init();
    client = mongoc_client_new("mongodb://localhost:27017");
    if (!client) {
        fprintf(stderr, "Failed to connect to MongoDB\n");
        return;
    }

    collection = mongoc_client_get_collection(client, "testdb", "users");
    doc = bson_new();
    BSON_APPEND_UTF8(doc, "name", name);
    BSON_APPEND_UTF8(doc, "password", pass);

    if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, &error)) {
        fprintf(stderr, "Insert failed: %s\n", error.message);
    } else {
        printf("User inserted successfully\n");
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}