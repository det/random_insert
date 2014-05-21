#include "btree_array.hpp"
#include "bench.hpp"

#include <algorithm>

template<typename T>
class btree_array_wrapper_t
{
private:
	btree_array_t<T> nums_;

public:
	std::size_t size()
	{
		return nums_.size();
	}

	void insert(std::size_t index, T num)
	{
		nums_.insert(index, num);
	}

	template<typename Functor>
	void iterate(Functor functor)
	{
		nums_.iterate([=](std::uint64_t * data, std::size_t data_size)
		{
			std::for_each(data, data + data_size, [=](std::uint64_t num)
			{
				functor(num);
			});
		});
	}
};

int main(int argc, char * * argv)
{
	bench<btree_array_wrapper_t>(argc, argv);
}
