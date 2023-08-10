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
%token SHORT INT RETURN CONST_MODIFIER
%token <str_val> IDENT
%token <int_val> INT_CONST


// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block BlockItems BlockItem Stmt
UnaryOp UnaryExp PrimaryExp Exp LVal
AddExp MulExp
LOrExp LAndExp EqExp RelExp
BType Decl
ConstDecl ConstDefList ConstDef ConstInitVal ConstExp
VarDecl   VarDefList      VarDef   VarInitVal   VarExp



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
    printf("FuncType IDENT '(' ')' Block => FuncDef\n");
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
  : '{' BlockItems '}' {
    printf("{ BlockItems } => Block\n");
    auto block = new BlockAST();
    block->block_item_list = unique_ptr<BaseAST>($2);
    $$ = block;
  }
  ;

BlockItems: /* Empty production */
  {
	printf("Empty Block => BlockItems\n");
	auto block_items = new BlockItemListAST();
	block_items->choice = EMPTY;
	$$ = block_items;
  }
  | BlockItems BlockItem {
	printf("BlockItems BlockItem => BlockItems\n");
	auto block_items = new BlockItemListAST();
	block_items->choice = BLOCK_LIST;
	printf("far\n");
	block_items->list.insert(block_items->list.end(),
					 std::make_move_iterator(((BlockItemListAST *)$1)->list.begin()),
					 std::make_move_iterator(((BlockItemListAST *)$1)->list.end()));
	printf("almost there\n");
	block_items->list.push_back(unique_ptr<BaseAST>($2));
	printf("you got it\n");
	$$ = block_items;
  }
  ;

BlockItem
: Decl
 {
   	printf("Decl => BlockItem\n");
   	auto block_item = new BlockItemAST();
   	block_item->choice = DECLARATION;
   	block_item->declaration = unique_ptr<BaseAST>($1);
   	$$ = block_item;
 }
 |
 Stmt
 {
 	printf("Stmt => BlockItem\n");
   	auto block_item = new BlockItemAST();
   	block_item->choice = STATEMENT;
   	block_item->statement = unique_ptr<BaseAST>($1);
   	$$ = block_item;
 }
 ;

Stmt:
  LVal '=' Exp ';' {
    printf("LVal = Exp ; => Stmt\n");
    auto stmt = new StmtAST();
    stmt->choice = ASSIGNMENT_STATEMENT;
    stmt->left_value = unique_ptr<BaseAST>($1);
    stmt->exp = unique_ptr<BaseAST>($3);
    $$ = stmt;
  }
  |
  RETURN Exp ';' {
    printf("return Exp ; => Stmt\n");
    auto stmt = new StmtAST();
    stmt->choice = RETURN_STATEMENT;
    stmt->exp = unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;

//Exp         ::= LOrExp;
Exp
: LOrExp {
	printf("LOrExp => Exp\n");
	auto exp = new ExpAST();
	exp->lor_exp = unique_ptr<BaseAST>($1);
	$$ = exp;
}
;

//MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
MulExp
: UnaryExp {
	printf(" UnaryExp => MulExp\n");
	auto mul_exp = new MulExpAST();
	mul_exp->choice = UNARYEXP;
	mul_exp->unary_exp = unique_ptr<BaseAST>($1);
	$$ = mul_exp;
}
|
MulExp '*' UnaryExp {
	printf(" MulExp * UnaryExp => MulExp\n");
	auto mul_exp = new MulExpAST();
	mul_exp->choice = MUL_OP_UNARYEXP;
	mul_exp->mul_op = "*";
	mul_exp->mul_exp = unique_ptr<BaseAST>($1);
	mul_exp->unary_exp = unique_ptr<BaseAST>($3);
	$$ = mul_exp;
}
|
MulExp '/' UnaryExp {
	printf(" MulExp / UnaryExp => MulExp\n");
	auto mul_exp = new MulExpAST();
	mul_exp->choice = MUL_OP_UNARYEXP;
	mul_exp->mul_op = "/";
	mul_exp->mul_exp = unique_ptr<BaseAST>($1);
	mul_exp->unary_exp = unique_ptr<BaseAST>($3);
	$$ = mul_exp;
}
|
MulExp '%' UnaryExp {
	printf(" MulExp mod UnaryExp => MulExp\n");
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
	printf("MulExp => AddExp\n");
	auto add_exp = new AddExpAST();
	add_exp->choice = MULEXP;
	add_exp->mul_exp = unique_ptr<BaseAST>($1);
	$$ = add_exp;
}
|
AddExp '+' MulExp {
	printf("AddExp + MulExp => AddExp\n");
	auto add_exp = new AddExpAST();
	add_exp->choice = ADD_OP_MULEXP;
	add_exp->add_op = "+";
	add_exp->add_exp = unique_ptr<BaseAST>($1);
	add_exp->mul_exp = unique_ptr<BaseAST>($3);
	$$ = add_exp;
}
|
AddExp '-' MulExp {
	printf("AddExp - MulExp => AddExp\n");
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
  		printf("PrimaryExp => UnaryExp  \n");
		auto unary_exp = new UnaryExpAST();
		unary_exp->choice = PRIMARY;
		unary_exp->primary_exp = unique_ptr<BaseAST>($1);
		$$ = unary_exp;
  }
  |
  UnaryOp UnaryExp {
  		printf("UnaryOp UnaryExp => UnaryExp  \n");
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

LVal
: IDENT {
	printf("LDENT => LVal\n");
	auto lval = new LValAST();
	lval->ident = *$1;
	$$ = lval;
}


//PrimaryExp  ::= "(" Exp ")" | Number;
PrimaryExp
  : '(' Exp ')'  {
  		printf("(Exp) => PrimaryExp\n");
		auto primary_exp = new PrimaryExpAST();
		primary_exp->choice = EXP;
		primary_exp->exp = unique_ptr<BaseAST>($2);
		$$ = primary_exp;
  }
  | LVal
  {
	  	printf("LVal => PrimaryExp\n");
  		auto primary_exp = new PrimaryExpAST();
  		primary_exp->choice = LEFT_VALUE;
  		primary_exp->left_value = unique_ptr<BaseAST>($1);
  		$$ = primary_exp;
  }
  |
  Number {
  		printf("Number => PrimaryExp\n");
  		auto primary_exp = new PrimaryExpAST();
  		primary_exp->choice = NUMBER;
  		primary_exp->number = $1;
  		$$ = primary_exp;
  }


Number
  : INT_CONST {
  	printf("%d => Number\n", $1);
	$$ = $1;
  }
  ;



//RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
//EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
//LAndExp     ::= EqExp | LAndExp "&&" EqExp;
//LOrExp      ::= LAndExp | LOrExp "||" LAndExp;


RelExp
: AddExp {
	printf(" AddExp=>RelExp \n");
	auto rel_exp = new RelExpAST();
	rel_exp->choice = ADDEXP;
	rel_exp->add_exp = unique_ptr<BaseAST>($1);
	$$ = rel_exp;
}
|
RelExp '<' AddExp {
	printf(" RelExp < AddExp=>RelExp \n");
	auto rel_exp = new RelExpAST();
	rel_exp->choice = REL_OP_ADDEXP;
	rel_exp->rel_op = "<";
	rel_exp->rel_exp = unique_ptr<BaseAST>($1);
	rel_exp->add_exp = unique_ptr<BaseAST>($3);
	$$ = rel_exp;
}
|
RelExp '>' AddExp {
	printf(" RelExp > AddExp=>RelExp \n");
	auto rel_exp = new RelExpAST();
	rel_exp->choice = REL_OP_ADDEXP;
	rel_exp->rel_op = ">";
	rel_exp->rel_exp = unique_ptr<BaseAST>($1);
	rel_exp->add_exp = unique_ptr<BaseAST>($3);
	$$ = rel_exp;
}
|
RelExp '<''=' AddExp {
	printf(" RelExp <= AddExp => RelExp \n");
	auto rel_exp = new RelExpAST();
	rel_exp->choice = REL_OP_ADDEXP;
	rel_exp->rel_op = "<=";
	rel_exp->rel_exp = unique_ptr<BaseAST>($1);
	rel_exp->add_exp = unique_ptr<BaseAST>($4);
	$$ = rel_exp;
}
|
RelExp '>''=' AddExp {
	printf(" RelExp >= AddExp => RelExp \n");
	auto rel_exp = new RelExpAST();
	rel_exp->choice = REL_OP_ADDEXP;
	rel_exp->rel_op = ">=";
	rel_exp->rel_exp = unique_ptr<BaseAST>($1);
	rel_exp->add_exp = unique_ptr<BaseAST>($4);
	$$ = rel_exp;
}


EqExp
: RelExp {
	printf("RelExp => EqExp\n");
	auto eq_exp = new EqExpAST();
	eq_exp->choice = RELEXP;
	eq_exp->rel_exp = unique_ptr<BaseAST>($1);
	$$ = eq_exp;
}
|
EqExp '=''=' RelExp {
	printf(" EqExp == RelExp => EqExp \n");
	auto eq_exp = new EqExpAST();
	eq_exp->choice = EQ_OP_RELEXP;
	eq_exp->eq_op = "==";
	eq_exp->eq_exp = unique_ptr<BaseAST>($1);
	eq_exp->rel_exp = unique_ptr<BaseAST>($4);
	$$ = eq_exp;
}
|
EqExp '!''=' RelExp {
	printf(" EqExp != RelExp => EqExp \n");
	auto eq_exp = new EqExpAST();
	eq_exp->choice = EQ_OP_RELEXP;
	eq_exp->eq_op = "!=";
	eq_exp->eq_exp = unique_ptr<BaseAST>($1);
	eq_exp->rel_exp = unique_ptr<BaseAST>($4);
	$$ = eq_exp;
}

LAndExp
: EqExp {
	printf("EqExp => LAndExp\n");
	auto land_exp = new LAndExpAST();
	land_exp->choice = EQEXP;
	land_exp->eq_exp = unique_ptr<BaseAST>($1);
	$$ = land_exp;
}
|
LAndExp '&''&' EqExp {
	printf("LAndExp && EqExp => LAndExp  \n");
	auto land_exp = new LAndExpAST();
	land_exp->choice = LAND_OP_EQEXP;
	land_exp->land_op = "&&";
	land_exp->land_exp = unique_ptr<BaseAST>($1);
	land_exp->eq_exp = unique_ptr<BaseAST>($4);
	$$ = land_exp;
}
|
LAndExp '|''|' EqExp {
	printf(" LAndExp || EqExp => LAndExp\n");
	auto land_exp = new LAndExpAST();
	land_exp->choice = LAND_OP_EQEXP;
	land_exp->land_op = "||";
	land_exp->land_exp = unique_ptr<BaseAST>($1);
	land_exp->eq_exp = unique_ptr<BaseAST>($4);
	$$ = land_exp;
}

LOrExp
: LAndExp {
	printf(" LAndExp => LOrExp \n");
	auto lor_exp = new LOrExpAST();
	lor_exp->choice = LAND_EXP;
	lor_exp->land_exp = unique_ptr<BaseAST>($1);
	$$ = lor_exp;
}
|
LOrExp '|''|' LAndExp {
	printf(" LOrExp || LAndExp => LOrExp\n");
	auto lor_exp = new LOrExpAST();
	lor_exp->choice = LOR_OP_LAND_EXP;
	lor_exp->lor_op = "||";
	lor_exp->lor_exp = unique_ptr<BaseAST>($1);
	lor_exp->land_exp = unique_ptr<BaseAST>($4);
	$$ = lor_exp;
}





BType:
INT {
	printf("INT => BType\n");
	auto b_type = new BTypeAST();
	b_type->type = "int";
	$$ = b_type;
}


Decl:
ConstDecl {
	printf("ConstDecl => Decl\n");
	auto decl = new DeclarationAST();
	decl->const_declaration = unique_ptr<BaseAST>($1);
	decl->choice = CONST_DECLARATION;
	$$ = decl;
}
| VarDecl {
	printf("VarDecl => Decl\n");
	auto decl = new DeclarationAST();
	decl->var_declaration = unique_ptr<BaseAST>($1);
	decl->choice = VAR_DECLARATION;
	$$ = decl;
}

// region: const declaration

ConstDecl:
CONST_MODIFIER BType ConstDefList ';' {
	printf("CONST_MODIFIER BType ConstDefList => ConstDecl\n");
	auto const_decl = new ConstDeclarationAST();
	const_decl->b_type = unique_ptr<BaseAST>($2);
	const_decl->const_definition_list = unique_ptr<BaseAST>($3);
	$$ = const_decl;
}

ConstDefList:
ConstDef
{
	printf("ConstDef => ConstDefList\n");
	auto const_def_list = new ConstDefinitionListAST();
	const_def_list->choice = CONST_DEFINITION;
	const_def_list->const_definition = unique_ptr<BaseAST>($1);
	$$ = const_def_list;
}
| ConstDefList ',' ConstDef
{
	printf("ConstDefList , ConstDef => ConstDefList\n");
	auto const_def_list = new ConstDefinitionListAST();
	const_def_list->choice = CONST_DEFINITION_LIST;
	const_def_list->list.insert(const_def_list->list.end(),
				 std::make_move_iterator(((ConstDefinitionListAST *)$1)->list.begin()),
				 std::make_move_iterator(((ConstDefinitionListAST *)$1)->list.end()));
	const_def_list->list.push_back(unique_ptr<BaseAST>($3));
	$$ = const_def_list;
}

ConstDef:
IDENT '=' ConstInitVal {
	printf("IDENT = ConstInitVal => ConstDef\n");
	auto const_def = new ConstDefinitionAST();
	const_def->ident = *$1;
	const_def->const_initialization_expression = unique_ptr<BaseAST>($3);
	$$ = const_def;
}

ConstInitVal:
ConstExp {
	printf("ConstExp => ConstInitVal\n");
	auto const_init_val = new ConstInitializationExpressionAST();
	const_init_val->const_expression = unique_ptr<BaseAST>($1);
	$$ = const_init_val;
}

ConstExp:
Exp {
	printf("Exp => ConstExp\n");
	auto const_exp = new ConstExpressionAST();
	const_exp->expression = unique_ptr<BaseAST>($1);
	$$ = const_exp;
}

// endregion


// region: var declaration

VarDecl:
BType VarDefList ';' {
	printf("BType VarDefList ;=> VarDecl\n");
	auto var_decl = new VarDeclarationAST();
	var_decl->b_type = unique_ptr<BaseAST>($1);
	var_decl->var_definition_list = unique_ptr<BaseAST>($2);
	$$ = var_decl;
}

VarDefList:
VarDef
{
	printf("VarDef => VarDefList\n");
	auto var_def_list = new VarDefinitionListAST();
	var_def_list->choice = VAR_DEFINITION;
	var_def_list->var_definition = unique_ptr<BaseAST>($1);
	$$ = var_def_list;
}
| VarDefList ',' VarDef
{
	printf("VarDefList , VarDef => VarDefList\n");
	auto
	var_def_list = new VarDefinitionListAST();
	var_def_list->choice = VAR_DEFINITION_LIST;
	var_def_list->list.insert(var_def_list->list.end(),
				 std::make_move_iterator(((VarDefinitionListAST *)$1)->list.begin()),
				 std::make_move_iterator(((VarDefinitionListAST *)$1)->list.end()));
	var_def_list->list.push_back(unique_ptr<BaseAST>($3));
	$$ = var_def_list;
}

VarDef:
IDENT '=' VarInitVal {
	printf("IDENT = VarInitVal => VarDef\n");
	auto
	var_def = new VarDefinitionAST();
	var_def->ident = *$1;
	var_def->var_initialization_expression = unique_ptr<BaseAST>($3);
	$$ = var_def;
}

VarInitVal:
VarExp {
	printf("VarExp => VarInitVal\n");
	auto
	var_init_val = new VarInitializationExpressionAST();
	var_init_val->var_expression = unique_ptr<BaseAST>($1);
	$$ = var_init_val;
}

VarExp:
Exp {
	printf("Exp => VarExp\n");
	auto
	var_exp = new VarExpressionAST();
	var_exp->expression = unique_ptr<BaseAST>($1);
	$$ = var_exp;
}

// endregion

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}

void logoutString(std::string str) {
  std::cout << "debug info:" << str << std::endl;
}