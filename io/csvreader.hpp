/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef IO_CSVREADER_HPP
#define IO_CSVREADER_HPP

#include <fstream>
#include <vector>

namespace IO {
	class CSVReader {
		public:
			CSVReader();
			bool Open(const std::string &filename);
			void Close();
			bool ReadRow(std::vector<std::string> &row, char splitter);
		private:
			std::ifstream ifs;
	};
}

#endif
