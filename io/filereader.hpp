/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef IO_FILEREADER_HPP
#define IO_FILEREADER_HPP

#include <fstream>

namespace IO {
	class FileReader {
		public:
			FileReader();
			bool IsOpen();
			bool Open(const std::string &filename);
			void Close();
			template <class T> bool Read(T &val) {
				bool res = false;
				do {
					if (!ifs.read(reinterpret_cast<char *>(&val), sizeof(val))) {
						break;
					}
					res = true;
				} while (false);
				return res;
			}
		private:
			std::ifstream ifs;
	};
}
#endif
