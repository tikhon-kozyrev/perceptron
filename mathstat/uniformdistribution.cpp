/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#include "mathstat/uniformdistribution.hpp"
#include <ctime>

namespace MathStat {
	UniformDistribution::UniformDistribution(double min, double max, bool randomize):d(min, max) {
		if (randomize) {
			engine.seed(time(0));
		}
	}
	double UniformDistribution::getDouble() {
		return d(engine);
	}
}
