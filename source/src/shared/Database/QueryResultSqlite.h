
#if !defined(QUERYRESULTSQLITE_H)
#define QUERYRESULTSQLITE_H

#include "sqlite3.h"

class QueryResultSqlite : public QueryResult
{
    public:
        QueryResultSqlite(sqlite3_stmt* res, uint32 rowCount, uint32 fieldCount);

        ~QueryResultSqlite();

        bool NextRow();

    private:
  
        sqlite3_stmt* mResult ;
};
#endif

