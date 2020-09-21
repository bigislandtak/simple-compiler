/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include "lexer.h"

class Parser {
public:
    void ConsumeAllInput();
    void parse_input();	// parser starting point
    friend void check_errors(Parser p);


private:
    LexicalAnalyzer lexer;

    std::map<std::string, int> line_of_name;
    
    std::set<std::string> id_list;
    std::set<std::string> initialized_arguments;	// might have to change to map for evaluation
    std::stack<int> argument_counter;
    std::vector<std::pair<std::string, int>> num_of_parameters;

    std::set<int> error_1_lines;
    std::vector<int> error_2_lines;
    std::vector<int> error_3_lines;
    std::vector<int> error_4_lines;
    std::vector<int> error_5_lines;
    


    void syntax_error();
    Token expect(TokenType expected_type);

    // Parser functions
    void parse_program();
    void parse_poly_decl_section();
    void parse_poly_decl();
    void parse_polynomial_header();
    void parse_id_list();
    void parse_polynomial_name();
    void parse_polynomial_body();
    void parse_term_list();
    void parse_term();
    void parse_monomial_list();
    void parse_monomial();
    void parse_exponent();
    void parse_add_operator();
    void parse_coefficient();
    void parse_start();
    void parse_inputs();
    void parse_statement_list();
    void parse_statement();
    void parse_poly_evaluation_statement();
    void parse_input_statement();
    void parse_polynomial_evaluation();
    void parse_argument_list();
    void parse_argument();
};

int partition(std::vector<int> &values, int left, int right);
void quicksort(std::vector<int> &values, int left, int right);

#endif

