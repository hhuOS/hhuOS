#include "lib/file/FileStatus.h"
#include "Mathexpr.h"

int precedence(char op){
    if(op == '+'||op == '-')
    return 1;
    if(op == '%') 
    return 2;
    if(op == '*'||op == '/')
    return 3;
    return 0;
}
 
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

int Mathexpr::calculateExpression(String tokens){
    uint32_t i;
    Util::ArrayList<int> values;
    Util::ArrayList<char> ops;
     
    for(i = 0; i < tokens.length(); i++){
       
        if(tokens[i] == '('){
            ops.add(tokens[i]);
        }
       
        else if(tokens[i]>=48 && tokens[i]<=57){
            int val = 0;
            
            while(i < tokens.length() && tokens[i]>=48 && tokens[i]<=57){
                val = (val*10) + (tokens[i]-'0');
                i++;
            }
             
            values.add(val);
            i--;
        }
         
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
            
            if(!ops.isEmpty()) ops.remove(ops.size()-1);
        }
         
        
        else {
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
     
    
    while(!ops.isEmpty()){
        int val2 = values.get(values.size()-1);
        values.remove(values.size()-1);
                 
        int val1 = values.get(values.size()-1);
        values.remove(values.size()-1);
                 
        int op = ops.get(ops.size()-1);
        ops.remove(ops.size()-1);
                 
        values.add(applyOperation(val1, val2, op));
    }
    return values.get(values.size()-1);
}

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
    int ans = calculateExpression(parser.getUnnamedArguments()[0]);
    stdout << parser.getUnnamedArguments()[0] << ": " << ans << endl;
}

const String Mathexpr::getHelpText() {
    return "Executes the math function given as an input.\n\n"
           "Usage: mathexpr [EXPRESSION]... \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}