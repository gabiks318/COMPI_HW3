#include "types.h"
#include "symbol_table.h"
#include "hw3_output.hpp"

#define TRUE "true"
#define FALSE "false"
extern TableStack tables;
extern int yylineno;

static bool check_types_compatible(string type1, string type2) {
    if (type1 == type2)
        return true;

    if (type1 == "bool" || type2 == "bool" || type1 == "string" || type2 == "string")
        return false;
    return true;
}

//************FORMALS******************
FormalDecl::FormalDecl(FormalDecl *formal) : Node(formal->value), type(formal->type) {
    if (DEBUG)
        std::cout << "FormalDecl " << this->type << " " << this->value << std::endl;
}

FormalDecl::FormalDecl(Type *type, Node *node) : Node(*node), type(type->type) {
    if (DEBUG)
        std::cout << "FormalDecl " << this->type << " " << this->value << std::endl;
//    delete[] node;
//    delete[] type;
}

FormalsList::FormalsList(Node *node) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "FormalsList " << std::endl;
    formals_list.push_back(new FormalDecl(dynamic_cast<FormalDecl *>(node)));
//    delete[] node;
}

FormalsList::FormalsList(Node *node, FormalsList *formals_list) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "FormalsList " << std::endl;
    for (auto it = formals_list->formals_list.begin(); it != formals_list->formals_list.end(); ++it) {
        FormalDecl *decl = new FormalDecl(*it);
        this->formals_list.push_back(decl);
    }
    this->formals_list.push_back(dynamic_cast<FormalDecl *>(node));
//    delete[] formals_list;
}

Formals::Formals() : Node(), formals_list() {
    if (DEBUG)
        std::cout << "Formals " << std::endl;

}

Formals::Formals(FormalsList *formals_list) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "Formals " << std::endl;
    for (auto it = formals_list->formals_list.begin(); it != formals_list->formals_list.end(); ++it) {
        FormalDecl *decl = new FormalDecl(*it);
        this->formals_list.push_back(decl);
    }

//    delete[] formals_list;
}

//************FUNCDECL*****************
// FuncDecl -> RetType ID LPAREN Formals RPAREN LBRACE OS Statements ES RBRACE
FuncDecl::FuncDecl(RetType *return_type, Node *id, Formals *params) {
    if (DEBUG)
        std::cout << "FuncDecl " << std::endl;
    if (tables.symbol_exists(id->value)) {

        output::errorDef(yylineno, id->value);
        exit(0);
    }
    vector<string> function_param_types = vector<string>();
    for (int i = 0; i < params->formals_list.size(); ++i) {
        auto it = params->formals_list[i];
        function_param_types.push_back((*it).type);
    }
    tables.add_symbol(id->value, return_type->type, true, function_param_types);
    tables.push_scope(false, return_type->type);
    int offset = -1;
    for (int i = 0; i < params->formals_list.size(); ++i) {
        auto it = params->formals_list[i];
        if (tables.symbol_exists(it->value)) {
            output::errorDef(yylineno, it->value);
            exit(0);
        }
        tables.add_function_symbol(it->value, it->type, offset);
        offset -= 1;
    }
//    delete[] return_type;
//    delete[] id;
//    delete[] params;
}

//************STATEMENT****************
// Statement -> BREAK SC / CONTINUE SC
Statement::Statement(Node *node) {
    if (node->value == "break") {
        SymbolTable *scope = tables.current_scope();
        if (!scope->is_loop) {
            output::errorUnexpectedBreak(yylineno);
            exit(0);
        }
    } else if (node->value == "continue") {
        SymbolTable *scope = tables.current_scope();
        if (!scope->is_loop) {
            output::errorUnexpectedContinue(yylineno);
            exit(0);
        }
    }

//    delete[] node;

}

Statement::Statement(Exp *exp, bool is_return) : Node() {
    if (DEBUG)
        std::cout << "Statement Expression is_return:" << is_return << std::endl;
    SymbolTable *scope = tables.current_scope();
    string* return_type = scope->return_type;

    if (*return_type != "" && *return_type != exp->type) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

// Statement -> Type ID SC
Statement::Statement(Type *type, Node *id) : Node() {
    if (DEBUG)
        std::cout << "Statement Type ID: " << type->type << " " << id->value << std::endl;
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    tables.add_symbol(id->value, type->type, false);
    value = type->value;//

}

// Statement -> Type ID ASSIGN Exp SC or Statement -> AUTO ID ASSIGN Exp SC
Statement::Statement(Type *type, Node *id, Exp *exp) : Node() {
    if(DEBUG)
        std::cout << "Statement -> Type ID ASSIGN Exp SC\n";
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    if (type) {
        if (!check_types_compatible(type->type, exp->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, type->type, false);
    } else {
        if (exp->type == "void" || exp->type == "string") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, exp->type, false);
    }

    // TODO: do something with value/type?
}

// Statement -> ID ASSIGN Exp SC
Statement::Statement(Node *id, Exp *exp) : Node() {
    if (!tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(id->value);
    if (symbol->is_function || !check_types_compatible(symbol->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // TODO: do something with value/type?
}

// Statement -> Call SC
Statement::Statement(Call *call) : Node() {
    if (DEBUG)
        std::cout << "Statement Call " << call->value << std::endl;
    if (!tables.symbol_exists(call->value)) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(call->value);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    // TODO: do something with value/type?
}

Statement::Statement(Exp *exp,
                     const string name) {
    if (exp->type != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

//****************TYPE************************
Type::Type(
        const string type) : Node(), type(type) {

}

RetType::RetType(
        const string type) : Node(), type(type) {}

// ***************EXP******************
// Exp -> LPAREN Exp RPAREN
Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type) {
}

// Exp -> CONST(bool, num, byte, string)
Exp::Exp(Node *terminal, string
type) : Node(terminal->value), type(type) {
    if (DEBUG)
        std::cout << "Exp Node+string " << type << " " << terminal->value << std::endl;

}

//Exp -> ID, Call
Exp::Exp(bool is_var, Node *terminal) : Node(), is_var(is_var) {
    if (DEBUG)
        std::cout << "Exp -> ID, Call " << terminal->value << " is var: " << is_var << std::endl;
    if (is_var && !tables.symbol_exists(terminal->value)) {
        output::errorUndef(yylineno, terminal->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(terminal->value);
    value = terminal->value;
    type = symbol->type;

}

Exp::Exp(Node *terminal1, Node *terminal2,
         const string op,
         const string type) {
    Exp *exp1 = dynamic_cast<Exp *>(terminal1);
    Exp *exp2 = dynamic_cast<Exp *>(terminal2);

    if (DEBUG)
        std::cout << "Exp op " << exp1->type << " " << exp1->value << " " << exp2->type << " " << exp2->value
                  << std::endl;
    if (exp1->is_var && !tables.symbol_exists(exp1->value)) {
        output::errorUndef(yylineno, terminal1->value);
        exit(0);
    }

    if (exp2->is_var && !tables.symbol_exists(exp2->value)) {
        output::errorUndef(yylineno, terminal2->value);
        exit(0);
    }

    if (!check_types_compatible(exp1->type, exp2->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    if (type == "bool") {
//        bool t1, t2, res;
        if (exp1->type != "bool" || exp2->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }
//        t1 = exp1->value == TRUE;
//        t2 = exp2->value == TRUE;
//
//        if (op == "or") {
//            res = t1 || t2;
//        } else if (op == "and") {
//            res = t1 && t2;
//        } else if (op == "not") {
//            res = !t1;
//        }
        this->type = "bool";
//        this->value = res ? TRUE : FALSE;
    } else if (type == "int") {

        if (exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
        int t1, t2, res;

//        t1 = stoi(exp1->value);
//        t2 = stoi(exp2->value);

//        if (op == "+") {
//            res = t1 + t2;
//        } else if (op == "-") {
//            res = t1 - t2;
//        } else if (op == "*") {
//            res = t1 * t2;
//        } else if (op == "/") {
//            res = t1 / t2;
//        }
//        this->value = to_string(res);

    } else if (type == "relop") {
        if (!check_types_compatible(exp1->type, exp2->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
//        int t1, t2;
//        bool res;
//        std::cout << "t1 value: " << exp1->value << " t2 value: " << exp2->value << std::endl;
//        t1 = stoi(exp1->value);
//        t2 = stoi(exp2->value);

//        if (op == "<=") {
//            res = t1 <= t2;
//        } else if (op == "==") {
//            res = t1 == t2;
//        } else if (op == ">=") {
//            res = t1 >= t2;
//        } else if (op == "!=") {
//            res = t1 != t2;
//        } else if (op == ">") {
//            res = t1 > t2;
//        } else if (op == "<") {
//            res = t1 < t2;
//        }
        this->type = "bool";
//        this->value = res ? TRUE : FALSE;
    }
}

// Exp -> LPAREN Type RPAREN Exp
Exp::Exp(Node *exp, Node *type) {
    Exp *converted_exp = dynamic_cast<Exp *>(exp);
    Type *converted_type = dynamic_cast<Type *>(type);

    if (!check_types_compatible(converted_exp->type, converted_type->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    this->value = converted_exp->value;
    this->type = converted_type->type;
}

//*******************EXPLIST************************

// ExpList -> Exp
    ExpList::ExpList(Node * exp) : expressions(1, *(dynamic_cast<Exp *>(exp)))
    {}

// ExpList -> ExpList, Exp
ExpList::ExpList(Node *exp_list, Node *exp) : expressions((dynamic_cast<ExpList *>(exp_list))->expressions) {
    expressions.push_back(*(dynamic_cast<Exp *>(exp)));
//        delete[] exp;
//        delete[] exp_list;
}

//*******************CALL*********************

// Call -> ID LPAREN RPAREN
Call::Call(Node *terminal) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;

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
    value = symbol->name;
}

// Call -> ID LPAREN ExpList RPAREN
Call::Call(Node *terminal, Node *exp_list) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;
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
    value = symbol->name;
}

