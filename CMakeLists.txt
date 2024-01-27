cmake_minimum_required(VERSION 3.26)
project(PJC)

set(CMAKE_CXX_STANDARD 20)


include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY  https://github.com/fmtlib/fmt
        GIT_TAG         10.0.0
)

FetchContent_MakeAvailable(fmt)

add_executable(PJC main.cpp
        DataType.h
        Column.h
        Column.cpp
        RowBuilder.cpp
        RowBuilder.h
        Table.cpp
        Table.h
        Database.cpp
        Database.h
        QueryExecutor.cpp
        QueryExecutor.h
        CommandLineInterface.cpp
        CommandLineInterface.h
        Logger.cpp
        Logger.h
        Token.h
        Lexer.cpp
        Lexer.h
        Parser.cpp
        Parser.h
        Query.cpp
        Query.h
        Token.cpp
        ForeignKey.cpp
        ForeignKey.h
        PrimaryKey.cpp
        PrimaryKey.h
        Relation.cpp
        Relation.h
        TableValidator.cpp
        TableValidator.h
        RowValidator.cpp
        RowValidator.h
        DataType.cpp
)

target_link_libraries(PJC PRIVATE fmt::fmt)
