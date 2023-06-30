/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#include "io/filereader.hpp"

namespace IO {
	FileReader::FileReader() {
	}
	bool FileReader::IsOpen() {
		return ifs.is_open();
	}
	bool FileReader::Open(const std::string &filename) {
		bool res=false;
		do {
			if (IsOpen()) {
				break;
			}
			ifs.open(filename, std::ios::binary);
			res = IsOpen();
		} while (false);
		return res;
	}
	void FileReader::Close() {
		do {
			if (!IsOpen()) {
				break;
			}
			ifs.close();
		} while (false);
	}
}
