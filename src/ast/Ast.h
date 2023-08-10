#include <sstream>
#include <vector>
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

class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> block_item_list;

    std::string dump() override {
        return "{" + block_item_list->dump() + "}";
    }
};

enum BlockItemListChoice {
    EMPTY,
    BLOCK_LIST
};

class BlockItemListAST : public BaseAST {
public:

    BlockItemListChoice choice;

    std::vector<std::unique_ptr<BaseAST>> list;

    std::string dump() override {
        std::cout << "block item list dump option: " << _tolower(choice) << std::endl;
        if (choice == EMPTY) {
            return "";
        } else {
            std::string block_item_string;
            std::cout << "block item list size: " << list.size() << std::endl;
            for (auto &block_item: list) {
                std::cout << "dump block item: " << std::endl;
                std::cout << block_item->dump() << std::endl;
                block_item_string += block_item->dump() + "\n";
            }
            return block_item_string;
        }
    }
};


enum BlockItemChoice {
    STATEMENT,
    DECLARATION
};

class BlockItemAST : public BaseAST {
public:
    BlockItemChoice choice;
    std::unique_ptr<BaseAST> statement;
    std::unique_ptr<BaseAST> declaration;

    std::string dump() override {
        std::cout << "block item dump option: " << choice << std::endl;
        if (choice == STATEMENT) {
            return statement->dump();
        } else {
            return declaration->dump();
        }
    }
};

class LValAST : public BaseAST {
public:
    std::string ident;

    std::string dump() override {
        return ident;
    }
};


enum StmtChoice {
    ASSIGNMENT_STATEMENT,
    RETURN_STATEMENT,
    EXPRESSION_STATEMENT,
    EMPTY_STATEMENT,
    BLOCK_STATEMENT,
    IF_STATEMENT,
    IF_ELSE_STATEMENT
};

// This is the AST node for a statement
class StmtAST : public BaseAST {
public:

    StmtChoice choice;

    std::unique_ptr<BaseAST> left_value;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> block;
    std::unique_ptr<BaseAST> if_statement;
    std::unique_ptr<BaseAST> else_statement;

    std::string dump() override {
        std::cout << "dump stmt: " << choice << std::endl;
        if (choice == ASSIGNMENT_STATEMENT) {
            std::cout << "dump assignment expression: " << std::endl;
            return left_value->dump() + " = " + exp->dump() + ";";
        } else if (choice == RETURN_STATEMENT) {
            std::cout << "dump return statement: " << std::endl;
            return "return " + exp->dump() + ";";
        }
        else if(choice == EXPRESSION_STATEMENT){
            std::cout << "dump expression statement: " << std::endl;
            return exp->dump() + ";";
        }
        else if(choice == EMPTY_STATEMENT){
            std::cout << "dump empty statement: " << std::endl;
            return ";";
        }
        else if(choice == BLOCK_STATEMENT){
            std::cout << "dump block statement: " << std::endl;
            return block->dump();
        } else if(choice == IF_STATEMENT){
            std::cout << "dump if statement: " << std::endl;
            return "if(" + exp->dump() + ")" + if_statement->dump();
        } else if(choice == IF_ELSE_STATEMENT){
            std::cout << "dump if else statement: " << std::endl;
            return "if(" + exp->dump() + ")" + if_statement->dump() + "else" + else_statement->dump();
        }
        else {
            std::cout << "dump error" << std::endl;
            return "error";
        }

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
    std::unique_ptr<BaseAST> lor_exp;

    std::string dump() override {
        std::cout << "exp dump" << std::endl;
        std::cout << lor_exp->dump() << std::endl;
        return lor_exp->dump();
    }

};






//
// //MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
// MulExp
// : UnaryExp {
// auto mul_exp = new MulExpAST();
// mul_exp->choice = UNARYEXP;
// mul_exp->unary_exp = unique_ptr<BaseAST>($1);
// $$ = mul_exp;
// }
// |
// MulExp '*' UnaryExp {
// auto mul_exp = new MulExpAST();
// mul_exp->choice = MUL_OP_UNARYEXP;
// mul_exp->mul_op = "*";
// mul_exp->unary_exp = unique_ptr<BaseAST>($3);
// $$ = mul_exp;
// }
// |
// MulExp '/' UnaryExp {
// auto mul_exp = new MulExpAST();
// mul_exp->choice = MUL_OP_UNARYEXP;
// mul_exp->mul_op = "/";
// mul_exp->unary_exp = unique_ptr<BaseAST>($3);
// $$ = mul_exp;
// }
// |
// MulExp '%' UnaryExp {
// auto mul_exp = new MulExpAST();
// mul_exp->choice = MUL_OP_UNARYEXP;
// mul_exp->mul_op = "%";
// mul_exp->unary_exp = unique_ptr<BaseAST>($3);
// $$ = mul_exp;
// }

enum MulExpASTChoice {
    UNARYEXP,
    MUL_OP_UNARYEXP
};

class MulExpAST : public BaseAST {
public:

    MulExpASTChoice choice;

    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> mul_exp;
    std::string mul_op;

    std::string dump() override {
        std::cout << "mul exp dump option: " << choice << std::endl;
        if (choice == UNARYEXP) {
            std::cout << "unary exp dump" << std::endl;
            return unary_exp->dump();
        } else {
            std::cout << "mul exp dump" << std::endl;
            return mul_exp->dump() + mul_op + unary_exp->dump();
        }
    }
};

// //AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;
// AddExp
// : MulExp {
// auto add_exp = new AddExpAST();
// add_exp->choice = MULEXP;
// add_exp->mul_exp = unique_ptr<BaseAST>($1);
// $$ = add_exp;
// }
// |
// AddExp '+' MulExp {
// auto add_exp = new AddExpAST();
// add_exp->choice = ADD_OP_MULEXP;
// add_exp->add_op = "+";
// add_exp->mul_exp = unique_ptr<BaseAST>($3);
// $$ = add_exp;
// }
// |
// AddExp '-' MulExp {
// auto add_exp = new AddExpAST();
// add_exp->choice = ADD_OP_MULEXP;
// add_exp->add_op = "-";
// add_exp->mul_exp = unique_ptr<BaseAST>($3);
// $$ = add_exp;
// }

enum AddExpASTChoice {
    MULEXP,
    ADD_OP_MULEXP
};

class AddExpAST : public BaseAST {
public:

    AddExpASTChoice choice;

    std::unique_ptr<BaseAST> mul_exp;
    std::unique_ptr<BaseAST> add_exp;
    std::string add_op;

    std::string dump() override {
        std::cout << "add exp dump" << std::endl;
        if (choice == MULEXP) {
            std::cout << "add exp dump mulexp" << std::endl;
            return mul_exp->dump();
        } else {
            std::cout << "add exp dump addop mulexp" << std::endl;
            return add_exp->dump() + add_op + mul_exp->dump();
        }
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
        std::cout << "dump unary exp: " << choice << std::endl;
        if (choice == PRIMARY) {
            std::cout << "unary exp dump primary" << std::endl;
            return primary_exp->dump();
        } else {
            std::cout << "unary exp dump unaryop unaryexp" << std::endl;
            return unary_op->dump() + unary_exp->dump();
        }
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
        std::cout << "dump unary op" << std::endl;
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
    NUMBER,
    LEFT_VALUE
};

class PrimaryExpAST : public BaseAST {
public:
    PrimaryExpASTChoice choice;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> left_value;
    int number;

    std::string dump() override {
        std::cout << "primary exp dump choice: " << choice << std::endl;
        if (choice == EXP) {
            std::cout << "primary exp dump exp" << std::endl;
            return "(" + exp->dump() + ")";
        } else if (choice == LEFT_VALUE) {
            std::cout << "primary exp dump left value" << std::endl;
            return left_value->dump();
        } else {
            std::cout << "primary exp dump number" << std::endl;
            return std::to_string(number);
        }
    }
};




//RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
//EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
//LAndExp     ::= EqExp | LAndExp "&&" EqExp;
//LOrExp      ::= LAndExp | LOrExp "||" LAndExp;

enum RelExpASTChoice {
    ADDEXP,
    REL_OP_ADDEXP
};

class RelExpAST : public BaseAST {
public:
    RelExpASTChoice choice;
    std::unique_ptr<BaseAST> add_exp;
    std::unique_ptr<BaseAST> rel_exp;
    std::string rel_op;

    std::string dump() override {
        std::cout << "dump relexp: " << choice << std::endl;

        if (choice == ADDEXP) {
            std::cout << "dump add_exp" << std::endl;
            return add_exp->dump();
        } else {
            std::cout << "dump rel_exp: " << std::endl;
            return rel_exp->dump() + rel_op + add_exp->dump();
        }
    }
};

enum EqExpASTChoice {
    RELEXP,
    EQ_OP_RELEXP
};

class EqExpAST : public BaseAST {
public:
    EqExpASTChoice choice;
    std::unique_ptr<BaseAST> rel_exp;
    std::unique_ptr<BaseAST> eq_exp;
    std::string eq_op;

    std::string dump() override {
        std::cout << "dump eqexp: " << choice << std::endl;
        if (choice == RELEXP) {
            std::cout << "dump rel_exp" << std::endl;
            return rel_exp->dump();
        } else {
            std::cout << "dump eq_exp: " << std::endl;
            return eq_exp->dump() + eq_op + rel_exp->dump();
        }
    }
};

enum LAndExpASTChoice {
    EQEXP,
    LAND_OP_EQEXP
};

class LAndExpAST : public BaseAST {
public:
    LAndExpASTChoice choice;
    std::unique_ptr<BaseAST> eq_exp;
    std::unique_ptr<BaseAST> land_exp;
    std::string land_op;

    std::string dump() override {
        std::cout << "dump landexp: " << choice << std::endl;
        if (choice == EQEXP) {
            std::cout << "dump eq_exp" << std::endl;
            return eq_exp->dump();
        } else {
            std::cout << "dump land_exp: " << std::endl;
            return land_exp->dump() + land_op + eq_exp->dump();
        }
    }
};

enum LOrExpASTChoice {
    LAND_EXP,
    LOR_OP_LAND_EXP
};

class LOrExpAST : public BaseAST {
public:
    LOrExpASTChoice choice;
    std::unique_ptr<BaseAST> land_exp;
    std::unique_ptr<BaseAST> lor_exp;
    std::string lor_op;

    std::string dump() override {
        std::cout << "dump lorexp: " << choice << std::endl;
        if (choice == LAND_EXP) {
            std::cout << "dump land_exp" << std::endl;
            return land_exp->dump();
        } else {
            std::cout << "dump lor_exp: " << lor_exp->dump() << std::endl;
            return lor_exp->dump() + lor_op + land_exp->dump();
        }
    }
};


class BTypeAST : public BaseAST {
public:
    std::string type;

    std::string dump() {
        return type;
    }
};


enum DeclarationASTChoice {
    CONST_DECLARATION,
    VAR_DECLARATION
};
class DeclarationAST : public BaseAST {
public:
    DeclarationASTChoice choice;

    std::unique_ptr<BaseAST> const_declaration;
    std::unique_ptr<BaseAST> var_declaration;

    std::string dump() override {
        std::cout << "dump declaration: " << choice << std::endl;
        if (choice == CONST_DECLARATION) {
            std::cout << "dump const_declaration" << std::endl;
            return const_declaration->dump();
        } else {
            std::cout << "dump var_declaration" << std::endl;
            return var_declaration->dump();
        }
    }
};

// region: const declaration

class ConstDeclarationAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> b_type;
    /**
    * If only a single const_definition is present, then var_definition_list is nullptr
    */
    std::unique_ptr<BaseAST> const_definition_list;

    std::string dump() override {
        auto ss = std::stringstream();
        ss << "const" << " ";
        ss << b_type->dump() << " ";
        ss << const_definition_list->dump();
        ss << ";";
        return ss.str();
    }
};


enum ConstDefinitionListASTChoice {
    CONST_DEFINITION,
    CONST_DEFINITION_LIST
};

class ConstDefinitionListAST : public BaseAST {
public:

    ConstDefinitionListASTChoice choice;
    std::unique_ptr<BaseAST> const_definition;
    std::vector<std::unique_ptr<BaseAST>> list;

    std::string dump() override {
        if (choice == CONST_DEFINITION) {
            return const_definition->dump();
        } else {
            auto ss = std::stringstream();
            for (auto &item: list) {
                ss << item->dump() << ", ";
            }
            auto final = ss.str();
            if (!list.empty()) {
                final.erase(final.length() - 2);
            }
            return final;
        }
    }
};


class ConstDefinitionAST : public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> const_initialization_expression;

    std::string dump() override {
        auto ss = std::stringstream();
        ss << ident << " ";
        ss << "=" << " ";
        ss << const_initialization_expression->dump();
        return ss.str();
    }
};

class ConstInitializationExpressionAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> const_expression;

    std::string dump() override {
        return const_expression->dump();
    }
};


class ConstExpressionAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> expression;

    std::string dump() override {
        return expression->dump();
    }
};

// endregion






// region: var declaration

class VarDeclarationAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> b_type;
    /**
    * If only a single const_definition is present, then var_definition_list is nullptr
    */
    std::unique_ptr<BaseAST> var_definition_list;

    std::string dump() override {
        auto ss = std::stringstream();
        ss << b_type->dump() << " ";
        ss << var_definition_list->dump();
        ss << ";";
        return ss.str();
    }
};


enum VarDefinitionListASTChoice {
    VAR_DEFINITION,
    VAR_DEFINITION_LIST
};

class VarDefinitionListAST : public BaseAST {
public:

    VarDefinitionListASTChoice choice;
    std::unique_ptr<BaseAST> var_definition;
    std::vector<std::unique_ptr<BaseAST>> list;

    std::string dump() override {
        if (choice == VAR_DEFINITION) {
            return var_definition->dump();
        } else {
            auto ss = std::stringstream();
            for (auto &item: list) {
                ss << item->dump() << ", ";
            }
            auto final = ss.str();
            if (!list.empty()) {
                final.erase(final.length() - 2);
            }
            return final;
        }
    }
};


class VarDefinitionAST : public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseAST> var_initialization_expression;

    std::string dump() override {
        auto ss = std::stringstream();
        ss << ident << " ";
        ss << "=" << " ";
        ss << var_initialization_expression->dump();
        return ss.str();
    }
};

class VarInitializationExpressionAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> var_expression;

    std::string dump() override {
        return var_expression->dump();
    }
};


class VarExpressionAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> expression;

    std::string dump() override {
        return expression->dump();
    }
};

// endregion