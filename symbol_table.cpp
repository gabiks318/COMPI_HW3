#include "symbol_table.h"
#include "hw3_output.hpp"



void SymbolTable::add_symbol(const Symbol &symbol) {
    this->symbols.push_back(symbol);
    this->max_offset = symbol.offset;
}


bool SymbolTable::symbol_exists(const string &name) {
    for(auto it = symbols.begin(); it != symbols.end(); ++it){
        if((*it).name == name)
            return true;
    }
    return false;
}

Symbol *SymbolTable::get_symbol(const string &name) {
    for(auto it = symbols.begin(); it != symbols.end(); ++it){
        if((*it).name == name)
            return &(*it);
    }
    return nullptr;
}

//**************TABLESTACK*************************

TableStack::TableStack(): table_stack(), offsets() {
    push_scope(false);
}

bool TableStack::symbol_exists(const string &name) {
    for(auto it = table_stack.begin(); it != table_stack.end(); ++it){
        if((*it).symbol_exists(name))
            return true;
    }
    return false;
}

Symbol *TableStack::get_symbol(const string &name) {
    for(auto it = table_stack.begin(); it != table_stack.end(); ++it){
        Symbol* symbol = (*it).get_symbol(name);
        if(symbol)
            return symbol;
    }
    return nullptr;
}

void TableStack::add_symbol(const string &name, const string &type, bool is_function, vector<string> params)  {
    SymbolTable current_scope = table_stack.back();
    int offset = offsets.back() + 1;
    offsets.push_back(offset);
    Symbol symbol = Symbol(name, type, offset, is_function, params);
    current_scope.add_symbol(symbol);
}

void TableStack::add_function_symbol(const string &name, const string &type, int offset) {
    SymbolTable current_scope = table_stack.back();
    Symbol symbol = Symbol(name, type, offset, false, vector<string>());
    current_scope.add_symbol(symbol);
}

void TableStack::push_scope(bool is_loop, string return_type) {
    table_stack.push_back(SymbolTable(offsets.back(), is_loop, return_type));
    offsets.push_back(table_stack.back().max_offset);
}

SymbolTable *TableStack::current_scope() {
    return &table_stack.back();
}

void TableStack::pop_scope() {
    SymbolTable scope = table_stack.back();
    table_stack.pop_back();
    output::endScope();
    for(auto it = scope.symbols.begin(); it != scope.symbols.end(); ++it){
        offsets.pop_back();
        if((*it).is_function){
            output::printID((*it).name, 0, output::makeFunctionType((*it).type, (*it).params));
        } else {
            output::printID((*it).name, (*it).offset, (*it).type);
        }
    }
}

