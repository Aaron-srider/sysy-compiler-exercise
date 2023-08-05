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
    std::unique_ptr<BaseAST> exp;

    std::string dump() override {
        return "return " + exp->dump() + ";";
    }

};


//
// //Exp         ::= UnaryExp;
// Exp
// : UnaryExp {
// auto exp = new ExpAST();
// exp->unary_exp = $1;
// $$ = exp;
// }
// ;

class ExpAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> unary_exp;

    std::string dump() override {
        return unary_exp->dump();
    }

};


// //UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp;
// UnaryExp
// : PrimaryExp {
// auto unary_exp = new UnaryExpAST();
// unary_exp->primary_exp = $1;
// $$ = unary_exp;
// }
// |
// UnaryOp UnaryExp {
//         auto unary_exp = new UnaryExpAST();
//         unary_exp->unary_op = $1;
//         unary_exp->unary_exp = $2;
//         $$ = unary_exp;
// }

enum UnaryExpASTChoice {
    PRIMARY,
    UNARYOP_UNARYEXP
};
class UnaryExpAST : public BaseAST {
public:

    UnaryExpASTChoice choice;

    std::unique_ptr<BaseAST> primary_exp;

    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> unary_op;

    std::string dump() override {
        if(choice == PRIMARY)
            return primary_exp->dump();
        else
            return unary_op->dump() + unary_exp->dump();
    }
};

//
// //UnaryOp     ::= "+" | "-" | "!";
// UnaryOp
// : UNARY_OP {
// auto unary_op = new UnaryOpAST();
// unary_op->op= $1;
// $$ = unary_op;
// }
// ;

class UnaryOpAST : public BaseAST {

public:
    std::string op;

    std::string dump() override {
        return op;
    }

};


// //PrimaryExp  ::= "(" Exp ")" | Number;
// PrimaryExp
// : '(' Exp ')'  {
// auto primary_exp = new PrimaryExpAST();
// primary_exp->exp = $2;
// $$ = primary_exp;
// }
// |
// Number {
// auto primary_exp = new PrimaryExpAST();
// primary_exp->exp = $1;
// $$ = primary_exp;
// }

enum PrimaryExpASTChoice {
    EXP,
    NUMBER
};
class PrimaryExpAST : public BaseAST {
public:
    PrimaryExpASTChoice choice;
    std::unique_ptr<BaseAST> exp;
    int number;

    std::string dump() override {
        if(choice == EXP)
            return "(" + exp->dump() + ")";
        else
            return std::to_string(number);
    }
};