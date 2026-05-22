#include <iostream>
#include <string>
#include <vector>
#include <stack>
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
int precedence(const string &op)
{
    if (op == "*" || op == "/")
        return 2;
    if (op == "+" || op == "-")
        return 1;
    return 0;
}
char matchingOpen(char close)
{
    if (close == ')')
        return '(';
    if (close == ']')
        return '[';
    return '{';
}
vector<Token> toPostfix(const vector<Token> &tokens)
{
    vector<Token> output;
    stack<Token> ops;
    bool expectOperand = true;
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const Token &tok = tokens[i];
        if (tok.type == TokenType::NUMBER || tok.type == TokenType::VARIABLE)
        {
            if (!expectOperand)
            {
                cerr << "Syntax error: two consecutive operands near '"
                     << tok.value << "'\n";
                exit(1);
            }
            output.push_back(tok);
            expectOperand = false;
        }
        else if (tok.type == TokenType::OPERATOR)
        {
            if (expectOperand)
            {
                cerr << "Syntax error: operator '" << tok.value
                     << "' without left operand\n";
                exit(1);
            }
            while (!ops.empty() &&
                   ops.top().type == TokenType::OPERATOR &&
                   precedence(ops.top().value) >= precedence(tok.value))
            {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(tok);
            expectOperand = true;
        }
        else if (tok.type == TokenType::LPAREN)
        {
            if (!expectOperand)
            {
                cerr << "Syntax error: unexpected '" << tok.value << "'\n";
                exit(1);
            }
            ops.push(tok);
            expectOperand = true;
        }
        else if (tok.type == TokenType::RPAREN)
        {
            if (expectOperand)
            {
                cerr << "Syntax error: empty brackets near '"
                     << tok.value << "'\n";
                exit(1);
            }
            char expectedOpen = matchingOpen(tok.value[0]);
            bool found = false;
            while (!ops.empty())
            {
                if (ops.top().type == TokenType::LPAREN)
                {
                    if (ops.top().value[0] != expectedOpen)
                    {
                        cerr << "Syntax error: mismatched brackets '"
                             << ops.top().value << "' and '"
                             << tok.value << "'\n";
                        exit(1);
                    }
                    ops.pop();
                    found = true;
                    break;
                }
                output.push_back(ops.top());
                ops.pop();
            }
            if (!found)
            {
                cerr << "Syntax error: unmatched closing bracket '"
                     << tok.value << "'\n";
                exit(1);
            }
            expectOperand = false;
        }
    }
    if (expectOperand && !tokens.empty())
    {
        cerr << "Syntax error: expression ends with an operator\n";
        exit(1);
    }
    while (!ops.empty())
    {
        if (ops.top().type == TokenType::LPAREN)
        {
            cerr << "Syntax error: unmatched opening bracket '"
                 << ops.top().value << "'\n";
            exit(1);
        }
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
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
    vector<Token> postfix = toPostfix(tokens);
    for (size_t i = 0; i < postfix.size(); ++i)
    {
        if (i)
            cout << " ";
        cout << postfix[i].value;
    }
    cout << "\n";
    return 0;
}