//
// Created by 2025 on 3/3/2026.
//#include "../header.h"
#include <boost/array.hpp>
#include <boost/container/vector.hpp>

using namespace boost;
int main() {
    boost::array<int, 4> arr = {{1, 2, 3, 4}};
    std::cout << "Boost array element: " << arr[0] << std::endl;
    boost::container::vector<int> boost_vector;
    for (int i=0;i<1000;i++) {
        boost_vector.push_back(i);
    }
    for (int i=0;i<1000;i++) {
        cout<<boost_vector[i];
    }


    return 0;


}
