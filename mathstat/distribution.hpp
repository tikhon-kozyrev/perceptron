/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#ifndef MATHSTAT_DISTRIBUTION_HPP
#define MATHSTAT_DISTRIBUTION_HPP

namespace MathStat {
	class Distribution {
		public:
			Distribution();
			virtual double getDouble()=0;
			virtual ~Distribution();
	};
}

#endif
