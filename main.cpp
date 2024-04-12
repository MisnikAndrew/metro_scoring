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
  init_line(1, 2, "1");
  init_line(2, 2, "2");
  init_line(3, 2, "3");
  init_line(4, 2, "4");
  init_line(5, 2, "5");
  init_line(6, 2, "6");
  init_line(7, 2, "7");
  init_line(8, 2, "8");
  init_line(9, 2, "9");
  init_line(10, 2, "10");
  init_line(11, 5, "11");
  init_line(12, 5, "12");
  init_line(13, 3, "8A");
  init_line(14, 5, "14");
  init_line(15, 4, "15");
  init_line(16, 6, "11A");
  init_line(17, 9, "D1");
  init_line(18, 9, "D2");
  init_line(19, 10, "D3");
  init_line(20, 10, "D4");
  init_line(21, 17, "D4A");
  init_line(22, 4, "4A");
}

void init_all_stations() {
  init_station("Бульвар Рокоссовского", 1); // 0
  init_station("Черкизовская", 1); // 1
  init_station("Преображенская площадь", 1); // 2
  init_station("Сокольники", 1); // 3
  init_station("Красносельская", 1); // 4
  init_station("Комсомольская", 1); // 5
  init_station("Красные Ворота", 1); // 6
  init_station("Чистые пруды", 1); // 7
  init_station("Лубянка", 1); // 8
  init_station("Охотный Ряд", 1); // 9
  init_station("Библиотека имени Ленина", 1); // 10
  init_station("Кропоткинская", 1); // 11
  init_station("Парк культуры", 1); // 12
  init_station("Фрунзенская", 1); // 13
  init_station("Спортивная", 1); // 14
  init_station("Воробьёвы горы", 1); // 15
  init_station("Университет", 1); // 16
  init_station("Проспект Вернадского", 1); // 17
  init_station("Юго-Западная", 1); // 18
  init_station("Тропарёво", 1); // 19
  init_station("Румянцево", 1); // 20
  init_station("Саларьево", 1); // 21
  init_station("Филатов Луг", 1); // 22
  init_station("Прокшино", 1); // 23
  init_station("Ольховая", 1); // 24
  init_station("Коммунарка", 1); // 25
  init_station("Ховрино", 2); // 26
  init_station("Беломорская", 2); // 27
  init_station("Речной вокзал", 2); // 28
  init_station("Водный стадион", 2); // 29
  init_station("Войковская", 2); // 30
  init_station("Сокол", 2); // 31
  init_station("Аэропорт", 2); // 32
  init_station("Динамо", 2); // 33
  init_station("Белорусская", 2); // 34
  init_station("Маяковская", 2); // 35
  init_station("Тверская", 2); // 36
  init_station("Театральная", 2); // 37
  init_station("Новокузнецкая", 2); // 38
  init_station("Павелецкая", 2); // 39
  init_station("Автозаводская", 2); // 40
  init_station("Технопарк", 2); // 41
  init_station("Коломенская", 2); // 42
  init_station("Каширская", 2); // 43
  init_station("Кантемировская", 2); // 44
  init_station("Царицыно", 2); // 45
  init_station("Орехово", 2); // 46
  init_station("Домодедовская", 2); // 47
  init_station("Красногвардейская", 2); // 48
  init_station("Алма-Атинская", 2); // 49
  init_station("Пятницкое шоссе", 3); // 50
  init_station("Митино", 3); // 51
  init_station("Волоколамская", 3); // 52
  init_station("Мякинино", 3); // 53
  init_station("Строгино", 3); // 54
  init_station("Крылатское", 3); // 55
  init_station("Молодёжная", 3); // 56
  init_station("Кунцевская", 3); // 57
  init_station("Славянский бульвар", 3); // 58
  init_station("Парк Победы", 3); // 59
  init_station("Киевская", 3); // 60
  init_station("Смоленская", 3); // 61
  init_station("Арбатская", 3); // 62
  init_station("Площадь Революции", 3); // 63
  init_station("Курская", 3); // 64
  init_station("Бауманская", 3); // 65
  init_station("Электрозаводская", 3); // 66
  init_station("Семёновская", 3); // 67
  init_station("Партизанская", 3); // 68
  init_station("Измайловская", 3); // 69
  init_station("Первомайская", 3); // 70
  init_station("Щёлковская", 3); // 71
  init_station("Кунцевская", 4); // 72
  init_station("Пионерская", 4); // 73
  init_station("Филёвский парк", 4); // 74
  init_station("Багратионовская", 4); // 75
  init_station("Фили", 4); // 76
  init_station("Кутузовская", 4); // 77
  init_station("Студенческая", 4); // 78
  init_station("Киевская", 4); // 79
  init_station("Смоленская", 4); // 80
  init_station("Арбатская", 4); // 81
  init_station("Александровский сад", 4); // 82
  init_station("Выставочная", 22); // 83
  init_station("Международная", 22); // 84
  init_station("Белорусская", 5); // 85
  init_station("Краснопресненская", 5); // 86
  init_station("Киевская", 5); // 87
  init_station("Парк культуры", 5); // 88
  init_station("Октябрьская", 5); // 89
  init_station("Добрынинская", 5); // 90
  init_station("Новослободская", 5); // 91
  init_station("Проспект Мира", 5); // 92
  init_station("Комсомольская", 5); // 93
  init_station("Курская", 5); // 94
  init_station("Таганская", 5); // 95
  init_station("Павелецкая", 5); // 96
  init_station("Медведково", 6); // 97
  init_station("Бабушкинская", 6); // 98
  init_station("Свиблово", 6); // 99
  init_station("Ботанический сад", 6); // 100
  init_station("ВДНХ", 6); // 101
  init_station("Алексеевская", 6); // 102
  init_station("Рижская", 6); // 103
  init_station("Проспект Мира", 6); // 104
  init_station("Сухаревская", 6); // 105
  init_station("Тургеневская", 6); // 106
  init_station("Китай-город", 6); // 107
  init_station("Третьяковская", 6); // 108
  init_station("Октябрьская", 6); // 109
  init_station("Шаболовская", 6); // 110
  init_station("Ленинский проспект", 6); // 111
  init_station("Академическая", 6); // 112
  init_station("Профсоюзная", 6); // 113
  init_station("Новые Черёмушки", 6); // 114
  init_station("Калужская", 6); // 115
  init_station("Беляево", 6); // 116
  init_station("Коньково", 6); // 117
  init_station("Тёплый Стан", 6); // 118
  init_station("Ясенево", 6); // 119
  init_station("Новоясеневская", 6); // 120
  init_station("Котельники", 7); // 121
  init_station("Жулебино", 7); // 122
  init_station("Лермонтовский проспект", 7); // 123
  init_station("Выхино", 7); // 124
  init_station("Рязанский проспект", 7); // 125
  init_station("Кузьминки", 7); // 126
  init_station("Текстильщики", 7); // 127
  init_station("Волгоградский проспект", 7); // 128
  init_station("Пролетарская", 7); // 129
  init_station("Таганская", 7); // 130
  init_station("Китай-город", 7); // 131
  init_station("Кузнецкий Мост", 7); // 132
  init_station("Пушкинская", 7); // 133
  init_station("Баррикадная", 7); // 134
  init_station("Улица 1905 года", 7); // 135
  init_station("Беговая", 7); // 136
  init_station("Полежаевская", 7); // 137
  init_station("Октябрьское Поле", 7); // 138
  init_station("Щукинская", 7); // 139
  init_station("Спартак", 7); // 140
  init_station("Тушинская", 7); // 141
  init_station("Сходненская", 7); // 142
  init_station("Планерная", 7); // 143
  init_station("Третьяковская", 8); // 144
  init_station("Марксистская", 8); // 145
  init_station("Площадь Ильича", 8); // 146
  init_station("Авиамоторная", 8); // 147
  init_station("Шоссе Энтузиастов", 8); // 148
  init_station("Перово", 8); // 149
  init_station("Новогиреево", 8); // 150
  init_station("Новокосино", 8); // 151
  init_station("Бульвар Дмитрия Донского", 9); // 152
  init_station("Аннино", 9); // 153
  init_station("Улица Академика Янгеля", 9); // 154
  init_station("Пражская", 9); // 155
  init_station("Южная", 9); // 156
  init_station("Чертановская", 9); // 157
  init_station("Севастопольская", 9); // 158
  init_station("Нахимовский проспект", 9); // 159
  init_station("Нагорная", 9); // 160
  init_station("Нагатинская", 9); // 161
  init_station("Тульская", 9); // 162
  init_station("Серпуховская", 9); // 163
  init_station("Полянка", 9); // 164
  init_station("Боровицкая", 9); // 165
  init_station("Чеховская", 9); // 166
  init_station("Цветной бульвар", 9); // 167
  init_station("Менделеевская", 9); // 168
  init_station("Савёловская", 9); // 169
  init_station("Дмитровская", 9); // 170
  init_station("Тимирязевская", 9); // 171
  init_station("Петровско-Разумовская", 9); // 172
  init_station("Владыкино", 9); // 173
  init_station("Отрадное", 9); // 174
  init_station("Бибирево", 9); // 175
  init_station("Алтуфьево", 9); // 176
  init_station("Физтех", 10); // 177
  init_station("Лианозово", 10); // 178
  init_station("Яхромская", 10); // 179
  init_station("Селигерская", 10); // 180
  init_station("Верхние Лихоборы", 10); // 181
  init_station("Окружная", 10); // 182
  init_station("Петровско-Разумовская", 10); // 183
  init_station("Фонвизинская", 10); // 184
  init_station("Бутырская", 10); // 185
  init_station("Марьина Роща", 10); // 186
  init_station("Достоевская", 10); // 187
  init_station("Трубная", 10); // 188
  init_station("Сретенский бульвар", 10); // 189
  init_station("Чкаловская", 10); // 190
  init_station("Римская", 10); // 191
  init_station("Крестьянская Застава", 10); // 192
  init_station("Дубровка", 10); // 193
  init_station("Кожуховская", 10); // 194
  init_station("Печатники", 10); // 195
  init_station("Волжская", 10); // 196
  init_station("Люблино", 10); // 197
  init_station("Братиславская", 10); // 198
  init_station("Марьино", 10); // 199
  init_station("Борисово", 10); // 200
  init_station("Шипиловская", 10); // 201
  init_station("Текстильщики", 11); // 202
  init_station("Печатники", 11); // 203
  init_station("Нагатинский Затон", 11); // 204
  init_station("Кленовый бульвар", 11); // 205
  init_station("Каширская", 11); // 206
  init_station("Варшавская", 11); // 207
  init_station("Каховская", 11); // 208
  init_station("Зюзино", 11); // 209
  init_station("Воронцовская", 11); // 210
  init_station("Новаторская", 11); // 211
  init_station("Проспект Вернадского", 11); // 212
  init_station("Мичуринский проспект", 11); // 213
  init_station("Аминьевская", 11); // 214
  init_station("Давыдково", 11); // 215
  init_station("Кунцевская", 11); // 216
  init_station("Терехово", 11); // 217
  init_station("Мнёвники", 11); // 218
  init_station("Нижегородская", 11); // 219
  init_station("Авиамоторная", 11); // 220
  init_station("Лефортово", 11); // 221
  init_station("Электрозаводская", 11); // 222
  init_station("Сокольники", 11); // 223
  init_station("Рижская", 11); // 224
  init_station("Марьина Роща", 11); // 225
  init_station("Савёловская", 11); // 226
  init_station("Петровский парк", 11); // 227
  init_station("ЦСКА", 11); // 228
  init_station("Хорошёвская", 11); // 229
  init_station("Народное Ополчение", 11); // 230
  init_station("Бунинская Аллея", 12); // 231
  init_station("Улица Горчакова", 12); // 232
  init_station("Бульвар Адмирала Ушакова", 12); // 233
  init_station("Улица Скобелевская", 12); // 234
  init_station("Улица Старокачаловская", 12); // 235
  init_station("Лесопарковая", 12); // 236
  init_station("Битцевский парк", 12); // 237
  init_station("Деловой Центр", 13); // 238
  init_station("Парк Победы", 13); // 239
  init_station("Минская", 13); // 240
  init_station("Ломоносовский проспект", 13); // 241
  init_station("Раменки", 13); // 242
  init_station("Мичуринский проспект", 13); // 243
  init_station("Озёрная", 13); // 244
  init_station("Говорово", 13); // 245
  init_station("Солнцево", 13); // 246
  init_station("Боровское шоссе", 13); // 247
  init_station("Новопеределкино", 13); // 248
  init_station("Рассказовка", 13); // 249
  init_station("Пыхтино", 13); // 250
  init_station("Аэропорт Внуково", 13); // 251
  init_station("Белокаменная", 14); // 252
  init_station("Бульвар Рокоссовского", 14); // 253
  init_station("Локомотив", 14); // 254
  init_station("Измайлово", 14); // 255
  init_station("Соколиная Гора", 14); // 256
  init_station("Шоссе Энтузиастов", 14); // 257
  init_station("Андроновка", 14); // 258
  init_station("Нижегородская", 14); // 259
  init_station("Новохохловская", 14); // 260
  init_station("Угрешская", 14); // 261
  init_station("Дубровка", 14); // 262
  init_station("Автозаводская", 14); // 263
  init_station("Зил", 14); // 264
  init_station("Верхние Котлы", 14); // 265
  init_station("Крымская", 14); // 266
  init_station("Площадь Гагарина", 14); // 267
  init_station("Ботанический сад", 14); // 268
  init_station("Владыкино", 14); // 269
  init_station("Окружная", 14); // 270
  init_station("Лихоборы", 14); // 271
  init_station("Коптево", 14); // 272
  init_station("Балтийская", 14); // 273
  init_station("Стрешнево", 14); // 274
  init_station("Панфиловская", 14); // 275
  init_station("Зорге", 14); // 276
  init_station("Хорошёво", 14); // 277
  init_station("Шелепиха", 14); // 278
  init_station("Деловой центр", 14); // 279
  init_station("Кутузовская", 14); // 280
  init_station("Лужники", 14); // 281
  init_station("Ростокино", 14); // 282
  init_station("Некрасовка", 15); // 283
  init_station("Лухмановская", 15); // 284
  init_station("Улица Дмитриевского", 15); // 285
  init_station("Косино", 15); // 286
  init_station("Юго-Восточная", 15); // 287
  init_station("Окская", 15); // 288
  init_station("Стахановская", 15); // 289
  init_station("Нижегородская", 15); // 290
  init_station("Лобня", 17); // 291
  init_station("Шереметьевская", 17); // 292
  init_station("Хлебниково", 17); // 293
  init_station("Водники", 17); // 294
  init_station("Долгопрудная", 17); // 295
  init_station("Новодачная", 17); // 296
  init_station("Марк", 17); // 297
  init_station("Лианозово", 17); // 298
  init_station("Бескудниково", 17); // 299
  init_station("Дегунино", 17); // 300
  init_station("Окружная", 17); // 301
  init_station("Тимирязевская", 17); // 302
  init_station("Савёловская", 17); // 303
  init_station("Белорусская", 17); // 304
  init_station("Беговая", 17); // 305
  init_station("Тестовская", 17); // 306
  init_station("Фили", 17); // 307
  init_station("Славянский бульвар", 17); // 308
  init_station("Кунцевская", 17); // 309
  init_station("Рабочий Посёлок", 17); // 310
  init_station("Сетунь", 17); // 311
  init_station("Немчиновка", 17); // 312
  init_station("Сколково", 17); // 313
  init_station("Баковка", 17); // 314
  init_station("Одинцово", 17); // 315
  init_station("Подольск", 18); // 316
  init_station("Силикатная", 18); // 317
  init_station("Остафьево", 18); // 318
  init_station("Щербинка", 18); // 319
  init_station("Бутово", 18); // 320
  init_station("Битца", 18); // 321
  init_station("Красный Строитель", 18); // 322
  init_station("Покровское", 18); // 323
  init_station("Царицыно", 18); // 324
  init_station("Москворечье", 18); // 325
  init_station("Курьяново", 18); // 326
  init_station("Перерва", 18); // 327
  init_station("Депо", 18); // 328
  init_station("Люблино", 18); // 329
  init_station("Печатники", 18); // 330
  init_station("Текстильщики", 18); // 331
  init_station("Новохохловская", 18); // 332
  init_station("Калитники", 18); // 333
  init_station("Москва-Товарная", 18); // 334
  init_station("Курская", 18); // 335
  init_station("Площадь трёх вокзалов", 18); // 336
  init_station("Рижская", 18); // 337
  init_station("Марьина Роща", 18); // 338
  init_station("Дмитровская", 18); // 339
  init_station("Гражданская", 18); // 340
  init_station("Красный Балтиец", 18); // 341
  init_station("Стрешнево", 18); // 342
  init_station("Щукинская", 18); // 343
  init_station("Тушинская", 18); // 344
  init_station("Трикотажная", 18); // 345
  init_station("Волоколамская", 18); // 346
  init_station("Пенягино", 18); // 347
  init_station("Павшино", 18); // 348
  init_station("Красногорская", 18); // 349
  init_station("Опалиха", 18); // 350
  init_station("Аникеевка", 18); // 351
  init_station("Нахабино", 18); // 352
  init_station("Зеленоград-Крюково", 19); // 353
  init_station("Фирсановская", 19); // 354
  init_station("Сходня", 19); // 355
  init_station("Подрезково", 19); // 356
  init_station("Новоподрезково", 19); // 357
  init_station("Молжаниново", 19); // 358
  init_station("Химки", 19); // 359
  init_station("Левобережная", 19); // 360
  init_station("Ховрино", 19); // 361
  init_station("Грачёвская", 19); // 362
  init_station("Моссельмаш", 19); // 363
  init_station("Лихоборы", 19); // 364
  init_station("Петровско-Разумовская", 19); // 365
  init_station("Останкино", 19); // 366
  init_station("Электрозаводская", 19); // 367
  init_station("Сортировочная", 19); // 368
  init_station("Авиамоторная", 19); // 369
  init_station("Андроновка", 19); // 370
  init_station("Перово", 19); // 371
  init_station("Плющево", 19); // 372
  init_station("Вешняки", 19); // 373
  init_station("Выхино", 19); // 374
  init_station("Косино", 19); // 375
  init_station("Ухтомская", 19); // 376
  init_station("Люберцы", 19); // 377
  init_station("Панки", 19); // 378
  init_station("Томилино", 19); // 379
  init_station("Красково", 19); // 380
  init_station("Малаховка", 19); // 381
  init_station("Удельная", 19); // 382
  init_station("Быково", 19); // 383
  init_station("Ильинская", 19); // 384
  init_station("Отдых", 19); // 385
  init_station("Кратово", 19); // 386
  init_station("Есенинская", 19); // 387
  init_station("Фабричная", 19); // 388
  init_station("Раменское", 19); // 389
  init_station("Ипподром", 19); // 390
  init_station("Апрелевка", 20); // 391
  init_station("Победа", 20); // 392
  init_station("Крёкшино", 20); // 393
  init_station("Санино", 20); // 394
  init_station("Кокошкино", 20); // 395
  init_station("Толстопальцево", 20); // 396
  init_station("Лесной Городок", 20); // 397
  init_station("Внуково", 20); // 398
  init_station("Мичуринец", 20); // 399
  init_station("Переделкино", 20); // 400
  init_station("Солнечная", 20); // 401
  init_station("Мещерская", 20); // 402
  init_station("Очаково", 20); // 403
  init_station("Аминьевская", 20); // 404
  init_station("Матвеевская", 20); // 405
  init_station("Минская", 20); // 406
  init_station("Поклонная", 20); // 407
  init_station("Кутузовская", 20); // 408
  init_station("Тестовская", 20); // 409
  init_station("Белорусская", 20); // 410
  init_station("Савёловская", 20); // 411
  init_station("Марьина Роща", 20); // 412
  init_station("Площадь трёх вокзалов", 20); // 413
  init_station("Курская", 20); // 414
  init_station("Серп и Молот", 20); // 415
  init_station("Нижегородская", 20); // 416
  init_station("Чухлинка", 20); // 417
  init_station("Кусково", 20); // 418
  init_station("Новогиреево", 20); // 419
  init_station("Реутов", 20); // 420
  init_station("Никольское", 20); // 421
  init_station("Салтыковская", 20); // 422
  init_station("Кучино", 20); // 423
  init_station("Ольгино", 20); // 424
  init_station("Железнодорожная", 20); // 425
  init_station("Солнечная", 21); // 426
  init_station("Новопеределкино", 21); // 427
  init_station("Казанский вокзал", 19); // 428
  init_station("Ленинградский вокзал", 19); // 429
  init_station("Зябликово", 10); // 430
  init_station("Шелепиха", 16); // 431
  init_station("Деловой центр", 16); // 432
}

void add_all_transfers() {
  add_transfer(0, 253, 10);
  add_transfer(1, 254, 6);
  add_transfer(3, 223, 4);
  add_transfer(5, 428, 7);
  add_transfer(5, 429, 8);
  add_transfer(5, 93, 5);
  add_transfer(7, 106, 3);
  add_transfer(7, 189, 4);
  add_transfer(8, 132, 4);
  add_transfer(9, 37, 5);
  add_transfer(37, 63, 5);
  add_transfer(9, 63, 11);
  add_transfer(10, 82, 3);
  add_transfer(10, 165, 6);
  add_transfer(10, 62, 5);
  add_transfer(12, 88, 5);
  add_transfer(14, 281, 8);
  add_transfer(17, 212, 4);
  add_transfer(26, 361, 10);
  add_transfer(30, 273, 14);
  add_transfer(30, 274, 18);
  add_transfer(30, 342, 14);
  add_transfer(33, 227, 6);
  add_transfer(34, 85, 3);
  add_transfer(34, 410, 8);
  add_transfer(34, 304, 8);
  add_transfer(36, 166, 4);
  add_transfer(36, 133, 3);
  add_transfer(38, 108, 3);
  add_transfer(38, 144, 3);
  add_transfer(39, 96, 4);
  add_transfer(40, 263, 11);
  add_transfer(43, 206, 2);
  add_transfer(45, 324, 8);
  add_transfer(48, 430, 3);
  add_transfer(52, 346, 7);
  add_transfer(57, 72, 3);
  add_transfer(57, 216, 5);
  add_transfer(57, 309, 12);
  add_transfer(58, 308, 5);
  add_transfer(59, 239, 2);
  add_transfer(60, 79, 6);
  add_transfer(60, 87, 3);
  add_transfer(62, 82, 3);
  add_transfer(62, 165, 4);
  add_transfer(64, 94, 4);
  add_transfer(64, 335, 8);
  add_transfer(64, 414, 8);
  add_transfer(66, 222, 8);
  add_transfer(66, 367, 6);
  add_transfer(68, 255, 10);
  add_transfer(72, 216, 5);
  add_transfer(72, 309, 12);
  add_transfer(76, 307, 6);
  add_transfer(77, 280, 5);
  add_transfer(79, 87, 6);
  add_transfer(82, 165, 7);
  add_transfer(83, 238, 3);
  add_transfer(83, 279, 11);
  add_transfer(83, 432, 3);
  add_transfer(84, 279, 7);
  add_transfer(84, 306, 9);
  add_transfer(85, 304, 9);
  add_transfer(85, 410, 9);
  add_transfer(91, 168, 3);
  add_transfer(92, 104, 4);
  add_transfer(93, 428, 10);
  add_transfer(94, 335, 10);
  add_transfer(94, 414, 10);
  add_transfer(94, 190, 6);
  add_transfer(95, 130, 3);
  add_transfer(95, 145, 3);
  add_transfer(90, 163, 3);
  add_transfer(89, 109, 3);
  add_transfer(86, 134, 3);
  add_transfer(100, 268, 6);
  add_transfer(103, 224, 3);
  add_transfer(106, 189, 3);
  add_transfer(107, 131, 2);
  add_transfer(108, 144, 2);
  add_transfer(111, 267, 5);
  add_transfer(115, 210, 4);
  add_transfer(120, 237, 3);
  add_transfer(141, 344, 6);
  add_transfer(139, 343, 7);
  add_transfer(138, 275, 13);
  add_transfer(138, 276, 14);
  add_transfer(137, 229, 3);
  add_transfer(137, 277, 14);
  add_transfer(136, 305, 10);
  add_transfer(133, 166, 4);
  add_transfer(130, 145, 4);
  add_transfer(129, 192, 5);
  add_transfer(127, 331, 7);
  add_transfer(127, 202, 6);
  add_transfer(123, 286, 4);
  add_transfer(123, 375, 12);
  add_transfer(124, 374, 6);
  add_transfer(151, 420, 15);
  add_transfer(150, 419, 15);
  add_transfer(148, 257, 11);
  add_transfer(147, 220, 10);
  add_transfer(147, 369, 11);
  add_transfer(146, 191, 3);
  add_transfer(146, 334, 14);
  add_transfer(146, 415, 7);
  add_transfer(173, 269, 6);
  add_transfer(172, 183, 2);
  add_transfer(172, 365, 13);
  add_transfer(171, 302, 7);
  add_transfer(170, 339, 8);
  add_transfer(169, 226, 3);
  add_transfer(169, 303, 8);
  add_transfer(169, 411, 8);
  add_transfer(167, 188, 4);
  add_transfer(161, 265, 15);
  add_transfer(158, 208, 3);
  add_transfer(152, 235, 3);
  add_transfer(178, 298, 5);
  add_transfer(182, 270, 6);
  add_transfer(182, 301, 6);
  add_transfer(185, 366, 12);
  add_transfer(186, 225, 3);
  add_transfer(186, 338, 12);
  add_transfer(186, 412, 9);
  add_transfer(190, 335, 12);
  add_transfer(190, 414, 12);
  add_transfer(191, 415, 9);
  add_transfer(191, 334, 14);
  add_transfer(193, 262, 14);
  add_transfer(195, 203, 3);
  add_transfer(195, 330, 10);
  add_transfer(202, 331, 5);
  add_transfer(203, 330, 6);
  add_transfer(213, 243, 5);
  add_transfer(214, 404, 8);
  add_transfer(216, 309, 9);
  add_transfer(229, 277, 14);
  add_transfer(226, 303, 8);
  add_transfer(226, 411, 11);
  add_transfer(225, 338, 12);
  add_transfer(225, 412, 9);
  add_transfer(224, 337, 14);
  add_transfer(222, 367, 6);
  add_transfer(220, 369, 13);
  add_transfer(219, 259, 6);
  add_transfer(219, 290, 3);
  add_transfer(219, 416, 6);
  add_transfer(238, 279, 12);
  add_transfer(238, 432, 3);
  add_transfer(239, 407, 18);
  add_transfer(240, 406, 6);
  add_transfer(280, 408, 3);
  add_transfer(279, 409, 7);
  add_transfer(278, 431, 5);
  add_transfer(278, 306, 13);
  add_transfer(274, 342, 6);
  add_transfer(271, 364, 8);
  add_transfer(270, 301, 5);
  add_transfer(258, 370, 8);
  add_transfer(259, 290, 6);
  add_transfer(259, 416, 6);
  add_transfer(260, 332, 7);
  add_transfer(286, 375, 7);
  add_transfer(290, 416, 6);
  add_transfer(303, 411, 8);
  add_transfer(304, 410, 8);
  add_transfer(306, 431, 13);
  add_transfer(334, 415, 17);
  add_transfer(335, 414, 3);
  add_transfer(336, 413, 2);
  add_transfer(336, 429, 12);
  add_transfer(336, 5, 9);
  add_transfer(336, 428, 12);
  add_transfer(336, 93, 9);
  add_transfer(337, 103, 12);
  add_transfer(338, 412, 4);
  add_transfer(371, 417, 10);
  add_transfer(413, 429, 12);
  add_transfer(413, 428, 12);
  add_transfer(413, 93, 9);
  add_transfer(413, 5, 9);
  add_transfer(401, 426, 5);
  add_transfer(428, 429, 12);
  add_transfer(428, 93, 10);
  add_transfer(428, 5, 7);
  add_transfer(429, 93, 11);
  add_transfer(429, 5, 8);
}

void add_all_rails() {
  add_rail(0, 1, 4);
  add_rail(1, 2, 3);
  add_rail(2, 3, 4);
  add_rail(3, 4, 2);
  add_rail(4, 5, 2);
  add_rail(5, 6, 2);
  add_rail(6, 7, 2);
  add_rail(7, 8, 2);
  add_rail(8, 9, 1);
  add_rail(9, 10, 2);
  add_rail(10, 11, 2);
  add_rail(11, 12, 2);
  add_rail(12, 13, 2);
  add_rail(13, 14, 2);
  add_rail(14, 15, 3);
  add_rail(15, 16, 3);
  add_rail(16, 17, 3);
  add_rail(17, 18, 3);
  add_rail(18, 19, 3);
  add_rail(19, 20, 4);
  add_rail(20, 21, 2);
  add_rail(21, 22, 5);
  add_rail(22, 23, 3);
  add_rail(23, 24, 4);
  add_rail(24, 25, 2);
  add_rail(26, 27, 2);
  add_rail(27, 28, 2);
  add_rail(28, 29, 3);
  add_rail(29, 30, 3);
  add_rail(30, 31, 3);
  add_rail(31, 32, 2);
  add_rail(32, 33, 3);
  add_rail(33, 34, 3);
  add_rail(34, 35, 2);
  add_rail(35, 36, 2);
  add_rail(36, 37, 2);
  add_rail(37, 38, 3);
  add_rail(38, 39, 2);
  add_rail(39, 40, 4);
  add_rail(40, 41, 3);
  add_rail(41, 42, 2);
  add_rail(42, 43, 4);
  add_rail(43, 44, 3);
  add_rail(44, 45, 3);
  add_rail(45, 46, 3);
  add_rail(46, 47, 3);
  add_rail(47, 48, 2);
  add_rail(48, 49, 5);
  add_rail(50, 51, 2);
  add_rail(51, 52, 3);
  add_rail(52, 53, 1);
  add_rail(53, 54, 4);
  add_rail(54, 55, 8);
  add_rail(55, 56, 3);
  add_rail(56, 57, 3);
  add_rail(57, 58, 3);
  add_rail(58, 59, 4);
  add_rail(59, 60, 5);
  add_rail(60, 61, 1);
  add_rail(61, 62, 3);
  add_rail(62, 63, 2);
  add_rail(63, 64, 3);
  add_rail(64, 65, 3);
  add_rail(65, 66, 2);
  add_rail(66, 67, 2);
  add_rail(67, 68, 3);
  add_rail(68, 69, 3);
  add_rail(69, 70, 3);
  add_rail(70, 71, 2);
  add_rail(72, 73, 2);
  add_rail(73, 74, 2);
  add_rail(74, 75, 2);
  add_rail(75, 76, 3);
  add_rail(76, 77, 3);
  add_rail(77, 78, 2);
  add_rail(78, 79, 3);
  add_rail(79, 80, 2);
  add_rail(80, 81, 2);
  add_rail(81, 82, 2);
  add_rail(79, 83, 5);
  add_rail(83, 84, 2);
  add_rail(85, 86, 3);
  add_rail(86, 87, 3);
  add_rail(87, 88, 3);
  add_rail(88, 89, 2);
  add_rail(89, 90, 2);
  add_rail(90, 96, 2);
  add_rail(91, 92, 3);
  add_rail(92, 93, 3);
  add_rail(93, 94, 3);
  add_rail(94, 95, 3);
  add_rail(95, 96, 2);
  add_rail(85, 91, 2);
  add_rail(97, 98, 3);
  add_rail(98, 99, 2);
  add_rail(99, 100, 3);
  add_rail(100, 101, 4);
  add_rail(101, 102, 2);
  add_rail(102, 103, 2);
  add_rail(103, 104, 3);
  add_rail(104, 105, 2);
  add_rail(105, 106, 2);
  add_rail(106, 107, 2);
  add_rail(107, 108, 3);
  add_rail(108, 109, 2);
  add_rail(109, 110, 2);
  add_rail(110, 111, 3);
  add_rail(111, 112, 4);
  add_rail(112, 113, 2);
  add_rail(113, 114, 1);
  add_rail(114, 115, 3);
  add_rail(115, 116, 2);
  add_rail(116, 117, 2);
  add_rail(117, 118, 3);
  add_rail(118, 119, 3);
  add_rail(119, 120, 2);
  add_rail(121, 122, 3);
  add_rail(122, 123, 3);
  add_rail(123, 124, 4);
  add_rail(124, 125, 4);
  add_rail(125, 126, 3);
  add_rail(126, 127, 3);
  add_rail(127, 128, 4);
  add_rail(128, 129, 3);
  add_rail(129, 130, 2);
  add_rail(130, 131, 3);
  add_rail(131, 132, 2);
  add_rail(132, 133, 2);
  add_rail(133, 134, 2);
  add_rail(134, 135, 3);
  add_rail(135, 136, 2);
  add_rail(136, 137, 2);
  add_rail(137, 138, 4);
  add_rail(138, 139, 3);
  add_rail(139, 140, 3);
  add_rail(140, 141, 3);
  add_rail(141, 142, 3);
  add_rail(142, 143, 3);
  add_rail(144, 145, 2);
  add_rail(145, 146, 3);
  add_rail(146, 147, 3);
  add_rail(147, 148, 3);
  add_rail(148, 149, 3);
  add_rail(149, 150, 3);
  add_rail(150, 151, 4);
  add_rail(152, 153, 3);
  add_rail(153, 154, 2);
  add_rail(154, 155, 3);
  add_rail(155, 156, 2);
  add_rail(156, 157, 3);
  add_rail(157, 158, 3);
  add_rail(158, 159, 2);
  add_rail(159, 160, 2);
  add_rail(160, 161, 2);
  add_rail(161, 162, 4);
  add_rail(162, 163, 3);
  add_rail(163, 164, 2);
  add_rail(164, 165, 3);
  add_rail(165, 166, 2);
  add_rail(166, 167, 2);
  add_rail(167, 168, 3);
  add_rail(168, 169, 2);
  add_rail(169, 170, 2);
  add_rail(170, 171, 2);
  add_rail(171, 172, 3);
  add_rail(172, 173, 2);
  add_rail(173, 174, 3);
  add_rail(174, 175, 3);
  add_rail(175, 176, 3);
  add_rail(177, 178, 4);
  add_rail(178, 179, 2);
  add_rail(179, 180, 3);
  add_rail(180, 181, 2);
  add_rail(181, 182, 2);
  add_rail(182, 183, 3);
  add_rail(183, 184, 2);
  add_rail(184, 185, 3);
  add_rail(185, 186, 3);
  add_rail(186, 187, 3);
  add_rail(187, 188, 2);
  add_rail(188, 189, 3);
  add_rail(189, 190, 2);
  add_rail(190, 191, 3);
  add_rail(191, 192, 3);
  add_rail(192, 193, 2);
  add_rail(193, 194, 3);
  add_rail(194, 195, 4);
  add_rail(195, 196, 2);
  add_rail(196, 197, 3);
  add_rail(197, 198, 3);
  add_rail(198, 199, 2);
  add_rail(199, 200, 3);
  add_rail(200, 201, 2);
  add_rail(231, 232, 2);
  add_rail(232, 233, 2);
  add_rail(233, 234, 1);
  add_rail(234, 235, 5);
  add_rail(235, 236, 3);
  add_rail(236, 237, 3);
  add_rail(238, 239, 5);
  add_rail(239, 240, 3);
  add_rail(240, 241, 4);
  add_rail(241, 242, 2);
  add_rail(242, 243, 3);
  add_rail(243, 244, 4);
  add_rail(244, 245, 3);
  add_rail(245, 246, 3);
  add_rail(246, 247, 3);
  add_rail(247, 248, 2);
  add_rail(248, 249, 2);
  add_rail(249, 250, 4);
  add_rail(250, 251, 4);
  add_rail(252, 253, 3);
  add_rail(253, 254, 3);
  add_rail(254, 255, 3);
  add_rail(255, 256, 2);
  add_rail(256, 257, 2);
  add_rail(257, 258, 3);
  add_rail(258, 259, 5);
  add_rail(259, 260, 3);
  add_rail(260, 261, 2);
  add_rail(261, 262, 2);
  add_rail(262, 263, 2);
  add_rail(263, 264, 3);
  add_rail(264, 265, 3);
  add_rail(265, 266, 2);
  add_rail(266, 267, 4);
  add_rail(268, 269, 3);
  add_rail(269, 270, 3);
  add_rail(270, 271, 2);
  add_rail(271, 272, 4);
  add_rail(272, 273, 3);
  add_rail(273, 274, 3);
  add_rail(274, 275, 2);
  add_rail(275, 276, 3);
  add_rail(276, 277, 2);
  add_rail(277, 278, 3);
  add_rail(278, 279, 2);
  add_rail(279, 280, 2);
  add_rail(280, 281, 4);
  add_rail(281, 267, 3);
  add_rail(282, 252, 3);
  add_rail(282, 268, 3);
  add_rail(283, 284, 3);
  add_rail(284, 285, 2);
  add_rail(285, 286, 3);
  add_rail(286, 287, 3);
  add_rail(287, 288, 4);
  add_rail(288, 289, 3);
  add_rail(289, 290, 3);
  add_rail(291, 292, 4);
  add_rail(292, 293, 3);
  add_rail(293, 294, 2);
  add_rail(294, 295, 3);
  add_rail(295, 296, 2);
  add_rail(296, 297, 4);
  add_rail(297, 298, 3);
  add_rail(298, 299, 4);
  add_rail(299, 300, 3);
  add_rail(300, 301, 4);
  add_rail(301, 302, 4);
  add_rail(302, 303, 7);
  add_rail(303, 304, 7);
  add_rail(304, 305, 5);
  add_rail(305, 306, 4);
  add_rail(306, 307, 3);
  add_rail(307, 308, 5);
  add_rail(308, 309, 3);
  add_rail(309, 310, 2);
  add_rail(310, 311, 3);
  add_rail(311, 312, 3);
  add_rail(312, 313, 3);
  add_rail(313, 314, 2);
  add_rail(314, 315, 4);
  add_rail(316, 317, 4);
  add_rail(317, 318, 3);
  add_rail(318, 319, 3);
  add_rail(319, 320, 4);
  add_rail(320, 321, 4);
  add_rail(321, 322, 2);
  add_rail(322, 323, 3);
  add_rail(323, 324, 6);
  add_rail(324, 325, 4);
  add_rail(325, 326, 2);
  add_rail(326, 327, 3);
  add_rail(327, 328, 3);
  add_rail(328, 329, 2);
  add_rail(329, 330, 4);
  add_rail(330, 331, 2);
  add_rail(331, 332, 2);
  add_rail(332, 333, 3);
  add_rail(333, 334, 3);
  add_rail(334, 335, 6);
  add_rail(335, 336, 6);
  add_rail(336, 337, 5);
  add_rail(337, 338, 4);
  add_rail(338, 339, 4);
  add_rail(339, 340, 3);
  add_rail(340, 341, 3);
  add_rail(341, 342, 3);
  add_rail(342, 343, 3);
  add_rail(343, 344, 4);
  add_rail(344, 345, 4);
  add_rail(345, 346, 3);
  add_rail(346, 347, 3);
  add_rail(347, 348, 3);
  add_rail(348, 349, 3);
  add_rail(349, 350, 4);
  add_rail(350, 351, 3);
  add_rail(351, 352, 3);
  add_rail(353, 354, 9);
  add_rail(354, 355, 4);
  add_rail(355, 356, 3);
  add_rail(356, 357, 2);
  add_rail(357, 358, 3);
  add_rail(358, 359, 6);
  add_rail(359, 360, 3);
  add_rail(360, 361, 2);
  add_rail(361, 362, 3);
  add_rail(362, 363, 3);
  add_rail(363, 364, 3);
  add_rail(364, 365, 4);
  add_rail(365, 366, 5);
  add_rail(366, 367, 14);
  add_rail(367, 368, 3);
  add_rail(368, 369, 3);
  add_rail(369, 370, 3);
  add_rail(370, 371, 3);
  add_rail(371, 372, 2);
  add_rail(372, 373, 3);
  add_rail(373, 374, 3);
  add_rail(374, 375, 3);
  add_rail(375, 376, 2);
  add_rail(376, 377, 5);
  add_rail(377, 378, 3);
  add_rail(378, 379, 2);
  add_rail(379, 380, 3);
  add_rail(380, 381, 3);
  add_rail(381, 382, 3);
  add_rail(382, 383, 2);
  add_rail(383, 384, 3);
  add_rail(384, 385, 3);
  add_rail(385, 386, 3);
  add_rail(386, 387, 2);
  add_rail(387, 388, 3);
  add_rail(388, 389, 2);
  add_rail(389, 390, 2);
  add_rail(391, 392, 4);
  add_rail(392, 393, 2);
  add_rail(393, 394, 3);
  add_rail(394, 395, 3);
  add_rail(395, 396, 3);
  add_rail(396, 397, 4);
  add_rail(397, 398, 5);
  add_rail(398, 399, 3);
  add_rail(399, 400, 3);
  add_rail(400, 401, 3);
  add_rail(401, 402, 3);
  add_rail(402, 403, 3);
  add_rail(403, 404, 3);
  add_rail(404, 405, 2);
  add_rail(405, 406, 3);
  add_rail(406, 407, 2);
  add_rail(407, 408, 4);
  add_rail(408, 409, 3);
  add_rail(409, 410, 11);
  add_rail(410, 411, 5);
  add_rail(411, 412, 4);
  add_rail(412, 413, 7);
  add_rail(413, 414, 6);
  add_rail(414, 415, 5);
  add_rail(415, 416, 5);
  add_rail(416, 417, 3);
  add_rail(417, 418, 3);
  add_rail(418, 419, 3);
  add_rail(419, 420, 3);
  add_rail(420, 421, 4);
  add_rail(421, 422, 3);
  add_rail(422, 423, 3);
  add_rail(423, 424, 3);
  add_rail(424, 425, 5);
  add_rail(426, 427, 5);
  add_rail(366, 429, 17);
  add_rail(367, 428, 7);
  add_rail(229, 431, 4);
  add_rail(431, 432, 2);
  add_rail(201, 430, 2);
  add_rail(202, 203, 2);
  add_rail(203, 204, 3);
  add_rail(204, 205, 3);
  add_rail(205, 206, 4);
  add_rail(206, 207, 3);
  add_rail(207, 208, 2);
  add_rail(208, 209, 3);
  add_rail(209, 210, 3);
  add_rail(210, 211, 3);
  add_rail(211, 212, 2);
  add_rail(212, 213, 3);
  add_rail(213, 214, 2);
  add_rail(214, 215, 3);
  add_rail(215, 216, 3);
  add_rail(216, 217, 3);
  add_rail(217, 218, 3);
  add_rail(202, 219, 4);
  add_rail(218, 230, 3);
  add_rail(219, 220, 5);
  add_rail(220, 221, 4);
  add_rail(221, 222, 3);
  add_rail(222, 223, 2);
  add_rail(223, 224, 4);
  add_rail(224, 225, 3);
  add_rail(225, 226, 3);
  add_rail(226, 227, 3);
  add_rail(227, 228, 3);
  add_rail(228, 229, 3);
  add_rail(229, 230, 4);
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
