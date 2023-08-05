#include "string"
#include "memory"
//%type <ast_val> FuncDef FuncType Block Stmt
// %type <int_val> Number
#include "iostream"
// AST
class BaseAST {
public:
    virtual ~BaseAST() = default;

    virtual std::string dump() = 0;

    virtual std::string koopa_ir() = 0;
};

// This is the top level AST node
class CompUnitAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_def;

    std::string dump() override {
        return func_def->dump();
    }

    std::string koopa_ir() override {
        std::cout << "CompUnitAST" << std::endl;
        return func_def->koopa_ir();
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

    std::string koopa_ir() override {
        std::cout << "FuncDefAST" << std::endl;
        return "fun @" + ident + "(): " + func_type->koopa_ir() + block->koopa_ir();
    }
};

// This is the AST node for a function type
class FuncTypeAST : public BaseAST {
public:
    std::string type_name;

    std::string dump() override {
        return type_name;
    }

    std::string koopa_ir() override {
        std::cout << "FuncTypeAST" << std::endl;
        if (type_name == "int") {
            return "i32";
        } else {
            return "unknown_type";
        }
    }
};

// This is the AST node for a block
class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> stmt;

    std::string dump() override {
        return "{" + stmt->dump() + "}";
    }

    std::string koopa_ir() override {
        std::cout << "BlockAST" << std::endl;
        return "{\n%entry:\n" + stmt->koopa_ir() + "\n}";
    }
};

// This is the AST node for a statement
class StmtAST : public BaseAST {
public:
    int number;

    std::string dump() override {
        return "return " + std::to_string(number) + ";";
    }

    std::string koopa_ir() override {
        std::cout << "StmtAST" << std::endl;
        return "  ret " + std::to_string(number);
    }
};
