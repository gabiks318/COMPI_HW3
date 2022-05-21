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

class FuncDecl : public Node {
};

class RetType : public Node {
};

class Formals : public Node {
};

class FormalsList : public Node {
};

class FormalDecl : public Node {
};



class Type : public Node {
public:
    string type;

    Type(const string type);
};

class Exp : public Node {
public:
    string type;
    string value;

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
    Statement(Type *type, Node *id);

    Statement(Type *type, Node *id, Exp *exp);

    Statement(Node *id, Exp *exp);

    Statement(Exp *exp, const string name);

    Statement(Call *call);
};

class Statements : public Node {
    Statements(Statement* statement): Node(){};
    Statements(Statements statements,Statement* statement): Node(){};
};
#define YYSTYPE Node*
#endif
