#include "sql_processor.hpp"

#include <iostream>
#include <sstream>

using namespace boost::asio;

void SqlProcessor::SendClient(const std::string& msg, boost::asio::ip::tcp::socket& socket)
{
    write(socket, buffer(msg, msg.size()));
}

std::map<int, std::string>* SqlProcessor::GetTable(std::string table)
{
    if (table == "A")
        return &table_a_;

    if (table == "B")
        return &table_b_;

    return nullptr;
}

bool SqlProcessor::InsertTable(std::map<int, std::string>* table, int id, const std::string& name)
{
    std::lock_guard<std::mutex> lock(table == &table_a_ ? mutex_a_ : mutex_b_);

    if (table->find(id) == table->end())
    {
        (*table)[id] = name;
        return true;
    }

    return false;
}

void SqlProcessor::ClearTable(std::map<int, std::string>* table)
{
    std::lock_guard<std::mutex> lock(table == &table_a_ ? mutex_a_ : mutex_b_);

    table->clear();
}

std::vector<std::string> SqlProcessor::IntersectionTable()
{
    std::vector<std::string> strings{};
    std::scoped_lock lock(mutex_a_, mutex_b_);

    for (const auto a : table_a_)
    {
        auto b = table_b_.find(a.first);

        if (b != table_b_.end())
        {
            strings.push_back(std::to_string(a.first) + "," + a.second + "," + b->second + "\n");
        }
    }

    return strings;
}

std::vector<std::string> SqlProcessor::SymmetricDifferenceTable()
{
    std::vector<std::string> strings{};
    std::scoped_lock lock(mutex_a_, mutex_b_);

    for (const auto a : table_a_)
    {
        auto b = table_b_.find(a.first);

        if (b == table_b_.end())
        {
            strings.push_back(std::to_string(a.first) + "," + a.second + ",\n");
        }
    }

    for (const auto b : table_b_)
    {
        auto a = table_a_.find(b.first);

        if (a == table_a_.end())
        {
            strings.push_back(std::to_string(b.first) + ",," + b.second + "\n");
        }
    }

    return strings;
}

void SqlProcessor::ProcessRequest(const std::string& request, boost::asio::ip::tcp::socket& socket)
{
     std::istringstream iss{ request };
     std::string table_name;
     std::string cmd;
     std::map<int, std::string>* table;

     while (iss >> cmd)
     {
         if (cmd == "INSERT")
         {
             iss >> table_name;

             table = GetTable(table_name);

             if (table == nullptr)
             {
                 SendClient(err_ + " Wrong table name\n", socket);
                 return;
             }

             int id;
             std::string name;

             iss >> id >> name;

             if (!iss)
             {
                 SendClient(err_ + " Request request parameters wrong\n", socket);
                 return;
             }

             if (InsertTable(table, id, name))
             {
                 SendClient(ok_ + "\n", socket);
             }
             else
             {
                 SendClient(err_ + " Record already exist\n", socket);
             }

         } // INSERT

         else if (cmd == "TRUNCATE")
         {
             iss >> table_name;

             table = GetTable(table_name);

             if (table == nullptr)
             {
                 SendClient(err_ + " Wrong table name\n", socket);
                 return;
             }

             ClearTable(table);

             SendClient(ok_ + "\n", socket);

         } // TRUNCATE

         else if (cmd == "INTERSECTION")
         {
             std::vector<std::string> strings = IntersectionTable();

             for (const auto& s : strings)
                 SendClient(s, socket);

             SendClient(ok_ + "\n", socket);

         } // INTERSECTION

         else if (cmd == "SYMMETRIC_DIFFERENCE")
         {
             std::vector<std::string> strings = SymmetricDifferenceTable();

             for (const auto& s : strings)
                 SendClient(s, socket);

             SendClient(ok_ + "\n", socket);

         } // SYMMETRIC_DIFFERENCE

         else
         {
             SendClient(err_ + " Wrong command\n", socket);
         }

     } // while (iss)
}
