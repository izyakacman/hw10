#include "sql_processor.hpp"

#include <iostream>
#include <sstream>

void SqlProcessor::ProcessRequest(const std::string& request)
{
     std::istringstream iss{ request };

     std::string cmd;

     iss >> cmd;

     if( cmd == "INSERT" )
     {
          std::string table_name;

          iss >> table_name;

          if(table_name != "A" && table_name != "B" )
          {
               std::cout << err_ << " Wrong table name!\n";
               return;
          }

          int id;
          std::string name;

          iss >> id >> name;

          if(!iss)
          {
               std::cout << err_ << " Request request parameters wrong!\n";
               return;
          }
     }
}