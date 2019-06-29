AUX_SOURCE_DIRECTORY(. DIR_CLIENT_SRCS)
add_executable( Client ${DIR_CLIENT_SRCS})



target_link_libraries(Client  pthread)
