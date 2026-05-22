#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
using namespace std;

enum class TokenType
{
    NUMBER,
    VARIABLE,
    OPERATOR,
    LPAREN,
    RPAREN
};

struct Token
{
    TokenType type;
    string value;
};

vector<Token> tokenize(const string &expr)
{
    vector<Token> tokens;
    size_t i = 0;

    while (i < expr.size())
    {
        char c = expr[i];
        if (isspace(c))
        {
            ++i;
            continue;
        }
        if (isdigit(c))
        {
            string num;
            bool hasDot = false;
            while (i < expr.size() &&
                   (isdigit(expr[i]) || (expr[i] == '.' && !hasDot)))
            {
                if (expr[i] == '.')
                    hasDot = true;
                num += expr[i++];
            }
            if (!num.empty() && num.back() == '.')
            {
                cerr << "Syntax error: invalid number '" << num << "'\n";
                exit(1);
            }
            tokens.push_back({TokenType::NUMBER, num});
            continue;
        }
        if (isalpha(c) || c == '_')
        {
            string id;
            while (i < expr.size() && (isalnum(expr[i]) || expr[i] == '_'))
                id += expr[i++];
            tokens.push_back({TokenType::VARIABLE, id});
            continue;
        }
        if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            tokens.push_back({TokenType::OPERATOR, string(1, c)});
            ++i;
            continue;
        }
        if (c == '(' || c == '[' || c == '{')
        {
            tokens.push_back({TokenType::LPAREN, string(1, c)});
            ++i;
            continue;
        }
        if (c == ')' || c == ']' || c == '}')
        {
            tokens.push_back({TokenType::RPAREN, string(1, c)});
            ++i;
            continue;
        }
        cerr << "Syntax error: unexpected character '" << c << "'\n";
        exit(1);
    }
    return tokens;
}

int main()
{
    string expr;
    if (!getline(cin, expr))
    {
        cerr << "Syntax error: empty input\n";
        return 1;
    }
    vector<Token> tokens = tokenize(expr);
    cerr << "Tokenization successful. Token count: " << tokens.size() << "\n";
    return 0;
}