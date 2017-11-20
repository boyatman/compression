#pragma once

#include <string>
#include <cinttypes>

/* They all need to move as classes */
typedef double Price;
typedef uint32_t Number;
typedef uint32_t Quantity;
typedef std::uint64_t Timestamp;
typedef std::string MarketDataKey;

enum class Side {
  BUY = 0, SELL
};
