#include <iostream>
#include <cstdlib>
#include "lexer.h"
#include "parser.h"

#include <set>
#include <iterator>

using namespace std;

int main() {
    Parser p;
    p.parse_input();
    check_errors(p);
}