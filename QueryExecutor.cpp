#include "QueryExecutor.h"


#include "Logger.h"
#include "Lexer.h"
#include "Parser.h"

QueryExecutor::QueryExecutor(const std::shared_ptr<Database> &sharedDB) : db(sharedDB) {} // Constructor


void QueryExecutor::execute(const std::string &query) {
    Lexer lexer(query);
    Parser parser(lexer);
    try {
        std::unique_ptr<Query> parsedQuery = parser.parseQuery();

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "ConstantConditionsOC"
        #pragma ide diagnostic ignored "UnreachableCode"

        if (auto selectQuery = dynamic_cast<SelectQuery *>(parsedQuery.get())) {
            db->selectFrom(*selectQuery);
        } else if (auto createTableQuery = dynamic_cast<CreateTableQuery *>(parsedQuery.get())) {
            db->createTable(*createTableQuery);
        } else if (auto insertQuery = dynamic_cast<InsertQuery *>(parsedQuery.get())) {
            db->insertInto(*insertQuery);
        } else if (auto alterQuery = dynamic_cast<AlterTableQuery *>(parsedQuery.get())) {
            db->alterTable(*alterQuery);
        } else if (auto dropQuery = dynamic_cast<DropTableQuery *>(parsedQuery.get())) {
            db->dropTable(*dropQuery);
        } else {
            throw std::runtime_error("Unknown query type");
        }

        #pragma clang diagnostic pop

    } catch (const std::exception &e) {
        Logger::error(e.what());
    }
}
