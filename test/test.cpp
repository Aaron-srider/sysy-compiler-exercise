#include "gtest/gtest.h"
#include "iostream"
#include "symbol_table.h"
#include "Ast.h"
#include "memory"

TEST(test1, test1) {
    auto symbolTable = SymbolTableFactory::symbol_table;

    std::make_shared<LocationImpl>(1, 1, 1, 1)->print();


    symbolTable->insert("name", SymbolTableFactory::wrap_symbol_info(
            false,
            "int",
            "testVar",
            "1",
            std::make_shared<LocationImpl>(1, 1, 1, 1)
    ));


    symbolTable->insert("age", SymbolTableFactory::wrap_symbol_info(
            false,
            "int",
            "age",
            "2",
            std::make_shared<LocationImpl>(1, 1, 1, 1)
    ));

    symbolTable->insert("address", SymbolTableFactory::wrap_symbol_info(
            true,
            "int",
            "address",
            "3",
            std::make_shared<LocationImpl>(1, 1, 1, 1)
    ));


    auto info = symbolTable->lookup("name");
    info->change_type("float");
    info->change_value("2222");
    info->change_if_const(true);
    info->change_location(std::make_shared<LocationImpl>(2, 2, 2, 2));


    symbolTable->print();
    std::cout << std::endl;

    auto name_exists = symbolTable->lookup("name");

    std::cout << (name_exists == nullptr) << std::endl;

    name_exists = symbolTable->lookup("foo");

    std::cout << (name_exists == nullptr) << std::endl;

}