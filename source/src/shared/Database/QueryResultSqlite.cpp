
#include "DatabaseEnv.h"
#include "sqlite3.h"

QueryResultSqlite::QueryResultSqlite(sqlite3_stmt* res, uint32 rowCount, uint32 fieldCount) :
QueryResult(rowCount, fieldCount), mResult(res)
{
	mCurrentRow = new Field[fieldCount] ;
}

QueryResultSqlite::~QueryResultSqlite()
{
	sqlite3_finalize(mResult) ;
	delete [] mCurrentRow ;
}

bool QueryResultSqlite::NextRow()
{
	if (sqlite3_step(mResult) != SQLITE_ROW)
		return false ;

	for (uint32 i = 0; i < mFieldCount; ++i)
		mCurrentRow[i].SetValue((const char*)sqlite3_column_text(mResult, i));

	return true;
}

