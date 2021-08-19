#pragma once

#include <ranges>

namespace common::views {

template<typename I>
struct IndirectViewIterator {
    I iterator;

    constexpr
    IndirectViewIterator(I i)
        : iterator(std::move(i))
    {}

    constexpr
    auto operator==(IndirectViewIterator i) {
        return iterator == i.iterator;
    }

    constexpr
    auto& operator*() const {
        return **iterator;
    }

    constexpr
    auto& operator++() {
        return ++iterator;
    }
};

template<typename I>
IndirectViewIterator(I) -> IndirectViewIterator<I>;

template<typename Iterator, typename Sentinel>
struct IndirectView
: public std::ranges::view_interface<IndirectView<Iterator, Sentinel>>
{
    Iterator iterator;
    Sentinel sentinel;

    constexpr
    IndirectView(Iterator&& i, Sentinel&& s)
        : iterator(std::move(i))
        , sentinel(std::move(s))
    {}

    constexpr
    auto begin() const noexcept {
        return IndirectViewIterator(iterator);
    }

    constexpr
    auto end() const noexcept {
        return IndirectViewIterator(sentinel);
    }
};

template<typename I, typename S>
IndirectView(I, S) -> IndirectView<I, S>;

struct IndirectViewFunctor {
    template<typename Range> constexpr
    auto operator()(Range&& r) const noexcept {
        return IndirectView(r.begin(), r.end());
    }
};

template<typename R> constexpr
auto operator|(R&& r, IndirectViewFunctor ivf) {
    return ivf(std::move(r));
}

constexpr auto indirect = IndirectViewFunctor();

}
