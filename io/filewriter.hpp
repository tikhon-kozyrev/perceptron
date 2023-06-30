/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef IO_FILEWRITER_HPP
#define IO_FILEWRITER_HPP

#include <fstream>

namespace IO {
	class FileWriter {
		public:
			FileWriter();
			bool IsOpen();
			bool Open(const std::string &filename);
			void Close();
			template <class T> bool Write(const T &val) {
				bool res = false;
				do {
					if (!ofs.write(reinterpret_cast<const char *>(&val), sizeof(val))) {
						break;
					}
					res = true;
				} while (false);
				return res;
			}
		private:
			std::ofstream ofs;
	};
}
#endif
