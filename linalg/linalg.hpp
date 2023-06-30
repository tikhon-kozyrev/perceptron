/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef LINALG_LINALG_HPP
#define LINALG_LINALG_HPP

#include "linalg/vector.hpp"
#include "linalg/matrix.hpp"

template <class NUMBER> struct LinearAlgebra {
	using Number = NUMBER;
	using Vector = LinAlg::Vector<Number>;
	using Matrix = LinAlg::Matrix<Number>;
};

#endif
