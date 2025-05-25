#include <mongoc/mongoc.h>

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

struct Category {
  long id;
  char* name;
  char* description;
};

struct Product {
  long id;
  char* name;
  char* description;
  double price;
  long categoryId;
};

mongoc_database_t* getDatabase(mongoc_client_t* client, const char* name) {
  return mongoc_client_get_database(client, name);
}

mongoc_collection_t* getCollection(mongoc_database_t* database, const char* name) {
  return mongoc_database_get_collection(database, name);
}

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
    cerr << "Error command: " << error.message << endl;
    return;
  }

  auto response = bson_as_json(&reply, nullptr);
  cout << "MongoDB ping response: " << response << endl;
  bson_free(response);

  bson_destroy(&reply);
  bson_destroy(command);
}

void createCategories(mongoc_collection_t* collection) {
  char name[32];
  char description[32];
  bson_t* categories[10];
  for (auto i = 0; i < 10; i++) {
    sprintf(name, "Category #%d", i + 1);
    sprintf(description, "Category #%d description", i + 1);

    auto document = BCON_NEW("_id", BCON_INT32(i), "name", BCON_UTF8(name), "description",
                             BCON_UTF8(description));
    categories[i] = document;
  }

  bson_error_t error;
  if (!mongoc_collection_insert_many(collection, (const bson_t**)&categories, 10, nullptr,
                                     nullptr, &error)) {
    cerr << "Error create categories: " << error.message << endl;
  }

  for (auto i = 0; i < 10; i++) {
    bson_destroy(categories[i]);
  }
}

void createProducts(mongoc_collection_t* collection) {
  char name[32];
  char description[32];
  for (auto i = 1; i < 1001; i++) {
    sprintf(name, "Product #%d", i);
    sprintf(description, "Porudct #%d awesome description", i);
    auto categoryId = i % 10;
    if (categoryId == 0) {
      categoryId = 1;
    }
    auto price = 999.99 + categoryId * 100.0;
    auto document = BCON_NEW("_id", BCON_INT32(i), "name", BCON_UTF8(name), "description",
                             BCON_UTF8(description), "price", BCON_DOUBLE(price),
                             "category_id", BCON_INT32(categoryId));

    bson_error_t error;
    if (!mongoc_collection_insert_one(collection, document, nullptr, nullptr, &error)) {
      cerr << "Insert product document error: " << error.message << endl;
    }

    bson_destroy(document);
  }
}
int main(int argc, const char** argv) {
  auto url{"mongodb://host.docker.internal:27017"};

  mongoc_init();

  auto client = getClient(url);
  if (!client) {
    return 1;
  }

  auto database = getDatabase(client, "store");
  auto categories = getCollection(database, "categories");
  auto products = getCollection(database, "products");

  commandPing(client, "store");

  createCategories(categories);
  createProducts(products);

  mongoc_collection_destroy(categories);
  mongoc_collection_destroy(products);
  mongoc_database_destroy(database);
  mongoc_client_destroy(client);
  mongoc_cleanup();

  return 0;
}