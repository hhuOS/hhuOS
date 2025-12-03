#include "InstanceFactory.h"

#include "util/collection/HashMap.h"
#include "util/reflection/Prototype.h"

namespace Util {
namespace Reflection {

HashMap<String, Prototype*> InstanceFactory::prototypeTable;

}
}