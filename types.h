#ifndef COMPI_HW3_TYPES_H
#define COMPI_HW3_TYPES_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class Node {
public:
    string value;

    explicit Node(const string value = "") : value(value) {};

    virtual ~Node() = default;
};

class Program : public Node {
};

class Funcs : public Node {
};




class Type : public Node {
public:
    string type;

    Type(const string type);
};

class RetType : public Node {
public:
    string type;
    RetType(const string type);
};

class FormalDecl : public Node {
public:
    string type;
    FormalDecl(Type* type, Node* node);
};

class FormalsList : public Node {
public:
    vector<FormalDecl*> formals_list;
    FormalsList(Node* node);
    FormalsList(Node* node, FormalsList* formals_list);
};

class Formals : public Node {
public:
    vector<FormalDecl*> formals_list;
    Formals();
    Formals(FormalsList* formals_list);
};



class FuncDecl : public Node {
public:
    FuncDecl(RetType* return_type, Node* id, Formals* params);
};


class Exp : public Node {
public:
    string type;
    string value;

    Exp():type("void"),value(""){}

    Exp(Node *terminal, string type);

    Exp(Node *terminal1, Node *terminal2, const string op, const string type);

    Exp(Node *terminal);

    Exp(Node *exp, Node *type);

    Exp(Exp *exp);
};

class ExpList : public Node {
public:
    vector<Exp> expressions;

    ExpList(Node *exp);

    ExpList(Node *exp_list, Node *exp);
};


class Call : public Node {
public:
    string type;

    Call(Node *terminal);

    Call(Node *terminal, Node *exp_list);
};

class Statement : public Node {
public:
    Statement(Node* node);

    Statement(Type *type, Node *id);

    Statement(Type *type, Node *id, Exp *exp);

    Statement(Node *id, Exp *exp);

    Statement(Exp *exp, const string name);

    Statement(Exp* exp);

    Statement(Call *call);
};

class Statements : public Node {
public:
    Statements(Statement* statement): Node(){};
    Statements(Statements* statements,Statement* statement): Node(){};
};
#define YYSTYPE Node*
#endif
