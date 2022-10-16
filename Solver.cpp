#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <format>


int occupied[4][4][4];

struct Point {
	int x, y, z;
public:
	bool valid() const {
		return 0 <= x && x < 4 && 0 <= y && y < 4 && 0 <= z && z < 4;
	}
	void occupy(int segment) const {
#ifdef _DEBUG
		_ASSERT(valid());
		_ASSERT(occupied[x][y][z] == 0);
#endif
		occupied[x][y][z] = segment;
	}
	void release() const {
#ifdef _DEBUG
		_ASSERT(valid());
		_ASSERT(occupied[x][y][z] != 0);
#endif
		occupied[x][y][z] = 0;
	}
	bool is_free() const {
#ifdef _DEBUG
		_ASSERT(valid());
#endif
		return occupied[x][y][z] == 0;
	}
	Point operator+(const Point& rhs) const {
#ifdef _DEBUG
		_ASSERT(valid());
#endif
		return { x + rhs.x, y + rhs.y, z + rhs.z };
	}
	Point operator*(const int& rhs) const {
		return { x * rhs, y * rhs, z * rhs };
	}
	std::vector<Point> get_orthogonal() {
		if (x != 0) return { { 0,1,0 }, { 0,-1,0 }, { 0,0,1 }, { 0,0,-1 } };
		if (y != 0) return { { 1,0,0 }, { -1,0,0 }, { 0,0,1 }, { 0,0,-1 } };
		if (z != 0) return { { 1,0,0 }, { -1,0,0 }, { 0,1,0 }, { 0,-1,0 } };
		_ASSERT(false);
		exit(-1);
	}
	std::string str() const {
#ifdef _DEBUG
		_ASSERT(valid());
#endif
		return std::format("({0},{1},{2})", x, y, z);
	}
};

const std::vector<int> segments = {1, 2, 1, 2, 1, 1, 3, 1, 2, 1,
								   2, 1, 2, 1, 1, 1, 1, 1, 1, 1,
								   1, 2, 2, 1, 1, 1, 1, 1, 2, 3,
								   1, 1, 1, 3, 1, 2, 1, 1, 1, 1,
								   1, 1, 1, 1, 1, 3, 1};
const std::vector<Point> start_locations = { {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}, {1,1,1} };


int high_score = 0;
bool extend_solution(std::vector<Point>& solution_array, Point heading, int segment_number) {
	int segment_length = segments[segment_number - 1];
	Point start_point = solution_array[segment_number - 2];

#ifdef _DEBUG
	if (segment_number > high_score) {
		high_score = segment_number;
		std::cout << high_score << " " << segment_length << std::endl;
	}
#endif

	for (int i = 1; i <= segment_length; i++) {
		if (!(start_point + heading * i).valid() || !(start_point + heading * i).is_free())
			return false;
	}
	// Path clear, now claim it
	for (int i = 1; i <= segment_length; i++) {
		(start_point + heading * i).occupy(segment_number);
	}
	// Add to solution
	solution_array[segment_number - 1] = start_point + heading * segment_length;

	// Check if that was the last segment
	if (segment_number == (int)segments.size())
		return true;

	// Extend solution recursively
	std::vector<Point> headings = heading.get_orthogonal();
	for (Point new_heading : headings) {
		if (extend_solution(solution_array, new_heading, segment_number + 1))
			return true;
	}

	// Solution is not correct, backtrack
	for (int i = 1; i <= segment_length; i++) {
		(start_point + heading * i).release();
	}
	return false;
}


int main() {
	std::vector<Point> solution((int)segments.size());
	const std::vector<Point> headings = { {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1} };
	
	bool found_solution = false;
	for (int start_index = 0; !found_solution && start_index < (int)segments.size(); start_index++) {
		memset(occupied, 0, sizeof(occupied));
		Point start = start_locations[start_index];
#ifdef _DEBUG
		std::cout << "Starting point is " << start.str() << std::endl;
#endif
		solution[0] = start;
		start.occupy(1);
		for (int heading_index = 0; !found_solution && heading_index < (int)headings.size(); heading_index++) {
			found_solution = extend_solution(solution, headings[heading_index], 2);
		}
	}

	if (found_solution) {
		std::cout << "Found a solution! Point path is:" << std::endl;
		std::cout << solution[0].str();
		for (int ii = 1; ii < (int)segments.size(); ii++) {
			std::cout << " " << solution[ii].str();
		}
		std::cout << std::endl << std::endl;
		std::cout << "Printing board:" << std::endl;
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				for (int z = 0; z < 4; z++) {
					std::cout << (char)('a' - 1 + occupied[x][y][z]);
				}
				std::cout << "    ";
			}
			std::cout << std::endl;
		}
	} else {
		std::cout << "Did not find a solution!" << std::endl;
	}

	return 0;
}