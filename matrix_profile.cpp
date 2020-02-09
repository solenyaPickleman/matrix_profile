#include <iostream>
#include <numeric> //std:transform
#include <algorithm>
#include <functional>
#include <vector>
#include <random> //std::random features

void z_norm(std::vector<float> &timeseries, std::vector<float> &znorm) {
	
	float sum = std::accumulate(timeseries.begin(), timeseries.end(), 0.0f);
	float mean = sum / timeseries.size();


	std::vector<float> diff(timeseries.size());
	std::transform(timeseries.begin(), timeseries.end(), diff.begin(), [mean](float x) { return x - mean; });
	float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	float stdev = std::sqrt(sq_sum / (timeseries.size()-1));


	for (int index = 0; index < timeseries.size(); index++) {
		znorm[index] = (timeseries[index] - mean) / stdev;
	}

}

float euclidean_distance(std::vector<float>& ts1, std::vector<float>& ts2) {

	//get all deltas
	std::vector<float> elem;
	for (int i = 0; i < ts1.size(); i++) {
		elem.push_back(std::abs(ts1[i] - ts2[i]));
	}

	//square each element
	for (int i = 0; i < elem.size(); i++) {
		elem[i] = elem[i] * elem[i];
	}


	float sum = std::accumulate(elem.begin(), elem.end(), 0.0f);
	float dist = std::sqrt(sum);
	return dist;

}


std::tuple<std::vector<float>, std::vector<int>> matrix_profile(std::vector<float> &timeseries, int window) {
	//copy of timeseries 
	std::vector<float> distances;
	std::vector<int> indices;

	////znormalize timeseries
	std::vector<float> znorm_a;
	znorm_a.resize(timeseries.size());
	z_norm(timeseries, znorm_a);
	
	std::cout << " -> Znormalized -> " <<znorm_a[3] << std::endl;

	//compute distance/index of nearest neighbor based on euclidean distance


	for (int i = 0; i < znorm_a.size() - window + 1; i++) {
		std::vector<float> seq_a(znorm_a.begin() + i, znorm_a.begin() + i + window);

		float mindist = -1.0f;
		int minindex = 0;

		for (int j = 0; j < znorm_a.size() - window + 1; j++) {
			if (i == j) {
				continue;
			}
			std::vector<float> seq_b(znorm_a.begin() + j, znorm_a.begin() + j + window);
			
			auto distance = euclidean_distance(seq_a, seq_b);

			if (mindist < 0.0f) {
				mindist = distance; 
			}

			if (distance < mindist) {
				mindist = distance;
				minindex = j;
			}			
		}


		distances.push_back(mindist);
		indices.push_back(minindex);
	}

	return std::make_tuple(distances, indices);

}


int main() {


	std::random_device mch;
	std::default_random_engine generator(mch());
	std::uniform_real_distribution<float> distribution(0.0f, 50.0f);

	auto ndistribution = std::bind(distribution, generator);

	std::vector<float> timeseries(20000);
	std::generate(timeseries.begin(), timeseries.end(), ndistribution);


	//std::vector<float> timeseries{ 0.0f, 1.0f, 3.0f, 2.0f, 9.0f, 1.0f, 14.0f, 15.0f, 1.0f, 2.0f, 2.0f, 10.0f, 7.0f };

	int window = 4;
	auto [distances, indices] = matrix_profile(timeseries, window);


	std::cout << "Hello World " << distances[0] << indices[0] <<std::endl;
	return 0;
}
