#include "InstanceFactory.h"

#include "Prototype.h"

#include <util/collection/HashMap.h>

namespace Util {
namespace Reflection {

HashMap<String, Prototype*> InstanceFactory::prototypeTable;

}
}