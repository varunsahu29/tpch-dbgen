#include "data_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::vector<std::string> DataLoader::splitLine(const std::string &line, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<Customer> DataLoader::loadCustomerData(const std::string &filePath) {
    std::vector<Customer> customers;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening customer file: " << filePath << "\n";
        return customers;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        if (tokens.size() < 4) continue;
        try {
            Customer c;
            c.custkey = std::stoi(tokens[0]);
            c.nationkey = std::stoi(tokens[3]);
            customers.push_back(c);
        } catch (const std::exception &e) {
            std::cerr << "Parsing error in customer file: " << e.what() << "\n";
        }
    }
    return customers;
}

std::vector<Orders> DataLoader::loadOrdersData(const std::string &filePath) {
    std::vector<Orders> orders;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening orders file: " << filePath << "\n";
        return orders;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        if (tokens.size() < 5) continue;
        try {
            Orders o;
            o.orderkey = std::stoi(tokens[0]);
            o.custkey = std::stoi(tokens[1]);
            o.orderdate = tokens[4];
            orders.push_back(o);
        } catch (const std::exception &e) {
            std::cerr << "Parsing error in orders file: " << e.what() << "\n";
        }
    }
    return orders;
}

std::vector<Lineitem> DataLoader::loadLineitemData(const std::string &filePath) {
    std::vector<Lineitem> lineitems;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening lineitem file: " << filePath << "\n";
        return lineitems;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        if (tokens.size() < 7) continue;
        try {
            Lineitem l;
            l.orderkey = std::stoi(tokens[0]);
            l.suppkey = std::stoi(tokens[2]);
            l.extendedprice = std::stod(tokens[5]);
            l.discount = std::stod(tokens[6]);
            lineitems.push_back(l);
        } catch (const std::exception &e) {
            std::cerr << "Parsing error in lineitem file: " << e.what() << "\n";
        }
    }
    return lineitems;
}

std::vector<Supplier> DataLoader::loadSupplierData(const std::string &filePath) {
    std::vector<Supplier> suppliers;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening supplier file: " << filePath << "\n";
        return suppliers;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        if (tokens.size() < 4) continue;
        try {
            Supplier s;
            s.suppkey = std::stoi(tokens[0]);
            s.nationkey = std::stoi(tokens[3]);
            suppliers.push_back(s);
        } catch (const std::exception &e) {
            std::cerr << "Parsing error in supplier file: " << e.what() << "\n";
        }
    }
    return suppliers;
}

std::vector<Nation> DataLoader::loadNationData(const std::string &filePath) {
    std::vector<Nation> nations;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening nation file: " << filePath << "\n";
        return nations;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        if (tokens.size() < 3) continue;
        try {
            Nation n;
            n.nationkey = std::stoi(tokens[0]);
            n.name = tokens[1];
            n.regionkey = std::stoi(tokens[2]);
            nations.push_back(n);
        } catch (const std::exception &e) {
            std::cerr << "Parsing error in nation file: " << e.what() << "\n";
        }
    }
    return nations;
}

std::vector<Region> DataLoader::loadRegionData(const std::string &filePath) {
    std::vector<Region> regions;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening region file: " << filePath << "\n";
        return regions;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        if (tokens.size() < 2) continue;
        try {
            Region r;
            r.regionkey = std::stoi(tokens[0]);
            r.name = tokens[1];
            regions.push_back(r);
        } catch (const std::exception &e) {
            std::cerr << "Parsing error in region file: " << e.what() << "\n";
        }
    }
    return regions;
}
