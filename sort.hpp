// =====================================================================================
//
//       Filename:  sort.hpp
//
//    Description:  Các thuật toán sắp xếp
//
//        Version:  1.0
//        Created:  08/28/2012 05:46:54 PM
//       Revision:  none
//       Compiler:  gcc
//
//         Author:  YOUR NAME (), 
//   Organization:  
//
// =====================================================================================

#ifndef __SORT__
#define __SORT__

#include <memory>
#include <functional>
#include <iterator>
#include <algorithm>
#include <vector>
#include <type_traits>
#include <cstdlib>
#include <ctime>
#include <cstring>

namespace algorithm {
namespace sort {

//-----------------------------------------------------------------------------
//  Các thuật toán O(n^2)
//-----------------------------------------------------------------------------

// ===  FUNCTION  ======================================================================
//         Name:  bubble_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename Comp 
                = std::less<typename std::iterator_traits<Iterator>::value_type>>
    void
bubble_sort ( Iterator first, Iterator last, Comp comp = Comp() )
{
    for(; first + 1 < last; ++first)
        for(Iterator i = first + 1; i != last; ++i)
            if(comp(*i, *first))
                std::swap(*first, *i);
}		// -----  end of function bubble_sort  ----- 



// ===  FUNCTION  ======================================================================
//         Name:  selection_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename Comp 
                = std::less<typename std::iterator_traits<Iterator>::value_type>>
    void
selection_sort ( Iterator first, Iterator last, Comp comp = Comp() )
{
    for(; first + 1 < last; ++first) {
        Iterator selected = first;
        for(Iterator i = first + 1; i != last; ++i)
            if(comp(*i, *selected))
                selected = i;
        std::swap(*first, *selected);
    }
}		// -----  end of function selection_sort  ----- 



// ===  FUNCTION  ======================================================================
//         Name:  insertion_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename Comp = 
                std::less<typename std::iterator_traits<Iterator>::value_type>>
    void
insertion_sort ( Iterator first, Iterator last, Comp comp = Comp() )
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    for(Iterator i = first + 1; i < last; ++i) {
        value_type value = *i;
        Iterator j = i;
        while(j != first && comp(value, *(j-1))) {
            *j = *(j-1);
            --j;
        }
        *j = value;
    }
}		// -----  end of function insertion_sort  ----- 



//-----------------------------------------------------------------------------
//  Các thuật toán O(n log(n) )
//-----------------------------------------------------------------------------

#define     NLOGN_THRESHOLD     32


// ===  FUNCTION  ======================================================================
//         Name:  merge
//  Description:  
// =====================================================================================
template <typename Iterator, typename TempIterator, typename Comp>
    void
merge ( Iterator first, Iterator last1, Iterator last, 
                        TempIterator temp, Comp comp )
{
    Iterator i1 = first, i2 = last1;
    TempIterator io = temp;

    while(i1 != last1 && i2 != last) {
        if(comp(*i1, *i2)) *io++ = *i1++;
        else               *io++ = *i2++;
    }
        
//    while(i1 != last)       *io++ = *i1++;
    std::copy(i1, last1, io);
    std::advance(io, std::distance(i1, last1));

    std::copy(temp, io, first);

}		// -----  end of function merge  ----- 


// ===  FUNCTION  ======================================================================
//         Name:  merge_sort_dispatch
//  Description:  
// =====================================================================================
template <typename Iterator, typename TempIterator, typename Comp>
    void
merge_sort_dispatch ( Iterator first, Iterator last, 
                            TempIterator temp, Comp comp)
{
    auto distance = std::distance(first, last);
    if(distance < NLOGN_THRESHOLD) {
        insertion_sort(first, last, comp);
        return;
    }
    
    Iterator last1 = first;
    std::advance(last1, distance >> 1);
    merge_sort_dispatch(first, last1, temp, comp);
    merge_sort_dispatch(last1, last , temp, comp);
    
    merge(first, last1, last, temp, comp);
}		// -----  end of function merge_sort_dispatch  ----- 


// ===  FUNCTION  ======================================================================
//         Name:  merge_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename Comp = 
                std::less<typename std::iterator_traits<Iterator>::value_type>>
    void
merge_sort ( Iterator first, Iterator last, Comp comp = Comp() )
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;
    std::vector<value_type> temp(std::distance(first, last));
    merge_sort_dispatch(first, last, temp.begin(), comp);
}		// -----  end of function merge_sort  ----- 


// ===  FUNCTION  ======================================================================
//         Name:  quick_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename Comp = 
                std::less<typename std::iterator_traits<Iterator>::value_type>>
    void
quick_sort ( Iterator first, Iterator last, Comp comp = Comp() )
{
    auto distance = std::distance(first, last);
    if(distance < NLOGN_THRESHOLD) {
        insertion_sort(first, last, comp);
        return;
    }

    Iterator pivot = first;
//    std::advance(pivot, std::rand() % distance);
    std::advance(pivot, distance >> 1);
    auto pivot_value = *pivot;

    Iterator i = first, j = i;
    std::advance(j, distance - 1);
    while(i <= j) {
        while( comp(*i, pivot_value)) ++i;
        while( comp(pivot_value, *j)) --j;
        if(i < j) {
            std::swap(*i, *j);
            ++i, --j;
        } else if(i == j)
            ++i, --j;
    }

    if(first < j) quick_sort(first, j+1, comp);
    if(i + 1 < last) quick_sort(i, last, comp);

}		// -----  end of function quick_sort  ----- 



// ===  FUNCTION  ======================================================================
//         Name:  heap_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename Comp = 
                std::less<typename std::iterator_traits<Iterator>::value_type>>
    void
heap_sort ( Iterator first, Iterator last, Comp comp = Comp() )
{
    auto distance = std::distance(first, last);
    if(distance < NLOGN_THRESHOLD) {
        insertion_sort(first, last, comp);
        return;
    }

    std::make_heap(first, last, comp);
    while(first != last)
        std::pop_heap(first, last--, comp);
}		// -----  end of function heap_sort  ----- 


//-----------------------------------------------------------------------------
//  Các thuật toán O(n)
//-----------------------------------------------------------------------------

#define         N_THRESHOLD     256


// ===  FUNCTION  ======================================================================
//         Name:  counting_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename value_type = typename std::enable_if<
                std::is_integral<typename std::iterator_traits<Iterator>::value_type>::value,
                typename std::iterator_traits<Iterator>::value_type       >::type
         >
    void
counting_sort ( Iterator first, Iterator last )
{
    auto minmax     = std::minmax_element(first, last);
    auto min        = *(minmax.first);
    auto max        = *(minmax.second);
    auto range      = max - min;
//    auto distance   = std::distance(first, last);

    std::size_t *count = (std::size_t*) std::calloc(range+1, sizeof(std::size_t));
    
    for(Iterator i = first; i != last; ++i) ++count[*i - min];
    
    for(decltype(range) i = 0; i != range + 1; ++i)
        while(count[i]--)
            *first++ = i + min;

    std::free(count);
}		// -----  end of function counting_sort  ----- 


// ===  FUNCTION  ======================================================================
//         Name:  radix
//  Description:  
// =====================================================================================
template <typename InputIterator, typename OutputIterator>
    void
radix ( std::size_t word, InputIterator first, InputIterator last, OutputIterator out )
{
    std::size_t *count = (std::size_t*) std::calloc(1<<16, sizeof(std::size_t));

    for(InputIterator i = first; i != last; ++i)
        ++count[(*i >> (word << 4)) & 0xffff];

    for(auto c = count + 1; c != count + (1<<16); ++c)
        *c += *(c-1);

    for(auto i = last - 1; i >= first; --i)
        *(out + --count[(*i >> (word << 4)) & 0xffff]) = *i;

    std::free(count);
}		// -----  end of function radix  ----- 

// ===  FUNCTION  ======================================================================
//         Name:  radix_sort
//  Description:  
// =====================================================================================
template <typename Iterator, typename value_type = typename std::enable_if<
                std::is_unsigned<typename std::iterator_traits<Iterator>::value_type>::value,
                typename std::iterator_traits<Iterator>::value_type       >::type
         >
    void
radix_sort ( Iterator first, Iterator last )
{
    auto distance = std::distance(first, last);
    if(sizeof(value_type) < 4) {
        counting_sort(first, last);
        return;
    }

    value_type *temp = new value_type[distance];
    for(std::size_t word = 0; word != sizeof(value_type) >> 1; ++word) {
        if(word & 1)
            radix(word, temp, temp + distance, first);
        else
            radix(word, first, last, temp);
    }

    delete[] temp;
}		// -----  end of function radix_sort  ----- 

} // ----- end of namespace sort -----
} // ----- end of namespace algorithm -----


#endif
