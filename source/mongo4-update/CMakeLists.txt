set(TARGET mongo4-update)
set(SRC main.cpp)

add_executable(${TARGET} ${SRC})
target_include_directories(${TARGET} PRIVATE 
  /usr/include/libmongoc-1.0 
  /usr/include/libbson-1.0
)
target_link_libraries(${TARGET} PRIVATE 
  mongoc-1.0
  bson-1.0
)