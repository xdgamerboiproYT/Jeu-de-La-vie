#pragma once
#include "IRule.hpp"
#include "Grid.hpp"

class ConwayRule : public IRule {
public:
    std::unique_ptr<CellState> calculerEtat(const Cell& cell, const Grid& grid) const override;
};