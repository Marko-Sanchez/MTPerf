#pragma once
#include <memory>

class FunctionWrapper
{
    private:
        struct impl_base
        {
            virtual void call() = 0;
            virtual ~impl_base(){}
        };
        std::unique_ptr<impl_base> impl;

        template<typename F>
        struct impl_type: impl_base
        {
            /* task to be ran */
            F func;
            /* Constructor */
            impl_type(F&& f):func(std::move(f)) {}
            /* execute task */
            void call(){func();}
        };

    public:
        template<typename F>
        FunctionWrapper(F&& func):impl(new impl_type<F>(std::move(func))){}

        /* Accept only r-values to avoid copies */
        FunctionWrapper() = default;
        FunctionWrapper(FunctionWrapper&& other):impl(std::move(other.impl)){};
        FunctionWrapper& operator=(FunctionWrapper&& other)
        {
            impl = std::move(other.impl);
            return *this;
        };

        FunctionWrapper(FunctionWrapper&) = delete;
        FunctionWrapper(const FunctionWrapper&) = delete;
        FunctionWrapper& operator=(const FunctionWrapper&) = delete;

        void operator()(){impl->call();}
};
