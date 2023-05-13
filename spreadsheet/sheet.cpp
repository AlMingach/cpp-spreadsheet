#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, const CellInterface::Value& val) {
    std::visit([&output](auto& v) {
        output << v;
    }, val);
    return output;
}

Sheet::Row& Sheet::Row::SetCellToRow(int pos, std::string text, Sheet& sheet) {
    auto result = row_.emplace(pos, std::make_unique<Cell>(sheet));
    result.first->second->Set(text);
    return *this;
}

const CellInterface* Sheet::Row::GetCell(int pos) const {
    auto cell = row_.find(pos);
    if (cell == row_.end()) {
        return nullptr;
    }
    return cell->second.get();
}

CellInterface* Sheet::Row::GetCell(int pos) {
    auto cell = row_.find(pos);
    if (cell == row_.end()) {
        return nullptr;
    }
    return cell->second.get();
}

void Sheet::Row::ClearCell(int pos) {
    auto cell = row_.find(pos);
    if (cell != row_.end()) {
        cell->second->Clear();
        if (!cell->second->IsReferenced()) {
            row_.erase(cell);
        }
    }
}

bool Sheet::Row::IsEmpty() {
    return row_.empty();
}

int Sheet::Row::GetRowPrintableSize() const {
    if (row_.empty()) {
        return 0;
    }
    auto last = --row_.end();
    return last->first;
}

void Sheet::Row::PrintValues(std::ostream& output, int size) const {
    bool is_first = true;
    int prev_print = -1;
    for (auto& cell : row_) {
        for (int j = 1; j < cell.first - prev_print; ++j) {
            output << '\t';
        }
        if (!is_first) {
            output << '\t';
        }
        prev_print = cell.first;
        output << cell.second->GetValue();
        is_first = false;
    }
    for( ;prev_print < size - 1; ++prev_print) {
        output << '\t';
    }
    output << '\n';
}

void Sheet::Row::PrintTexts(std::ostream& output, int size) const {
    bool is_first = true;
    int prev_print = -1;
    for (auto& cell : row_) {
        for (int j = 1; j < cell.first - prev_print; ++j) {
            output << '\t';
        }
        if (!is_first) {
            output << '\t';
        }
        prev_print = cell.first;
        output << cell.second->GetText();
        is_first = false;
    }
    for( ;prev_print < size - 1; ++prev_print) {
        output << '\t';
    }
    output << '\n';
}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Mistake position");
    }
    auto& result = sheet_[pos.row];
    result.SetCellToRow(pos.col, text, *this);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Mistake position");
    }
    auto row = sheet_.find(pos.row);
    if (row == sheet_.end()) {
        return nullptr;
    }
    return row->second.GetCell(pos.col);
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Mistake position");
    }
    auto row = sheet_.find(pos.row);
    if (row == sheet_.end()) {
        return nullptr;
    }

    return row->second.GetCell(pos.col);
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Mistake position");
    }
    auto row = sheet_.find(pos.row);
    if (row != sheet_.end()) {
        row->second.ClearCell(pos.col);
        if (row->second.IsEmpty()) {
            sheet_.erase(row);
        }
    }
}

Size Sheet::GetPrintableSize() const {
    if (sheet_.empty()) {
        return {0, 0};
    }
    auto last = --sheet_.end();
    int rows = 0;
    int cols = last->first;
    for (auto& row : sheet_) {
        auto size = row.second.GetRowPrintableSize();
        if (rows < size) {
            rows = size;
        }
    }
    return {cols + 1, rows + 1};
}

void Sheet::PrintValues(std::ostream& output) const {
    Size size = GetPrintableSize();
    int prev_print = -1;
    for (auto& row : sheet_) {
        for (int j = 1; j < row.first - prev_print; ++j) {
            PrintEmptyRow(output, size.cols);
        }
        prev_print = row.first;
        row.second.PrintValues(output, size.cols);
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    Size size = GetPrintableSize();
    int prev_print = -1;
    for (auto& row : sheet_) {
        for (int j = 1; j < row.first - prev_print; ++j) {
            PrintEmptyRow(output, size.cols);
        }
        prev_print = row.first;
        row.second.PrintTexts(output, size.cols);
    }
}

void Sheet::PrintEmptyRow(std::ostream& output, int count) const {
    for (int i = 0; i < count - 1; ++i) {
        output << '\t';
    }
    output << '\n';
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}