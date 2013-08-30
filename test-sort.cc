/*
 * =====================================================================================
 *
 *       Filename:  test-sort.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/04/2012 10:29:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <fstream>
#include <chrono>

#include <cassert>
#include <cstring>

#include <getopt.h>

#include "sort.hpp"

enum Algorithm { None = 0, Bubble = 1, Selection = 2, Insertion = 4,
       Merge  = 8, Heap = 16, Quick = 32,
       Counting = 64, Radix = 128, All = 255 };

namespace test {

//namespace sort = algorithm::sort;
using namespace algorithm::sort;
using namespace std::chrono;
typedef duration<double,std::ratio<1>> real_seconds;

#define     PROCESS_TIME(COMMAND, STR)      auto b = high_resolution_clock::now();      \
                                            COMMAND;                                    \
                                            auto time = duration_cast<real_seconds>(    \
                                                high_resolution_clock::now()-b).count();\
                                            std::cout << STR << " : " << time << "s\n"


#define     TEST_SORT_FUNCTION(NAME)        template <typename T>                               \
                                            void   test_##NAME(T const *sample, std::size_t n) { \
                                                static std::ofstream f(#NAME".data");           \
                                                T *a = new T[n];                                \
                                                std::memcpy(a, sample, n*sizeof(T));            \
                                                if( n > 50 ) {                                  \
                                                    PROCESS_TIME(NAME(a, a+n), #NAME);          \
                                                    assert(std::is_sorted(a, a+n));             \
                                                    f << n << '\t'<< time << '\n';              \
                                                } else {                                        \
                                                    NAME(a, a+n);                               \
                                                    std::ostream_iterator<T> oi(std::cout, " ");\
                                                    std::cout << #NAME << " : ";                \
                                                    std::copy(a, a+n, oi);                      \
                                                    std::cout << '\n';                          \
                                                }                                               \
                                                delete[] a;                                     \
                                            }


TEST_SORT_FUNCTION(bubble_sort)
TEST_SORT_FUNCTION(selection_sort)
TEST_SORT_FUNCTION(insertion_sort)
TEST_SORT_FUNCTION(merge_sort)
TEST_SORT_FUNCTION(heap_sort)
TEST_SORT_FUNCTION(quick_sort)
TEST_SORT_FUNCTION(counting_sort)
TEST_SORT_FUNCTION(radix_sort)

template <typename T>
using TestFunction = decltype(&test_radix_sort<T>);

template <typename T>
void test_sort(T const *sample, std::size_t n, 
        std::unordered_set<TestFunction<T>> const &algorithms) 
{

    std::cout << "--------------------------------\n";
    std::cout << "Array size : " << n << "\n";
    if(n <= 50) {
        std::cout << "Array: ";
        std::ostream_iterator<T> oi(std::cout, " ");
        std::copy(sample, sample + n, oi);
        std::cout << "\n\n";
    } else
        std::cout << '\n';

    for(auto &func : algorithms)
        func(sample, n);

    std::cout << '\n';

}

template <typename T>
void test_sort_random(std::deque<std::pair<std::size_t, std::size_t>> const &size_list,
        std::unordered_set<TestFunction<T>> const &algorithms) 
{
    std::srand(std::time(NULL));

    for(auto &p : size_list) {
        T *sample = new T[p.first];
        for(std::size_t i = 0; i != p.first; ++i)
            sample[i] = std::rand() % p.second;

        test_sort(sample, p.first, algorithms);

        delete[] sample;
    }
}

template <typename T>
void test_sort_input(std::istream& is,
        std::unordered_set<TestFunction<T>> const &algorithms) 
{
    std::size_t sz;
    is >> sz;

    T *sample = new T[sz];
    for(std::size_t i = 0; i != sz; ++i) is >> sample[i];

    test_sort<T>(sample, sz, algorithms);

    delete[] sample;
}

} // ---- end of namespace test -----

int main(int argc, char *argv[]) {

    typedef unsigned int T;
    
    std::deque<std::pair<std::size_t, std::size_t>> size_list;
    std::size_t start_size = 0, end_size = 0;

    enum SizeListPolicy { UserInputSizes, ArithProg, GeoProg };
    SizeListPolicy size_list_policy = UserInputSizes;
    std::size_t difference = 10000;
    float ratio = 1.5;

    enum MaxValuePolicy { FixedMaxValue, MaxValueEqSize };
    MaxValuePolicy max_value_policy = MaxValueEqSize;
    std::size_t max_value;

    std::string filename;

    std::unordered_map<std::string, test::TestFunction<T>>
    alg_map = {
        { "bubble", &test::test_bubble_sort<T> },
        { "selection", &test::test_selection_sort<T> },
        { "insertion", &test::test_insertion_sort<T> },
        { "merge", &test::test_merge_sort<T> },
        { "quick", &test::test_quick_sort<T> },
        { "heap", &test::test_heap_sort<T> },
        { "radix", &test::test_radix_sort<T> },
        { "counting", &test::test_counting_sort<T> },
    };

    std::unordered_set<test::TestFunction<T>> alg_list;

    // parse param
    struct option options[] = {
        {"max-eq-size",     no_argument,       0, 'q'},
        {"fixed-max-value", required_argument, 0, 'x'},
        {"from-file",       required_argument, 0, 'f'},
        {"start-size",      required_argument, 0, 's'},
        {"end-size",        required_argument, 0, 'e'},
        {"difference",      required_argument, 0, 'd'},
        {"ratio",           required_argument, 0, 'r'},
        {"algorithm",       required_argument, 0, 'a'},
        {"help",            no_argument,       0, 'h'} 
    };

    int c;

    while( (c = getopt_long(argc, argv, "qx:f:s:e:d:r:a:h", options, 0)) != -1) 
    {
        switch(c) {
            case 'q':
                max_value_policy = MaxValueEqSize;
                break;
            case 'x':
                max_value_policy = FixedMaxValue;
                max_value = std::atoi(optarg);
                break;
            case 'f':
                filename = optarg;
                break;
            case 's':
                start_size = std::atoi(optarg);
                break;
            case 'e':
                end_size = std::atoi(optarg);
                break;
            case 'd':
                size_list_policy = ArithProg;
                difference = std::atoi(optarg);
                break;
            case 'r':
                size_list_policy = GeoProg;
                ratio = std::atof(optarg);
                break;
            case 'a':
                assert(alg_map.find(optarg) != alg_map.end());
                alg_list.insert(alg_map[optarg]);
                break;
            case 'h':
                break;
        }
    }

    if(filename != "") {
        if(filename == "-")
            test::test_sort_input<T>(std::cin, alg_list);
        else {
            std::ifstream ifs(filename);
            test::test_sort_input<T>(ifs, alg_list);
            ifs.close();
        }
    } else {
        float f;
        std::size_t s;
        auto *max = (max_value_policy == FixedMaxValue) ? &max_value : &s;
        switch(size_list_policy) {
            case ArithProg:
                for(s = start_size; s <= end_size; s += difference)
                    size_list.emplace_back(s, *max);
                break;
            case GeoProg:
                for(s = f = start_size; f <= end_size; f *= ratio, s = std::lrintf(f))
                    size_list.emplace_back(s, *max);
                break;
            case UserInputSizes:
                while(optind < argc) {
                    s = std::atoi(argv[optind++]);
                    size_list.emplace_back(s, *max);
                }
        }

        test::test_sort_random<T>(size_list, alg_list);
    }

    return 0;
}
