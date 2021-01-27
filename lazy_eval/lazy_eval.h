#include <mutex>

template <typename F>
class lazy_eval
{
private:
	F m_computation;
	mutable decltype(m_computation()) m_cache;
	mutable std::once_flag m_value_flag;

public:
	template<typename U>
	lazy_eval(U&& computation)
	: m_computation(std::forward<U>(computation))
	{
		// empty
	}

	operator const decltype(m_computation())& () const
	{
		std::call_once(m_value_flag, [this] {
			m_cache = m_computation();
		});

		return m_cache;
	}
};

template<typename F> lazy_eval(F) -> lazy_eval<F>;
