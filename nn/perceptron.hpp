/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef NN_PERCEPTRON_HPP
#define NN_PERCEPTRON_HPP

#include "mathstat/uniformdistribution.hpp"
#include "linalg/linalg.hpp"
#include "io/filereader.hpp"
#include "io/filewriter.hpp"

namespace NN {
	static MathStat::UniformDistribution ud(-1., 1.);

	template <class NUMBER> class TPerceptron {
		public:
			using LA = LinearAlgebra<NUMBER>;
			using Number = typename LA::Number;
			using Vector = typename LA::Vector;
			using Matrix = typename LA::Matrix;
			using UnaryFunction = NUMBER(*)(const Number &);
			using UnaryInplaceFunction = void (*)(Number &);

			struct Sample {
				Vector input;
				Vector output;
				Sample() = default;
				Sample(size_t iSize, size_t oSize) {
					input.resize(iSize);
					output.resize(oSize);
				}
			};

			static void RND(NUMBER &n) {
				n = ud.getDouble();
			}
			void BuildTopology(const std::vector<size_t> topology) {
				size_t layerCount = topology.size();
				_layer.resize(layerCount);
				_bias.resize(layerCount);
				_weight.resize(layerCount-1);

				for (size_t i = 0; i < layerCount; i++) {
					if (i < topology.size() - 1) {
						_weight[i].Resize(topology[i], topology[i+1]);
					}
					_bias[i].Resize(1, topology[i]);
					_layer[i].Resize(1, topology[i]);
				}
			}
			void Init() {
				for (Matrix &v: _bias) {
					v.ApplyForEach(RND, true);
				}
				for (Matrix &m: _weight) {
					m.ApplyForEach(RND, true);
				}
			}


			size_t InSize() const {
				size_t res = 0;
				do {
					if (_layer.empty()) {
						break;
					}
					res = _layer.front().Size();
				} while (false);
				return res;
			}
			size_t OutSize() const {
				size_t res = 0;
				do {
					if (_layer.empty()) {
						break;
					}
					res = _layer.back().Size();
				} while (false);
				return res;
			}

			TPerceptron(double learningRate, UnaryInplaceFunction sigmoid, UnaryFunction dsigmoid)
				: learningRate(learningRate)
				, activation(sigmoid)
				, derivative(dsigmoid) {
			}

			Vector feedForward(const Vector &input) {
				_layer[0] = input;
				for (size_t i = 1; i < _layer.size(); ++i)  {
					Matrix &in = _layer[i - 1];
					Matrix &out = _layer[i];
					out = in * _weight[i-1] + _bias[i];
					out.ApplyForEach(activation, true);
				}
				return _layer[_layer.size() - 1];
			}

			void backpropagation(const Vector &right_answer) {
				Matrix errors = Matrix::Row(right_answer) - _layer.back();
				for (int k = _layer.size() - 2; k >= 0; k--) {
					Matrix &in = _layer[k];
					Matrix &out = _layer[k + 1];
					Matrix errorsNext;
					errorsNext.Resize(in.Size(), 1);
					Matrix gradients;
					gradients.Resize(out.Size(), 1);
					for (size_t i = 0; i < out.Size(); i++) {
						gradients[i] = errors[i] * derivative(_layer[k + 1][i]) * learningRate;
					}
					Matrix deltas;
					deltas.Resize(_weight[k].Cols(), _weight[k].Rows());
					for (size_t i = 0; i < out.Size(); i++) {
						for (size_t j = 0; j < in.Size(); j++) {
							deltas.at(i, j) = gradients[i] * in[j];
						}
					}
					for (size_t i = 0; i < in.Size(); i++) {
						errorsNext[i] = 0;
						for (size_t j = 0; j < out.Size(); j++) {
							errorsNext[i] += _weight[k].at(i, j) * errors[j];
						}
					}
					errors = errorsNext;
					_weight[k] += deltas.Transp();
					_bias[k+1] += gradients.Transp();
				}
			}
			bool SaveToFile(const std::string &filename) {
				bool res = false;
				do {
					IO::FileWriter f;
					auto NumSaver = [&f](NUMBER& v)->void{
						f.Write(v);
					};
					if (!f.Open(filename)) {
						break;
					}
					_saveTopology(f);
					for (auto &b : _bias) {
						b.ApplyForEach(NumSaver);
					}
					for (auto &w : _weight) {
						w.ApplyForEach(NumSaver);
					}
					res = true;
				} while (false);
				return res;
			}
			bool LoadFromFile(const std::string &filename) {
				bool res = false;
				do {
					IO::FileReader f;
					auto NumLoader = [&f](NUMBER& v)->void{
						f.Read(v);
					};

					if (!f.Open(filename)) {
						break;
					}
					_loadTopology(f);
					for (auto &b : _bias) {
						b.ApplyForEach(NumLoader);
					}
					for (auto &w : _weight) {
						w.ApplyForEach(NumLoader);
					}
					res = true;
				} while (false);
				return res;
			}

		private:
			void _saveTopology(IO::FileWriter &f) {
				do {
					if (!f.Write<uint32_t>(_layer.size())) {
						break;
					}
					for (auto &l: _layer) {
						if (!f.Write<uint32_t>(l.Size())) {
							break;
						}
					}
				} while (false);
			}
			void _loadTopology(IO::FileReader &f) {
				do {
					uint32_t v;
					std::vector<size_t> topo;
					if (!f.Read<uint32_t>(v)) {
						break;
					}
					topo.resize(v);
					for (auto &ls: topo) {
						if (!f.Read<uint32_t>(v)) {
							break;
						}
						ls = v;
					}
					BuildTopology(topo);
				} while (false);
			}
			std::vector<Matrix> _layer;
			std::vector<Matrix> _bias;
			std::vector<Matrix> _weight;

			double learningRate;
			UnaryInplaceFunction activation;
			UnaryFunction derivative;
	};
}

#endif
