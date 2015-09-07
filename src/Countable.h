//
//  Countable.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 04/07/2015.
//

#pragma once

template <typename T>
class Countable
{
    static unsigned _count;
    public:
        Countable() { ++_count; }
        Countable( Countable const& ) { ++_count; }
        virtual ~Countable() { --_count; }
        static unsigned count() { return _count; }
};

template <typename T>
unsigned Countable<T>::_count = 0;
