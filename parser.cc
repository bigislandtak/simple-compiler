/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include <iterator>
#include "parser.h"

using namespace std;

void Parser::syntax_error() {
    cout << "SYNTAX ERROR !&%!\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// Parsing functions

void Parser::parse_input() {
    parse_program();
    parse_inputs();
    expect(END_OF_FILE);
}

void Parser::parse_program() {
    parse_poly_decl_section();
    parse_start();
}

void Parser::parse_poly_decl_section() {
    parse_poly_decl();
    Token t = lexer.peek(1);
    if (t.token_type == POLY)
        parse_poly_decl_section();
}

void Parser::parse_poly_decl() {
    expect(POLY);
    parse_polynomial_header();

    if (num_of_parameters.back().second == 0)
        num_of_parameters.back().second++;

    expect(EQUAL);
    parse_polynomial_body();

    // Clear id_list for next polynomial_decl
    id_list.clear();

    expect(SEMICOLON);
}

void Parser::parse_polynomial_header() {
    parse_polynomial_name();
    Token t = lexer.peek(1);
    if (t.token_type == LPAREN) {
        lexer.GetToken();
        parse_id_list();
        expect(RPAREN);
    }
}

void Parser::parse_id_list() {
    Token id = expect(ID);

    // Add ID to valid variable set and increment count
    id_list.insert(id.lexeme);
    num_of_parameters.back().second++;

    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        lexer.GetToken();
        parse_id_list();
    }
}

void Parser::parse_polynomial_name() {
    lexer.UngetToken(1);
    bool is_decl = (lexer.GetToken().token_type == POLY);

    Token name = expect(ID);

    // If polynomial_name in polynomial_decl check for duplicates
    if (is_decl) {
        // If duplicate polynomial_name add line_no to error_1_lines
        if (line_of_name.find(name.lexeme) != line_of_name.end()) {
            error_1_lines.insert(line_of_name[name.lexeme]);
            error_1_lines.insert(name.line_no);
        }
        // Otherwise mark polynomial_name as previously seen
        else
            line_of_name[name.lexeme] = name.line_no;

        // Push back current polynomial_name with num_of_parameters initialized to zero
        // For argument number checking
        num_of_parameters.push_back(make_pair(name.lexeme, 0));
    }
    // Else check if declared
    else if (line_of_name.find(name.lexeme) == line_of_name.end())
        error_3_lines.push_back(name.line_no);

}

void Parser::parse_polynomial_body() {
    parse_term_list();
}

void Parser::parse_term_list() {
    parse_term();
    Token t = lexer.peek(1);
    if (t.token_type == PLUS || t.token_type == MINUS) {
        parse_add_operator();
        parse_term_list();
    }
}

void Parser::parse_term() {
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        parse_monomial_list();
    else if (t.token_type == NUM){
        parse_coefficient();
        t = lexer.peek(1);
        if (t.token_type == ID)
            parse_monomial_list();
    }
    else
        syntax_error();
}

void Parser::parse_monomial_list() {
    parse_monomial();
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        parse_monomial_list();
}

void Parser::parse_monomial() {
    Token id = expect(ID);

    // If univariate and id not "x"
    if (id_list.empty() && id.lexeme.compare("x") != 0)
        error_2_lines.push_back(id.line_no);
    // Else if id not in id_list add line_no to error_2_lines
    else if (!id_list.empty() && id_list.find(id.lexeme) == id_list.end())
        error_2_lines.push_back(id.line_no);

    Token t = lexer.peek(1);
    if (t.token_type == POWER)
        parse_exponent();
}

void Parser::parse_exponent() {
    expect(POWER);
    expect(NUM);
}

void Parser::parse_add_operator() {
    Token t = lexer.GetToken();
    if (t.token_type != PLUS && t.token_type != MINUS)
        syntax_error();
}

void Parser::parse_coefficient() {
    expect(NUM);
    // convert string to int and store in memory
    // to be done after parser implementation
}

void Parser::parse_start() {
    expect(START);
    parse_statement_list();
}

void Parser::parse_inputs() {
    expect(NUM);
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
        parse_inputs();
}

void Parser::parse_statement_list() {
    parse_statement();
    Token t = lexer.peek(1);
    if (t.token_type == INPUT || t.token_type == ID)
        parse_statement_list();
}

void Parser::parse_statement() {
    Token t = lexer.peek(1);
    if (t.token_type == INPUT)
        parse_input_statement();
    else if (t.token_type == ID)
        parse_poly_evaluation_statement();
    else
        syntax_error();
}

void Parser::parse_poly_evaluation_statement() {
    parse_polynomial_evaluation();
    expect(SEMICOLON);
}

void Parser::parse_input_statement() {
    expect(INPUT);
    Token id = expect(ID);
    initialized_arguments.insert(id.lexeme);
    expect(SEMICOLON);
}

void Parser::parse_polynomial_evaluation() {
    Token name = lexer.peek(1);
    parse_polynomial_name();
    expect(LPAREN);

    argument_counter.push(0);
    parse_argument_list();
    int num_of_arguments = argument_counter.top();
    argument_counter.pop();

    // Check if number of arguments matches polynomial_decl
    vector <pair<string,int>>::iterator itr;
    for (itr = num_of_parameters.begin(); itr != num_of_parameters.end(); ++itr)
        if ((*itr).first.compare(name.lexeme) == 0)
            break;

    if (num_of_arguments != (*itr).second)
        error_4_lines.push_back(name.line_no);

    expect(RPAREN);
}

void Parser::parse_argument_list() {
    parse_argument();
    argument_counter.top()++;

    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        lexer.GetToken();
        parse_argument_list();
    }
}

void Parser::parse_argument() {
    Token t = lexer.peek(2);
    if (t.token_type == LPAREN)
        parse_polynomial_evaluation();
    else {
        t = lexer.GetToken();
        if (t.token_type != ID && t.token_type != NUM)
            syntax_error();
        else if (t.token_type == ID && initialized_arguments.find(t.lexeme) == initialized_arguments.end())
            error_5_lines.push_back(t.line_no);
    }
}
    

// This function is simply to illustrate the GetToken() function
// you will not need it for your project and you can delete it
// the function also illustrates the use of peek()
void Parser::ConsumeAllInput() {
    Token token;
    int i = 1;
    
    token = lexer.peek(i);
    token.Print();
    while (token.token_type != END_OF_FILE) {
        i = i+1;
        token = lexer.peek(i);
        token.Print();
    }
   
    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE) {
        token = lexer.GetToken();
        token.Print();
    }

}

void check_errors(Parser p) {
    if (!p.error_1_lines.empty()) {
        set <int> :: iterator itr;
        cout << "Error Code 1: ";
        for (itr = p.error_1_lines.begin(); itr != p.error_1_lines.end(); ++itr)
            cout << " " << *itr; 
        cout << endl;
    }
    else if (!p.error_2_lines.empty()) {
        cout << "Error Code 2: ";
        for (int line_no : p.error_2_lines)
            cout << " " << line_no; 
        cout << endl;
    }
    else if (!p.error_3_lines.empty()) {
        cout << "Error Code 3: ";
        for (int line_no : p.error_3_lines)
            cout << " " << line_no; 
        cout << endl;
    }
    else if (!p.error_4_lines.empty()) {
        quicksort(p.error_4_lines, 0, p.error_4_lines.size()-1);
        cout << "Error Code 4: ";
        for (int line_no : p.error_4_lines)
            cout << " " << line_no; 
        cout << endl;
    }
    else if (!p.error_5_lines.empty()) {
        quicksort(p.error_5_lines, 0, p.error_5_lines.size()-1);
        cout << "Error Code 5: ";
        for (int line_no : p.error_5_lines)
            cout << " " << line_no; 
        cout << endl;
    }
}

int partition(vector<int> &values, int left, int right) {
    int pivotIndex = left + (right - left) / 2;
    int pivotValue = values[pivotIndex];
    int i = left, j = right;
    int temp;
    while(i <= j) {
        while(values[i] < pivotValue) {
            i++;
        }
        while(values[j] > pivotValue) {
            j--;
        }
        if(i <= j) {
            temp = values[i];
            values[i] = values[j];
            values[j] = temp;
            i++;
            j--;
        }
    }
    return i;
}

void quicksort(vector<int> &values, int left, int right) {
    if(left < right) {
        int pivotIndex = partition(values, left, right);
        quicksort(values, left, pivotIndex - 1);
        quicksort(values, pivotIndex, right);
    }
}
