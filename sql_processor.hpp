#pragma once

#include <string>
#include <map>

/**
 *   SQL request processor
 */
class SqlProcessor
{
public:

	SqlProcessor() = default;
	virtual ~SqlProcessor() = default;

	void ProcessRequest(const std::string& request);

private:

     std::map<int, std::string> table_a_;
     std::map<int, std::string> table_b_;

     static constexpr auto ok_ = "OK";
     static constexpr auto err_ = "ERR";
};