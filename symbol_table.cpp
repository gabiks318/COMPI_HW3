#include "symbol_table.h"
#include "hw3_output.hpp"
TableStack tables();

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
}

void TableStack::add_symbol(const string &name, const string &type, bool is_function) {
    SymbolTable current_scope = table_stack.back();
    int offset = offsets.back() + 1;
    offsets.push_back(offset);
    Symbol symbol = Symbol(name, type, offset, is_function);
    current_scope.add_symbol(symbol);
}

void TableStack::push_scope() {
    table_stack.push_back(SymbolTable(offsets.back()));
    offsets.push_back(table_stack.back().max_offset);
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

