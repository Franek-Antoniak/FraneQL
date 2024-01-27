#pragma once
#include "QueryExecutor.h"


class CommandLineInterface {
    std::shared_ptr<QueryExecutor> queryExecutor;
    std::vector<std::string> commandHistory;
    std::vector<std::string> successfulQueries;
public:
    explicit CommandLineInterface(std::shared_ptr<QueryExecutor> queryExecutor);
    virtual void run();
    virtual void printHistory();
    virtual void saveQueries();
    virtual void loadQueries(const std::string &filename);
    virtual void addQueryToSuccessfulQueries(std::string const& basicString);
};