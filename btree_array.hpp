#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

template<
	typename T,
	std::size_t target_branch_size = 512,
	std::size_t target_leaf_size = 512,
	std::size_t maximum_size = std::numeric_limits<std::size_t>::max()>
class btree_array_t
{
private:
	struct node_t
	{
		std::size_t size;
		void * pointer;
	};

	// Compute the logarithm rounded up to the nearest int

	static std::size_t constexpr log(std::size_t num, std::size_t base, std::size_t result)
	{
		return num != 0 ? log(num / base, base, result + 1) : result;
	}

	static std::size_t constexpr log(std::size_t num, std::size_t base)
	{
		return log(num - 1, base, 0);
	}

	static_assert(std::is_pod<T>::value, "T must be a pod");

	static std::size_t constexpr maximum_branch_size = target_branch_size  / sizeof(node_t);
	static std::size_t constexpr maximum_leaf_size = target_leaf_size / sizeof(T);

	static_assert(maximum_branch_size >= 3, "maximum_branch_size must be at least 3");

	static std::size_t constexpr minimum_branch_size = (maximum_branch_size + 1) / 2;
	static std::size_t constexpr minimum_leaf_size = (maximum_leaf_size + 1) / 2;
	static std::size_t constexpr stack_size = log(maximum_size / minimum_leaf_size, minimum_branch_size);

	struct branch_t
	{
		node_t children[maximum_branch_size];
	};

	struct leaf_t
	{
		T buffer[maximum_leaf_size];
	};

	struct branch_entry_t
	{
		std::size_t size;
		std::size_t index;
		branch_t * pointer;
	};

	struct leaf_entry_t
	{
		std::size_t size;
		std::size_t index;
		leaf_t * pointer;
	};

	node_t root_;
	std::size_t height_;

	template<typename Functor>
	static void iterate(node_t node, std::size_t height, Functor functor)
	{
		if (height != 0)
		{
			auto branch = static_cast<branch_t *>(node.pointer);
			auto size = node.size;
			std::size_t index = 0;

			while (size != 0)
			{
				iterate(branch->children[index], height - 1, functor);
				size -= branch->children[index].size;
				++index;
			}
		}
		else
		{
			auto leaf = static_cast<leaf_t *>(node.pointer);
			functor(leaf->buffer, node.size);
		}
	}

	static std::size_t get_length(branch_t * branch, std::size_t size)
	{
		std::size_t index = 0;
		while (size != 0)
		{
			size -= branch->children[index].size;
			++index;
		}
		return index;
	}

	static void delete_node(node_t node, std::size_t height)
	{
		if (height != 0)
		{
			auto branch = static_cast<branch_t *>(node.pointer);
			std::size_t index = 0;
			auto size = node.size;

			while (size != 0)
			{
				delete_node(branch->children[index], height - 1);
				size -= branch->children[index].size;
				++index;
			}

			delete branch;
		}
		else
		{
			auto leaf = static_cast<leaf_t *>(node.pointer);
			delete leaf;
		}
	}

	template<typename Kind>
	static void merge(
		std::size_t index,
		Kind * orig, std::size_t orig_size,
		Kind value)
	{
		std::char_traits<Kind>::move(
			orig + index + 1,
			orig + index,
			orig_size - index);
		orig[index] = value;
	}

	template<typename Kind>
	static void split(
		std::size_t index,
		std::size_t left_size, std::size_t right_size, Kind * right,
		Kind * orig, std::size_t orig_size,
		Kind value)
	{
		if (index < left_size)
		{
			std::char_traits<Kind>::copy(
				right,
				orig + orig_size - right_size,
				right_size);
			std::char_traits<Kind>::move(
				orig + index + 1,
				orig + index,
				left_size - 1 - index);
			orig[index] = value;
		}
		else
		{
			std::char_traits<Kind>::copy(
				right,
				orig + left_size,
				index - left_size);
			std::char_traits<Kind>::copy(
				right + index + 1 - left_size,
				orig + index,
				orig_size - index);
			right[index - left_size] = value;
		}
	}

	leaf_entry_t seek(branch_entry_t * first, branch_entry_t * last, node_t current, std::size_t index)
	{
		while (first != last)
		{
			auto branch = static_cast<branch_t *>(current.pointer);

			std::size_t branch_index = 0;
			while (true)
			{
				auto child_size = branch->children[branch_index].size;
				if (index <= child_size) break;
				index -= child_size;
				++branch_index;
			}
			branch_entry_t entry;
			entry.size = current.size;
			entry.index = branch_index;
			entry.pointer = branch;
			*--last = entry;
			current = branch->children[branch_index];
		}

		leaf_entry_t entry;
		entry.size = current.size;
		entry.index = index;
		entry.pointer = static_cast<leaf_t *>(current.pointer);
		return entry;
	}

	void update_sizes(branch_entry_t * first, branch_entry_t * last)
	{
		while (first != last)
		{
			auto & entry = *first++;
			++entry.pointer->children[entry.index].size;
		}

		++root_.size;
	}

	void insert(
		branch_entry_t * first, branch_entry_t * last,
		T value,
		leaf_entry_t & entry)
	{
		auto sum = entry.size + 1;

		// If we have room for the data in this leaf, we are done
		if (sum <= maximum_leaf_size)
		{
			merge(entry.index, entry.pointer->buffer, entry.size, value);
			update_sizes(first, last);
			return;
		}

		// No room, split into 2 and insert the first half in the parent
		auto left_size = sum / 2;
		auto right_size = sum - left_size;
		auto right = new leaf_t;
		split(
			entry.index,
			left_size, right_size, right->buffer,
			entry.pointer->buffer, entry.size,
			value);
		insert(first, last, left_size, {right_size, right});
	}

	void insert(
		branch_entry_t * first, branch_entry_t * last,
		std::size_t left_size, node_t right_node)
	{
		while (first != last)
		{
			auto & entry = *first++;
			auto branch_length = get_length(entry.pointer, entry.size);
			auto sum = branch_length + 1;
			entry.pointer->children[entry.index].size = left_size;

			// If we have room for the child we are done
			if (sum <= maximum_branch_size)
			{
				merge(entry.index + 1, entry.pointer->children, branch_length, right_node);
				update_sizes(first, last);
				return;
			}

			// No room, split into 2 and insert the first half in the parent
			auto left_length = sum / 2;
			auto right_length = sum - left_length;
			auto right = new branch_t;
			split(
				entry.index + 1,
				left_length, right_length, right->children,
				entry.pointer->children, branch_length,
				right_node);
			std::size_t right_size = 0;
			for (std::size_t I = 0; I != right_length; ++I) right_size += right->children[I].size;
			right_node = {right_size, right};
			left_size = entry.size + 1 - right_size;
		}

		// We have reached the root, grow upward
		auto branch = new branch_t;
		branch->children[0].pointer = root_.pointer;
		branch->children[0].size = left_size;
		branch->children[1] = right_node;
		root_.pointer = branch;
		root_.size = left_size + right_node.size;
		height_++;
	}

public:
	btree_array_t()
	:
		root_{0, nullptr},
		height_{0}
	{}

	~btree_array_t()
	{
		delete_node(root_, height_);
	}

	void insert(std::size_t index, T value)
	{
		if (root_.pointer == nullptr) root_.pointer = new leaf_t;
		branch_entry_t stack[stack_size];
		auto entry = seek(stack, stack + height_, root_, index);
		insert(stack, stack + height_, value, entry);
	}

	template<typename Functor>
	void iterate(Functor functor) const
	{
		iterate(root_, height_, functor);
	}

	std::size_t size() const
	{
		return root_.size;
	}
};

