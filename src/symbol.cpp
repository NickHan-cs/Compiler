#include <string>
#include "symbol.h"

using namespace std;

Symbol::Symbol(string name, SymbolType type, int layer) {
	symbol_name = name;
	symbol_type = type;
	symbol_layer = layer;
}

VarSymbol::VarSymbol(string name, SymbolType type, int layer): Symbol(name, type, layer){
}

ArrSymbol::ArrSymbol(string name, SymbolType type, int layer) : Symbol(name, type, layer) {
}

FuncSymbol::FuncSymbol(string name, SymbolType type, int layer) : Symbol(name, type, layer) {
}