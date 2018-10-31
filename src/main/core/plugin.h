#pragma once

#include "logger.h"

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
			/// <param name="subject">Subject</param>
			Plugin(T* subject);
			virtual ~Plugin();

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
			Logger LOGGER = ao::core::Logger::getInstance<Plugin<T>>();
			T* subject = nullptr;
		};

		template<class T>
		Plugin<T>::Plugin(T * subject) {
			this->subject = subject;
		}

		template<class T>
		Plugin<T>::~Plugin() {}
	}
}

