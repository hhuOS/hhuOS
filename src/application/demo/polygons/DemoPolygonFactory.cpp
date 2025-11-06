#include "DemoPolygonFactory.h"

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector2.h"
#include "DemoPolygon.h"

const Util::Array<Util::Math::Vector2<double>> DemoPolygonFactory::shape1 = {{0,   0}, {0.5, 1}, {1,   0}};
const Util::Array<Util::Math::Vector2<double>> DemoPolygonFactory::shape2 = {{0,  1}, {1,  0}, {0,  -1}, {-1, 0}};
const Util::Array<Util::Math::Vector2<double>> DemoPolygonFactory::shape3 = {{0,     1}, {1,     0.25}, {0.75,  -1}, {-0.75, -1}, {-1,    0.25}};
const Util::Array<Util::Math::Vector2<double>> DemoPolygonFactory::shape4 = {{-0.5, 1}, {0.5,  1}, {1,    0}, {0.5,  -1}, {-0.5, -1}, {-1,   0}};
const Util::Array<Util::Math::Vector2<double>> DemoPolygonFactory::shape5 = {{0,     1}, {0.75,  0.75}, {1,     -0.25}, {0.5,   -1}, {-0.5,  -1}, {-1,    -0.25}, {-0.75, 0.75}};

const Util::Array<const Util::Array<Util::Math::Vector2<double>>*> DemoPolygonFactory::shapes = Util::Array<const Util::Array<Util::Math::Vector2<double>>*>({
      &shape1, &shape2, &shape3, &shape4, &shape5
});

DemoPolygon* DemoPolygonFactory::createPolygon() {
    const auto shape = 1; // random.getRandomNumber(0, shapes.length() - 1);
    const auto initialScaleFactor = random.getRandomNumber() + 0.1;
    const auto rotationSpeed = random.getRandomNumber() - 1.0;
    const auto scaleSpeed = random.getRandomNumber();
    const auto position = Util::Math::Vector2<double>(random.getRandomNumber() * 2 - 1.0, random.getRandomNumber() * 2 - 1.0);
    const auto color = Util::Graphic::Color(random.getRandomNumber(0, 255), random.getRandomNumber(0, 255), random.getRandomNumber(0, 255));

    auto *polygon = new DemoPolygon(*shapes[shape], position, color, initialScaleFactor, scaleSpeed, rotationSpeed);
    return polygon;
}