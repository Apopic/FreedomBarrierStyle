#pragma once
#include <vector>

template<class T>
std::vector<std::vector<std::reference_wrapper<T>>> Chunk(std::vector<T>& vec, int count) {
	std::vector<std::vector<std::reference_wrapper<T>>> ret;
	size_t index = 0;
	size_t size = vec.size();
	auto begin = vec.begin();
	while (index < size) {
		size_t range = count;
		if (index + count >= size) {
			range = size - index;
		}
		std::vector<std::reference_wrapper<T>> item;
		for (auto it = begin + index, start = it, end = it + range; it != end; ++it) {
			std::reference_wrapper<T> refobj = *it;
			item.push_back(refobj);
		}
		ret.push_back(item);
		index += count;
	}
	return ret;
}

template<class T>
class ChunkVectorVal {
public:
	ChunkVectorVal() {}
	ChunkVectorVal(std::vector<std::reference_wrapper<T>>& vec) { VectorSet(vec); }
	~ChunkVectorVal() { m_chunk.clear(); }

	void VectorSet(std::vector<std::reference_wrapper<T>>& vec) {
		m_chunk = vec;
	}

	auto& operator[](size_t idx) noexcept {
		return m_chunk[idx];
	}

	size_t size() const noexcept {
		return m_chunk.size();
	}

	auto begin() noexcept {
		return m_chunk.begin();
	}

	auto end() noexcept {
		return m_chunk.end();
	}

	auto& First() const noexcept {
		return m_chunk.front();
	}

	auto& Last() const noexcept {
		return m_chunk.back();
	}

private:

	std::vector<std::reference_wrapper<T>> m_chunk;
};

template<class T>
class ChunkVector {
public:
	ChunkVector() {}
	ChunkVector(std::vector<T> &vec, int count) { ChunkSet(vec, count); }
	~ChunkVector() { clear(); }

	void ChunkSet(std::vector<T>& vec, int count) {
		auto&& item = Chunk(vec, count);
		for (auto&& vec : item) {
			m_chunkvec.push_back(ChunkVectorVal<T>(vec));
		}
	}

	auto& operator[](size_t idx) noexcept {
		return m_chunkvec[idx];
	}

	size_t size() const noexcept {
		return m_chunkvec.size();
	}
	
	auto begin() noexcept {
		return m_chunkvec.begin();
	}
	
	auto end() noexcept {
		return m_chunkvec.end();
	}

	void clear() {
		m_chunkvec.clear();
	}

	template<class _chunk>
	auto& Find(bool(*match)(const _chunk&)) {
		for (auto&& item : m_chunkvec) {
			if (match(item)) {
				return item;
			}
		}
		return m_chunkvec.end();
	}

	template<class _chunk>
	auto& Find(bool(*match)(const _chunk&), size_t off) {
		if (off >= m_chunkvec.size()) {
			return m_chunkvec.end();
		}
		for (auto&& item : std::vector<ChunkVectorVal<T>>(m_chunkvec.begin() + off, m_chunkvec.end())) {
			if (match(item)) {
				return item;
			}
		}
		return m_chunkvec.end();
	}

private:

	std::vector<ChunkVectorVal<T>> m_chunkvec;

};


