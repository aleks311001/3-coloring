//
// Created by aleks311001 on 05.01.2022.
//

#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>
#include <map>
#include <exception>

#ifndef INC_3COLORING__SSS_H_
#define INC_3COLORING__SSS_H_

using Vertex = size_t;
using Color = size_t;

struct Pair {
  Vertex vertex = -1;
  Color color = -1;
  auto operator<=>(const Pair& other) const = default;
};

class Constraint: public std::set<Pair> {
 public:
  Constraint(std::initializer_list<Pair> list): std::set<Pair>(list) {}
  Constraint(const std::set<Pair>& set): std::set<Pair>(set) {}
  Constraint(std::set<Pair>&& set): std::set<Pair>(std::move(set)) {}

  bool operator<(const Constraint& other) const {
    if (this->size() != other.size()) {
      return this->size() < other.size();
    }

    auto it_left = this->begin();
    auto it_right = other.begin();
    for (; it_left != this->end(); ++it_left, ++it_right) {
      if (*it_left != *it_right) {
        return *it_left < *it_right;
      }
    }

    return false;
  }
};
using Constraints = std::set<Constraint>;

class Constraints_iterator : public Constraints::iterator {
 public:
  Constraints_iterator(const Constraints::iterator& iterator): Constraints::iterator(iterator) {}
  Constraints_iterator(Constraints::iterator&& iterator): Constraints::iterator(std::move(iterator)) {}

  bool operator<(const Constraints_iterator& other) const {
    return **this < *other;
  }
};

template<size_t a, size_t b>
class BaseSSS {
 public:
  BaseSSS() {
    for (Color c = 0; c < a; ++c) {
      colors_.insert(c);
    }
  }

  BaseSSS(const BaseSSS&) = default;
  BaseSSS& operator=(const BaseSSS&) = default;
  BaseSSS(BaseSSS&&) noexcept = default;
  BaseSSS& operator=(BaseSSS&&) noexcept = default;

  ~BaseSSS() = default;

  size_t num_vertexes();
  size_t num_constraints();

  void add_constraint(const Constraint& constraint);
  std::vector<Constraints::iterator> get_constraints(const Pair& pair);

  void reset_vertexes();
  void set_vertexes(size_t num);
  void add_vertexes(const std::set<Vertex>& vertexes);
  void drop_vertex(Vertex vertex, bool drop_constraints = true);

  void reset_colors();
  void reset_colors(Vertex vertex);
  void add_color(const Pair& pair);
  void add_all_colors(Vertex vertex);
  void add_all_colors();
  void set_allow_colors(const std::map<Vertex, std::set<Color>>& allowed_colors) {
    allowed_colors_ = allowed_colors;
  }
  void drop_allow_color(const Pair& pair, bool drop_constraints = true);
  bool is_allow_color(const Pair& pair) {
    return allowed_colors_[pair.vertex].contains(pair.color);
  }

  std::set<Color> get_allow_color(Vertex vertex) {
    return allowed_colors_[vertex];
  }

 protected:
  std::set<Color> colors_;
  std::set<Vertex> vertexes_;
  std::map<Vertex, std::set<Color>> allowed_colors_;
  Constraints constraints_;
};

template<size_t a, size_t b>
size_t BaseSSS<a, b>::num_vertexes() {
  return vertexes_.size();
}
template<size_t a, size_t b>
size_t BaseSSS<a, b>::num_constraints() {
  return constraints_.size();
}

template<size_t a, size_t b>
void BaseSSS<a, b>::add_constraint(const Constraint &constraint) {
  if (constraint.size() > b) {
    std::string message = "Size of constraint = " + std::to_string(constraint.size()) + " > " + std::to_string(b) + " = b";
    throw std::overflow_error(message);
  }

  std::set<Vertex> constraint_vertexes;
  for (auto& pair: constraint) {
    if (!vertexes_.contains(pair.vertex)) {
      std::string message = "Vertex from constraint don't allowed (" + std::to_string(pair.vertex) + ")";
      throw std::range_error(message);
    }
    if (!allowed_colors_[pair.vertex].contains(pair.color)) {
      std::string message = "Color " + std::to_string(pair.color) + " don't allowed for vertex " + std::to_string(pair.vertex);
      throw std::range_error(message);
    }

    if (!constraint_vertexes.insert(pair.vertex).second) {
      return;
    }
  }

  if (constraint.size() == 1) {
    auto& pair = *constraint.begin();
//    allowed_colors_[pair.vertex].erase(pair.color);
    drop_allow_color(pair);
    return;
  }

  constraints_.insert(constraint);
}
template<size_t a, size_t b>
std::vector<Constraints::iterator> BaseSSS<a, b>::get_constraints(const Pair &pair) {
  std::vector<Constraints::iterator> result;

  for (auto it = constraints_.begin(); it != constraints_.end(); ++it) {
    if (it->contains(pair)) {
      result.push_back(it);
    }
  }

  return result;
}

template<size_t a, size_t b>
void BaseSSS<a, b>::reset_vertexes() {
  vertexes_.clear();
}
template<size_t a, size_t b>
void BaseSSS<a, b>::add_vertexes(const std::set<Vertex>& vertexes) {
  vertexes_.insert(vertexes.begin(), vertexes.end());
}
template<size_t a, size_t b>
void BaseSSS<a, b>::set_vertexes(size_t num) {
  vertexes_.clear();
  for (Vertex i = 0; i < num; ++i) {
    vertexes_.insert(i);
  }
}
template<size_t a, size_t b>
void BaseSSS<a, b>::drop_vertex(Vertex vertex, bool drop_constraints) {
  size_t n =vertexes_.erase(vertex);

  if (drop_constraints && n >= 1) {
    for (auto color: allowed_colors_[vertex]){
      auto iterators = get_constraints({vertex, color});

      for (auto& iter: iterators) {
        constraints_.erase(iter);
      }
    }
  }

  allowed_colors_.erase(vertex);
}

template<size_t a, size_t b>
void BaseSSS<a, b>::reset_colors() {
  allowed_colors_.clear();
}
template<size_t a, size_t b>
void BaseSSS<a, b>::reset_colors(Vertex vertex) {
  allowed_colors_[vertex].clear();
}
template<size_t a, size_t b>
void BaseSSS<a, b>::add_color(const Pair& pair) {
  if (!vertexes_.contains(pair.vertex)) {
    std::string message = "Vertexes set doesn't contain vertex " + std::to_string(pair.vertex);
    throw std::overflow_error(message);
  }

  allowed_colors_[pair.vertex].insert(pair.color);
  if (allowed_colors_[pair.vertex].size() > a) {
    std::string message = "Size of allowed colors set of vertex " + std::to_string(pair.vertex) + " = " +
                          std::to_string(allowed_colors_[pair.vertex].size()) + " > " + std::to_string(a) + " = a";
    throw std::overflow_error(message);
  }
}
template<size_t a, size_t b>
void BaseSSS<a, b>::add_all_colors(Vertex vertex) {
  for (auto color: colors_) {
    allowed_colors_[vertex].insert(color);
  }
}
template<size_t a, size_t b>
void BaseSSS<a, b>::add_all_colors() {
  for (auto vertex: vertexes_) {
    add_all_colors(vertex);
  }
}
template<size_t a, size_t b>
void BaseSSS<a, b>::drop_allow_color(const Pair &pair, bool drop_constraints) {
  size_t n = allowed_colors_[pair.vertex].erase(pair.color);

  if (drop_constraints && n >= 1) {
    auto iterators = get_constraints(pair);

    for (auto& iter: iterators) {
      constraints_.erase(iter);
    }
  }
}

template<size_t a, size_t b>
class SSS: public BaseSSS<a, b> {};


template<size_t a>
class BaseColoringSSS: public BaseSSS<a, 2> {
 public:
  void drop_2_colors_vertexes(Vertex vertex) {
    auto set_colors = this->allowed_colors_[vertex];
    auto colors = std::vector(set_colors.begin(), set_colors.end());

    if (colors.size() != 2) {
      return;
    }

    Pair pair_R = Pair{vertex, colors[0]};
    Pair pair_G = Pair{vertex, colors[1]};

    auto constraints_R = this->get_constraints(pair_R);
    auto constraints_G = this->get_constraints(pair_G);

    std::set<Pair> disable_colors;
    for (auto it_constraint_R: constraints_R) {
      for (auto it_constraint_G: constraints_G) {
        auto pair_R2 = get_other_pair(*it_constraint_R, pair_R);
        auto pair_G2 = get_other_pair(*it_constraint_G, pair_G);

        if (pair_R2 != pair_G2) {
          this->add_constraint({pair_R2, pair_G2});
        } else {
          disable_colors.insert(pair_R2);
        }
      }
    }

    for (auto& pair_dis: disable_colors) {
      this->drop_allow_color(pair_dis);
    }

    this->drop_vertex(vertex);
  }

  void drop_2_colors_vertexes() {
    auto copy_vertexes = this->vertexes_;
    for (auto vertex: copy_vertexes) {
      drop_2_colors_vertexes(vertex);
    }
  }

 protected:
  static Pair get_other_pair(Constraint constraint, const Pair& pair) {
    constraint.erase(pair);
    return *constraint.begin();
  }
};


template<size_t a>
class SSS<a, 2>: public BaseColoringSSS<a> {};

template<>
class SSS<3, 2>: public BaseColoringSSS<3> {
 public:
  bool solve() {
    for (auto& item: allowed_colors_) {
      if (item.second.empty()) {
        return false;
      }
    }

    drop_2_colors_vertexes();

    recalculate_pair_maps_();

    for (auto& item: pair_vertexes_constr_) {
      if (item.second.size() >= 3) {
        return case_3_different_vertexes_(item.first);
      }
    }

    for (auto& item: pair_constraints_) {
      if (item.second.size() == 1) {
        return case_only_1_constraint_(item.first);
      }

      if (item.second.empty()) {
        return case_0_constraint_(item.first);
      }
    }

    for (auto& item: pair_constraints_) {
      if (item.second.size() >= 3) {
        return case_3_different_constraints_(item.first);
      }
    }

    return case_2_different_constraints_();
  }

 private:
  static Pair get_other_pair(Constraint constraint, const Pair& pair) {
    constraint.erase(pair);
    return *constraint.begin();
  }

  template<class Iterable>
  std::pair<Color, Color> unpack_two_elems(const Iterable& container) {
    auto iter = container.begin();
    auto c1 = *iter;
    ++iter;
    auto c2 = *iter;

    return {c1, c2};
  }

  std::pair<Pair, Pair> get_adj_pairs(const Pair& pair) {
    auto colors = allowed_colors_[pair.vertex];// colors_;
    colors.erase(pair.color);

    auto [c1, c2] = unpack_two_elems(colors);
    return {{pair.vertex, c1}, {pair.vertex, c2}};
  }
  Pair get_adj_pair(const Pair& pair1, const Pair& pair2) {
    auto colors = allowed_colors_[pair1.vertex];;
    colors.erase(pair1.color);
    colors.erase(pair2.color);

    auto c = *colors.begin();
    return {pair1.vertex, c};
  }

  void recalculate_pair_maps_() {
    pair_constraints_.clear();
    pair_vertexes_constr_.clear();

    for (auto it = constraints_.begin(); it != constraints_.end(); ++it) {
      auto& constraint = *it;
//      auto pair_it = constraint.begin();

      for (auto& pair: constraint) {
        pair_constraints_[pair].insert(it);
        Vertex v = get_other_pair(constraint, pair).vertex;
        pair_vertexes_constr_[pair].insert(v);
      }
    }
  }

  //-----------------------------------------------

  bool case_3_different_vertexes_(const Pair& pair) {
    auto copy = *this;

    for (auto& it: pair_constraints_[pair]) {
      for (auto& p: *it) {
        if (p != pair) {
          copy.drop_allow_color(p);
        }
      }
    }
    copy.drop_vertex(pair.vertex);
    copy.drop_2_colors_vertexes();

    if (copy.solve()) {
      coloring = std::move(copy.coloring);
      coloring[pair.vertex] = pair.color;
      return true;
    }

    drop_allow_color(pair);
    drop_2_colors_vertexes();

    return solve();
  }

  //-----------------------------------------------

  bool case_2_a(const Pair& pair_v, const Pair& pair_w, const Pair& pair_x) {
    auto copy = *this;
    copy.drop_allow_color(pair_v);
    copy.drop_allow_color(pair_x);
    copy.drop_vertex(pair_w.vertex);
    copy.drop_2_colors_vertexes();

    if (copy.solve()) {
      coloring = std::move(copy.coloring);
      coloring[pair_w.vertex] = pair_w.color;
      return true;
    }

    drop_allow_color(pair_w);
    drop_vertex(pair_v.vertex);
    drop_2_colors_vertexes();

    bool ans = solve();
    coloring[pair_v.vertex] = pair_v.color;
    return ans;
  }
  bool case_2_b(const Pair& pair_x) {
    drop_allow_color(pair_x);
    drop_2_colors_vertexes();
    return solve();
  }

  bool case_2_c_i(const Pair& pair_v, const Pair& pair_w, const Pair& adj_pair, const std::set<Pair>& adj_pairs) {
    Pair pair3 = {};
    for (auto& iter: pair_constraints_[adj_pair]) {
      pair3 = get_other_pair(*iter, adj_pair);
      if (!adj_pairs.contains(pair3)) {
        break;
      }
    }

    auto copy = *this;

    copy.drop_allow_color(pair3);
    copy.drop_vertex(adj_pair.vertex);
    copy.drop_vertex(adj_pair.vertex == pair_v.vertex ? pair_w.vertex : pair_v.vertex);
    copy.drop_2_colors_vertexes();

    if (copy.solve()) {
      coloring = std::move(copy.coloring);
      if (adj_pair.vertex == pair_v.vertex) {
        coloring[pair_w.vertex] = pair_w.color;
      } else {
        coloring[pair_v.vertex] = pair_v.color;
      }
      coloring[adj_pair.vertex] = adj_pair.color;

      return true;
    }

    drop_allow_color(adj_pair);
    drop_vertex(pair3.vertex);
    drop_2_colors_vertexes();

    bool ans = solve();
    coloring[pair3.vertex] = pair3.color;
    return ans;
  }
  bool case_2_c_ii(const Pair& pair_v, const Pair& pair_w, const Pair& adj_pair) {
    drop_vertex(pair_v.vertex);
    drop_vertex(pair_w.vertex);

    bool ans = solve();

    coloring[adj_pair.vertex] = adj_pair.color;
    if (adj_pair.vertex == pair_v.vertex) {
      coloring[pair_w.vertex] = pair_w.color;
    } else {
      coloring[pair_v.vertex] = pair_v.color;
    }

    return ans;
  }
  bool case_2_c_iii(const Pair& pair_v, const Pair& pair_w, const std::set<Pair>& adj_pairs) {
    std::map<Vertex, std::set<Color>> possible_colors;

    for (auto& adj_pair: adj_pairs) {
      std::vector<Pair> pairs_x;
      for (auto &it: pair_constraints_[adj_pair]) {
        auto other_pair = get_other_pair(*it, adj_pair);
        if (!adj_pairs.contains(other_pair)) {
          pairs_x.push_back(other_pair);
        }
      }

      if (pairs_x.size() == 3) {
        if (pairs_x[0].vertex != pairs_x[1].vertex || pairs_x[0].vertex != pairs_x[2].vertex) {
          std::cerr << "They must be equal (3)!" << std::endl;
        }

        drop_allow_color(adj_pair);
        drop_2_colors_vertexes();

        return solve();
      }
      if (pairs_x.size() != 2) {
        std::cerr << "Impossible way!" << std::endl;
      }
      if (pairs_x[0].vertex != pairs_x[1].vertex) {
        std::cerr << "They must be equal (2)!" << std::endl;
      }

      auto colors = this->allowed_colors_[pairs_x[0].vertex];
      colors.erase(pairs_x[0].color);
      colors.erase(pairs_x[1].color);

      possible_colors[pairs_x[0].vertex].insert(*colors.begin());
    }

    if (possible_colors.size() > 2) {
      std::cerr << "Size must be <= 2" << std::endl;
    }

    if (possible_colors.size() == 1) {
      auto [v, set_colors] = *possible_colors.begin();
      for (auto color: this->allowed_colors_[v]) {
        if (!set_colors.contains(color)) {
          drop_allow_color({v, color});
        }
      }

      drop_2_colors_vertexes();
    }

    if (possible_colors.size() == 2) {
      auto it = possible_colors.begin();
      auto [v1, set_colors1] = *it;
      ++it;
      auto [v2, set_colors2] = *it;

      for (auto color1: this->allowed_colors_[v1]) {
        if (set_colors1.contains(color1)) {
          continue;
        }
        for (auto color2: this->allowed_colors_[v2]) {
          if (set_colors2.contains(color2)) {
            continue;
          }

          add_constraint({{v1, color1}, {v2, color2}});
        }
      }
    }

    drop_vertex(pair_v.vertex);
    drop_vertex(pair_w.vertex);

    return solve();
  }

  bool case_2_c(const Pair& pair_v, const Pair& pair_w) {
    auto[pair_B, pair_G] = get_adj_pairs(pair_v);
    auto[pair2_B, pair2_G] = get_adj_pairs(pair_w);

    add_constraint({pair_B, pair2_B});
    add_constraint({pair_G, pair2_B});
    add_constraint({pair_B, pair2_G});
    add_constraint({pair_G, pair2_G});
//    if (is_allow_color(pair_B)) {
//      adj_pairs.insert(pair_B);
//
//      if (is_allow_color(pair2_B)) {
//        add_constraint({pair_B, pair2_B});
//      }
//      if (is_allow_color(pair2_G)) {
//        add_constraint({pair_B, pair2_G});
//      }
//    }
//    if (is_allow_color(pair_G)) {
//      adj_pairs.insert(pair_G);
//
//      if (is_allow_color(pair2_B)) {
//        add_constraint({pair_G, pair2_B});
//      }
//      if (is_allow_color(pair2_G)) {
//        add_constraint({pair_G, pair2_G});
//      }
//    }
//
//    if (is_allow_color(pair2_B)) {
//      adj_pairs.insert(pair2_B);
//    }
//    if (is_allow_color(pair2_G)) {
//      adj_pairs.insert(pair2_G);
//    }

    recalculate_pair_maps_();

//    if (adj_pairs.empty()) {
//      return false;
//    }

    auto adj_pairs = std::set<Pair>({pair_B, pair_G, pair2_B, pair2_G});

    for (auto& adj_pair: adj_pairs) {
      // 2.c.i)
      if (pair_constraints_[adj_pair].size() == 3) {
        return case_2_c_i(pair_v, pair_w, adj_pair, adj_pairs);
      }

      // 2.c.ii)
      if (pair_constraints_[adj_pair].size() == 2) {
        return case_2_c_ii(pair_v, pair_w, adj_pair);
      }

      // part 2.c.iii)
      if (pair_vertexes_constr_[adj_pair].size() >= 3) {
        return case_3_different_vertexes_(adj_pair);
      }
    }

    // 2.c.iii)
    return case_2_c_iii(pair_v, pair_w, adj_pairs);
  }

  bool case_only_1_constraint_(const Pair& pair) {
    auto constr = **pair_constraints_[pair].begin();
    auto pair2 = get_other_pair(constr, pair);

    for (auto& constraint: pair_constraints_[pair2]) {
      // 2.a)
      auto pair3 = get_other_pair(*constraint, pair2);
      if (pair3.vertex != pair.vertex && pair3.vertex != pair2.vertex) {
        return case_2_a(pair, pair2, pair3);
      }

      // 2.b)
      if (pair3.vertex == pair.vertex && pair3.color != pair.color) {
        return case_2_b(pair3);
      }
    }

    // 2.c)
    return case_2_c(pair, pair2);
  }

  //-----------------------------------------------

  bool case_0_constraint_(const Pair& pair) {
    drop_vertex(pair.vertex);
    bool ans = solve();
    coloring[pair.vertex] = pair.color;
    return ans;
  }

  //-----------------------------------------------

  bool case_3_a(const Pair& pair) {
    drop_allow_color(pair);
    drop_2_colors_vertexes();

    return solve();
  }
  bool case_3_b(const Pair& pair, const std::vector<Pair>& pairs) {
    Pair pair_w, pair_x;
    if (pairs[0].vertex == pairs[1].vertex) {
      pair_w = get_adj_pair(pairs[0], pairs[1]);
      pair_x = pairs[2];
    }
    if (pairs[0].vertex == pairs[2].vertex) {
      pair_w = get_adj_pair(pairs[0], pairs[2]);
      pair_x = pairs[1];
    }
    if (pairs[1].vertex == pairs[2].vertex) {
      pair_w = get_adj_pair(pairs[1], pairs[2]);
      pair_x = pairs[0];
    }

    if (pair_constraints_[pair_w].empty()) {
      drop_vertex(pair_w.vertex);

      bool ans = solve();
      coloring[pair_w.vertex] = pair_w.color;
      return ans;
    }

    auto it = pair_constraints_[pair_w].begin();
    auto pair_1 = get_other_pair(**it, pair_w);
    ++it;
    auto pair_2 = get_other_pair(**it, pair_w);

    auto copy = *this;

    copy.drop_vertex(pair_w.vertex);
    if (pair_1.vertex != pair_2.vertex) {
      copy.drop_allow_color(pair_1);
      copy.drop_allow_color(pair_2);
      copy.drop_2_colors_vertexes();

      if (copy.solve()) {
        coloring = std::move(copy.coloring);
        coloring[pair_w.vertex] = pair_w.color;
        return true;
      }
    } else {
      copy.drop_vertex(pair.vertex);
      copy.drop_allow_color(pair_x);
      copy.drop_2_colors_vertexes();

      if (copy.solve()) {
        coloring = std::move(copy.coloring);
        coloring[pair_w.vertex] = pair_w.color;
        coloring[pair.vertex] = pair.color;
        return true;
      }
    }

    drop_allow_color(pair_w);
    drop_allow_color(pair);
    drop_2_colors_vertexes();

    return solve();
  }
  bool case_3_c(const Pair& pair, const std::vector<Pair>& pairs) {
    Pair pair_w, pair_x;
    if (pairs[0].vertex == pairs[1].vertex) {
      pair_w = get_adj_pair(pairs[0], pairs[1]);
      pair_x = get_adj_pair(pairs[2], pairs[3]);
    }
    if (pairs[0].vertex == pairs[2].vertex) {
      pair_w = get_adj_pair(pairs[0], pairs[2]);
      pair_x = get_adj_pair(pairs[1], pairs[3]);
    }
    if (pairs[0].vertex == pairs[3].vertex) {
      pair_w = get_adj_pair(pairs[0], pairs[3]);
      pair_x = get_adj_pair(pairs[1], pairs[2]);
    }

    drop_vertex(pair.vertex);
    drop_vertex(pair_w.vertex);
    drop_vertex(pair_x.vertex);

    bool ans = solve();
    coloring[pair.vertex] = pair.color;
    coloring[pair_w.vertex] = pair_w.color;
    coloring[pair_x.vertex] = pair_x.color;
    return ans;
  }

  bool case_3_different_constraints_(const Pair& pair) {
    if (pair_vertexes_constr_[pair].size() == 1) {
      return case_3_a(pair);
    }

    std::vector<Pair> pairs;
    for (auto& it: pair_constraints_[pair]) {
      pairs.push_back(get_other_pair(*it, pair));
    }

    if (pair_constraints_[pair].size() == 3) {
      return case_3_b(pair, pairs);
    }

    if (pair_constraints_[pair].size() == 4) {
      return case_3_c(pair, pairs);
    }

    std::cerr << "We can't be here!" << std::endl;
    return false;
  }

  //-----------------------------------------------

  std::pair<Pair, Pair> get_two_neighbor(const Pair& pair) {
    auto it = pair_constraints_[pair].begin();
    auto pair1 = get_other_pair(**it, pair);
    ++it;
    auto pair2 = get_other_pair(**it, pair);

    return {pair1, pair2};
  }

  bool case_4_a(const std::vector<Pair>& path) {
    auto copy = *this;
    copy.drop_vertex(path[0].vertex);
    copy.drop_vertex(path[3].vertex);
    copy.drop_allow_color(path[1]);
    copy.drop_allow_color(path[2]);
    copy.drop_allow_color(path[4]);
    copy.drop_2_colors_vertexes();

    if (copy.solve()) {
      coloring = std::move(copy.coloring);
      coloring[path[0].vertex] = path[0].color;
      coloring[path[3].vertex] = path[3].color;
      return true;
    }

    auto copy2 = *this;
    copy2.drop_vertex(path[1].vertex);
    copy2.drop_allow_color(path[0]);
    copy2.drop_allow_color(path[2]);
    copy2.drop_2_colors_vertexes();

    if (copy2.solve()) {
      coloring = std::move(copy2.coloring);
      coloring[path[1].vertex] = path[1].color;
      return true;
    }

    drop_vertex(path[2].vertex);
    drop_allow_color(path[1]);
    drop_allow_color(path[3]);
    drop_2_colors_vertexes();

    bool ans = solve();
    coloring[path[2].vertex] = path[2].color;
    return ans;
  }
  bool case_4_b(const std::vector<Pair>& path) {
    auto copy = *this;
    copy.drop_vertex(path[0].vertex);
    copy.drop_vertex(path[2].vertex);
    copy.drop_allow_color(path[1]);
    copy.drop_allow_color(path[3]);
    copy.drop_2_colors_vertexes();

    if (copy.solve()) {
      coloring = std::move(copy.coloring);
      coloring[path[0].vertex] = path[0].color;
      coloring[path[2].vertex] = path[2].color;
      return true;
    }

    drop_vertex(path[1].vertex);
    drop_vertex(path[3].vertex);
    drop_allow_color(path[0]);
    drop_allow_color(path[2]);
    drop_2_colors_vertexes();

    bool ans = solve();
    coloring[path[1].vertex] = path[1].color;
    coloring[path[3].vertex] = path[3].color;
    return ans;
  }
  bool case_4_c(const std::vector<Pair>& path) {
    auto anti_pair = get_adj_pair(path[0], path[2]);
    auto [anti_pair_1, anti_pair_2] = get_two_neighbor(anti_pair);

    if (anti_pair_1.vertex != path[1].vertex) {
      add_constraint({path[1], anti_pair_1});
      return solve();
    }
    if (anti_pair_2.vertex != path[1].vertex) {
      add_constraint({path[1], anti_pair_2});
      return solve();
    }

    drop_vertex(path[1].vertex);
    drop_vertex(path[0].vertex);

    bool ans = solve();
    coloring[path[1].vertex] = path[1].color;
    coloring[anti_pair.vertex] = anti_pair.color;
    return ans;
  }
  bool case_4_d(const std::vector<Pair>& path) {
    auto copy = *this;
    copy.drop_vertex(path[1].vertex);
    copy.drop_allow_color(path[0]);
    copy.drop_allow_color(path[2]);
    copy.drop_2_colors_vertexes();

    if (copy.solve()) {
      coloring = std::move(copy.coloring);
      coloring[path[1].vertex] = path[1].color;
      return true;
    }

    drop_vertex(path[2].vertex);
    drop_allow_color(path[1]);
    drop_allow_color(path[3]);
    drop_2_colors_vertexes();

    bool ans = solve();
    coloring[path[2].vertex] = path[2].color;
    return ans;
  }
  bool case_4_e_ii(const std::vector<Pair>& path) {
    std::vector<Pair> adj_pairs;
    for (int i = 0; i < 4; ++i) {
      adj_pairs.push_back(get_adj_pair(path[i], path[i + 4]));
    }

    for (int i = 0; i < 4; ++i) {
      for (int j = i + 1; j < 4; ++j) {
        add_constraint({adj_pairs[i], adj_pairs[j]});
      }
    }

    return solve();
  }
  bool case_4_e_iii(const std::vector<Pair>& path) {
    for (int i = 0; i < 4; ++i) {
      drop_vertex(path[i].vertex);
    }

    bool ans = solve();
    coloring[path[0].vertex] = path[0].color;
    coloring[path[2].vertex] = path[2].color;
    coloring[path[5].vertex] = path[5].color;
    coloring[path[7].vertex] = path[7].color;
    return ans;
  }

  bool case_2_different_constraints_() {
    for (auto& item: pair_constraints_) {
      auto [pair1, pair2] = get_two_neighbor(item.first);
      std::vector<Pair> path({pair1, item.first, pair2});

      while (path.back().vertex != path.front().vertex && path.size() < 5) {
        auto [pair_1, pair_2] = get_two_neighbor(path.back());
        if (pair_1 != path[path.size() - 2]) {
          path.push_back(pair_1);
        } else {
          path.push_back(pair_2);
        }
      }

      if (path.size() == 5) {
        // A-B-C-D-E
        if (path.back().vertex != path.front().vertex) {
          return case_4_a(path);
        }

        // A-B-C-D - square
        if (path.back() == path.front()) {
          path.pop_back();

          return case_4_b(path);
        }

        // A-B-C-D-E, E.v = A.v
        bool is_skip = false;
        while (path.back() != path.front()) {
          auto [pair_1, pair_2] = get_two_neighbor(path.back());
          if (pair_1 != path[path.size() - 2]) {
            path.push_back(pair_1);
          } else {
            path.push_back(pair_2);
          }

          if (path.back().vertex != path[path.size() - 5].vertex) {
            is_skip = true;
            break;
          }
        }
        if (is_skip) {
          continue;
        }

        path.pop_back();

        if (path.size() == 8) {
          return case_4_e_ii(path);
        }

        if (path.size() == 12) {
          return case_4_e_iii(path);
        }

        std::cerr << "We can't be here!" << std::endl;
      }

      // A-B-C-D, A.v = D.v, A != D
      if (path.size() == 4 && path.front() != path.back()) {
        return case_4_d(path);
      }

      // A-B-C, A.v = C.v
      if (path.size() == 3) {
        return case_4_c(path);
      }
    }

    // all: A-B-C-A
    return true;
  }

  //-----------------------------------------------

  std::map<Pair, std::set<Constraints_iterator>> pair_constraints_;
  std::map<Pair, std::set<Vertex>> pair_vertexes_constr_;

 public:
  std::map<Vertex, Color> coloring;
};

#endif //INC_3COLORING__SSS_H_
