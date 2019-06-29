AUX_SOURCE_DIRECTORY(. DIR_CLIENT_SRCS)
add_executable( Server ${DIR_CLIENT_SRCS})



target_link_libraries(Server  pthread)
