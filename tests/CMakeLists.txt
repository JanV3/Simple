add_executable(signal_test "signal_test.cpp")
target_link_libraries(signal_test Simple)
add_test(NAME signal_test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/signal_test)

add_executable(thread_pool_test "thread_pool_test.cpp")
target_link_libraries(thread_pool_test Simple)
IF( NOT WIN32 )
target_link_libraries(thread_pool_test "-lstdc++fs")
ENDIF()
add_test(NAME thread_pool_test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/thread_pool_test)

add_executable(logger_test "logger_test.cpp")
target_link_libraries(logger_test Simple)
IF( NOT WIN32 )
target_link_libraries(logger_test "-lstdc++fs")
ENDIF()
add_test(NAME logger_test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/logger_test)

add_executable(condition_buffer_test "condition_buffer_test.cpp")
target_link_libraries(condition_buffer_test Simple)
IF( NOT WIN32 )
target_link_libraries(condition_buffer_test "-lstdc++fs")
ENDIF()
add_test(NAME condition_buffer_test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/condition_buffer_test)

#add_executable(http_server_test "http_server_test.cpp")
#target_link_libraries(http_server_test Simple SimpleHttpServer)
#add_test(NAME http_server_test
         #WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         #COMMAND ${CMAKE_BINARY_DIR}/bin/http_server_test)

add_executable(zipper_test "zipper_test.cpp")
target_link_libraries(zipper_test Simple)

add_executable(static_map_test "static_map_test.cpp")
add_test(NAME static_map_test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/static_map_test)
target_link_libraries(static_map_test Simple)
