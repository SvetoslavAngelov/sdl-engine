#pragma once

#include "vertex.h"
#include <vector>

class Scene {
public:
    Scene() = default;
    explicit Scene(std::vector<vertex> vertices);

    std::vector<vertex> const& GetGeometry() const noexcept;

private:
    std::vector<vertex> vertices_;
};
