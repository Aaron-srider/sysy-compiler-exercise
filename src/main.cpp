#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "Ast.h"


using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;

int yyparse(std::unique_ptr<BaseAST> &ast);


#include <filesystem>
#include "symbol_table.h"

namespace fs = std::filesystem;

void listFilesRecursively(const fs::path &directory) {

    try {
        for (const auto &entry: fs::directory_iterator(directory)) {
            if (fs::is_regular_file(entry.path())) {
                std::cout << "f " << entry.path().string() << '\n';
            } else {
                std::cout << "u " << entry.path().string() << '\n';
            }
        }
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main(int argc, const char *argv[]) {
    // try {
    //     fs::path currentPath = fs::current_path();
    //     std::cout << "Current folder path: " << currentPath.string() << std::endl;
    //
    //     std::cout << "Files in the current directory and its subdirectories:\n";
    //     listFilesRecursively(currentPath);
    // } catch (const std::exception &ex) {
    //     std::cerr << "Error: " << ex.what() << std::endl;
    //     return 1;
    // }


    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    std::cout << "input: " << input << std::endl;
    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = fopen(input, "r");
    assert(yyin);

    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);

    cout << "syntax analyze result:" << endl;
    // 输出解析得到的 AST
    cout << ast->dump() << endl;

    SymbolTableFactory::symbol_table->print();

    return 0;
}