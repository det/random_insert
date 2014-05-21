#include "avl_array.hpp"
#include "bench.hpp"

template<typename T>
class avl_array_wrapper_t
{
private:
	mkr::avl_array<T> nums_;

public:
	std::size_t size()
	{
		return nums_.size();
	}

	void insert(std::size_t index, T num)
	{
		auto iter = nums_.begin() + index;
		nums_.insert(iter, num);
	}

	template<typename Functor>
	void iterate(Functor functor)
	{
		for (auto num : nums_) functor(num);
	}
};

int main(int argc, char * * argv)
{
	bench<avl_array_wrapper_t>(argc, argv);
}
