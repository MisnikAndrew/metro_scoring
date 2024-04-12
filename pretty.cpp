#include <bits/stdc++.h>
using namespace std;

bool print_all_transfers_for_debug = false;
bool print_all_rails_for_debug = false;

vector <vector<pair<int, long double>>> g;
map <int, string> idName;
map <int, int> idLine;
vector <int> line_durations;
vector <vector<long double>> dists;
map <int, string> lineName;

const int numStations = 433;
const int numLines = 23;

namespace {
  int cnt = 0;
  const long double INF = 100000;
  const long double EPS = 1e-7;
  vector <vector<long double>> floyd(vector<vector<pair<int, long double>>>& g) {
      int n = g.size();
      vector <vector<long double>> ans(n, vector <long double> (n, INF));
      for (int i = 0; i < n; i++) {
          for (auto& v : g[i]) {
              ans[i][v.first] = min(ans[i][v.first], v.second);
          }
      }
      for (int k = 0; k < n; ++k) {
          for (int i = 0; i < n; ++i) {
              for (int j = 0; j < n; ++j) {
                  if (ans[i][k] + EPS < INF && ans[k][j] + EPS < INF) {
                      ans[i][j] = min(ans[i][j], ans[i][k] + ans[k][j]);
                  }
              }
          }
      }
      return ans;
  }
  void cast_input_to_init_stations() {
    int q; string s;
    int cur = 0;
    while (cin>>q) {
      getline(cin, s);
      while (s.front() == ' ') s.erase(0, 1);
      cout<<"init_station(\"" << s<<"\", "<<q<<"); // " << cur <<"\n";
      cur++;
    }
  }

  void cast_input_to_transfers() {
    int id1, id2, w;
    while (cin >> id1 >> id2 >> w) {
      cout<<"add_transfer(" << id1 << ", " << id2 << ", " << w <<");\n";
    }
  }

  void cast_input_to_rails() {
    int id1, id2, w;
    while (cin >> id1 >> id2 >> w) {
      cout<<"add_rail(" << id1 << ", " << id2 << ", " << w <<");\n";
    }
  }
}

int get_fictive(int id) {
  return id + numStations;
}

int get_real(int fictiveId) {
  return fictiveId - numStations;
}

void init_station(string name, int lineId) {
  idName[cnt] = name;
  idLine[cnt] = lineId;
  g[cnt].push_back({get_fictive(cnt), line_durations[lineId] / 2});
  g[get_fictive(cnt)].push_back({cnt, line_durations[lineId] / 2});
  cnt++;
}

void init_line(int lineId, int interval, string name) {
  line_durations[lineId] = interval;
  lineName[lineId] = name;
}

string readable_name(int stationId) {
  return idName[stationId] + " (" + lineName[idLine[stationId]] + ")";
}

void add_transfer(int id1, int id2, int duration) {
  if (print_all_transfers_for_debug) {
    cout << "Переход | " << readable_name(id1) << " | " << readable_name(id2) << " | " << duration << " минут\n";
  }
  g[id1].push_back({id2, duration});
  g[id2].push_back({id1, duration});
  g[get_fictive(id1)].push_back({id2, duration});
  g[get_fictive(id2)].push_back({id1, duration});
}

void add_rail(int id1, int id2, int duration) {
  if (print_all_rails_for_debug) {
    cout << "Путь | " << readable_name(id1) << " | " << readable_name(id2) << " | " << duration << " минут\n";
  }
  g[get_fictive(id1)].push_back({get_fictive(id2), duration});
  g[get_fictive(id2)].push_back({get_fictive(id1), duration});
}

void calc_dists() {
  dists = floyd(g);
}

long double dist(int id1, int id2) {
  return min(dists[id1][id2], dists[id1][get_fictive(id2)]);
}

void print_dist(int id1, int id2) {
  cout << "Расстояние между " << readable_name(id1) << " и " << readable_name(id2) << " равно " << dist(id1, id2) << " мин.\n";
}

void print_up_to(int num) {
  for (int i = 0; i < num; ++i) {
    for (int j = i + 1; j < num; ++j) {
      print_dist(i, j);
    }
  }
}

void print_all() {
  print_up_to(cnt);
}

void gen_sequence_for_rails() {
  int start, finish;
  cin >> start >> finish;
  for (int i = start; i + 1 <= finish; ++i) {
    cout << i << " " << i + 1 << "\n";
  }
}

bool check_if_connected(int id1, int id2) {
  return (dist(id1, id2) + EPS < INF);
}

int count_connected_pairs() {
  int ans = 0;
  for (int id1 = 0; id1 < cnt; ++id1) {
    for (int id2 = id1 + 1; id2 < cnt; ++id2) {
      ans += check_if_connected(id1, id2);
    }
  }
  return ans;
}

int count_unconnected_pairs() {
  return (cnt * (cnt - 1) ) / 2 - count_connected_pairs();
}

void print_connection_stats() {
  cout << "Достижимо: " << count_connected_pairs() <<" пар, недостижимо: " << count_unconnected_pairs() << " пар\n";
}

vector <pair<long double, int>> get_dist_top_list(int iterations = 1) {
  vector <long double> w(cnt, 1);
  vector <pair<long double, int>> ans;
  for (int i = 0; i < iterations; ++i) {
    ans.clear();
    for (int i = 0; i < cnt; ++i) {
      long double sum = 0;
      for (int j = 0; j < cnt; ++j) {
        sum += dist(i, j) * w[j];
      }
      ans.push_back({sum, i});
    }
    sort(ans.begin(), ans.end());
    long double best = ans[0].first;
    for (auto& c : ans) {
      w[c.second] = best / c.first;
    }
  }
  return ans;
}

void print_top_results(vector <pair<long double, int>>& ans, int top, bool best) {
  if (best) {
    cout << top << " лучших станций Московского метрополитена:\n";
  } else {
    cout << top << " худших станций Московского метрополитена:\n";
  }
  for (int i = 0; i < min(top, cnt); ++i) {
    cout << "№ " << i + 1 << ": " << readable_name(ans[i].second) << " | суммарное время = " << ans[i].first << " минут | среднее время = " << ans[i].first / cnt << " минут\n";
  }
  cout << "\n\n";
}

void get_best_stations(int to_print, int iterations = 1) {
  vector <pair<long double, int>> ans = get_dist_top_list(iterations);
  print_top_results(ans, to_print, 1);
}

void get_worst_stations(int to_print, int iterations = 1) {
  vector <pair<long double, int>> ans = get_dist_top_list(iterations);
  reverse(ans.begin(), ans.end());
  print_top_results(ans, to_print, 0);
}

void init_all_lines() {
// NOT IN PRETTY VERSION
}

void init_all_stations() {
// NOT IN PRETTY VERSION
}

void add_all_transfers() {
// NOT IN PRETTY VERSION
}

void add_all_rails() {
// NOT IN PRETTY VERSION
}

signed main() {
  g.resize(2 * numStations);
  line_durations.resize(numLines);

  init_all_lines();
  init_all_stations();
  add_all_transfers();
  add_all_rails();

  // gen_sequence_for_rails();
  // cast_input_to_rails();

  calc_dists();
  print_connection_stats();

  get_best_stations(10, 5);
  get_worst_stations(10, 5);
  return 0;
}
