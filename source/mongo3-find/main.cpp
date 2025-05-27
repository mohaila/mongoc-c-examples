#include <mongoc/mongoc.h>

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

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

void dropDatabase(mongoc_database_t* database) {
  bson_error_t error;
  if (!mongoc_database_drop(database, &error)) {
    cerr << "Drop database error: " << error.message << endl;
  }
}

void createIndex(mongoc_collection_t* products) {
  auto keys1 = BCON_NEW("price", BCON_INT32(1));
  auto models1 = mongoc_index_model_new(keys1, nullptr);

  auto keys2 = BCON_NEW("category_id", BCON_INT32(1));
  auto models2 = mongoc_index_model_new(keys2, nullptr);

  mongoc_index_model_t* models[] = {models1, models2};

  bson_error_t error;
  if (!mongoc_collection_create_indexes_with_opts(products, models, 2, nullptr, nullptr,
                                                  &error)) {
    cerr << "Failed to create products proce index. "
         << "Error message: " << error.message << endl;
  }
  mongoc_index_model_destroy(models2);
  mongoc_index_model_destroy(models1);
  bson_destroy(keys2);
  bson_destroy(keys1);
}

void findProductsByCategory(mongoc_collection_t* collection, int id) {
  bson_error_t error;
  auto query = BCON_NEW("category_id", BCON_INT32(id));

  auto count = mongoc_collection_count_documents(collection, query, nullptr, nullptr,
                                                 nullptr, &error);
  if (count == -1) {
    cerr << "Find products by category error: " << error.message << endl;
  } else {
    cout << "Found products in category " << id << ": " << count << endl;
  }

  bson_destroy(query);
}

void findProductsByPrice(mongoc_collection_t* collection, double price) {
  bson_error_t error;
  auto query = BCON_NEW("price", "{", "$lte", BCON_DOUBLE(price), "}");

  auto count = mongoc_collection_count_documents(collection, query, nullptr, nullptr,
                                                 nullptr, &error);
  if (count == -1) {
    cerr << "Find products by price error: " << error.message << endl;
  } else {
    cout << "Found products price <= " << price << ": " << count << endl;
  }

  bson_destroy(query);
}

enum class Command { None, Ping, Drop, Insert, Index, Find };

int main(int argc, const char** argv) {
  auto url{"mongodb://host.docker.internal:27017"};

  Command command{Command::None};

  if (argc == 2) {
    if (strcmp(argv[1], "drop") == 0) {
      command = Command::Drop;
    } else if (strcmp(argv[1], "insert") == 0) {
      command = Command::Insert;
    } else if (strcmp(argv[1], "ping") == 0) {
      command = Command::Ping;
    } else if (strcmp(argv[1], "index") == 0) {
      command = Command::Index;
    } else if (strcmp(argv[1], "find") == 0) {
      command = Command::Find;
    }
  } else {
    cerr << "mongo3-find ping|drop|insert|index|find" << endl;
  }

  mongoc_init();

  auto client = getClient(url);
  if (!client) {
    return 1;
  }

  mongoc_database_t* database = nullptr;
  mongoc_collection_t* categories = nullptr;
  mongoc_collection_t* products = nullptr;

  switch (command) {
    case Command::Ping:
      commandPing(client, "store");
      break;

    case Command::Insert:
      database = getDatabase(client, "store");
      categories = getCollection(database, "categories");
      products = getCollection(database, "products");

      createCategories(categories);
      createProducts(products);
      break;

    case Command::Drop:
      database = getDatabase(client, "store");
      dropDatabase(database);
      break;

    case Command::Index:
      database = getDatabase(client, "store");
      products = getCollection(database, "products");
      createIndex(products);
      break;

    case Command::Find:
      database = getDatabase(client, "store");
      products = getCollection(database, "products");
      findProductsByCategory(products, 5);
      findProductsByPrice(products, 1200.0);
      break;

    default:
      break;
  }

  mongoc_collection_destroy(categories);
  mongoc_collection_destroy(products);
  mongoc_database_destroy(database);
  mongoc_client_destroy(client);
  mongoc_cleanup();

  return 0;
}