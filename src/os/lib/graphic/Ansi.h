#ifndef __Ansi_include__
#define __Ansi_include__

class Ansi {

public:

    static constexpr const char *RESET = "\\u001b[0m";

    static constexpr const char *BLACK = "\\u001b[30m";

    static constexpr const char *RED = "\\u001b[31m";

    static constexpr const char *GREEN = "\\u001b[32m";

    static constexpr const char *YELLOW = "\\u001b[33m";

    static constexpr const char *BLUE = "\\u001b[34m";

    static constexpr const char *MAGENTA = "\\u001b[35m";

    static constexpr const char *CYAN = "\\u001b[36m";

    static constexpr const char *WHITE = "\\u001b[37m";

    static constexpr const char *BRIGHT_BLACK = "\\u001b[30;1m";

    static constexpr const char *BRIGHT_RED = "\\u001b[31;1m";

    static constexpr const char *BRIGHT_GREEN = "\\u001b[32;1m";

    static constexpr const char *BRIGHT_YELLOW = "\\u001b[33;1m";

    static constexpr const char *BRIGHT_BLUE = "\\u001b[34;1m";

    static constexpr const char *BRIGHT_MAGENTA = "\\u001b[35;1m";

    static constexpr const char *BRIGHT_CYAN = "\\u001b[36;1m";

    static constexpr const char *BRIGHT_WHITE = "\\u001b[37;1m";

    static const char ESCAPE_END = 'm';

};

#endif
