#ifndef COMPI_HW3_SYMBOL_TABLE_H
#define COMPI_HW3_SYMBOL_TABLE_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class Symbol {
public:
    string name;
    // If class is function, this is return type
    string type;
    int offset;
    bool is_function;
    // param types
    vector<string> params;

    Symbol(const string name, const string type, int offset, bool is_function) : name(name), type(type), offset(offset),
                                                                                 is_function(is_function), params() {}
};

class SymbolTable {

public:
    vector<Symbol> symbols;
    int max_offset;

    SymbolTable(int offset) : symbols(), max_offset(offset) {}

    void add_symbol(const Symbol &symbol);
    bool symbol_exists(const string &name);
    Symbol *get_symbol(const string &name);
};

class TableStack {
    vector<SymbolTable> table_stack;
    vector<int> offsets;

public:
    TableStack() : table_stack() {}

    void push_scope();
    void pop_scope();

    void add_symbol(const string &name,const string &type, bool is_function);
    bool symbol_exists(const string &name);
    Symbol *get_symbol(const string &name);
    void insert_symbol(SymbolTable &table, Symbol &symbol);
};


#endif
