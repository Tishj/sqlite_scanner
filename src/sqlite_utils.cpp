#include "sqlite_utils.hpp"

namespace duckdb {

void SQLiteUtils::Check(int rc, sqlite3 *db) {
	if (rc != SQLITE_OK) {
		throw std::runtime_error(string(sqlite3_errmsg(db)));
	}
}

string SQLiteUtils::TypeToString(int sqlite_type) {
	switch (sqlite_type) {
	case SQLITE_ANY:
		return "any";
	case SQLITE_INTEGER:
		return "integer";
	case SQLITE_TEXT:
		return "text";
	case SQLITE_BLOB:
		return "blob";
	case SQLITE_FLOAT:
		return "float";
	default:
		return "unknown";
	}
}
LogicalType SQLiteUtils::TypeToLogicalType(const string &sqlite_type) {
	// type affinity rules are taken from here: https://www.sqlite.org/datatype3.html

	// If the declared type contains the string "INT" then it is assigned INTEGER affinity.
	if (StringUtil::Contains(sqlite_type, "int")) {
		return LogicalType::BIGINT;
	}
	// If the declared type of the column contains any of the strings "CHAR", "CLOB", or "TEXT" then that column has
	// TEXT affinity. Notice that the type VARCHAR contains the string "CHAR" and is thus assigned TEXT affinity.
	if (StringUtil::Contains(sqlite_type, "char") || StringUtil::Contains(sqlite_type, "clob") ||
	    StringUtil::Contains(sqlite_type, "text")) {
		return LogicalType::VARCHAR;
	}

	// If the declared type for a column contains the string "BLOB" or if no type is specified then the column has
	// affinity BLOB.
	if (StringUtil::Contains(sqlite_type, "blob") || sqlite_type.empty()) {
		return LogicalType::BLOB;
	}

	// If the declared type for a column contains any of the strings "REAL", "FLOA", or "DOUB" then the column has REAL
	// affinity.
	if (StringUtil::Contains(sqlite_type, "real") || StringUtil::Contains(sqlite_type, "floa") ||
	    StringUtil::Contains(sqlite_type, "doub")) {
		return LogicalType::DOUBLE;
	}
	// Otherwise, the affinity is NUMERIC.
	// now numeric sounds simple, but it is rather complex:
	// A column with NUMERIC affinity may contain values using all five storage classes.
	// ...
	// we add some more extra rules to try to be somewhat sane

	if (sqlite_type == "date") {
		return LogicalType::DATE;
	}

	// datetime, timestamp
	if (StringUtil::Contains(sqlite_type, "time")) {
		return LogicalType::TIMESTAMP;
	}

	// decimal, numeric
	if (StringUtil::Contains(sqlite_type, "dec") || StringUtil::Contains(sqlite_type, "num")) {
		return LogicalType::DOUBLE;
	}

	// alright, give up and fallback to varchar
	return LogicalType::VARCHAR;
}

}
