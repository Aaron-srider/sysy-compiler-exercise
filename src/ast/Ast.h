#include "string"
#include "memory"
//%type <ast_val> FuncDef FuncType Block Stmt
// %type <int_val> Number

// AST
class BaseAST {
public:
    virtual ~BaseAST() = default;

    virtual std::string dump() = 0;
};

// This is the top level AST node
class CompUnitAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_def;

    std::string dump() override {
        return func_def->dump();
    }
};

// This is the AST node for a function definition
class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    std::string dump() override {
        return func_type->dump() + " " + ident + " " + block->dump();
    }
};

// This is the AST node for a function type
class FuncTypeAST : public BaseAST {
public:
    std::string type_name;

    std::string dump() override {
        return type_name;
    }
};

// This is the AST node for a block
class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> stmt;

    std::string dump() override {
        return "{" + stmt->dump() + "}";
    }
};

// This is the AST node for a statement
class StmtAST : public BaseAST {
public:
    int number;

    std::string dump() override {
        return "return " + std::to_string(number) + ";";
    }
};
