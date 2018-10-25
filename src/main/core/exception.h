#pragma once

#include <boost/stacktrace.hpp>
#include <sstream>
#include <exception>
#include <string>

namespace ao {
	namespace core {
		class Exception : public std::exception {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="message">Messagge</param>
			Exception(std::string message);
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="e">Exception</param>
			Exception(std::exception& e);
			/// <summary>
			/// Destructor
			/// </summary>
			~Exception();

			/// <summary>
			/// Method to define << operator
			/// </summary>
			/// <param name="os">Stream</param>
			/// <param name="e">Exception</param>
			/// <returns>Stream</returns>
			friend std::ostream& operator<<(std::ostream& os, const Exception& e);

		private:
			boost::stacktrace::stacktrace* stack = nullptr;
		};
	}
}
