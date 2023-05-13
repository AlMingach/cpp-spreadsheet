#include "cell.h"
#include "sheet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <variant>

using namespace std::literals;

Cell::EmptyImpl::EmptyImpl() = default;

Cell::EmptyImpl::~EmptyImpl() = default;

Cell::Value Cell::EmptyImpl::GetValue(const SheetInterface&) {
    return ""s;
}

std::string Cell::EmptyImpl::GetText() const {
    return ""s;
}

Cell::TextImpl::TextImpl(std::string text) : text_(text)
{
}

Cell::TextImpl::~TextImpl() = default;

Cell::Value Cell::TextImpl::GetValue(const SheetInterface&) {
    if (text_.at(0) == ESCAPE_SIGN) {
        return text_.substr(1);
    }
    return text_;
}

std::string Cell::TextImpl::GetText() const {
    return text_;
}

Cell::FormulaImpl::FormulaImpl(std::string text) : formula_(ParseFormula(text))
{
}

Cell::Value Cell::FormulaImpl::GetValue(const SheetInterface& sheet) {
    if (!cache_) {
        cache_ = formula_->Evaluate(sheet);
    }
    return std::visit([](auto val) {
        return CellInterface::Value(val);
    }, cache_.value());
}

std::string Cell::FormulaImpl::GetText() const {
    return FORMULA_SIGN + formula_->GetExpression();
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
    return formula_->GetReferencedCells();
}

void Cell::FormulaImpl::ClearCahce() {
    cache_.reset();
}

Cell::Cell(Sheet &sheet) : impl_(std::make_unique<EmptyImpl>())
    , sheet_(sheet)
{
}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (text == impl_->GetText()) {
        return;
    }
    auto size = text.size();
    if (size > 0) {
        if (size > 1 && text.at(0) == FORMULA_SIGN) {
            auto temp_impl = std::make_unique<FormulaImpl>(text.substr(1));
            TestCycles(temp_impl->GetReferencedCells(), this);
            impl_ = std::move(temp_impl);
        } else {
            impl_ = std::make_unique<TextImpl>(text);
        }
    } else {
        impl_ = std::make_unique<EmptyImpl>();
    }
    ClearAllParents();
    SetParentsAndChild();
    ClearCache();
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
    ClearAllParents();
    ClearCache();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue(sheet_);
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return !children_.empty();
}

void Cell::TestCycles(const std::vector<Position>& parents, const Cell *root) {
    for (const auto& parent_pos : parents) {
        auto parent = dynamic_cast<Cell*>(sheet_.GetCell(parent_pos));
        if (parent) {
            if (parent == root) {
                throw CircularDependencyException("Cycle"s);
            }
            parent->TestCycles(parent->GetReferencedCells(), root);
        }
    }
}

void Cell::ClearAllParents() {
    for (auto parent : parents_) {
        parent->children_.erase(this);
    }
    parents_.clear();
}

void Cell::ClearCache() {
    impl_->ClearCahce();
    for (auto child : children_) {
        child->ClearCache();
    }
}

void Cell::SetParentsAndChild() {
    for (const auto &parent_pos : GetReferencedCells()) {
        if (sheet_.GetCell(parent_pos) == nullptr) {
            sheet_.SetCell(parent_pos, ""s);
        }
        auto parent_cell = static_cast<Cell*>(sheet_.GetCell(parent_pos));
        parent_cell->children_.insert(this);
        parents_.insert(parent_cell);
    }
}
