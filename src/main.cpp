#include "data_manager.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

// Structure for CLI options.
struct CLIOptions {
    std::string region;
    std::string startDate;
    std::string endDate;
    int threads;             // Number of threads to use for query processing.
    std::string customerPath;
    std::string ordersPath;
    std::string lineitemPath;
    std::string supplierPath;
    std::string nationPath;
    std::string regionPath;
    std::string resultPath;
};

void printUsage(const char *progName) {
    std::cout << "Usage: " << progName 
              << " --region <region> --start-date <start_date> --end-date <end_date> --threads <num_threads> "
              << "--customer <customer_file> --orders <orders_file> --lineitem <lineitem_file> "
              << "--supplier <supplier_file> --nation <nation_file> --regionfile <region_file> --result <result_file>\n";
}

CLIOptions parseCLI(int argc, char *argv[]) {
    CLIOptions opts;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--region" && i + 1 < argc) {
            opts.region = argv[++i];
        } else if (arg == "--start-date" && i + 1 < argc) {
            opts.startDate = argv[++i];
        } else if (arg == "--end-date" && i + 1 < argc) {
            opts.endDate = argv[++i];
        } else if (arg == "--threads" && i + 1 < argc) {
            opts.threads = std::stoi(argv[++i]);
        } else if (arg == "--customer" && i + 1 < argc) {
            opts.customerPath = argv[++i];
        } else if (arg == "--orders" && i + 1 < argc) {
            opts.ordersPath = argv[++i];
        } else if (arg == "--lineitem" && i + 1 < argc) {
            opts.lineitemPath = argv[++i];
        } else if (arg == "--supplier" && i + 1 < argc) {
            opts.supplierPath = argv[++i];
        } else if (arg == "--nation" && i + 1 < argc) {
            opts.nationPath = argv[++i];
        } else if (arg == "--regionfile" && i + 1 < argc) {
            opts.regionPath = argv[++i];
        } else if (arg == "--result" && i + 1 < argc) {
            opts.resultPath = argv[++i];
        } else {
            std::cerr << "Unknown parameter: " << arg << "\n";
            printUsage(argv[0]);
            exit(1);
        }
    }
    return opts;
}

// Query processing function that uses the thread pool to partition query work.
// For demonstration, we compute the revenue sum over lineitems in parallel.
void executeQuery(DataManager &dm, const CLIOptions &opts) {
    std::cout << "Executing Query with parameters:\n";
    std::cout << "Region: " << opts.region << "\n";
    std::cout << "Start Date: " << opts.startDate << "\n";
    std::cout << "End Date: " << opts.endDate << "\n";
    std::cout << "Query Processing Threads: " << opts.threads << "\n";
    
    int threadCount = opts.threads;
    size_t total = dm.lineitems.size();
    size_t partitionSize = (threadCount > 0) ? total / threadCount : total;
    std::vector<double> partialSums(threadCount, 0.0);
    std::vector<std::future<void>> futures;
    
    // Create a thread pool for query processing.
    ThreadPool queryPool(threadCount);
    
    for (int i = 0; i < threadCount; i++) {
        size_t start = i * partitionSize;
        // Ensure the last task processes any remaining records.
        size_t end = (i == threadCount - 1) ? total : (i + 1) * partitionSize;
        
        // Enqueue a task for each partition.
        futures.push_back(queryPool.enqueue([&, i, start, end]() {
            double localSum = 0.0;
            for (size_t j = start; j < end; j++) {
                // In a complete implementation, join tables and apply filters here.
                localSum += dm.lineitems[j].extendedprice * (1.0 - dm.lineitems[j].discount);
            }
            partialSums[i] = localSum;
        }));
    }
    
    // Wait for all query tasks to finish.
    for (auto &f : futures) {
        f.get();
    }
    
    double totalRevenue = 0.0;
    for (double sum : partialSums) {
        totalRevenue += sum;
    }
    
    // Write the query result to the output file.
    std::ofstream outFile(opts.resultPath);
    if (!outFile) {
        std::cerr << "Error opening result file: " << opts.resultPath << "\n";
        return;
    }
    outFile << "Query result for region " << opts.region << " from " 
            << opts.startDate << " to " << opts.endDate << "\n";
    outFile << "Total Revenue: " << totalRevenue << "\n";
    outFile.close();
    
    std::cout << "Query executed successfully. Results written to " << opts.resultPath << "\n";
}

int main(int argc, char *argv[]) {
    if (argc < 23) { // Expecting 11 flags each with a value.
        printUsage(argv[0]);
        return 1;
    }
    
    CLIOptions options = parseCLI(argc, argv);
    
    // Create a DataManager instance using file paths from CLI options.
    DataManager dm(options.customerPath, options.ordersPath, options.lineitemPath,
                   options.supplierPath, options.nationPath, options.regionPath,options.threads);
    
    // Perform data loading in a separate thread.
    dm.loadAllTables();

    // Immediately queue a query—even though data might not be loaded yet.
    dm.processQuery([&dm, &options]() {
        executeQuery(dm, options);
    });

    // Wait for the data loading to complete.
    {
        std::unique_lock<std::mutex> lock(dm.mtx);
        dm.cv.wait(lock, [&dm]() { 
            return dm.dataLoaded; });
    }
    std::cout << "Processing query after data load completed.\n";
    dm.processQuery([&dm, &options]() {
        executeQuery(dm, options);
    });

    
    return 0;
}
