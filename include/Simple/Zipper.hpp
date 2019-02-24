#ifndef SIMPLE_ZIPPER_HPP
#define SIMPLE_ZIPPER_HPP

#include <iterator>
#include <tuple>

namespace Simple {
namespace detail {
template <std::size_t TRem>
class tuple_for_each {
public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect parameters");
        constexpr std::size_t Idx = TupleSize - TRem;

        func(std::get<Idx>(std::forward<TTuple>(tuple)));

        tuple_for_each<TRem - 1>::exec(std::forward<TTuple>(tuple),
                                       std::forward<TFunc>(func));
    }
};

template <>
class tuple_for_each<0> {
public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& /*tuple*/, TFunc&& /*func*/){};
};

template <std::size_t TRem>
class are_equal {
public:
    template <typename TTuple>
    static bool exec(const TTuple& tuple1, const TTuple& tuple2)
    {
        using Tuple = typename std::decay<TTuple>::type;
        constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect parameters");
        static constexpr std::size_t Idx = TupleSize - TRem;

        if (std::get<Idx>(tuple1) != std::get<Idx>(tuple2)) {
            return false;
        }
        return are_equal<TRem - 1>::exec(tuple1, tuple2);
    }
};

template <>
class are_equal<0> {
public:
    // Stop compile time recursion
    template <typename TTuple>
    static bool exec(const TTuple& /*tuple1*/, const TTuple& /*tuple2*/)
    {
        return true;
    }
};
} // namespace detail

template <class... Ts>
class Zipper {
    static_assert(sizeof...(Ts) > 0, "Zipper take one ore more types.");

    template <typename TTuple, typename TFunc>
    static void tuple_for_each(TTuple&& tuple, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;

        detail::tuple_for_each<TupleSize>::exec(std::forward<TTuple>(tuple),
                                                std::forward<TFunc>(func));
    }

    template <typename TTuple>
    static bool are_same(const TTuple& tuple1, const TTuple& tuple2)
    {
        using Tuple = typename std::decay<TTuple>::type;
        constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;

        return detail::are_equal<TupleSize>::exec(tuple1, tuple2);
    }

public:
    class iterator {

    public:
        using type = std::tuple<typename Ts::iterator...>;
        using dereferenced_type = std::tuple<typename Ts::value_type&...>;
        using reference = type&;
        using pointer = type*;

        explicit iterator(typename Ts::iterator... s)
            : current(s...){};

        iterator(const iterator& rhs)
            : current(rhs.current){};

        iterator& operator++()
        {
            increment();
            return *this;
        }

        iterator operator++(int)
        {
            auto a = *this;
            increment();
            return a;
        }

        bool operator!=(const iterator& other) const
        {
            return !are_same(current, other.current);
        }

        auto operator*()
        {
            return deref(std::index_sequence_for<Ts...>());
        }

    protected:
        std::tuple<typename Ts::iterator...> current;

    private:
        template <size_t... Indexes>
        dereferenced_type deref(std::index_sequence<Indexes...>)
        {
            return dereferenced_type(*std::get<Indexes>(current)...);
        }

        struct Increment {
            template <class T>
            void operator()(T& other)
            {
                other++;
            }
        };

        void increment()
        {
            tuple_for_each(current, Increment());
        }
    };

    using IteratorType = Zipper<Ts...>::iterator;
    using ConstIteratorType = const IteratorType;

    explicit Zipper(Ts&... c)
        : begin_{c.begin()...}
        , end_{c.end()...}
    {
    }

    IteratorType begin() const
    {
        return begin_;
    }

    IteratorType end() const
    {
        return end_;
    }

    ConstIteratorType cbegin() const
    {
        return begin_;
    }

    ConstIteratorType cend() const
    {
        return end_;
    }

private:
    IteratorType begin_;
    IteratorType end_;
};
} // namespace Simple

#endif
