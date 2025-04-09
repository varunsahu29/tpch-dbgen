#pragma once

#include <vector>
#include <string>

// Record structures for TPC-H tables.

// Customer: c_custkey (index 0) and c_nationkey (index 3)
struct Customer {
    int custkey;
    int nationkey;
};

// Orders: o_orderkey (index 0), o_custkey (index 1), o_orderdate (index 4)
struct Orders {
    int orderkey;
    int custkey;
    std::string orderdate;
};

// Lineitem: l_orderkey (index 0), l_extendedprice (index 5), l_discount (index 6), l_suppkey (index 2)
struct Lineitem {
    int orderkey;
    double extendedprice;
    double discount;
    int suppkey;
};

// Supplier: s_suppkey (index 0), s_nationkey (index 3)
struct Supplier {
    int suppkey;
    int nationkey;
};

// Nation: n_nationkey (index 0), n_name (index 1), n_regionkey (index 2)
struct Nation {
    int nationkey;
    std::string name;
    int regionkey;
};

// Region: r_regionkey (index 0), r_name (index 1)
struct Region {
    int regionkey;
    std::string name;
};

class DataLoader {
public:
    static std::vector<Customer> loadCustomerData(const std::string &filePath);
    static std::vector<Orders> loadOrdersData(const std::string &filePath);
    static std::vector<Lineitem> loadLineitemData(const std::string &filePath);
    static std::vector<Supplier> loadSupplierData(const std::string &filePath);
    static std::vector<Nation> loadNationData(const std::string &filePath);
    static std::vector<Region> loadRegionData(const std::string &filePath);
    static std::vector<std::string> splitLine(const std::string &line, char delimiter='|');
};