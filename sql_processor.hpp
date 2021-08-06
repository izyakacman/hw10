#pragma once

#include <string>

/**
 *   SQL request processor
 */
class SqlProcessor
{
public:

	SqlProcessor() = default;
	virtual ~SqlProcessor() = default;

	void ProcessRequest(const std::string& request);
};