
#ifndef _DATABASESQLITE_H
#define _DATABASESQLITE_H

#include "Policies/Singleton.h"
#include "zthread/FastMutex.h"
#include "sqlite3.h"

class DatabaseSqlite : public Database
{
	 friend class MaNGOS::OperatorNew<DatabaseSqlite>;

    public:
        DatabaseSqlite();
        ~DatabaseSqlite();

        bool Initialize(const char *infoString);

		QueryResult* PQuery(const char *format,...) ;
        QueryResult* Query(const char *sql);

        bool Execute(const char *sql);
		bool PExecute(const char *format,...) ;

		bool BeginTransaction() ;
		bool CommitTransaction() ;
		bool RollbackTransaction() ;

        operator bool () const { return mSqlite != NULL; }

		unsigned long escape_string(char *to, const char *from, unsigned long length) ;
		using Database::escape_string ;


    private:
		ZThread::FastMutex mMutex ;
		ZThread::FastMutex tranMutex ;

		ZThread::ThreadImpl* tranThread ;

        sqlite3 *mSqlite;

		static size_t db_count;

		bool _TransactionCmd(const char *sql);
};
#endif
