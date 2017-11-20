#include "market_data_snapshot.h"

#include <string>
#include <sstream>

MarketDataSnapshot::MarketDataSnapshot() {
  bid_levels_.resize(10, BookLevel(0.0, 0, 0));
  ask_levels_.resize(10, BookLevel(0.0, 0, 0));
}

std::ostream &operator<<(std::ostream &os, MarketDataSnapshot const &mds) {
  std::stringstream ss;

  ss << mds.GetTimestamp() << " ";
  ss << mds.GetMarketDataKey() << " ";

  ss << mds.GetBidBitset();
  const std::vector<BookLevel>& blevels = mds.GetBidLevels();
  for(size_t i = 0;i < 10;++i) {
    const BookLevel& level = blevels[i];

    ss << " " << level.GetNumber();
    ss << " " << level.GetPrice();
    ss << " " << level.GetQuantity();
  }

  ss << " " << mds.GetAskBitset();
  const std::vector<BookLevel>& alevels = mds.GetAskLevels();
  for(size_t i = 0;i < 10;++i) {
    const BookLevel& level = alevels[i];

    ss << " " << level.GetNumber();
    ss << " " << level.GetPrice();
    ss << " " << level.GetQuantity();
  }
  return os << ss.str();
}
