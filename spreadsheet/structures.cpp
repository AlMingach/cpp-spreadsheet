#include "common.h"

#include <cctype>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <tuple>

namespace {
    template <typename InputIt>
    bool IsCorrectNumber(InputIt begin, InputIt end);

    template <typename InputIt>
    bool IsCorrectLetters(InputIt begin, InputIt end);

    std::string NumberToLetters(int number);

    template <typename InputIt>
    int LettersToNumber(InputIt begin, InputIt end);

    const int LETTERS = 26;
    const int MAX_POSITION_LENGTH = 17;
    const int MAX_POS_LETTER_COUNT = 3;
    const int MAX_POS_DIGITS_COUNT = 5;
}

const Position Position::NONE = {-1, -1};

bool Position::operator==(const Position rhs) const {
    return (this->col == rhs.col) && (this->row == rhs.row);
}

bool Position::operator<(const Position rhs) const {
    return std::tie(row, col) < std::tie(rhs.row, rhs.col);
}

bool Position::IsValid() const {
    return (col >= 0 && col < Position::MAX_COLS && row >= 0 && row < Position::MAX_ROWS);
}

std::string Position::ToString() const {
    if (IsValid()) {
        return NumberToLetters(col) + std::to_string(row + 1);
    }
    return {};
}

Position Position::FromString(std::string_view str) {

    if (str.size() > MAX_POSITION_LENGTH) {
        return NONE;
    }

    auto begin = str.begin();
    auto end = str.end();

    auto index = std::find_if(begin, end, [](const char ch) {
        return std::isdigit(ch);
    });


    if (!IsCorrectLetters(begin, index) || !IsCorrectNumber(index, end)) {
        return NONE;
    }

    int col = LettersToNumber(begin, index);

    int row = std::stoi(std::string(index, str.end())) - 1;

    Position result{row, col};

    return result.IsValid() ? result : NONE;

}

namespace {

    template <typename InputIt>
    bool IsCorrectNumber(InputIt begin, InputIt end) {

        int num_digits = end - begin;

        if (num_digits > 0 && num_digits <= MAX_POS_DIGITS_COUNT) {
            for (auto i = begin; i < end; ++i) {
                if (!std::isdigit(*i)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    template <typename InputIt>
    bool IsCorrectLetters(InputIt begin, InputIt end) {

        int num_letters = end - begin;

        if (num_letters > 0 && num_letters <= MAX_POS_LETTER_COUNT) {
            for (auto i = begin; i < end; ++i) {
                if (*i < 'A' || *i > 'Z') {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    std::string NumberToLetters(int number) {
        std::string result;
        if (number >= 0) {
            int num = number / 26;
            int remainder = number % 26;
            result = ('A' + remainder);
            if (num > 0) {
                result = NumberToLetters(num - 1) + result;
            }
        }
        return result;
    }

    template <typename InputIt>
    int LettersToNumber(InputIt begin, InputIt end) {
        int result = 0;
        int v = end - begin - 1;
        for (auto i = begin; i < end; ++i) {
            result += static_cast<int>(*i - 'A' + 1) * std::pow(LETTERS, v--);
        }
        return result - 1;
    }
}

bool Size::operator==(Size rhs) const {
    return cols == rhs.cols && rows == rhs.rows;
}