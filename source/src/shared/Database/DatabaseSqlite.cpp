
#include "DatabaseEnv.h"
#include "Util.h"
#include "Policies/SingletonImp.h"
#include "Platform/Define.h"
#include "../src/zthread/ThreadImpl.h"
#include "sqlite3.h"

using namespace std;


DatabaseSqlite::DatabaseSqlite() : Database(), mSqlite(0)
{
}

DatabaseSqlite::~DatabaseSqlite()
{
    if (mSqlite)
        sqlite3_close(mSqlite);
}

bool DatabaseSqlite::Initialize(const char *infoString)
{
	int result ;

	result = sqlite3_open( infoString, &mSqlite ) ;

	if( mSqlite == NULL || result != SQLITE_OK )
	{
		sLog.outError("SQLiteDatabase: Could not open %s.", infoString);
		return false;
	}
  
    return true;
}

QueryResult* DatabaseSqlite::PQuery(const char *format,...)
{
	if(!format) return NULL;

	va_list ap;
	char szQuery [1024];
	va_start(ap, format);
	int res = vsnprintf( szQuery, 1024, format, ap );
	va_end(ap);

	if(res==-1)
	{
		sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
		return false;
	}

	return Query(szQuery);
}

QueryResult* DatabaseSqlite::Query(const char *sql)
{
	if (!mSqlite)
		return NULL ;
	
	sqlite3_stmt* stmt = NULL ;


	{
		ZThread::Guard<ZThread::FastMutex> query_connection_guard((ZThread::ThreadImpl::current()==tranThread?tranMutex:mMutex));

		if ( sqlite3_prepare(mSqlite, sql, strlen(sql), &stmt, NULL) != SQLITE_OK )
		{
			sLog.outError("SQLiteDatabase Query Prepare failed '%s'.", sql) ;
			return NULL ;
		}

		uint32 uRows = 0 ;
		uint32 uFields = 0 ;

		while ( 1 )
		{
			int result=sqlite3_step(stmt) ;
			if (result == SQLITE_ROW)
			{
				uRows++ ;
			}
			else if (result == SQLITE_ERROR)
			{
				sLog.outError("SQLiteDatabase Query failed '%s'.", sql) ;
				sqlite3_finalize(stmt) ;
				return NULL ;
			}
			else
			{
				sqlite3_reset(stmt) ;
				break ;
			}
		}
		
		uFields = (uint32)sqlite3_column_count(stmt) ;
	
		
		if( uRows == 0 || uFields == 0 )
		{
			sqlite3_finalize(stmt) ;
			return NULL ;
		}

		QueryResultSqlite *queryResult = new QueryResultSqlite(stmt, uRows, uFields) ;
		queryResult->NextRow() ;

		return queryResult;
	}


}

bool DatabaseSqlite::Execute(const char *sql)
{
    char *errmsg;

    if (!mSqlite)
        return false;

    if (sqlite3_exec(mSqlite, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		sLog.outError("SQL ERROR:%s\nERRMSG: %s\n", sql, errmsg) ;
        return false ;
	}

    return true;
}

bool DatabaseSqlite::PExecute(const char * format,...)
{
	if (!format)
		return false;

	va_list ap;
	char szQuery [1024];
	va_start(ap, format);
	int res = vsnprintf( szQuery, 1024, format, ap );
	va_end(ap);

	if(res==-1)
	{
		sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
		return false;
	}

	return Execute(szQuery);
}

bool DatabaseSqlite::_TransactionCmd(const char *sql)
{
	char *errmsg;
	if (sqlite3_exec(mSqlite, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		sLog.outError("SQL ERROR: %s\n ERRMSG: %s\n", sql, errmsg) ;
		return false;
	}
	else
	{
		DEBUG_LOG("SQL: %s", sql);
	}
	return true;
}

bool DatabaseSqlite::BeginTransaction()
{
	if (!mSqlite)
		return false;
	if (tranThread==ZThread::ThreadImpl::current())
		return false;                                       // huh? this thread already started transaction
	mMutex.acquire();
	
	// mMutex가 TRANS역할을 해주므로 굳이 필요가 없다.

	//if (!_TransactionCmd("BEGIN TRANSACTION"))
	//{
	//	mMutex.release();                                   // can't start transaction
	//	return false;
	//}
	
	// transaction started
	tranThread = ZThread::ThreadImpl::current();            // owner of this transaction
	return true;
}

bool DatabaseSqlite::CommitTransaction()
{
	if (!mSqlite)
		return false;
	if (tranThread!=ZThread::ThreadImpl::current())
		return false;
	
	//bool _res = _TransactionCmd("COMMIT");

	tranThread = NULL;
	mMutex.release();

	//return _res;
	return true ;
}

bool DatabaseSqlite::RollbackTransaction()
{
	if (!mSqlite)
		return false;
	if (tranThread!=ZThread::ThreadImpl::current())
		return false;
	
	//bool _res = _TransactionCmd("ROLLBACK");
	
	tranThread = NULL;
	mMutex.release();

	//return _res;
	return true ;

}

unsigned long DatabaseSqlite::escape_string(char *to, const char *from, unsigned long length)
{
	if (!mSqlite || !to || !from || !length)
		return 0;

	sqlite3_snprintf((int)length+1, to, from) ;

	return length ;
}
