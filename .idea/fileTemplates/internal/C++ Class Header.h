#parse("C File Header.h")
#[[#ifndef]]# ${INCLUDE_GUARD}
#[[#define]]# ${INCLUDE_GUARD}

${NAMESPACES_OPEN}

class ${NAME} {

public:
    /**
     * Default Constructor.
     */
    ${NAME}() = default;

    /**
     * Copy Constructor.
     */
    ${NAME}(const ${NAME} &other) = delete;

    /**
     * Assignment operator.
     */
    ${NAME} &operator=(const ${NAME} &other) = delete;

    /**
     * Destructor.
     */
    ~${NAME}() = default;
};

${NAMESPACES_CLOSE}

#[[#endif]]#
