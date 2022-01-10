//
// Created by aleks311001 on 07.01.2022.
//

#ifndef INC_3COLORING__COLORING_HPP_
#define INC_3COLORING__COLORING_HPP_

#include "SSS.hpp"

struct Tree{
  Vertex root;
  std::map<Vertex, std::set<Vertex>> children_grandchildren;

  bool operator<(const Tree& other) const {
    return root < other.root;
  }
};
using Forest = std::set<Tree>;
using Coloring = std::map<Vertex, Color>;

class ColoringSolver {
 public:
  void add_vertex(Vertex vertex) {
    vertexes_.insert(vertex);
  }

  void create_vertexes(size_t n) {
    for (Vertex v = 0; v < n; ++v) {
      vertexes_.insert(v);
    }
  }

  void add_edge(Vertex v1, Vertex v2) {
    edges_[v1].insert(v2);
    edges_[v2].insert(v1);
  }

  void add_edge(Vertex v1, std::set<Vertex> vertexes) {
    for (auto v: vertexes) {
      add_edge(v1, v);
    }
  }


  void add_all_colors(Vertex vertex) {
    for (auto color: colors_) {
      allowed_colors_[vertex].insert(color);
    }
  }
  void add_all_colors() {
    for (auto vertex: vertexes_) {
      add_all_colors(vertex);
    }
  }

  bool solve() {
    add_all_colors();
    drop_2_deg_vertexes_();

    if (vertexes_.empty()) {
      return true;
    }

    make_forest_();

    auto coloring_vertex = get_coloring_vertexes_();
    auto sss = make_SSS_();
    std::vector<Color> coloring_vec;
    Coloring coloring;

    for (auto v: coloring_vertex) {
      coloring[v] = 0;
    }

    while (true) {
      bool end = true;
      for (auto& item: coloring) {
        if (item.second != 2) {
          item.second += 1;
          end = false;
          break;
        } else {
          item.second = 0;
        }
      }

      if (end) {
        break;
      }

      auto sss_copy = sss;
      set_coloring_vertexes_(coloring, sss_copy);
      if (sss_copy.solve()) {
        coloring_ = sss_copy.coloring;
        return true;
      }
    }

    return false;
  }

  bool stupid_solve() {
    auto v = *vertexes_.begin();

    if (vertexes_.size() == 1) {
      if (!allowed_colors_[v].empty()) {
        coloring_[v] = *allowed_colors_[v].begin();
        return true;
      } else {
        return false;
      }
    }

    if (allowed_colors_[v].empty()) {
      return false;
    }

    for (auto color: allowed_colors_[v]) {
      auto copy = *this;

      for (auto u: edges_[v]) {
        copy.drop_allow_color(u, color);
        if (copy.allowed_colors_[u].empty()) {
          return false;
        }
      }

      copy.drop_vertex(v);

      if (copy.stupid_solve()) {
        coloring_ = copy.coloring_;
        coloring_[v] = color;
        return true;
      }
    }

    return false;
  }

//  Coloring get_coloring() {
//    add_all_colors();
//    auto copy = *this;
//
//    Coloring coloring;
//    for (auto& v: vertexes_) {
//      copy.drop_vertex(v);
//
//      for (auto color: copy.allowed_colors_[v]) {
//        auto copy_copy = copy;
//
//        for (auto u: edges_[v]) {
//          copy_copy.drop_allow_color(u, color);
//        }
//
//        if (copy_copy.solve()) {
//          coloring[v] = color;
//          for (auto u: edges_[v]) {
//            copy.drop_allow_color(u, color);
//          }
//          break;
//        }
//      }
//
//      if (!coloring.contains(v)) {
//        break;
//      }
//    }
//
//    return coloring;
//  }

 private:
  void drop_2_deg_vertexes_() {
    bool dropped = true;

    while (dropped) {
      dropped = false;
      auto copy = edges_;

      for (auto& item: copy) {
        if (item.second.size() <= 2) {
          dropped = true;

          drop_vertex(item.first);
        }
      }
    }
  }

  void make_forest_() {
    std::set<Vertex> X;
    std::set<Vertex> Y;

    for (auto& item: edges_) {
      bool in_X = !Y.contains(item.first);

      for (auto v: item.second) {
        in_X &= !Y.contains(v);
        if (!in_X) {
          break;
        }
      }

      if (in_X) {
        X.insert(item.first);
        for (auto v: item.second) {
          Y.insert(v);
        }
      }
    }

    std::set<Vertex> Z;

    for (auto x: X) {
      Tree tree;
      tree.root = x;

      for (auto ch: edges_[x]) {
        tree.children_grandchildren.insert({ch, {}});
        for (auto gr: edges_[ch]) {
          if (!Z.contains(gr) && !Y.contains(gr) && !X.contains(gr)) {
            tree.children_grandchildren[ch].insert(gr);
            Z.insert(gr);
          }
        }
      }

      forest_.insert(tree);
    }
  }

  std::set<Vertex> get_coloring_vertexes_() {
//    std::map<Vertex, std::set<Vertex>> coloring_vertexes_;
    std::set<Vertex> coloring_vertexes;

    for (auto& tree: forest_) {
//      coloring_vertexes_.insert({tree.root, {}});

      // lemma 3.5
      if (tree.children_grandchildren.size() >= 4) {
//        coloring_vertexes_[tree.root].insert(tree.root);
        coloring_vertexes.insert(tree.root);
        for (auto& item: tree.children_grandchildren) {
          if (item.second.size() >= 3) {
//            coloring_vertexes_[tree.root].insert(item.first);
            coloring_vertexes.insert(item.first);
          }
        }

        continue;
      }

      if (tree.children_grandchildren.size() <= 2) {
        std::cerr << "We can't be here" << std::endl;
      }

      //-------------
      Vertex x, y, z;
      std::map<Vertex, size_t> number_ch_;
      for (auto& item: tree.children_grandchildren) {
        number_ch_.insert({item.first, item.second.size()});
      }
      auto it = number_ch_.begin();
      x = it->first; ++it;
      y = it->first; ++it;
      z = it->first;

      if (number_ch_[x] < number_ch_[y]) {
        std::swap(x, y);
      }
      if (number_ch_[x] < number_ch_[z]) {
        std::swap(x, z);
      }
      if (number_ch_[y] < number_ch_[z]) {
        std::swap(y, z);
      }

      // lemma 3.4
      if (number_ch_[y] >= 2) {
//        coloring_vertexes_[tree.root].insert(x);
//        coloring_vertexes_[tree.root].insert(y);
        coloring_vertexes.insert(x);
        coloring_vertexes.insert(y);

        if (number_ch_[z] >= 3) {
//          coloring_vertexes_[tree.root].insert(z);
          coloring_vertexes.insert(z);
        }

        continue;
      }

      // lemma 3.7
      if (number_ch_[x] >= 3) {
//        coloring_vertexes_[tree.root].insert(x);
        coloring_vertexes.insert(x);
        continue;
      }

      // lemma 3.6 and 3.9
//      coloring_vertexes_[tree.root].insert(tree.root);
      coloring_vertexes.insert(tree.root);
    }

//    return coloring_vertexes_;
    return coloring_vertexes;
  }

  void drop_allow_color(Vertex v, Color c) {
    allowed_colors_[v].erase(c);
  }

  void drop_vertex(Vertex vertex) {
    vertexes_.erase(vertex);

    for (auto v: edges_[vertex]) {
      edges_[v].erase(vertex);
    }
    edges_.erase(vertex);
  }

  SSS<3, 2> make_SSS_() {
    SSS<3, 2> sss;
    sss.reset_vertexes();
    sss.add_vertexes(vertexes_);
    sss.set_allow_colors(allowed_colors_);
//    sss.add_all_colors();

    for (auto& item: edges_) {
      for (auto v: item.second) {
        for (auto color: colors_) {
          if (allowed_colors_[v].contains(color) && allowed_colors_[item.first].contains(color)) {
            sss.add_constraint({{item.first, color}, {v, color}});
          }
        }
      }
    }

    return sss;
  }

  SSS<3, 2>& set_coloring_vertexes_(const Coloring& coloring, SSS<3, 2>& sss) {
    for (auto item: coloring) {
      coloring_.insert(item);

      sss.drop_vertex(item.first);
      for (auto& v: edges_[item.first]) {
        sss.drop_allow_color({v, item.second});

        if (sss.get_allow_color(v).size() == 1) {
          sss.drop_vertex(v);
          auto color = *sss.get_allow_color(v).begin();

          for (auto& u: edges_[v]) {
            sss.drop_allow_color({u, color});
          }
        }
      }
    }

    sss.drop_2_colors_vertexes();
    return sss;
  }

 private:
  std::set<Vertex> vertexes_;
  std::map<Vertex, std::set<Vertex>> edges_;

  Forest forest_;
//  std::map<Vertex, Forest::iterator> vertex_tree_;

  std::map<Vertex, std::set<Color>> allowed_colors_;
  std::set<Color> colors_ = {0, 1, 2};

 public:
  Coloring coloring_;
//  SSS<3, 2> sss_;
};

#endif //INC_3COLORING__COLORING_HPP_
