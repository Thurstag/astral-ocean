#pragma once

/// <summary>
/// Macro to skip a test if condition is true
/// </summary>
#define SKIP_TEST(condition, message) {                                     \
    if (condition) {					                                    \
		std::cout << "Skip test because: " << #message << std::endl;      	\
        return;                                                             \
	}                                                                       \
}
