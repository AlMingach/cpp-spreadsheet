#pragma once

#include "common.h"
#include "formula.h"

#include <set>
#include <optional>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);

    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

    void ClearAllParents();

    void ClearCache();

    void SetParentsAndChild();

private:

    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    
    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;

    std::set<Cell*> parents_;
    std::set<Cell*> children_;

    void TestCycles(const std::vector<Position>& parents, const Cell *root);
    
};

class Cell::Impl {
public:

    virtual ~Impl() = default;

    virtual Value GetValue(const SheetInterface& sheet) = 0;

    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const {
        return {};
    }

    virtual void ClearCahce() {}
};

class Cell::EmptyImpl : public Cell::Impl {
public:
    EmptyImpl();

    ~EmptyImpl();

    Value GetValue(const SheetInterface& sheet) override;

    std::string GetText() const override;
};

class Cell::TextImpl : public Cell::Impl {
public:

    TextImpl(std::string text);

    ~TextImpl();

    Value GetValue(const SheetInterface& sheet) override;

    std::string GetText() const override;

private:
    std::string text_;
};

class Cell::FormulaImpl : public Cell::Impl {
public:

    FormulaImpl(std::string text);

    ~FormulaImpl() = default;

    Value GetValue(const SheetInterface& sheet) override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    void ClearCahce() override;

private:
    std::unique_ptr<FormulaInterface> formula_;
    std::optional<FormulaInterface::Value> cache_;
};