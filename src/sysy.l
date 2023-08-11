%option yylineno
%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string>

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
#include "Ast.h"

#include "sysy.tab.hpp"

#include "iostream"

using namespace std;

int yycolumn = 1;
%}

/* 空白符和注释 */
WhiteSpace    [ \t\r]*
LineComment   "//".*

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%

{WhiteSpace}    {

    // we record the length of space
    yycolumn += yyleng;

}


{LineComment}   { /* 忽略, 不做任何操作 */ }

"short"         {yycolumn += yyleng; return SHORT; }
"int"           {yycolumn += yyleng; return INT; }

"return"        {
    printf("return encountered, yyleng = %d\n", yyleng);
    printf("yytext = %s\n", yytext);

    printf("yylineno = %d\n", yylineno);

    printf("yycolumn = %d\n", yycolumn);
    yycolumn += yyleng;
    yylloc.first_line = yylloc.last_line = yylineno;
    yylloc.first_column = yycolumn;
    yylloc.last_column = yycolumn + yyleng - 1;
    return RETURN;
 }

"const" {yycolumn += yyleng; return CONST_MODIFIER;}

"if" { yycolumn += yyleng; return KEY_WORD_IF;}
"else" {yycolumn += yyleng; return KEY_WORD_ELSE;}

{Identifier}    {
    yylloc.first_line = yylloc.last_line = yylineno;
    yylloc.first_column = yycolumn;
    yylloc.last_column = yycolumn + yyleng;
    yylval.str_val = new string(yytext);
    yycolumn += yyleng;
    return IDENT;
}

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); yycolumn +=yyleng; return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); yycolumn +=yyleng; return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); yycolumn +=yyleng; return INT_CONST; }

\n      { yycolumn = 1;}

. {
    yycolumn ++;
    return yytext[0];
}

%%


// the order matters, the latter will be the expansion of the former