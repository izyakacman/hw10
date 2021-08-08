#pragma once

#include <string>
#include <map>
#include <mutex>
#include <atomic>

#include <boost/asio.hpp>

/**
 *   SQL request processor
 */
class SqlProcessor
{
public:

    SqlProcessor() = default;
	virtual ~SqlProcessor() = default;

	void ProcessRequest(const std::string& request, boost::asio::ip::tcp::socket& socket);

private:

    void SendClient(const std::string&, boost::asio::ip::tcp::socket& socket);
    std::map<int, std::string>* GetTable(std::string table);
    bool InsertTable(std::map<int, std::string>* table, int id, const std::string& name);
    void ClearTable(std::map<int, std::string>* table);
    std::vector<std::string> IntersectionTable();
    std::vector<std::string> SymmetricDifferenceTable();

    std::map<int, std::string> table_a_;
    std::map<int, std::string> table_b_;
    std::mutex mutex_a_;
    std::mutex mutex_b_;

    const std::string ok_ = "OK";
    const std::string err_ = "ERR";
};