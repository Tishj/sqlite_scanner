//===----------------------------------------------------------------------===//
//                         DuckDB
//
// sqlite_utils.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb.hpp"
#include "sqlite3.h"

namespace duckdb {
class SQLiteStatement;

class SQLiteDB {
public:
	SQLiteDB();
	SQLiteDB(sqlite3 *db);
	~SQLiteDB();
	// disable copy constructors
	SQLiteDB(const SQLiteDB &other) = delete;
	SQLiteDB &operator=(const SQLiteDB &) = delete;
	//! enable move constructors
	SQLiteDB(SQLiteDB &&other) noexcept;
	SQLiteDB &operator=(SQLiteDB &&) noexcept;

	sqlite3 *db;

public:
	static SQLiteDB Open(const string &path, bool is_read_only = true, bool is_shared = false);
	SQLiteStatement Prepare(const string &query);
	void Execute(const string &query);
	vector<string> GetTables();
	void GetTableInfo(const string &table_name, ColumnList &columns, vector<unique_ptr<Constraint>> &constraints, bool all_varchar);
	idx_t GetMaxRowId(const string &table_name);
	bool ColumnExists(const string &table_name, const string &column_name);

	bool IsOpen();
	void Close();
};

class SQLiteStatement {
public:
	SQLiteStatement();
	SQLiteStatement(sqlite3 *db, sqlite3_stmt *stmt);
	~SQLiteStatement();
	// disable copy constructors
	SQLiteStatement(const SQLiteStatement &other) = delete;
	SQLiteStatement &operator=(const SQLiteStatement &) = delete;
	//! enable move constructors
	SQLiteStatement(SQLiteStatement &&other) noexcept;
	SQLiteStatement &operator=(SQLiteStatement &&) noexcept;

	sqlite3 *db;
	sqlite3_stmt *stmt;

public:
	int Step();
	template<class T>
	T GetValue(idx_t col) {
		throw InternalException("Unsupported type for SQLiteStatement::GetValue");
	}
	template<class T>
	void Bind(idx_t col, T value) {
		throw InternalException("Unsupported type for SQLiteStatement::Bind");
	}
	int GetType(idx_t col);
	bool IsOpen();
	void Close();
	void CheckTypeMatches(sqlite3_value *val, int sqlite_column_type, int expected_type, idx_t col_idx);
	void CheckTypeIsFloatOrInteger(sqlite3_value *val, int sqlite_column_type, idx_t col_idx);
	void Reset();
};

template<>
string SQLiteStatement::GetValue(idx_t col);
template<>
int SQLiteStatement::GetValue(idx_t col);
template<>
int64_t SQLiteStatement::GetValue(idx_t col);
template<>
sqlite3_value * SQLiteStatement::GetValue(idx_t col);

template<>
void SQLiteStatement::Bind(idx_t col, int32_t value);
template<>
void SQLiteStatement::Bind(idx_t col, int64_t value);
template<>
void SQLiteStatement::Bind(idx_t col, double value);
template<>
void SQLiteStatement::Bind(idx_t col, string_t value);
template<>
void SQLiteStatement::Bind(idx_t col, nullptr_t value);

class SQLiteUtils {
public:
	static void Check(int rc, sqlite3 *db);
	static string TypeToString(int sqlite_type);
	static LogicalType TypeToLogicalType(const string &sqlite_type);
};

}
