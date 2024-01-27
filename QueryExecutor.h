#pragma once

#include <string>
#include <memory>
#include "Database.h"

class QueryExecutor {
protected:
    std::shared_ptr<Database> db; // Make sure this is a shared pointer
public:
    explicit QueryExecutor(const std::shared_ptr<Database> &sharedDB);

    virtual void execute(const std::string &query); // Function to execute a query
};