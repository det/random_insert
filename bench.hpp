#pragma once

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>

template<template<typename> class Seq>
void bench(int argc, char * * argv)
{
	std::size_t count = std::atoi(argv[1]);
	std::mt19937_64 engine;
	std::uniform_int_distribution<std::uint64_t> num_dist;
	Seq<std::uint64_t> nums;

	// Insert count random integers
	for (std::size_t i = 0; i != count; ++i)
	{
		std::uniform_int_distribution<std::size_t> index_dist(0, nums.size());
		auto index = index_dist(engine);
		auto num = num_dist(engine);
		nums.insert(index, num);
	}

	// Run a checksum to verify result
//	std::uint64_t constexpr prime = (1ULL << 32) - 5;
//	std::uint64_t a = 1;
//	std::uint64_t b = 0;

//	nums.iterate([&](std::uint64_t num)
//	{
//		a = (a + num) % prime;
//		b = (b + a) % prime;
//	});

//	std::uint64_t total = (b << 32) | a;
//	std::cout << total << "\n";
}
