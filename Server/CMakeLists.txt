# Add Current Directory as Include
include_directories (${CMAKE_CURRENT_LIST_DIR})

add_subdirectory (WorldServerBase)
add_subdirectory (ServerRakNetNetwork)
add_subdirectory (WorldServer)