#include <iostream>
#include "SSS.hpp"
#include "Coloring.hpp"
#include <fstream>
#include <chrono>

void test_SSS() {
  SSS<3, 2> sss;
  sss.set_vertexes(4);
  sss.add_vertexes({4});

//  sss.add_all_colors(0);
//  sss.add_all_colors(1);
//  sss.add_all_colors(2);

  sss.add_all_colors();

  sss.add_constraint({{0, 0}, {2, 0}});
//  sss.add_constraint({{2, 4}});
//  sss.add_constraint({{1, 2}, {1, 1}});
//  sss.add_constraint({{0, 3}, {1, 2}, {3, 2}, {2, 1}});
  sss.add_constraint({{0, 2}, {1, 2}});
//  sss.add_constraint({{0, 0}, {1, 0}});

  sss.drop_allow_color({0, 1});
//  sss.drop_vertex(0);
//  sss.drop_2_colors_vertexes();
}

void test_Coloring() {
  ColoringSolver coloring_solver;

  coloring_solver.create_vertexes(10);
  coloring_solver.add_edge(0, {1, 4, 5});
  coloring_solver.add_edge(1, {2, 6});
  coloring_solver.add_edge(2, {3, 7});
  coloring_solver.add_edge(3, {4, 8});
  coloring_solver.add_edge(4, 9);
  coloring_solver.add_edge(5, {7, 8});
  coloring_solver.add_edge(6, {8, 9});
  coloring_solver.add_edge(7, 9);

//  coloring_solver.create_vertexes(4);
//  coloring_solver.add_edge(0, {1, 2, 3});
//  coloring_solver.add_edge(3, {1, 2});
//  coloring_solver.add_edge(1, 2);

//  coloring_solver.create_vertexes(6);
//  coloring_solver.add_edge(0, {1, 2, 4, 5});
//  coloring_solver.add_edge(1, {2, 3, 5});
//  coloring_solver.add_edge(2, {3, 4});
//  coloring_solver.add_edge(3, {4, 5});
//  coloring_solver.add_edge(4, 5);

  coloring_solver.add_all_colors();

//  Coloring coloring = coloring_solver.get_coloring();
  std::cout << "Answer: " << coloring_solver.stupid_solve() << "\n\n";
  for (auto& item: coloring_solver.coloring_) {
    std::cout << item.first << ": " << item.second << "\n";
  }
}

std::pair<ColoringSolver, bool> parse(std::istream& file) {
  ColoringSolver solver;

  size_t n, m = 0;
  file >> n >> m;

  solver.create_vertexes(n);
  solver.add_all_colors();

  Vertex v, u;
  for (size_t i = 0; i < m; ++i) {
    file >> v >> u;
    solver.add_edge(v, u);
  }

  std::string str;
  file >> str;

  return {solver, str == "fast"};
}

int main(/*int argc, char* argv[]*/) {
//  if (argc == 1) {
//    std::cerr << "Input filename";
//    return 1;
//  }
//
//  std::ifstream file(argv[1]);
  auto [solver, fast] = parse(std::cin);

  auto begin = std::chrono::high_resolution_clock::now();
  if (fast) {
    bool ans = solver.solve();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    std::cout << ans << std::endl;
    std::cout << duration.count() << std::endl;
  } else {
    bool ans = solver.stupid_solve();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    std::cout << ans << std::endl;
    std::cout << duration.count() << std::endl;
    for (auto& item: solver.coloring_) {
      std::cout << item.first << ": " << item.second << "\n";
    }
  }

  return 0;
}
