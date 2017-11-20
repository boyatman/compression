#include "book_level.h"

BookLevel::BookLevel(Price price, Number number, Quantity quantity) {
  std::get<0>(level_) = price;
  std::get<1>(level_) = number;
  std::get<2>(level_) = quantity;
}
