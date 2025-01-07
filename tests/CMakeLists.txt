add_executable(logger_test LoggerTest.cpp)
add_executable(asyncFileLogger_test AsyncFileLoggerTest.cpp)

set(targets_list
    logger_test
    asyncFileLogger_test
)
set_property(TARGET ${targets_list} PROPERTY CXX_STANDARD 14)

foreach(T ${targets_list})
    target_link_libraries(${T} PRIVATE xiaoLog)
endforeach(T ${targets_list})
