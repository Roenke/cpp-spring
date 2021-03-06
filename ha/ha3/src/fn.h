#pragma once
#include <stdexcept>
#include <memory>
#include <tuple>
#include <cstddef>

namespace fn {
    namespace inner {
        template <typename T, T... I>
        struct integer_sequence {
        };

        template <size_t... I>
        struct index_sequence : integer_sequence<size_t, I...> {};

        template <size_t N, size_t... I>
        struct build_index_impl : build_index_impl<N - 1, N - 1, I...> {};

        template <size_t... I>
        struct build_index_impl<0, I...> : index_sequence<I...> {};

        template <typename... Ts>
        struct index_sequence_for : build_index_impl<sizeof...(Ts)> {};

        template<size_t N>
        struct placeholder {};

        template<typename>
        struct is_placeholder : std::integral_constant<size_t, 0> {};

        template<size_t N>
        struct is_placeholder<placeholder<N>> : std::integral_constant<size_t, N> {};

        template<size_t N>
        struct is_placeholder<const placeholder<N>> : std::integral_constant<size_t, N> {};

        template<typename>
        struct binder;

        template<typename ARG, bool IS_PLACEHOLDER = (is_placeholder<ARG>::value != 0)>
        struct tuple_type_extractor;

        // placehodler specialization
        template<typename ARG>
        struct tuple_type_extractor<ARG, true> {
            template<typename TUPLE, size_t INDEX = (is_placeholder<ARG>::value - 1),
            typename RES = typename std::add_rvalue_reference<typename std::tuple_element<INDEX, TUPLE>::type>::type>
            auto get(ARG const& placeholder, TUPLE const& tuple) -> RES {
                return std::forward<RES>(std::get<INDEX>(tuple));
            }
        };

        // binded argument specialization
        template<typename ARG>
        struct tuple_type_extractor<ARG, false> {
            template<typename ARGUMENT, typename TUPLE>
            auto get(ARGUMENT && arg, TUPLE const& tuple)->ARGUMENT&& {
                return std::forward<ARGUMENT>(arg);
            }
        };

        template<typename FUN, typename... BINDED_ARGS>
        struct binder<FUN(BINDED_ARGS...)> {
            template<typename... CALL_ARGS>
            explicit binder(FUN&& function, CALL_ARGS&&... args)
                : function_(std::forward<FUN>(function))
                , binded_args_(std::forward<CALL_ARGS>(args)...) {
            }

            binder(binder const& other)
                : function_(other.function_)
                , binded_args_(other.binded_args_) {
            }

            binder& operator=(binder const& other) {
                function_ = other.function_;
                binded_args_ = other.binded_args_;
                return *this;
            }

            binder& operator=(binder&& other) {
                function_ = std::move(other.function_);
                binded_args_ = std::move(other.binded_args_);
                return *this;
            }

            binder(binder&& other)
                : function_(std::move(other.function_))
                , binded_args_(std::move(other.binded_args_)) {
            }

            template<typename... ARGS, typename RES = decltype(std::declval<FUN>()(tuple_type_extractor<BINDED_ARGS>()
                .get(std::declval<BINDED_ARGS&>(), std::declval<std::tuple<ARGS...>&>())...))>
                auto operator()(ARGS&&... args) -> RES {
                return invoke<RES>(std::forward_as_tuple(std::forward<ARGS>(args)...),
                    index_sequence_for<BINDED_ARGS...>());
            }

        private:
            template<typename RES, typename... ARGS, size_t... INDICES>
            RES invoke(std::tuple<ARGS...> && args, index_sequence<INDICES...>) {
                return function_(tuple_type_extractor<BINDED_ARGS>()
                    .get(std::get<INDICES>(binded_args_), args)...);
            }

            typename std::decay<FUN>::type function_;
            std::tuple<BINDED_ARGS...> binded_args_;
        };
    }

    const inner::placeholder<1> _1{};
    const inner::placeholder<2> _2{};
    const inner::placeholder<3> _3{};
    const inner::placeholder<4> _4{};
    const inner::placeholder<5> _5{};
    const inner::placeholder<6> _6{};
    const inner::placeholder<7> _7{};

    template<typename FUN, typename... ARGS>
    auto bind(FUN&& function, ARGS&& ... args) -> decltype(inner::binder<FUN(typename std::decay<ARGS>::type...)>(std::forward<FUN>(function), std::forward<ARGS>(args)...)) {
        return inner::binder<FUN(typename std::decay<ARGS>::type...)>(std::forward<FUN>(function), std::forward<ARGS>(args)...);
    }

    // function
    struct bad_function_call : std::runtime_error {
        explicit bad_function_call(std::string const& message)
            : runtime_error(message) {
        }
    };

    template<typename>
    struct function;

    template<typename RES, typename... ARGS>
    struct function<RES(ARGS...)> {
        function() : invoker_(nullptr) {}
        function(nullptr_t) : function() {}
        function(function const& other) : invoker_(nullptr) { if (other.invoker_) { invoker_.reset(other.invoker_->clone()); } }
        function(function && other) : invoker_(std::move(other.invoker_)) {}

        template<typename F>
        function(F fun) : invoker_(new invoker<F>(fun)) {}

        function& operator=(function other) {
            swap(other); return *this;
        }

        void swap(function& other) {
            std::swap(invoker_, other.invoker_);
        }

        RES operator()(ARGS ... args) const {
            if (invoker_ == nullptr) {
                throw bad_function_call("function object is empty");
            }

            return invoker_->operator()(std::forward<ARGS>(args)...);
        }

        operator bool() const {
            return invoker_ != nullptr;
        }

    private:
        struct invoker_base {
            virtual RES operator()(ARGS... args) = 0;
            virtual invoker_base* clone() = 0;
            virtual ~invoker_base() {}
        };

        template<typename FUN>
        struct invoker : invoker_base {
        private:
            FUN function_;
        public:
            explicit invoker(FUN function)
                : function_(function) {
            }

            invoker_base* clone() override {
                return new invoker(function_);
            }

            RES operator()(ARGS ... args) override {
                return function_(args...);
            }
        };

        std::unique_ptr<invoker_base> invoker_;
    };

    template<typename T>
    void swap(function<T>& f1, function<T>& f2) {
        f1.swap(f2);
    }
}
