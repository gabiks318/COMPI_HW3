%{
    #include "hw3_output.hpp"
    #include "parser.tab.hpp"
%}

%option yylineno
%option noyywrap
digit            ([0-9])
letter           ([a-zA-Z])
letterdigit      ([a-zA-Z0-9])
whitespace       ([\t\n\r ])

%%
void                                                                                return VOID;
int                                                                                 return INT;
byte                                                                                return BYTE;
b                                                                                   return B;
bool                                                                                return BOOL;
auto                                                                                return AUTO;
and                                                                                 return AND;
or                                                                                  return OR;
not                                                                                 return NOT;
true                                                                                return TRUE;
false                                                                               return FALSE;
return                                                                              return RETURN;
if                                                                                  return IF;
else                                                                                return ELSE;
while                                                                               return WHILE;
break                                                                               return BREAK;
continue                                                                            return CONTINUE;
;                                                                                   return SC;
,                                                                                   return COMMA;
\(                                                                                  return LPAREN;
\)                                                                                  return RPAREN;
\{                                                                                  return LBRACE;
\}                                                                                  return RBRACE;
=                                                                                   return ASSIGN;
==|!=|<|>|<=|>=                                                                     return RELOP;
\+|\-                                                                               return ADD_SUB;
\*|\/                                                                               return MULT_DIV;
{letter}{letterdigit}*                                                              return ID;
([1-9]+{digit}*)|0                                                                  return NUM;
\"([^\n\r\"\\]|\\[rnt"\\])+\"                                                       return STRING;
\/\/[^\n\r]*[\r|\n|\r\n]?                                                           ;
{whitespace}                                                                        ;
.                                                                                   {output::errorLex(yylineno); exit(0);}

