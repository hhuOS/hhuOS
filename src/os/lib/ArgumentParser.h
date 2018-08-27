#ifndef HHUOS_ARGPARSER_H
#define HHUOS_ARGPARSER_H

#include <lib/util/HashMap.h>
#include <lib/util/HashSet.h>
#include "String.h"

class ArgumentParser {

private:

    String helpText;
    uint32_t startIndex;

    String errorString;

    Util::ArrayList<String> parameters;
    Util::ArrayList<String> switches;
    Util::HashSet<String> requiredParameters;
    Util::HashMap<String, String> abbreviationMap;

    Util::HashMap<String, String> namedArguments;
    Util::HashMap<String, bool> switchMap;
    Util::ArrayList<String> unnamedArguments;

public:

    explicit ArgumentParser(String helpText, uint32_t startAt = 0);

    ArgumentParser(const ArgumentParser &copy) = delete;

    ~ArgumentParser() = default;

    void addParameter(const String &name, const String &abbreviation, bool required = false);

    void addSwitch(const String &name, const String &abbreviation, bool required = false);

    const String& getErrorString();

    bool parse(Util::Array<String> &arguments);

    Util::Array<String> getUnnamedArguments();

    const String getNamedArgument(const String &argument);

    bool checkSwitch(const String &name);
};

#endif
