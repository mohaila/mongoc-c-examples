# Examples of using MongoDB C Driver
Development using Alpine container.

Tools for C/C++ development:
- cmake
- ninja
- clang-format

.devcontainer contains settings for Dev Containers

.vscode contains settings for Visual Studio Code

Image used for developement is build using docker/Dockerfile. to build:
```sh
cd docker
docker build -t mongoc:devel .
```
Start a MongoDB instance using Docker
```sh
cd infra
docker compose up -d
```
## Examples
### mongo1-connect
Create a MongoDB client, send a ping command and print response or error message
### mongo2-insert
has 3 commands:
- ping: Ping MongoDB Server and print json response or error message
- drop : drop database
- insert: Create two collections and add some documents by using insert one or many
### mongo3-find
has 2 more commands:
- index: creates indexes on products collection
- find: find products by category and by price