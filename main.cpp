#include "Database.h"
#include "QueryExecutor.h"
#include "CommandLineInterface.h"
#include <fmt/core.h>


int main() {
    auto db = std::make_shared<Database>();
    auto qe = std::make_shared<QueryExecutor>(db);
    CommandLineInterface cli(qe);
    cli.run();
    return 0;
}

