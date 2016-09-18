#ifndef UTILITY_HPP
#define UTILITY_HPP

namespace NLV {    
  namespace utility {
    
    // based on http://stackoverflow.com/a/6454211/403571
    
    using namespace std;
    // ------------- UTILITY---------------
    template<int...> struct index_tuple{}; 

    template<int I, typename IndexTuple, typename... Types> 
    struct make_indexes_impl; 

    template<int I, int... Indexes, typename T, typename ... Types> 
    struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...> 
    { 
        typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type; 
    }; 

    template<int I, int... Indexes> 
    struct make_indexes_impl<I, index_tuple<Indexes...> > 
    { 
        typedef index_tuple<Indexes...> type; 
    }; 

    struct default_mapper {
      template<typename T>
      inline T operator()(T t) {
        return t;
      }
    };

    template<typename ... Types> 
    struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...> 
    {}; 
    
    
    #define __impl pf( MapFunc()(std::forward<decltype(get<Indexes>(tup))>(get<Indexes>(tup)))...)
    template<class MapFunc, class Z, class... Args, int... Indexes > 
    auto apply_helper( Z&& pf, index_tuple< Indexes... >, tuple<Args...>&& tup) -> decltype(__impl) 
    { 
        return __impl;
    } 
    #undef __impl

    template<class MapFunc, class Z, class ... Args> 
    auto apply(Z&& pf, const tuple<Args...>&  tup) -> decltype(apply_helper<MapFunc>(pf, typename make_indexes<Args...>::type(), tuple<Args...>(tup)))
    {
        return apply_helper<MapFunc>(pf, typename make_indexes<Args...>::type(), tuple<Args...>(tup));
    }

    template<class MapFunc = default_mapper, class Z, class ... Args> 
    auto apply(Z&& pf, tuple<Args...>&&  tup) -> decltype(apply_helper<MapFunc>(pf, typename make_indexes<Args...>::type(), forward<tuple<Args...>>(tup)))
    {
        return apply_helper<MapFunc>(pf, typename make_indexes<Args...>::type(), forward<tuple<Args...>>(tup));
    }
  }
};

#endif // UTILITY_HPP
