// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "logger/logger.h"

namespace ao {
	namespace core {
		/// <summary>
		/// Plugin
		/// </summary>
		template<class T>
		class Plugin {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_subject">Subject</param>
			explicit Plugin(T* _subject);

			/// <summary>
			/// Constructor
			/// </summary>
			virtual ~Plugin() = default;

			/// <summary>
			/// Method called on subject init
			/// </summary>
			virtual void onInit() = 0;
			/// <summary>
			/// Method called on subject update
			/// </summary>
			virtual void onUpdate() = 0;
			/// <summary>
			/// Method called before subject destroy
			/// </summary>
			virtual void beforeDestroy() = 0;

		protected:
			Logger LOGGER = Logger::getInstance<Plugin<T>>();
			T* const subject = nullptr;
		};

		template<class T>
		Plugin<T>::Plugin(T * _subject) : subject(_subject) {}
	}
}

