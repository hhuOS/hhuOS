#include "lib/file/FileStatus.h"
#include "Mathexpr.h"

// #include "LinkedBlockingStack.h"

double applyOp(double a, double b, char op);
int precedence(char op);

Mathexpr::Mathexpr(Shell &shell) : Command(shell) {

};

double Mathexpr::calculateExpression(String tokens){
    uint32_t i;
    // stack to store integer values.
    Util::ArrayList<double> values;
    Util::ArrayList<char> ops;

    // Util::LinkedBlockingStack<char> stk;
     
    for(i = 0; i < tokens.length(); i++){
         
        // Current token is a whitespace,
        // skip it.
        if(tokens[i] == ' ')
            continue;
         
        // Current token is an opening 
        // brace, push it to 'ops'
        else if(tokens[i] == '('){
            ops.add(tokens[i]);
        }
         
        // Current token is a number, push 
        // it to stack for numbers.
        else if(tokens[i]>=48 && tokens[i]<=57){
            double val = 0;
             
            // There may be more than one
            // digits in number.
            while(i < tokens.length() && tokens[i]>=48 && tokens[i]<=57){
                val = (val*10) + (tokens[i]-'0');
                i++;
            }
             
            values.add(val);
             
            // right now the i points to 
            // the character next to the digit,
            // since the for loop also increases 
            // the i, we would skip one 
            //  token position; we need to 
            // decrease the value of i by 1 to
            // correct the offset.
              i--;
        }
         
        // Closing brace encountered, solve 
        // entire brace.
        else if(tokens[i] == ')')
        {
            while(!ops.isEmpty() && ops.get(0) != '(')
            {
                double val2 = values.get(0);
                values.remove(values.size()-1);
                 
                double val1 = values.get(0);
                values.remove(values.size()-1);
                 
                char op = ops.get(0);
                ops.remove(ops.size()-1);
                 
                values.add(applyOp(val1, val2, op));
            }
             
            // pop opening brace.
            if(!ops.isEmpty())
               ops.remove(ops.size()-1);
        }
         
        // Current token is an operator.
        else
        {
            // While top of 'ops' has same or greater 
            // precedence to current token, which
            // is an operator. Apply operator on top 
            // of 'ops' to top two elements in values stack.
            while(!ops.isEmpty() && precedence(ops.get(0))
                                >= precedence(tokens[i])){
                double val2 = values.get(0);
                values.remove(values.size()-1);
                 
                double val1 = values.get(0);
                values.remove(values.size()-1);
                 
                char op = ops.get(0);
                ops.remove(ops.size()-1);
                 
                values.add(applyOp(val1, val2, op));
            }
             
            // Push current token to 'ops'.
            ops.add(tokens[i]);
        }
    }
     
    // Entire expression has been parsed at this
    // point, apply remaining ops to remaining
    // values.
    while(!ops.isEmpty()){
        double val2 = values.get(0);
        values.remove(values.size()-1);
                 
        double val1 = values.get(0);
        values.remove(values.size()-1);
                 
        char op = ops.get(0);
        ops.remove(ops.size()-1);
                 
        values.add(applyOp(val1, val2, op));
    }
     
    // Top of 'values' contains result, return it.
    return values.get(0);
}

int precedence(char op){
    if(op == '+'||op == '-')
    return 1;
    if(op == '*'||op == '/')
    return 3;
    return 0;
}
 
// Function to perform arithmetic operations.
double applyOp(double a, double b, char op){
    switch(op){
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
    return 0;
}

// https://www.geeksforgeeks.org/convert-floating-point-number-string/

void Mathexpr::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    if(parser.getUnnamedArguments().length()>1){
        stderr << args[0] << ": Invalid command" << endl;
        return;
    }
    stdout << calculateExpression(parser.getUnnamedArguments()[0]) << endl;
}

const String Mathexpr::getHelpText() {
    return "Executes the math function given as an input.\n\n"
           "Usage: mkdir [EXPRESSION]... \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}