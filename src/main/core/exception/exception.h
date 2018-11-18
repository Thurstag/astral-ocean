#pragma once

#include <exception>
#include <sstream>
#include <string>

#include <boost/stacktrace.hpp>
#include <boost/optional.hpp>

namespace ao {
	namespace core {
		/// <summary>
		/// Exception with a stack trace
		/// </summary>
		class Exception : public std::runtime_error {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="message">Messagge</param>
			/// <param name="stacktrace">Enable stack trace</param>
			Exception(std::string message, bool stacktrace = true);

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="message">Messagge</param>
			/// <param name="stacktrace">Enable stack trace</param>
			Exception(char const* message, bool stacktrace = true);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Exception() = default;

			/// <summary>
			/// Method to define << operator
			/// </summary>
			/// <param name="os">Stream</param>
			/// <param name="e">Exception</param>
			/// <returns>Stream</returns>
			friend std::ostream& operator<<(std::ostream& os, const Exception& e) {
	            // Display message
				os << e.what();

				// Display stack trace
				if (e.stack) {
					std::stringstream _stack;
					_stack << *e.stack;
					std::string stack = _stack.str();

					os << std::endl << stack.substr(0, stack.size() - 1);
				}
				return os;
			}

		private:
			boost::optional<boost::stacktrace::stacktrace> stack;
		};
	}
}
