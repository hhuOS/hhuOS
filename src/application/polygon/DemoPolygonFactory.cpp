#include "DemoPolygonFactory.h"

const Util::Data::Array<double> DemoPolygonFactory::xShape1 = Util::Data::Array<double>({0, 0.5, 1});
const Util::Data::Array<double> DemoPolygonFactory::yShape1 = Util::Data::Array<double>({0, 1, 0});
const Util::Data::Array<double> DemoPolygonFactory::xShape2 = Util::Data::Array<double>({0, 1, 0, -1});
const Util::Data::Array<double> DemoPolygonFactory::yShape2 = Util::Data::Array<double>({1, 0, -1, 0});
const Util::Data::Array<double> DemoPolygonFactory::xShape3 = Util::Data::Array<double>({0, 1, 0.75, -0.75, -1});
const Util::Data::Array<double> DemoPolygonFactory::yShape3 = Util::Data::Array<double>({1, 0.25, -1, -1, 0.25});
const Util::Data::Array<double> DemoPolygonFactory::xShape4 = Util::Data::Array<double>({-0.5, 0.5, 1, 0.5, -0.5, -1});
const Util::Data::Array<double> DemoPolygonFactory::yShape4 = Util::Data::Array<double>({1, 1, 0, -1, -1, 0});
const Util::Data::Array<double> DemoPolygonFactory::xShape5 = Util::Data::Array<double>({0, 0.75, 1, 0.5, -0.5, -1, -0.75});
const Util::Data::Array<double> DemoPolygonFactory::yShape5 = Util::Data::Array<double>({1, 0.75, -0.25, -1, -1, -0.25, 0.75});

const Util::Data::Array<const Util::Data::Array<double>*> DemoPolygonFactory::xShapes = Util::Data::Array<const Util::Data::Array<double>*>({
      &xShape1, &xShape2, &xShape3, &xShape4, &xShape5
});

const Util::Data::Array<const Util::Data::Array<double>*> DemoPolygonFactory::yShapes = Util::Data::Array<const Util::Data::Array<double>*>({
      &yShape1, &yShape2, &yShape3, &yShape4, &yShape5
});

DemoPolygon DemoPolygonFactory::createPolygon() {
    const auto shape = random.nextRandomNumber() % xShapes.length();
    const auto scaleFactor = random.nextRandomNumber() / (2.0 * random.getBoundary()) + 0.1;
    const auto rotationSpeed = random.nextRandomNumber() / (0.5 * random.getBoundary()) - 1.0;
    const auto scaleSpeed = random.nextRandomNumber() / (1.0 * random.getBoundary());
    const auto translateX = random.nextRandomNumber() / (0.5 * random.getBoundary()) - 1.0;
    const auto translateY = random.nextRandomNumber() / (0.5 * random.getBoundary()) - 1.0;
    const auto color = Util::Graphic::Color(random.nextRandomNumber(), random.nextRandomNumber(), random.nextRandomNumber());
    const auto &x = *xShapes[shape];
    const auto &y = *yShapes[shape];

    auto polygon = DemoPolygon(x, y, color, rotationSpeed, scaleSpeed);
    polygon.scale(scaleFactor);
    polygon.translate(translateX, translateY);
    return polygon;
}