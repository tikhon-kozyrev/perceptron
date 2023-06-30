/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef LINALG_VECTOR_HPP
#define LINALG_VECTOR_HPP

#include <vector>

namespace LinAlg {
	template <class NUMBER> class Vector: public std::vector<NUMBER> {
		public:
			using Numer = NUMBER;
	};
}
#endif
