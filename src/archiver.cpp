#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>

#include "archiver.h"
#include "archiver.h"
#include "types/types.h"

Archiver::Archiver(const std::string& ifile, const std::string& ofile) : ifile_(ifile), ofile_(ofile), mds_() {
}

Archiver::ARCHIVER_RETURN_CODE Archiver::archive() {
	std::fstream isfile;
	isfile.open(ifile_, std::ios::in);

	if(!isfile.is_open()) {
		return Archiver::ARCHIVER_RETURN_CODE::INPUT_FILE_NOT_FOUND;
	}

	std::fstream osfile_data;
	std::string ofile_data_name = ofile_ + ".data.dat";
	osfile_data.open(ofile_data_name, std::ios::out | std::ios::binary | std::ios::trunc);
	if(!osfile_data.is_open()) {
		return Archiver::ARCHIVER_RETURN_CODE::INPUT_FILE_NOT_FOUND;
	}

	std::string row;
	while(getline(isfile, row)) {
		std::vector<std::string> tokens;

	    std::string buffer;
	    std::stringstream ss(row);
	    while (ss >> buffer) {
	        tokens.push_back(buffer);
	    }

	    ++index_;
	    ProcessRow(tokens);

	    ArchiveRow(osfile_data);
	}

	std ::cout << "processed " << index_ << " records." << std::endl;

	std::fstream osfile_index;
	std::string ofile_index_name = ofile_ + ".index.dat";
	osfile_index.open(ofile_index_name, std::ios::out | std::ios::trunc);
	if(!osfile_index.is_open()) {
		return Archiver::ARCHIVER_RETURN_CODE::INPUT_FILE_NOT_FOUND;
	}

	osfile_index << index_ << "\n";

	isfile.close();
	osfile_data.close();
	osfile_index.close();

	return Archiver::ARCHIVER_RETURN_CODE::SUCCESSFUL;
}

Archiver::ARCHIVER_RETURN_CODE  Archiver::unarchive() {
	std::fstream isfile;
	std::string isfile_data_name = ofile_ + ".data.dat";
	isfile.open(isfile_data_name, std::ios::binary | std::ios::in);

	if(!isfile.is_open()) {
		return Archiver::ARCHIVER_RETURN_CODE::INPUT_FILE_NOT_FOUND;
	}

	std::fstream osfile_index;
	std::string ofile_index_name = ofile_ + ".index.dat";
	osfile_index.open(ofile_index_name, std::ios::in);

	if(!osfile_index.is_open()) {
		return Archiver::ARCHIVER_RETURN_CODE::INPUT_FILE_NOT_FOUND;
	}

	std::string data_size_string;
	getline(osfile_index, data_size_string);
	std::cout << data_size_string << std::endl;

	std::size_t data_size = static_cast<std::size_t>(stoi(data_size_string));


	std::fstream osfile_data;
	std::string ofile_data_name = ofile_ + ".ascii.txt";
	osfile_data.open(ofile_data_name, std::ios::out | std::ios::trunc);
	if(!osfile_data.is_open()) {
		return Archiver::ARCHIVER_RETURN_CODE::INPUT_FILE_NOT_FOUND;
	}

	for(size_t i = 0; i < data_size; ++i) {
		Timestamp ts;
		isfile.read(reinterpret_cast<char*>(&ts), sizeof(ts));
		mds_.SetTimestamp(ts);

		std::bitset<10> bbs;
		isfile.read(reinterpret_cast<char*>(&bbs), sizeof(std::bitset<10>));
		mds_.SetBidBitset(bbs);

		BookLevel level {0.0, 0, 0};
		std::vector<BookLevel>& bid_levels = mds_.GetBidLevels();
		for(size_t i = 0; i < 10; ++i) {
			if(!bbs.test(i)) continue;
			isfile.read(reinterpret_cast<char*>(&level), sizeof(BookLevel));

			bid_levels[i].SetPrice(level.GetPrice());
			bid_levels[i].SetNumber(level.GetNumber());
			bid_levels[i].SetQuantity(level.GetQuantity());
		}

		isfile.read(reinterpret_cast<char*>(&bbs), sizeof(std::bitset<10>));
		mds_.SetAskBitset(bbs);

		std::vector<BookLevel>& ask_levels = mds_.GetAskLevels();
		for(size_t i = 0; i < 10; ++i) {
			if(!bbs.test(i)) continue;

			isfile.read(reinterpret_cast<char*>(&level), sizeof(BookLevel));

			ask_levels[i].SetPrice(level.GetPrice());
			ask_levels[i].SetNumber(level.GetNumber());
			ask_levels[i].SetQuantity(level.GetQuantity());
		}
		std::cout << std::endl;
		osfile_data << mds_ << "\n";
	}

	isfile.close();
	osfile_data.close();
	return Archiver::ARCHIVER_RETURN_CODE::SUCCESSFUL;
}

void Archiver::ProcessSegment(std::size_t index,
		const std::bitset<10>& bitset,
		const std::vector<std::string>& row,
		std::vector<BookLevel>& levels) const {
	for (size_t i = 0; i < 10; ++i) {
		/* Base index */
		size_t base = index + 1 + (3 * i);
		BookLevel& level = levels[i];
		if (!bitset.test(i)) {
			if (level.GetNumber()
					!= static_cast<std::uint32_t>(stoi(row[base]))) {
				std::cout << "This row has an issue " << index_ << " on index "
						<< i << " Number(" << level.GetNumber() << ", "
						<< static_cast<std::uint32_t>(stoi(row[base])) << ")"
						<< std::endl;
			}
			if (level.GetQuantity()
					!= static_cast<std::uint32_t>(stoi(row[base + 2]))) {
				std::cout << "This row has an issue " << index_ << " on index "
						<< i << " Quantity(" << level.GetQuantity() << ", "
						<< static_cast<std::uint32_t>(stoi(row[base + 2]))
						<< ")" << std::endl;
			}
			if (level.GetPrice() - stod(row[base + 1]) > 0.0001) {
				std::cout << "This row has an issue " << index_ << " on index "
						<< i << " Price(" << level.GetPrice() << ", "
						<< stod(row[base + 1]) << ")" << std::endl;
			}
		}
		level.SetPrice(stod(row[base + 1]));
		level.SetNumber(static_cast<std::uint32_t>(stoi(row[base])));
		level.SetQuantity(static_cast<std::uint32_t>(stoi(row[base + 2])));
	}
}

void Archiver::ProcessRow(const std::vector<std::string>& row) const {
	mds_.SetTimestamp(stoull(row[0]));
	mds_.SetMarketDataKey(row[1]);

	/* Process Bid side */
	ProcessBidSide(row);

	/* Process Ask side */
	ProcessAskSide(row);
}

std::bitset<10> Archiver::ProcessBitset(const std::string& row) const {
	std::bitset<10> bits(row);
	return bits;
}

void Archiver::ArchiveRow(std::fstream& fs) const {
	auto ts = mds_.GetTimestamp();
	fs.write(reinterpret_cast<const char*>(&ts), sizeof(Timestamp));

	auto bbs = mds_.GetBidBitset();
	fs.write(reinterpret_cast<const char*>(&bbs), sizeof(std::bitset<10>));

	const std::vector<BookLevel>& blevels = mds_.GetBidLevels();
	for(size_t i = 0; i < 10; ++i) {
		if(!bbs.test(i)) continue;
		const BookLevel& level = blevels[i];
		fs.write(reinterpret_cast<const char*>(&level), sizeof(BookLevel));
	}

	auto abs = mds_.GetAskBitset();
	fs.write(reinterpret_cast<const char*>(&abs), sizeof(std::bitset<10>));

	const std::vector<BookLevel>& alevels = mds_.GetAskLevels();
	for(size_t i = 0; i < 10; ++i) {
		if(!abs.test(i)) continue;
		const BookLevel& level = alevels[i];
		fs.write(reinterpret_cast<const char*>(&level), sizeof(BookLevel));
	}
}
