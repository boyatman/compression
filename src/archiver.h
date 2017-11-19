#pragma once

#include <bitset>
#include <string>
#include <vector>

#include "types/market_data_snapshot.h"

class Archiver {
	enum class ARCHIVER_RETURN_CODE {
		SUCCESSFUL = 1,
		INPUT_FILE_NOT_FOUND,
		OUTPUT_FILE_ALREADY_EXISTS
	};

public:
	Archiver(const std::string& ifile, const std::string& ofile);
	~Archiver() = default;

	ARCHIVER_RETURN_CODE archive();

	ARCHIVER_RETURN_CODE unarchive();
private:
	std::size_t index_ = 0;

	std::string ifile_;
	std::string ofile_;

	mutable MarketDataSnapshot mds_;

	void ArchiveRow(std::fstream& fs) const;

	void ProcessRow(const std::vector<std::string>& row) const;

	std::bitset<10> ProcessBitset(const std::string& row) const;

	void ProcessBidSide(const std::vector<std::string>& row) const {
		std::size_t index = 2;
		std::vector<BookLevel>& levels = mds_.GetBidLevels();

		std::bitset<10> bitset = ProcessBitset(row[index]);
		mds_.SetBidBitset(bitset);

		ProcessSegment(index, bitset, row, levels);
	}

	void ProcessAskSide(const std::vector<std::string>& row) const {
		std::size_t index = 33;
		std::vector<BookLevel>& levels = mds_.GetAskLevels();

		std::bitset<10> bitset = ProcessBitset(row[index]);
		mds_.SetAskBitset(bitset);

		ProcessSegment(index, bitset, row, levels);
	}

	void ProcessSegment(std::size_t index,
			const std::bitset<10>& bitset,
			const std::vector<std::string>& row,
			std::vector<BookLevel>& levels) const;
};

