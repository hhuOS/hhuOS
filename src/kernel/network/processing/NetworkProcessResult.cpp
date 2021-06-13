//
// Created by hannes on 13.06.21.
//

#include "NetworkProcessResult.h"

NetworkProcessResult::NetworkProcessResult(NetworkProcessResult::ResultType type, String *message) {
    this->type=type;
    this->message=message;
}

NetworkProcessResult::ResultType NetworkProcessResult::getType() const {
    return type;
}

String *NetworkProcessResult::getMessage() const {
    return message;
}
