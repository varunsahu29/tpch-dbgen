#include "data_manager.hpp"
#include "thread_pool.hpp"
#include <thread>
#include <iostream>

DataManager::DataManager(const std::string &custF, const std::string &ordF,
                         const std::string &lineF, const std::string &suppF,
                         const std::string &natF, const std::string &regF, const int threads)
    : customerFile(custF), ordersFile(ordF), lineitemFile(lineF),
      supplierFile(suppF), nationFile(natF), regionFile(regF), dataLoaded(false), pool(threads) {}

void DataManager::loadAllTables()
{

    auto f1 = pool.enqueue([this]()
                           {
        customers = DataLoader::loadCustomerData(customerFile);
        std::cout << "Loaded " << customers.size() << " customer records.\n"; });
    auto f2 = pool.enqueue([this]()
                           {
        orders = DataLoader::loadOrdersData(ordersFile);
        std::cout << "Loaded " << orders.size() << " orders records.\n"; });
    auto f3 = pool.enqueue([this]()
                           {
        lineitems = DataLoader::loadLineitemData(lineitemFile);
        std::cout << "Loaded " << lineitems.size() << " lineitem records.\n"; });
    auto f4 = pool.enqueue([this]()
                           {
        suppliers = DataLoader::loadSupplierData(supplierFile);
        std::cout << "Loaded " << suppliers.size() << " supplier records.\n"; });
    auto f5 = pool.enqueue([this]()
                           {
        nations = DataLoader::loadNationData(nationFile);
        std::cout << "Loaded " << nations.size() << " nation records.\n"; });
    auto f6 = pool.enqueue([this]()
                           {
        regions = DataLoader::loadRegionData(regionFile);
        std::cout << "Loaded " << regions.size() << " region records.\n"; });

    // Wait for all loading tasks to finish.
    f1.get();
    f2.get();
    f3.get();
    f4.get();
    f5.get();
    f6.get();
    std::cout << "All tables loaded successfully.\n";
    {
        std::lock_guard<std::mutex> lock(mtx);
        dataLoaded = true;
    }
    cv.notify_all(); // Notify all waiting threads that data is loaded.
    processQueuedQueries(); // Process any queued queries.
    std::cout << "Queued queries processed.\n";
    std::cout << "Data loading complete.\n";
}

void DataManager::processQuery(std::function<void()> query)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (!dataLoaded)
    {
        queryQueue.push(query);
        std::cout << "Query queued until data is loaded.\n";
    }
    else
    {
        lock.unlock();
        query();
    }
}

void DataManager::processQueuedQueries()
{
    std::lock_guard<std::mutex> lock(mtx);
    while (!queryQueue.empty())
    {
        auto query = queryQueue.front();
        queryQueue.pop();
        query();
    }
}
