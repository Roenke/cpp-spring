#include <type_traits>
#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>

template<typename it>
using ivt = typename std::iterator_traits<it>::value_type;

template<typename container>
using cvt = typename container::value_type;

template<typename fwd_it, typename cmp>
size_t elem_num_in_sorted(fwd_it begin, fwd_it end, 
        ivt<fwd_it> value, 
        cmp comparer = std::less<ivt<fwd_it>>()) {
    auto it_lower = std::lower_bound(begin, end, value, comparer);
    auto it_upper = std::upper_bound(begin, end, value, comparer);
    return std::distance(it_lower, it_upper);
}

template<typename container, typename cmp = std::less<cvt<container>>>
typename container::iterator set_add(container & set, cvt<container> value, cmp comparer = cmp()) {
    auto pos = std::lower_bound(set.begin(), set.end(), value, comparer);
    if (pos != set.end() && *pos != value) {
        return set.insert(pos, value);
    }

    return set.end();
}

template<typename it>
void print(it begin, it end) {
    while(begin != end) {
        std::cout << *begin << " ";
        ++begin;
    }

    std::cout << std::endl;
}

template<typename container, typename pred>
void erase_if(container & set, pred predicate) {
    auto after_move = std::remove_if(set.begin(), set.end(), predicate);
    set.erase(after_move, set.end());
}

template<typename it>
void merge_sort(it begin, it end) {
    auto distance = std::distance(begin, end);
    if(distance > 1) {
        auto middle = begin + distance / 2;
        merge_sort(begin, middle);
        merge_sort(middle, end);
        std::inplace_merge(begin, middle, end);
    }
}

template<typename it>
void heap_sort(it begin, it end) {
    std::make_heap(begin, end);
    while(begin != end) {
        std::pop_heap(begin, end);
        --end;
    }
}
template<typename Arg>
struct inputed_filter {
    bool operator()(Arg const & arg) {
        if (values_.size() < 5) {
            std::cout << "Enter 5 values:" << std::endl;
            while(values_.size() != 5) {
                Arg value;
                std::cin >> value;
                values_.emplace_back(std::move(value));
            }
        }

        return std::find(values_.begin(), values_.end(), arg) != values_.end();
    }

private:
    std::vector<Arg> values_;
};

template<typename container_it>
struct linearized_container_iterator : std::iterator<
    std::forward_iterator_tag,
    typename std::iterator_traits<typename std::iterator_traits<container_it>::value_type::iterator>::value_type
    > {
    linearized_container_iterator(container_it begin, container_it end) 
        : current_container_iterator_(begin)
        , end_container_iterator_(end)
        , current_iterator_(begin->begin())
    {}

    linearized_container_iterator() {}

    linearized_container_iterator(linearized_container_iterator & other) = default;

    linearized_container_iterator& operator++() {
        if(current_container_iterator_ == end_container_iterator_) {
            return *this;
        }

        ++current_iterator_;
        if(current_iterator_ == current_container_iterator_->end()) {
            ++current_container_iterator_;
            if(current_container_iterator_ == end_container_iterator_) {
                return *this;
            }
            current_iterator_ = current_container_iterator_->begin();
            if(current_iterator_ == current_container_iterator_->end()) {
                ++(*this);
            }
        }

        return *this;
    }

    linearized_container_iterator& operator++(int) {
        linearized_container_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator==(linearized_container_iterator const& other) {
        return current_container_iterator_ == end_container_iterator_ &&
            other.current_container_iterator_ == other.end_container_iterator_;
    }

    bool operator!=(linearized_container_iterator const &other) {
        return !((*this) == other);
    }

    typename container_it::value_type::value_type operator*() {
        return *current_iterator_;
    }
private:
    container_it current_container_iterator_;
    container_it end_container_iterator_;
    typename container_it::value_type::iterator current_iterator_;
};

template<typename container_of_container>
linearized_container_iterator<typename container_of_container::iterator> make_linearized_container_iterator(container_of_container & src) {
    return linearized_container_iterator<typename container_of_container::iterator>(src.begin(), src.end());
}

template<typename container_of_container>
linearized_container_iterator<typename container_of_container::iterator> make_linearized_container_iterator() {
    return linearized_container_iterator<typename container_of_container::iterator>();
}

void tests() {
    int sorted[] = { 1, 2, 3, 3, 4, 5, 6, 7, 7, 7, 7 };
    assert(elem_num_in_sorted(sorted, sorted + 11, 2, std::less<int>()) == 1);
    assert(elem_num_in_sorted(sorted, sorted + 11, 3, std::less<int>()) == 2);
    assert(elem_num_in_sorted(sorted, sorted + 11, 7, std::less<int>()) == 4);

    std::vector<int> vect = { 1, 2, 3, 4, 5, 6, 9 };
    assert(*set_add(vect, 7) == 7);
    assert(set_add(vect, 7) == vect.end());
    assert(set_add(vect, 1) == vect.end());
    assert(*set_add(vect, 8) == 8);
    assert(vect[6] == 7 && vect[7] == 8);

    std::vector<int> vect2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    erase_if(vect2, [](int val) { return val > 3; });
    assert(vect2.size() == 3);
    assert(vect2[2] == 3);

    std::vector<int> for_sorting1 = { 0, 4, -2, 5, 6, 1, 3, 10, 22, -40, -50, -30, 23 };
    std::vector<int> for_sorting2 = { 0, 4, -2, 5, 6, 1, 3, 10, 22, -40, -50, -30, 23 };

    merge_sort(for_sorting1.begin(), for_sorting1.end());
    heap_sort(for_sorting2.begin(), for_sorting2.end());
    for (size_t i = 1; i < for_sorting1.size(); ++i) {
        assert(for_sorting1[i - 1] <= for_sorting1[i]);
        assert(for_sorting2[i - 1] <= for_sorting2[i]);
        assert(for_sorting1[i] == for_sorting2[i]);
        assert(for_sorting1[i - 1] == for_sorting2[i - 1]);
    }

    std::vector<int> str_vec = { 1, 2, 3, 4, 6 };
    inputed_filter<int> filter;
    erase_if(str_vec, std::ref(filter));
    
    print(str_vec.begin(), str_vec.end());

    typedef std::vector<std::vector<int>> vec_vec_int_t;
    vec_vec_int_t vec_vec_int = {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 },
        { 10, 11, 12 }
    };

    auto vec_vec_linearized_it = make_linearized_container_iterator(vec_vec_int);
    print(vec_vec_linearized_it, make_linearized_container_iterator<vec_vec_int_t>());
}

int main() {
    tests();
}
