//(C) 2018 fahaas
/*!
 *	\file algorithms.hpp
 *	\brief definition and implementation of algorithms used in numeric::bigint
 */
#pragma once

#include <vector>
#include <algorithm>
#include <boost/integer.hpp>

template <typename data_type, class data_container_type = std::vector<data_type>, class d_data_container_type = std::vector<boost::uint_t<sizeof(data_type) * 16>::exact>, typename size_type = std::size_t>
data_container_type mult_with_digit(const data_container_type& data, data_type digit)
{
	using d_data_type = typename boost::uint_t<sizeof(data_type) * 16>::exact;

	d_data_container_type tmp(data.size(), 0);

	for (size_type i_data = 0; i_data < data.size(); ++i_data)
		tmp[i_data] = static_cast<d_data_type>(data[i_data]) * digit;

	auto* tmp_p = reinterpret_cast<data_type*>(tmp.data());

	data_container_type result(tmp.size() + 1, 0);

	result[0] = tmp_p[0];

	bool carry = false;
	for (size_type i_res = 1, i_tmp = 2; i_res < result.size() - 1; ++i_res, i_tmp += 2) {
		result[i_res] = tmp_p[i_tmp] + tmp_p[i_tmp - 1];
		if (carry) {
			++(result[i_res]);
			if (!(result[i_res] <= tmp_p[i_tmp] || result[i_res] <= tmp_p[i_tmp - 1]))
				carry = false;
			continue;
		}
		if (result[i_res] < tmp_p[i_tmp] || result[i_res] < tmp_p[i_tmp - 1])
			carry = true;
	}

	result[result.size() - 1] = tmp_p[tmp.size() * 2 - 1];
	if (carry) {
		++(result[result.size() - 1]);
		if (result.back() < tmp_p[tmp.size() * 2 - 1]) {
			result.push_back(1);
		}
	}

	return result;
}

template <typename data_type, class data_container_type = std::vector<data_type>>
data_container_type rshift(const data_container_type& data, data_type count)
{
	data_container_type tmp(data.size() + count, 0);
	std::copy(data.begin(), data.end(), tmp.begin() + count);
	return tmp;
}

//requires sorted input
template <typename data_type, class data_container_type = std::vector<data_type>, typename size_type = std::size_t>
void sum_vec(const std::vector<data_container_type>& vec, std::vector<data_type>& result)
{
	result.resize(vec.back().size());

	std::copy(vec.back().begin(), vec.back().end(), result.begin());

	for (size_type i = vec.size() - 2; i < vec.size(); --i) {
		bool carry = false;
		for (size_type j = 0; j < vec[i].size(); ++j) {
			result[j] += vec[i][j];
			if (carry) {
				++(result[j]);
				if (result[j] > vec[i][j])
					carry = false;
				continue;
			}
			if (result[j] < vec[i][j])
				carry = true;
		}

		if (carry) {
			for (size_type j = vec[i].size(); j < result.size(); ++j) {
				if (result[j] != std::numeric_limits<data_type>::max()) {
					++result[j];
					carry = false;
					break;
				}
				result[j] = 0;
			}

			if (carry)
				result.push_back(1);
		}
	}
}