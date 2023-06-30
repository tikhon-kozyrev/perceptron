/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef MATHSTAT_UNIFORMDISTRIBUTION_HPP
#define MATHSTAT_UNIFORMDISTRIBUTION_HPP

#include <random>
#include "mathstat/distribution.hpp"

namespace MathStat {
	class UniformDistribution: public Distribution {
		public:
			UniformDistribution(double min=0., double max=1., bool randomize=true);
			double getDouble() override;
		private:
			std::default_random_engine engine;
			std::uniform_real_distribution<double> d;
	};
}

#endif
