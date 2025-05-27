#include <mongoc/mongoc.h>

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

mongoc_client_t* getClient(const char* url) {
  bson_error_t error;
  auto uri = mongoc_uri_new_with_error(url, &error);
  if (!uri) {
    cerr << "Failed to create URI: " << url << endl
         << "Error message: " << error.message << endl;
    return nullptr;
  }

  auto client = mongoc_client_new_from_uri_with_error(uri, &error);
  if (!client) {
    cerr << "Failed to connect to: " << url << endl
         << "Error message: " << error.message << endl;
    mongoc_uri_destroy(uri);

    return nullptr;
  }
  mongoc_uri_destroy(uri);
  return client;
}

void commandPing(mongoc_client_t* client, const char* name) {
  bson_t reply;
  bson_error_t error;

  auto command = BCON_NEW("ping", BCON_INT32(1));
  if (!mongoc_client_command_simple(client, name, command, nullptr, &reply, &error)) {
    bson_destroy(&reply);
    bson_destroy(command);

    cerr << "Error command: " << error.message << endl;
    return;
  }

  auto response = bson_as_json(&reply, nullptr);
  cout << "MongoDB ping response: " << response << endl;
  bson_free(response);

  bson_destroy(&reply);
  bson_destroy(command);
}

int main(int argc, const char** argv) {
  auto url{"mongodb://host.docker.internal:27017"};

  mongoc_init();

  auto client = getClient(url);
  if (!client) {
    return 1;
  }

  commandPing(client, "store");

  mongoc_client_destroy(client);
  mongoc_cleanup();

  return 0;
}