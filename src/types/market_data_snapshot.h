#pragma once

#include <vector>
#include <bitset>

#include "book_level.h"

class Archiver;

class MarketDataSnapshot {
	friend class Archiver;

public:
	MarketDataSnapshot();
	~MarketDataSnapshot() = default;

	const std::string& GetMarketDataKey() const {
		return mdk_;
	}

	std::bitset<10> GetAskBitset() const {
		return ask_bitset_;
	}

	std::bitset<10> GetBidBitset() const {
		return bid_bitset_;
	}

	Timestamp GetTimestamp() const {
		return timestamp_;
	}

	const std::vector<BookLevel>& GetBidLevels() const {
		return bid_levels_;
	}

	const std::vector<BookLevel>& GetAskLevels() const {
		return ask_levels_;
	}

private:
	/* Timestamp */
	Timestamp timestamp_ = 0;

	/* Market data key */
	MarketDataKey mdk_;

	/* Bitset */
	std::bitset<10> bid_bitset_;
	std::bitset<10> ask_bitset_;

	/* All levels */
	std::vector<BookLevel> bid_levels_;
	std::vector<BookLevel> ask_levels_;

	void SetMarketDataKey(const std::string& mdk) {
		mdk_ = mdk;
	}

	void SetAskBitset(std::bitset<10> bitset) {
		ask_bitset_ = bitset;
	}

	void SetBidBitset(std::bitset<10> bitset) {
		bid_bitset_ = bitset;
	}

	void SetTimestamp(Timestamp ts) {
		timestamp_ = ts;
	}

	/* For Archiver or similar friends */
	std::vector<BookLevel>& GetBidLevels() {
		return bid_levels_;
	}

	std::vector<BookLevel>& GetAskLevels() {
		return ask_levels_;
	}
};

std::ostream &operator<<(std::ostream &os, MarketDataSnapshot const &mds);
