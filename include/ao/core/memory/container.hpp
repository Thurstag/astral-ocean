// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

namespace ao::core {
    /// <summary>
    /// Container class
    /// </summary>
    template<class Key, class Value>
    class Container {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        Container() = default;

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Container() = default;

        /// <summary>
        /// [] operator. Throws an execption if object doesn't exist
        /// </summary>
        /// <param name="index">Index</param>
        /// <returns>U object</returns>
        virtual Value& operator[](Key const& index) = 0;

        /// <summary>
        /// Method to remove an object
        /// </summary>
        /// <param name="index">Index</param>
        /// <returns>Removed or not</returns>
        virtual bool remove(Key const& index) = 0;

        /// <summary>
        /// Method to know if an object exists
        /// </summary>
        /// <param name="index">Index</param>
        /// <returns>True or False</returns>
        virtual bool exists(Key const& index) const = 0;

        /// <summary>
        /// Method to clear container
        /// </summary>
        virtual void clear() = 0;

        /// <summary>
        /// Method to get container's size
        /// </summary>
        /// <returns>Size</returns>
        virtual size_t size() const = 0;
    };
}  // namespace ao::core