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
void executeQuery(DataManager &dm, const CLIOptions &opts) {
    std::cout << "Executing Query with parameters:\n";
    std::cout << "Region: " << opts.region << "\n";
    std::cout << "Start Date: " << opts.startDate << "\n";
    std::cout << "End Date: " << opts.endDate << "\n";
    std::cout << "Query Processing Threads: " << opts.threads << "\n";
    
    // Build hash maps for quick lookup.
    std::unordered_map<int, Orders> orderMap;
    for (const auto &o : dm.orders) {
        orderMap[o.orderkey] = o;
    }
    
    std::unordered_map<int, Supplier> supplierMap;
    for (const auto &s : dm.suppliers) {
        supplierMap[s.suppkey] = s;
    }
    
    std::unordered_map<int, Customer> customerMap;
    for (const auto &c : dm.customers) {
        customerMap[c.custkey] = c;
    }
    
    std::unordered_map<int, Nation> nationMap;
    for (const auto &n : dm.nations) {
        nationMap[n.nationkey] = n;
    }
    
    std::unordered_map<int, Region> regionMap;
    for (const auto &r : dm.regions) {
        regionMap[r.regionkey] = r;
    }
    
    // Partition the lineitems vector for parallel processing.
    int threadCount = opts.threads;
    size_t total = dm.lineitems.size();
    size_t partitionSize = (threadCount > 0) ? total / threadCount : total;
    
    // Each task returns a local revenue map: nation name -> revenue.
    std::vector<std::future<std::unordered_map<std::string, double>>> futures;
    
    
    for (int i = 0; i < threadCount; i++) {
        size_t start = i * partitionSize;
        size_t end = (i == threadCount - 1) ? total : (i + 1) * partitionSize;
        
        futures.push_back(dm.pool.enqueue([=, &dm, &orderMap, &supplierMap, &customerMap, &nationMap, &regionMap, &opts]() -> std::unordered_map<std::string, double> {
            std::unordered_map<std::string, double> localRevenue;
            for (size_t j = start; j < end; j++) {
                const auto &li = dm.lineitems[j];
                // Join: l_orderkey = o_orderkey
                auto orderIt = orderMap.find(li.orderkey);
                if(orderIt == orderMap.end())
                    continue;
                const Orders &o = orderIt->second;
                
                // Filter on order date.
                if(o.orderdate < opts.startDate || o.orderdate >= opts.endDate)
                    continue;
                
                // Join: l_suppkey = s_suppkey
                auto suppIt = supplierMap.find(li.suppkey);
                if(suppIt == supplierMap.end())
                    continue;
                const Supplier &s = suppIt->second;
                
                // Join: c_custkey = o_custkey
                auto custIt = customerMap.find(o.custkey);
                if(custIt == customerMap.end())
                    continue;
                const Customer &c = custIt->second;
                
                // Condition: c_nationkey = s_nationkey
                if(c.nationkey != s.nationkey)
                    continue;
                
                // Join: s_nationkey = n_nationkey
                auto natIt = nationMap.find(s.nationkey);
                if(natIt == nationMap.end())
                    continue;
                const Nation &n = natIt->second;
                
                // Join: n_regionkey = r_regionkey 
                auto regIt = regionMap.find(n.regionkey);
                if(regIt == regionMap.end())
                    continue;
                const Region &r = regIt->second;

                // filter: r_name must equal opts.region
                if(r.name != opts.region)
                    continue;
                
                // All conditions satisfied: compute revenue.
                double revenue = li.extendedprice * (1.0 - li.discount);
                // Group revenue by nation name.
                localRevenue[n.name] += revenue;
            }
            return localRevenue;
        }));
    }
    
    // Merge the partial maps.
    std::unordered_map<std::string, double> resultRevenue;
    for (auto &future : futures) {
        auto partial = future.get();
        for (const auto &p : partial) {
            resultRevenue[p.first] += p.second;
        }
    }
    
    // Convert the map to a vector and sort descending by revenue.
    std::vector<std::pair<std::string, double>> sortedResults(resultRevenue.begin(), resultRevenue.end());
    std::sort(sortedResults.begin(), sortedResults.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    
    // Write the sorted results to the output file.
    std::ofstream outFile(opts.resultPath);
    if (!outFile) {
        std::cerr << "Error opening result file: " << opts.resultPath << "\n";
        return;
    }
    outFile << "Nation,Revenue\n";
    for (const auto &entry : sortedResults) {
        outFile << entry.first << "," << std::to_string(entry.second) << "\n";
    }
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
