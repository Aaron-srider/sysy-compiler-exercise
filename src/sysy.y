%{
#include <iostream>
#include <memory>
#include <string>
#include "Ast.h"
#define YYERROR_VERBOSE 1
// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);
void logoutString(std::string str);
using namespace std;

%}
// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
//%union {
//  std::string *str_val;
//  int int_val;
//}

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token SHORT INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST


// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt        UnaryOp UnaryExp PrimaryExp Exp     AddExp MulExp
%type <int_val> Number


%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担
FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    logoutString(std::string() + "func name: " + *$2);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;


FuncType
  	:INT {
  		  auto func_type = new FuncTypeAST();
  		  func_type->type_name = std::string("int");
  		  $$ = func_type;
        }
  ;

Block
  : '{' Stmt '}' {
    auto block = new BlockAST();
    block->stmt = unique_ptr<BaseAST>($2);
    $$ = block;
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto stmt = new StmtAST();
    stmt->exp = unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;

//Exp         ::= AddExp;
Exp
: AddExp {
	auto exp = new ExpAST();
	exp->add_exp = unique_ptr<BaseAST>($1);
	$$ = exp;
}
;

//MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
MulExp
: UnaryExp {
	auto mul_exp = new MulExpAST();
	mul_exp->choice = UNARYEXP;
	mul_exp->unary_exp = unique_ptr<BaseAST>($1);
	$$ = mul_exp;
}
|
MulExp '*' UnaryExp {
	auto mul_exp = new MulExpAST();
	mul_exp->choice = MUL_OP_UNARYEXP;
	mul_exp->mul_op = "*";
	mul_exp->mul_exp = unique_ptr<BaseAST>($1);
	mul_exp->unary_exp = unique_ptr<BaseAST>($3);
	$$ = mul_exp;
}
|
MulExp '/' UnaryExp {
	auto mul_exp = new MulExpAST();
	mul_exp->choice = MUL_OP_UNARYEXP;
	mul_exp->mul_op = "/";
	mul_exp->mul_exp = unique_ptr<BaseAST>($1);
	mul_exp->unary_exp = unique_ptr<BaseAST>($3);
	$$ = mul_exp;
}
|
MulExp '%' UnaryExp {
	auto mul_exp = new MulExpAST();
	mul_exp->choice = MUL_OP_UNARYEXP;
	mul_exp->mul_op = "%";
	mul_exp->mul_exp = unique_ptr<BaseAST>($1);
	mul_exp->unary_exp = unique_ptr<BaseAST>($3);
	$$ = mul_exp;
}

//AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;
AddExp
: MulExp {
	auto add_exp = new AddExpAST();
	add_exp->choice = MULEXP;
	add_exp->mul_exp = unique_ptr<BaseAST>($1);
	$$ = add_exp;
}
|
AddExp '+' MulExp {
	auto add_exp = new AddExpAST();
	add_exp->choice = ADD_OP_MULEXP;
	add_exp->add_op = "+";
	add_exp->add_exp = unique_ptr<BaseAST>($1);
	add_exp->mul_exp = unique_ptr<BaseAST>($3);
	$$ = add_exp;
}
|
AddExp '-' MulExp {
	auto add_exp = new AddExpAST();
	add_exp->choice = ADD_OP_MULEXP;
	add_exp->add_op = "-";
	add_exp->add_exp = unique_ptr<BaseAST>($1);
	add_exp->mul_exp = unique_ptr<BaseAST>($3);
	$$ = add_exp;
}



//UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp;
UnaryExp
  : PrimaryExp {
		auto unary_exp = new UnaryExpAST();
		unary_exp->choice = PRIMARY;
		unary_exp->primary_exp = unique_ptr<BaseAST>($1);
		$$ = unary_exp;
  }
  |
  UnaryOp UnaryExp {
	  auto unary_exp = new UnaryExpAST();
	  unary_exp->choice = UNARYOP_UNARYEXP;
	  unary_exp->unary_op =  unique_ptr<BaseAST>($1);
	  unary_exp->unary_exp =  unique_ptr<BaseAST>($2);
	  $$ = unary_exp;
  }
  ;

//UnaryOp     ::= "+" | "-" | "!";
UnaryOp: '+'
       | '-'
       | '!'
       ;

//PrimaryExp  ::= "(" Exp ")" | Number;
PrimaryExp
  : '(' Exp ')'  {
		auto primary_exp = new PrimaryExpAST();
		primary_exp->choice = EXP;
		primary_exp->exp = unique_ptr<BaseAST>($2);
		$$ = primary_exp;
  }
  |
  Number {
  		auto primary_exp = new PrimaryExpAST();
  		primary_exp->choice = NUMBER;
  		primary_exp->number = $1;
  		$$ = primary_exp;
  }


Number
  : INT_CONST {
	$$ = $1;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}

void logoutString(std::string str) {
  std::cout << "debug info:" << str << std::endl;
}