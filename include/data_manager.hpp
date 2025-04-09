#define pragma once

#include "data_loader.hpp"
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "thread_pool.hpp"

class DataManager {
public:
    // Data storage for each table.
    std::vector<Customer> customers;
    std::vector<Orders> orders;
    std::vector<Lineitem> lineitems;
    std::vector<Supplier> suppliers;
    std::vector<Nation> nations;
    std::vector<Region> regions;
    
    bool dataLoaded;
    std::mutex mtx;
    std::condition_variable cv;
    
    // Queue for storing queries that arrive before data is loaded.
    std::queue<std::function<void()>> queryQueue;
    
    // File paths for TPC-H table files.
    std::string customerFile;
    std::string ordersFile;
    std::string lineitemFile;
    std::string supplierFile;
    std::string nationFile;
    std::string regionFile;
    ThreadPool pool;
    
    DataManager(const std::string &custF, const std::string &ordF,
                const std::string &lineF, const std::string &suppF,
                const std::string &natF, const std::string &regF,const int threads);
    
    void loadAllTables();
    void processQuery(std::function<void()> query);
    void processQueuedQueries();
};

