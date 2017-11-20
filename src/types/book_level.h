#pragma once

#include <tuple>

#include "types.h"

class Archiver;

class BookLevel {
  friend class Archiver;

public:
  ~BookLevel() = default;

  BookLevel(Price price, Number number, Quantity quantity);

  Price GetPrice() const {
    return std::get<0>(level_);
  }

  Number GetNumber() const {
    return std::get<1>(level_);
  }

  Quantity GetQuantity() const {
    return std::get<2>(level_);
  }

private:

  std::tuple<Price, Number, Quantity> level_;

  void SetPrice(Price price) {
    std::get<0>(level_) = price;
  }

  void SetNumber(Number number) {
    std::get<1>(level_) = number;
  }

  void SetQuantity(Quantity quantity) {
    std::get<2>(level_) = quantity;
  }
};
