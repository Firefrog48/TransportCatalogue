#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

namespace Geo {

    struct Coordinates {
        double lat; // Широта
        double lng; // Долгота
        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    double ComputeDistance(Coordinates from, Coordinates to);

    namespace detail{
        
        template <typename Container>
        double ComputeGeoLength (const Container& stops_container){
            double result = 0;
            size_t end_of_calculating = static_cast<size_t>(stops_container.size()) - 1;
            for (size_t i = 0; i < end_of_calculating;) {
                Coordinates this_stop = stops_container[i]->coordinates;
                Coordinates next_stop = stops_container[++i]->coordinates;
                result += ComputeDistance(this_stop, next_stop);
            }
            return result;
        }
    }

    
    
}  
