#pragma once

#include "memory"
#include "iostream"
#include "string"
#include "map"

class Location {
public:
    virtual int start_line() const = 0;

    virtual int start_column() const = 0;

    virtual int end_line() const = 0;

    virtual int end_column() const = 0;

    virtual void print() const = 0;
};


class LocationImpl : public Location {
private:
    int _start_line;
    int _start_column;
    int _end_line;
    int _end_column;

public:

    LocationImpl(int start_line, int start_column, int end_line, int end_column) {
        this->_start_line = start_line;
        this->_start_column = start_column;
        this->_end_line = end_line;
        this->_end_column = end_column;
    }

    int start_line() const override {
        return this->_start_line;
    }

    int start_column() const override {
        return this->_start_column;
    }

    int end_line() const override {
        return this->_end_line;
    }

    int end_column() const override {
        return this->_end_column;
    }

    void print() const override {
        operator<<(std::cout, *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const LocationImpl &location) {
        os << "(" << location.start_line() << ", " << location.start_column() << ") -> ("
           << location.end_line() << ", " << location.end_column() << ")";
        return os;
    }
};


class SymbolInformation {
public:
    virtual bool if_const() = 0;

    virtual std::string type() = 0;

    virtual std::string name() = 0;

    virtual std::string value() = 0;

    virtual std::shared_ptr<Location> location() = 0;

    virtual void change_type(std::string type) = 0;

    virtual void change_value(std::string value) = 0;

    virtual void change_location(std::shared_ptr<Location> location) = 0;

    virtual void change_if_const(bool if_const) = 0;
};

class SymbolInformationImpl : public SymbolInformation {
private:
    std::string _type;
    std::string _name;
    std::string _value;
    std::shared_ptr<Location> _location;
    bool _if_const;

public:

    SymbolInformationImpl(bool _if_const, std::string type, std::string name, std::string value,
                          std::shared_ptr<Location> location) {
        this->_type = type;
        this->_name = name;
        this->_value = value;
        this->_location = location;
        this->_if_const = _if_const;
    }

    std::string type() override {
        return this->_type;
    }

    std::string name() override {
        return this->_name;
    }

    std::string value() override {
        return this->_value;
    }

    std::shared_ptr<Location> location() override {
        return this->_location;
    }

    bool if_const() override {
        return _if_const;
    }

    void change_type(std::string type) override {
        this->_type = type;
    }

    void change_value(std::string value) override {
        this->_value = value;
    }

    void change_location(std::shared_ptr<Location> location) override {
        this->_location = location;
    }

    void change_if_const(bool if_const) override {
        this->_if_const = if_const;
    }
};

// support var only
class SymbolTable {
public:
    virtual void insert(const std::string &name, std::shared_ptr<SymbolInformation>) = 0;

    virtual std::shared_ptr<SymbolInformation> lookup(const std::string &name) = 0;

    virtual void print() = 0;

    // Virtual destructor for proper cleanup
    virtual ~SymbolTable() = default;
};


class SymbolTableImpl : public SymbolTable {

private:
    std::map<std::string, std::shared_ptr<SymbolInformation>> _symbol_map;

public:

    SymbolTableImpl() {
        std::cout << "SymbolTableImpl()" << std::endl;
    }

    ~SymbolTableImpl() override {
        std::cout << "~SymbolTableImpl()" << std::endl;
    }

    void insert(const std::string &name, std::shared_ptr<SymbolInformation> symbol_information) override {
        std::cout << "SymbolTableImpl::insert()" << std::endl;
        this->_symbol_map.insert(std::pair<std::string, std::shared_ptr<SymbolInformation>>(name, symbol_information));
    }

    std::shared_ptr<SymbolInformation> lookup(const std::string &name) override {
        std::cout << "SymbolTableImpl::lookup()" << std::endl;
        return this->_symbol_map[name];
    }

    void print() override {
        std::cout << "SymbolTable like below" << std::endl;
        for (auto &it: this->_symbol_map) {


            std::cout << "name: " << it.first << "; ";


            std::cout << " type: ";
            if (it.second->if_const()) {
                std::cout << "const" << " ";
            }
            std::cout << it.second->type() << "; ";


            std::cout << " value: " << it.second->value() << "; ";


            std::cout << " location: ";
            it.second->location()->print();
            std::cout << "; ";


            std::cout << std::endl;
        }
    }
};

class SymbolTableFactory {
public:
    static std::shared_ptr<SymbolInformation> wrap_symbol_info(
            bool if_const,
            std::string type,
            std::string name,
            std::string value,
            std::shared_ptr<Location> location
    ) {
        return std::make_shared<SymbolInformationImpl>(if_const, type, name, value, location);
    }

    static std::shared_ptr<SymbolTable> new_symbol_table() {
        return std::make_shared<SymbolTableImpl>();
    }

    // singleton
    static std::shared_ptr<SymbolTable> symbol_table;
};

