#include "DemoPolygonFactory.h"

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector2.h"
#include "DemoPolygon.h"

const Util::Array<Util::Math::Vector2<float>> DemoPolygonFactory::shape1 = {{-1, -1}, {0, 1.0}, {1,   -1}};
const Util::Array<Util::Math::Vector2<float>> DemoPolygonFactory::shape2 = {{0,  1}, {1,  0}, {0,  -1}, {-1, 0}};
const Util::Array<Util::Math::Vector2<float>> DemoPolygonFactory::shape3 = {{0,     1}, {1,     0.25}, {0.75,  -1}, {-0.75, -1}, {-1,    0.25}};
const Util::Array<Util::Math::Vector2<float>> DemoPolygonFactory::shape4 = {{-0.5, 1}, {0.5,  1}, {1,    0}, {0.5,  -1}, {-0.5, -1}, {-1,   0}};
const Util::Array<Util::Math::Vector2<float>> DemoPolygonFactory::shape5 = {{0,     1}, {0.75,  0.75}, {1,     -0.25}, {0.5,   -1}, {-0.5,  -1}, {-1,    -0.25}, {-0.75, 0.75}};

const Util::Array<const Util::Array<Util::Math::Vector2<float>>*> DemoPolygonFactory::shapes = Util::Array<const Util::Array<Util::Math::Vector2<float>>*>({
      &shape1, &shape2, &shape3, &shape4, &shape5
});

DemoPolygon* DemoPolygonFactory::createPolygon() {
    const auto shape = random.getRandomNumber(0, shapes.length() - 1);
    const auto initialScaleFactor = random.getRandomNumber() + 0.1;
    const auto rotationSpeed = static_cast<int32_t>(random.getRandomNumber(0, 360)) - 180;
    const auto scaleSpeed = random.getRandomNumber();
    const auto position = Util::Math::Vector2<float>(random.getRandomNumber() * 2 - 1.0, random.getRandomNumber() * 2 - 1.0);
    const auto color = Util::Graphic::Color(random.getRandomNumber(0, 255), random.getRandomNumber(0, 255), random.getRandomNumber(0, 255));

    auto *polygon = new DemoPolygon(*shapes[shape], position, color, initialScaleFactor, scaleSpeed, rotationSpeed);
    return polygon;
}