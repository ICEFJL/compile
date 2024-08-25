#pragma once
#include "global.h"
static map<int, string> symMap = {{-1, "error 非法字符"},
                                  {-2, "error :后没有="},
                                  {31, "数字"},
                                  {32, "标识符"}};

/*保留字 种别码 1-30*/

static unordered_map<string, int> ReserveWord = {
    {"program", 1}, {"const", 2}, {"var", 3},    {"procedure", 4},
    {"begin", 5},   {"end", 6},   {"if", 7},     {"then", 8},
    {"else", 9},    {"odd", 10},  {"while", 11}, {"do", 12},
    {"call", 13},   {"read", 14}, {"write", 15}};

static unordered_map<string, int> Delimiter = {
    {";", 16}, {",", 17}, {"(", 18}, {")", 19}, {":=", 20}};

static unordered_map<string, int> Operator = {
    {"+", 21}, {"-", 22}, {"*", 23},  {"/", 24},  {"=", 25},
    {"<", 26}, {">", 27}, {"<>", 28}, {"<=", 29}, {">=", 30}};
/*常数种别码 31*/
/*标识符种别码 32*/

void word_analysis(string place);
