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

// Parsing
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
        parse_poly_decl();
}

void Parser::parse_poly_decl() {
    expect(POLY);
    parse_polynomial_header();
    expect(EQUAL);
    parse_polynomial_body();
    expect(SEMICOLON);
}

void Parser::parse_polynomial_header() {
    parse_polynomial_header();
    Token t = lexer.peek(1);
    if (t.TokenType == LPAREN) {
        lexer.GetToken();
        parse_id_list();
        expect(RPAREN);
    }
}

void Parser::parse_id_list() {
    expect(ID);
    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        lexer.GetToken();
        parse_id_list();
    }
}

void Parser::parse_polynomial_name() {
    Token t = expect(ID);
    // check for duplicate name (semantic checking)
    // then store it
    // to be done after parser implementation
}

void Parser::parse_polynomial_body() {
    parse_term_list();
}

void Parser::parse_term_list() {
    parse_term();
    // check for add_operator
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
        t = lexer.peek(2);
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
    expect(ID);
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
    Token t = expect(NUM);
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

void Parser::parse_statement_list(); {
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
    expect(ID);
    expect(SEMICOLON);
}

void Parser::parse_polynomial_evaluation() {
    parse_polynomial_name();
    expect(LPAREN);
    parse_argument_list();
    expect(RPAREN);
}

void Parser::parse_argument_list() {
    parse_argument();
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

int main() {
	Parser parser;
	parser.ConsumeAllInput();
}
