#include <chrono>
#include <ostream>
#include "time/centiseconds.h"

namespace std {

	std::ostream& operator <<(std::ostream& stream, const centiseconds cs) {
		return stream << cs.count() << "cs";
	}

}