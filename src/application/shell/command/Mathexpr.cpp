#include "lib/file/FileStatus.h"
#include "Mathexpr.h"

/* Getting the precendence of the operator in order to know which one to evaluate first */
int precedence(char op){
    if(op == '+'||op == '-')
    return 1;
    if(op == '%') 
    return 2;
    if(op == '*'||op == '/')
    return 3;
    return 0;
}

/* Function for calucating the result of two number on basis of the operator */ 
int applyOperation(int a, int b, char op){
    switch(op){
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
    }
    return 0;
}

Mathexpr::Mathexpr(Shell &shell) : Command(shell) {

};

/* Function for calculating the expression entered by the user by using infix, postfix conversions */
int Mathexpr::calculateExpression(String tokens){
    uint32_t i;
    Util::ArrayList<int> values; /* List for the storing values */
    Util::ArrayList<char> ops; /* List for storing the operators */
     
    for(i = 0; i < tokens.length(); i++){
        
        /* Check for '(' bracket */
        if(tokens[i] == '('){
            ops.add(tokens[i]);
        }

        /* Check if the current token is number or not */
        else if(tokens[i]>=48 && tokens[i]<=57){
            int val = 0;
            
            /* Checks for the more than 1 digit number and adds to the value list */
            while(i < tokens.length() && tokens[i]>=48 && tokens[i]<=57){
                val = (val*10) + (tokens[i]-'0');
                i++;
            }
             
            values.add(val);
            i--;
        }
         
        /* If closing bracket is encountered then solve the entire brace */ 
        else if(tokens[i] == ')') {
            while(!ops.isEmpty() && ops.get(ops.size()-1) != '(') {
                int val2 = values.get(values.size()-1);
                values.remove(values.size()-1);
                 
                int val1 = values.get(values.size()-1);
                values.remove(values.size()-1);
                 
                char op = ops.get(ops.size()-1);
                ops.remove(ops.size()-1);
                 
                values.add(applyOperation(val1, val2, op));
            }
            
            /* Remove the opening brace from the list */
            if(!ops.isEmpty()) ops.remove(ops.size()-1);
        }
         
        /*Else the current token is the operator */
        else {
            /* On basis of the precedence and other factors calculate the expression */
            while(!ops.isEmpty() && precedence(ops.get(ops.size()-1)) >= precedence(tokens[i])){
                int val2 = values.get(values.size()-1);
                values.remove(values.size()-1);
                 
                int val1 = values.get(values.size()-1);
                values.remove(values.size()-1);
                 
                int op = ops.get(ops.size()-1);
                ops.remove(ops.size()-1);
                 
                values.add(applyOperation(val1, val2, op));
            }
             
            
            ops.add(tokens[i]);
        }
    }
     
    /* The expression has been calculated, now we only need to calculate the remaining ops to the values */
    while(!ops.isEmpty()){
        int val2 = values.get(values.size()-1);
        values.remove(values.size()-1);
                 
        int val1 = values.get(values.size()-1);
        values.remove(values.size()-1);
                 
        int op = ops.get(ops.size()-1);
        ops.remove(ops.size()-1);
                 
        values.add(applyOperation(val1, val2, op));
    }
    
    /* Returning the evaluted value of the expression */
    return values.get(values.size()-1);
}

void Mathexpr::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    /* Parser checks for the error if any on the basis of the command entered by the user */
    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    /* Check whether more than one unamed arguments are given by the user and display the appropriate message it they do */
    if(parser.getUnnamedArguments().length()>1){
        stderr << args[0] << ": Invalid command" << endl;
        return;
    }
    /* Calcualting the expression */
    int ans = calculateExpression(parser.getUnnamedArguments()[0]);
    stdout << parser.getUnnamedArguments()[0] << ": " << ans << endl;
}

/* Function for the help flags (-h or --help) */
const String Mathexpr::getHelpText() {
    return "Executes the math function given as an input.\n\n"
           "Usage: mathexpr [EXPRESSION]... \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}