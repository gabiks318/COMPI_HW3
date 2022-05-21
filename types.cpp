#include "types.h"
#include "symbol_table.h"
#include "hw3_output.hpp"

#define TRUE "true"
#define FALSE "false"

extern int yylineno;
extern TableStack tables;

static bool check_types_compatible(string type1, string type2){
    if(type1 == type2)
        return true;

    if(type1 == "bool" || type2 == "bool" || type1 == "string" || type2 == "string")
        return false;

    return true;
}

//************STATEMENT****************
// Statement -> Type ID SC
Statement::Statement(Type* type, Node* id): Node(){
    if(!tables.symbol_exists(id->value)){
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    tables.add_symbol(id->value, type->type, false);
    value = type->value;//

}

// Statement -> Type ID ASSIGN Exp SC or Statement -> AUTO ID ASSIGN Exp SC
Statement::Statement(Type* type, Node *id, Exp *exp): Node() {
    if(tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    if(type){
        if(!check_types_compatible(type->type, exp->type)){
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, type->type, false);
    } else {
        tables.add_symbol(id->value, exp->type, false);
    }
    // TODO: do something with value/type?
}

// Statement -> ID ASSIGN Exp SC
Statement::Statement(Node *id, Exp *exp): Node() {
    if(!tables.symbol_exists(id->value)){
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    Symbol* symbol = tables.get_symbol(id->value);
    if(symbol->is_function || !check_types_compatible(symbol->type, exp->type)){
        output::errorMismatch(yylineno);
        exit(0);
    }

    // TODO: do something with value/type?
}

// Statement -> Call SC
Statement::Statement(Call *call): Node() {
    if(!tables.symbol_exists(call->value)){
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Symbol* symbol = tables.get_symbol(call->value);
    if(!symbol->is_function){
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    // TODO: do something with value/type?
}

Statement::Statement(Exp *exp, const string name){
    if(exp->type != "BOOL"){
        output::errorMismatch(yylineno);
        exit(0);
    }
}

//****************TYPE************************
Type::Type(const string type): Node(), type(type){

}
// ***************EXP******************
// Exp -> LPAREN Exp RPAREN
Exp::Exp(Exp *exp): Node(exp->value), type(exp->type) {}

// Exp -> CONST(bool, num, byte, string)
Exp::Exp(Node *terminal, string type) : Node(terminal->value), type(type) {

}

// Exp -> ID, Call
Exp::Exp(Node *terminal) : Node() {
    if (!tables.symbol_exists(terminal->value)) {
        output::errorUndef(yylineno, terminal->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(terminal->value);
    value = terminal->value;
    type = symbol->type;
}

Exp::Exp(Node *terminal1, Node *terminal2, const string op, const string type) {
    Exp *exp1 = dynamic_cast<Exp*>(terminal1);
    Exp *exp2 = dynamic_cast<Exp*>(terminal2);

    if(!check_types_compatible(exp1->type, exp2->type)){
        output::errorMismatch(yylineno);
        exit(0);
    }

    if (type == "bool") {
        bool t1, t2, res;
        t1 = exp1->value == TRUE;
        t2 = exp2->value == TRUE;

        if (op == "or") {
            res = t1 || t2;
        } else if (op == "and") {
            res = t1 && t2;
        } else if (op == "not") {
            res = !t1;
        }
        this->type = "bool";
        this->value = res ? TRUE : FALSE;
    } else if (type == "int") {

        if (exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
        int t1, t2, res;
        t1 = stoi(exp1->value);
        t2 = stoi(exp2->value);

        if (op == "+") {
            res = t1 + t2;
        } else if (op == "-") {
            res = t1 - t2;
        } else if (op == "*") {
            res = t1 * t2;
        } else if (op == "/") {
            res = t1 / t2;
        }
        this->value = to_string(res);

    } else if (type == "relop") {
        int t1, t2;
        bool res;
        t1 = stoi(exp1->value);
        t2 = stoi(exp2->value);

        if (op == "<=") {
            res = t1 <= t2;
        } else if (op == "==") {
            res = t1 == t2;
        } else if (op == ">=") {
            res = t1 >= t2;
        } else if (op == "!=") {
            res = t1 != t2;
        } else if (op == ">") {
            res = t1 > t2;
        } else if (op == "<") {
            res = t1 < t2;
        }
        this->type = "bool";
        this->value = res ? TRUE : FALSE;
    }
}

// Exp -> LPAREN Type RPAREN Exp
Exp::Exp(Node *exp, Node *type) {
    Exp *converted_exp = dynamic_cast<Exp *>(exp);
    Type *converted_type = dynamic_cast<Type *>(type);

    if(!check_types_compatible(converted_exp->type, converted_type->type)){
        output::errorMismatch(yylineno);
        exit(0);
    }

    this->value = converted_exp->value;
    this->type = converted_type->type;
}

//*******************EXPLIST************************

// ExpList -> Exp
ExpList::ExpList(Node *exp) : expressions(1, *(dynamic_cast<Exp *>(exp))) {}

// ExpList -> ExpList, Exp
ExpList::ExpList(Node *exp_list, Node *exp) : expressions((dynamic_cast<ExpList *>(exp_list))->expressions) {
    expressions.push_back(*(dynamic_cast<Exp *>(exp)));
}

// Call -> ID LPAREN RPAREN
Call::Call(Node *terminal) : Node() {
    string name = terminal->value;
    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    if (symbol->params.size() > 0) {
        output::errorPrototypeMismatch(yylineno, name, symbol->params);
        exit(0);
    }

    type = symbol->type;
}

//*******************CALL*********************

// Call -> ID LPAREN ExpList RPAREN
Call::Call(Node *terminal, Node *exp_list) : Node() {
    ExpList *expressions_list = dynamic_cast<ExpList *>(exp_list);
    string name = terminal->value;

    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }


    if (symbol->params.size() != expressions_list->expressions.size()) {
        output::errorPrototypeMismatch(yylineno, name, symbol->params);
        exit(0);
    }
    for (int i = 0; i < symbol->params.size(); i++) {
        if (symbol->params[i] != expressions_list->expressions[i].type) {
            output::errorPrototypeMismatch(yylineno, name, symbol->params);
            exit(0);
        }
    }

    type = symbol->type;
}

