#pragma once
#include <vector>
#include <string>
#include "strconv.h"

std::vector<std::string> split(const std::string& str, char del);

std::string strtrim(const std::string& str);

std::string ToUpper(const std::string& str);
std::string ToLower(const std::string& str);

int GetStrlen(std::string str, int handle);

template<typename func>
void Exsubstr(const std::string& raw, const std::string& name, func f) {
	if (raw.find(name) != 0) {
		return;
	}
	std::string data = strtrim(raw.substr(name.size(), raw.size() - name.size()));
	f(data);
}

namespace murmur3
{
	constexpr inline uint32_t seed = 0;

	constexpr inline uint32_t to_uint32(char const* key, size_t i = sizeof(uint32_t), uint32_t u32 = 0) { return i ? to_uint32(key, i - 1, (u32 << 8) | key[i - 1]) : u32; }

	constexpr inline uint32_t murmur3a_5(uint32_t h) { return (h * 5) + 0xe6546b64; }
	constexpr inline uint32_t murmur3a_4(uint32_t h) { return murmur3a_5((h << 13) | (h >> 19)); }
	constexpr inline uint32_t murmur3a_3(uint32_t k, uint32_t h) { return murmur3a_4(h ^ k); }
	constexpr inline uint32_t murmur3a_2(uint32_t k, uint32_t h) { return murmur3a_3(k * 0x1b873593, h); }
	constexpr inline uint32_t murmur3a_1(uint32_t k, uint32_t h) { return murmur3a_2((k << 15) | (k >> 17), h); }
	constexpr inline uint32_t murmur3a_0(uint32_t k, uint32_t h) { return murmur3a_1(k * 0xcc9e2d51, h); }
	constexpr inline uint32_t murmur3a(char const* key, size_t i, uint32_t h = seed) { return i ? murmur3a(key + sizeof(uint32_t), i - 1, murmur3a_0(to_uint32(key), h)) : h; }

	constexpr inline uint32_t murmur3b_3(uint32_t k, uint32_t h) { return h ^ k; }
	constexpr inline uint32_t murmur3b_2(uint32_t k, uint32_t h) { return murmur3b_3(k * 0x1b873593, h); }
	constexpr inline uint32_t murmur3b_1(uint32_t k, uint32_t h) { return murmur3b_2((k << 15) | (k >> 17), h); }
	constexpr inline uint32_t murmur3b_0(uint32_t k, uint32_t h) { return murmur3b_1(k * 0xcc9e2d51, h); }
	constexpr inline uint32_t murmur3b(char const* key, size_t i, uint32_t h) { return i ? murmur3b_0(to_uint32(key, i), h) : h; }

	constexpr inline uint32_t murmur3c_4(uint32_t h) { return h ^ (h >> 16); }
	constexpr inline uint32_t murmur3c_3(uint32_t h) { return murmur3c_4(h * 0xc2b2ae35); }
	constexpr inline uint32_t murmur3c_2(uint32_t h) { return murmur3c_3(h ^ (h >> 13)); }
	constexpr inline uint32_t murmur3c_1(uint32_t h) { return murmur3c_2(h * 0x85ebca6b); }
	constexpr inline uint32_t murmur3c_0(uint32_t h) { return murmur3c_1(h ^ (h >> 16)); }
	constexpr inline uint32_t murmur3c(uint32_t h, size_t len) { return murmur3c_0(h ^ len); }

}
constexpr inline uint32_t operator "" _murmur3(char const* str, size_t len) { return murmur3::murmur3c(murmur3::murmur3b(str + ((len >> 2) * sizeof(uint32_t)), len & 3, murmur3::murmur3a(str, len >> 2)), len); }