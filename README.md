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
### mongo1-onnect
Create a MongoDB client, send a ping command and print response or error message
### mongo2-insert
Create two collections and add some documents by using insert one or many