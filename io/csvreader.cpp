/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#include "io/csvreader.hpp"
#include <sstream>

namespace IO {
	CSVReader::CSVReader() {
	}
	bool CSVReader::Open(const std::string &filename) {
		ifs.open(filename, std::ios::in);
		bool res = ifs.is_open();
		return res;
	}
	void CSVReader::Close() {
		ifs.close();
	}
	bool CSVReader::ReadRow(std::vector<std::string> &row, char splitter) {
		std::string line;
		bool res = false;
		do {
			row.clear();
			if (!std::getline(ifs, line)) {
				break;
			}
			res = true;
			std::string field;
			std::istringstream iss(line);
			while (std::getline(iss, field, splitter)) {
				row.push_back(field);
			}
		} while (false);
		return res;
	}
}
