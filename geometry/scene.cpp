#include "scene.h"

#include <utility>

Scene::Scene(std::vector<vertex> vertices) : vertices_{std::move(vertices)} {}

std::vector<vertex> const& Scene::GetGeometry() const noexcept {
    return vertices_;
}
