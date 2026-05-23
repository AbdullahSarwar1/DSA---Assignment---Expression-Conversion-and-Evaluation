#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
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
                cerr << "Syntax error: unexpected '"
                     << tok.value << "'\n";
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
                    ops.pop(); // discard the open bracket
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
vector<string> collectVariables(const vector<Token> &tokens)
{
    vector<string> vars;
    map<string, bool> seen;
    for (const auto &t : tokens)
    {
        if (t.type == TokenType::VARIABLE && !seen[t.value])
        {
            vars.push_back(t.value);
            seen[t.value] = true;
        }
    }
    return vars;
}
double evaluate(const vector<Token> &postfix, const map<string, double> &varValues)
{
    stack<double> stk;
    for (const auto &tok : postfix)
    {
        if (tok.type == TokenType::NUMBER)
        {
            stk.push(stod(tok.value));
        }
        else if (tok.type == TokenType::VARIABLE)
        {
            auto it = varValues.find(tok.value);
            if (it == varValues.end())
            {
                cerr << "Runtime error: no value for variable '" << tok.value << "'\n";
                exit(2);
            }
            stk.push(it->second);
        }
        else if (tok.type == TokenType::OPERATOR)
        {
            if (stk.size() < 2)
            {
                cerr << "Runtime error: not enough operands for '" << tok.value << "'\n";
                exit(2);
            }
            double b = stk.top();
            stk.pop();
            double a = stk.top();
            stk.pop();
            if (tok.value == "+")
                stk.push(a + b);
            else if (tok.value == "-")
                stk.push(a - b);
            else if (tok.value == "*")
                stk.push(a * b);
            else if (tok.value == "/")
            {
                if (b == 0.0)
                {
                    cerr << "Logical error: division by zero\n";
                    exit(3);
                }
                stk.push(a / b);
            }
        }
    }
    if (stk.size() != 1)
    {
        cerr << "Runtime error: malformed expression (leftover operands)\n";
        exit(2);
    }
    return stk.top();
}
bool isBlankExpression(const string &expr)
{
    for (char c : expr)
        if (!isspace(static_cast<unsigned char>(c)))
            return false;
    return true;
}
int main()
{
    string expr;
    if (!getline(cin, expr))
    {
        cerr << "Syntax error: empty input\n";
        return 1;
    }
    if (expr.empty() || isBlankExpression(expr))
    {
        cerr << "Syntax error: empty or blank expression\n";
        return 1;
    }
    vector<Token> tokens = tokenize(expr);
    if (tokens.empty())
    {
        cerr << "Syntax error: no valid tokens found\n";
        return 1;
    }
    vector<Token> postfix = toPostfix(tokens);
    vector<string> vars = collectVariables(tokens);
    map<string, double> varValues;
    for (const auto &v : vars)
    {
        cerr << "Enter value for " << v << ": ";
        double val;
        if (!(cin >> val))
        {
            cerr << "\nRuntime error: invalid numeric input for '" << v << "'\n";
            return 2;
        }
        cerr << "\n";
        varValues[v] = val;
    }
    for (size_t i = 0; i < postfix.size(); ++i)
    {
        if (i)
            cout << " ";
        cout << postfix[i].value;
    }
    cout << "\n";
    double result = evaluate(postfix, varValues);
    if (result == static_cast<long long>(result))
        cout << static_cast<long long>(result) << "\n";
    else
        cout << result << "\n";
    return 0;
}