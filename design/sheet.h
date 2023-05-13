#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <map>
#include <memory>

std::ostream& operator<<(std::ostream& output, const CellInterface::Value& val);

class Sheet : public SheetInterface {
public:
    ~Sheet() = default;

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:

    class Row;

    std::map<int, Row> sheet_;

    void PrintEmptyRow(std::ostream &output, int count) const;

};

class Sheet::Row {
public:

    Row() = default;

    ~Row() = default;

    Row& SetCellToRow(int pos, std::string text);

    const CellInterface* GetCell(int pos) const;

    CellInterface* GetCell(int pos);

    void ClearCell(int pos);
    
    bool IsEmpty();

    int GetRowPrintableSize() const;

    void PrintValues(std::ostream& output, int size) const;

    void PrintTexts(std::ostream& output, int size) const;

private:
    std::map<int, std::unique_ptr<CellInterface>> row_;
};