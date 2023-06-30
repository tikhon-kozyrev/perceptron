/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#include "io/filewriter.hpp"

namespace IO {
	FileWriter::FileWriter() {
	}
	bool FileWriter::IsOpen() {
		return ofs.is_open();
	}
	bool FileWriter::Open(const std::string &filename) {
		bool res=false;
		do {
			if (IsOpen()) {
				break;
			}
			ofs.open(filename, std::ios::binary);
			res = IsOpen();
		} while (false);
		return res;
	}
	void FileWriter::Close() {
		do {
			if (!IsOpen()) {
				break;
			}
			ofs.close();
		} while (false);
	}
}
