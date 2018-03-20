#include "Constants.h"

bool Tar::Header::operator!=(const Tar::Header &other) const {

    return this != &other;
}
