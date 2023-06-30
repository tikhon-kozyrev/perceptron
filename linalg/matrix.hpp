/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef LINALG_MATRIX_HPP
#define LINALG_MATRIX_HPP

#include "linalg/vector.hpp"
#include <functional>
#include <cstdint>
#include <stdexcept>
#ifdef _OPENMP
	#include <omp.h>
#endif
#include <mutex>

namespace LinAlg {
	template <class NUMBER>class Matrix {
		public:
			using Number = NUMBER;
			using Vector = LinAlg::Vector<Number>;
			static Matrix Col(const Vector &v) {
				Matrix res(v.size(), 1);
				res._data = v;
				return res;
			}
			static Matrix Row(const Vector &v) {
				Matrix res(1, v.size());
				res._data = v;
				return res;
			}
			static Matrix Col(const size_t &s) {
				Matrix res(s, 1);
				return res;
			}
			static Matrix Row(const size_t &s) {
				Matrix res(1, s);
				return res;
			}
			Matrix()
				:Matrix(0, 0) {
			}
			Matrix (size_t rows, size_t cols) {
				Resize(rows, cols);
			}
			size_t Cols() const {
				return _cols;
			}
			size_t Rows() const {
				return _rows;
			}
			Matrix &Resize(size_t rows, size_t cols) {
				_rows = rows;
				_cols = cols;
				_data.resize(Rows()*Cols());
				return *this;
			}

			Matrix &toCol() {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't tell linear size of non-vector matrix");
				}
				Resize(Size(), 1);
				return *this;
			}
			Matrix &toRow() {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't tell linear size of non-vector matrix");
				}
				Resize(1, Size());
				return *this;
			}

			void ApplyForEach(std::function<void(Number &)> for_each, bool parallel=false) {
#ifdef _OPENMP
				#pragma omp parallel for schedule(static) if (parallel)
#endif
				for (size_t i=0; i<_data.size(); ++i) {
					for_each(_data[i]);
				}
			}
			NUMBER &at(size_t r, size_t c) {
				if (r >= Rows()) {
					throw std::runtime_error("Row Out Of Range");
				}
				if (c >= Cols()) {
					throw std::runtime_error("Row Out Of Range");
				}
				return _data[r*_cols+c];
			}
			const NUMBER &at(size_t r, size_t c) const {
				if (r >= Rows()) {
					throw std::runtime_error("Row Out Of Range");
				}
				if (c >= Cols()) {
					throw std::runtime_error("Row Out Of Range");
				}
				return _data[r*_cols+c];
			}
			Matrix &operator = (const Vector &other) {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't assign vector to non-vector matrix");
				}
				_data = other;
				return *this;
			}
			Matrix Transp() const {
				Matrix res(Cols(), Rows());
				for (size_t r=0; r<_rows; ++r) {
					for (size_t c=0; c<_cols; ++c) {
						res.at(c, r) = at(r, c);
					}
				}
				return res;
			}

			struct Statistics {
				Statistics() {
					reset();
				}
				void reset() {
#ifdef _OPENMP
					Mul.l_barrier = 0;
#else
					Mul.p_barrier = std::numeric_limits<size_t>::max();
#endif
					Mul.p_barrier = std::numeric_limits<size_t>::max();
				}
				struct {
					size_t l_barrier;
					size_t p_barrier;
				} Mul;
			};
			static inline Statistics Stat;

			Matrix operator * (const Matrix &other) const {
				if (Cols() != other.Rows()) {
					throw std::runtime_error("Matrix * Matrix size mismatch");
				}
				Matrix res(Rows(), other.Cols());
				size_t items = res._data.size();
				do {
#ifdef _OPENMP
					using Clock = std::chrono::high_resolution_clock;
					Clock clock;

					if ((items > Stat.Mul.l_barrier) && (items < Stat.Mul.p_barrier)) { // надо замерить время
						Clock::duration lin_time = Clock::duration::max();
						if (true) { // последовательно считаем
							auto start = clock.now();
							for (size_t i = 0; i<items; ++i) {
								size_t r = i/res._cols;
								size_t c = i%res._cols;

								res._data[i] = 0;
								for (size_t t=0; t<_cols; ++t) {
									res._data[i] += _data[r*_cols+t]*other._data[t*other._cols+c];
								}
							}
							lin_time = clock.now() - start;
						}
						Clock::duration par_time = Clock::duration::max();
						if (true) { // параллельно считаем
							auto start = clock.now();
							#pragma omp parallel for schedule (static)
							for (size_t i = 0; i<items; ++i) {
								size_t r = i/res._cols;
								size_t c = i%res._cols;

								res._data[i] = 0;
								for (size_t t=0; t<_cols; ++t) {
									res._data[i] += _data[r*_cols+t]*other._data[t*other._cols+c];
								}
							}
							par_time = clock.now() - start;
						}

						if (par_time < lin_time) {
							Stat.Mul.p_barrier = items;
						} else if (lin_time < par_time) {
							Stat.Mul.l_barrier = items;
						}
						break;
					}
					#pragma omp parallel for schedule (static) if (items >= Stat.Mul.p_barrier)
#endif
					for (size_t i = 0; i<items; ++i) {
						size_t r = i/res._cols;
						size_t c = i%res._cols;

						res._data[i] = 0;
						for (size_t t=0; t<_cols; ++t) {
							res._data[i] += _data[r*_cols+t]*other._data[t*other._cols+c];
						}
					}
				} while (false);
				return res;
			}
			Matrix operator - (const Matrix &other) const {
				if (Cols() != other.Cols()) {
					throw std::runtime_error("Matrix - Matrix size mismatch");
				}
				if (Rows() != other.Rows()) {
					throw std::runtime_error("Matrix - Matrix size mismatch");
				}
				Matrix res(Rows(), Cols());
				for (size_t i = 0; i<_data.size(); ++i) {
					res._data[i] = _data[i] - other._data[i];
				}
				return res;
			}

			Matrix operator + (const Matrix &other) const {
				if (Cols() != other.Cols()) {
					throw std::runtime_error("Matrix - Matrix size mismatch");
				}
				if (Rows() != other.Rows()) {
					throw std::runtime_error("Matrix - Matrix size mismatch");
				}
				Matrix res(Rows(), Cols());
				for (size_t i = 0; i<_data.size(); ++i) {
					res._data[i] = _data[i] + other._data[i];
				}
				return res;
			}
			void operator += (const Matrix &other) {
				if (Cols() != other.Cols()) {
					throw std::runtime_error("Matrixes sizes mismatch");
				}
				if (Rows() != other.Rows()) {
					throw std::runtime_error("Matrixes sizes mismatch");
				}
				for (size_t i = 0; i<_data.size(); ++i) {
					_data[i] += other._data[i];
				}
			}
			operator Vector() const {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't convert non-vector matrix to vector");
				}
				return _data;
			}

			size_t Size() const {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't tell linear size of non-vector matrix");
				}
				return _data.size();
			}
			NUMBER &operator [](size_t i) {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't access to non-vector matrix by linear index");
				}
				return _data[i];
			}
			const NUMBER &at(size_t i) const {
				if (!IsRow()&&!IsCol()) {
					throw std::runtime_error("Can't access to non-vector matrix by linear index");
				}
				return _data[i];
			}

			void Dump() {
				for (size_t r=0; r<Rows(); ++r) {
					printf ((0==r)?"[":" ");
					for (size_t c=0; c<Cols(); ++c) {
						if (0 != c) {
							printf ("\t");
						}
						printf ("%0.02f", at(r, c));
					}
					if (r == Rows()-1) {
						printf("]");
					}
					printf ("\n");
				}
			}

			virtual ~Matrix() {
			}

			bool IsRow() const {
				return 1 == Cols();
			}
			bool IsCol() const {
				return 1 == Rows();
			}

		protected:
			size_t _rows;
			size_t _cols;
			Vector _data;
	};
}


#endif
