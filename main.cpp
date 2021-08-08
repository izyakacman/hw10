#include <iostream>
#include <string>

#include "server.hpp"
#include "sql_processor.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Usage: join_server <port>";
		return -1;
	}

	try
	{
		boost::asio::io_context io_context;

        SqlProcessor sql_processor;
		TcpServer server(io_context, static_cast<uint16_t>(stoi(argv[1])), sql_processor);

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(
            [&io_context](boost::system::error_code const&, int)
            {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                io_context.stop();
            });

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> threads;
        threads.reserve(5);

        for (int i = 0; i < 5; ++i)
        {
            threads.emplace_back(
                [&io_context]
                {
                    io_context.run();
                });
        }

        // (If we get here, it means we got a SIGINT or SIGTERM)

        // Block until all the threads exit
        for (auto& t : threads)
            t.join();

	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
}
