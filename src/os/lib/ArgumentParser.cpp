#include "ArgumentParser.h"

ArgumentParser::ArgumentParser(const String helpText, uint32_t startAt) :
        helpText(helpText), startIndex(startAt) {

}

void ArgumentParser::addParameter(const String &name, const String &abbreviation, bool required) {
    parameters.add(name);
    abbreviationMap.put(abbreviation, name);

    if(required) {
        requiredParameters.add(name);
    }
}

void ArgumentParser::addSwitch(const String &name, const String &abbreviation) {
    switches.add(name);
    abbreviationMap.put(abbreviation, name);
}

const String& ArgumentParser::getErrorString() {
    return errorString;
}

bool ArgumentParser::parse(Util::Array<String> &arguments) {
    namedArguments.clear();
    switchMap.clear();
    unnamedArguments.clear();

    for (uint32_t i = startIndex; i < arguments.length(); i++) {
        String &currentArg = arguments[i];

        if (!currentArg.beginsWith("-") || currentArg == "-") {
            unnamedArguments.add(currentArg);
        } else if(currentArg == "-h" || currentArg =="--help") {
            errorString = helpText;

            return false;
        } else {
            if (abbreviationMap.containsKey(currentArg.substring(1, currentArg.length()))) {
                currentArg = abbreviationMap.get(currentArg.substring(1, currentArg.length()));
            } else {
                if(currentArg.beginsWith("--")) {
                    currentArg = currentArg.substring(2, currentArg.length());
                } else {
                    currentArg = currentArg.substring(1, currentArg.length());
                }
            }

            if(requiredParameters.contains(currentArg)) {
                requiredParameters.remove(currentArg);
            }

            if (parameters.contains(currentArg)) {
                if (i < arguments.length() - 1) {
                    namedArguments.put(currentArg, arguments[++i]);
                } else {
                    errorString = "No value given for parameter '" + currentArg + "'!";

                    return false;
                }
            } else if (switches.contains(currentArg)) {
                switchMap.put(currentArg, true);
            } else {
                errorString = "Unknown parameter '" + currentArg + "'!";

                return false;
            }
        }
    }

    if(!requiredParameters.isEmpty()) {
        errorString = "Missing required parameters: ";

        for(const String &name : requiredParameters) {
            errorString += "'" + name + "' ";
        }

        return false;
    }

    return true;
}

const String ArgumentParser::getNamedArgument(const String &name) {
    if(namedArguments.containsKey(name)) {
        return namedArguments.get(name);
    }

    return "";
}

Util::Array<String> ArgumentParser::getUnnamedArguments() {
    return unnamedArguments.toArray();
}

bool ArgumentParser::checkSwitch(const String &name) {
    if(switchMap.containsKey(name)) {
        return switchMap.get(name);
    }

    return false;
}


