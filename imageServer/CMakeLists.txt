aux_source_directory(. SRC_LIST)
include_directories(base)
aux_source_directory(base SRC_LIST)

find_package(Threads)
find_package( OpenCV REQUIRED )
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} )