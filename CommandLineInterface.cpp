#include "CommandLineInterface.h"
#include "Lexer.h"
#include <string>
#include <iostream>

#include <fmt/color.h>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <utility>


void CommandLineInterface::printHistory() {
    fmt::print(fg(fmt::color::yellow), "Command History (Last 5 commands):\n");
    // Print last 5 commands
    for (int i = std::max(0, static_cast<int>(commandHistory.size()) - 5); i < commandHistory.size(); ++i) {
        fmt::print(fg(fmt::color::yellow), "{}: {}\n", i, commandHistory[i]);
    }
}


void CommandLineInterface::run() {
    std::string input;
    std::string query;
    fmt::print(fg(fmt::color::green), "Welcome to FranekQL!\n");
    fmt::print(fg(fmt::color::green), "Type \\q to quit, \\s to save current state of database, \\d <filename> to "
                                      "load queries from file, \\h to print command history\n");
    fmt::print(fg(fmt::color::green), "db > ");
    while (true) {
        std::getline(std::cin, input);

        if (input == "\\q") {
            saveQueries();
            break;
        }

        if (input == "\\s") {
            saveQueries();
            continue;
        }

        if (input.substr(0, 2) == "\\d") {
            loadQueries(input.substr(3));
            continue;
        }

        if (input == "\\h") {
            printHistory();
            continue;
        }

        query += input;

        if (!query.empty() && query.back() == ';') {
            commandHistory.push_back(input);
            try {
                queryExecutor->execute(query);
                addQueryToSuccessfulQueries(query);
                // Check if the query is not a SELECT query before adding it to successfulQueries
                fmt::print(fg(fmt::color::green), "db > ");
            } catch (const std::exception &e) {
                fmt::print(fg(fmt::color::red), "Error executing query: {}\n", e.what());
            }
            query.clear();
        }
    }
}

CommandLineInterface::CommandLineInterface(std::shared_ptr<QueryExecutor> queryExecutor)
        : queryExecutor(std::move(queryExecutor)) {}

void CommandLineInterface::saveQueries() {
    std::string filename = "event_source_backup.franekql";

    std::ofstream file(filename, std::ios::app);
    if (!file) {
        fmt::print(fg(fmt::color::red), "Error opening file for writing: {}\n", filename);
        return;
    }

    for (const auto &query: successfulQueries) {
        file << query << std::endl;
    }

    file.close();

    successfulQueries.clear();

    fmt::print(fg(fmt::color::green), "Queries saved to {}\n", filename);
}

void CommandLineInterface::loadQueries(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        fmt::print(fg(fmt::color::red), "Error opening file for reading: {}\n", filename);
        return;
    }

    std::string query;
    while (std::getline(file, query)) {
        try {
            queryExecutor->execute(query);
        } catch (const std::exception &e) {
            std::cerr << "Error executing query: " << e.what() << std::endl;
        }
    }

    file.close();
    fmt::print(fg(fmt::color::green), "Queries loaded from {}\n", filename);
}

void CommandLineInterface::addQueryToSuccessfulQueries(std::string const& query) {
    Lexer lexer(query);
    Token token = lexer.nextToken();

    if (token.type != TokenType::SELECT) {
        successfulQueries.push_back(query);
    }
}

