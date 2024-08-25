#include <assert.h>

#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

#define ull unsigned long long
#define MAXN 50
#define MAXS 10
#define IN "in_1.txt"    // 输入文件
#define OUT "out_1.txt"  // 输出文件
using namespace std;

class Automata {  // 自动机类
 public:
  string type;  // 自动机类型：NFA DFA
  bool is_min;  // 是否最小化的标志

  int letter_num;     // 字符表长度
  char letter[MAXN];  // 字符表数组

  int state_num;               // 状态集合数目
  struct State {               // 储存状态信息的结构体
    int id;                    // 状态id
    ull bin_id;                // 在状态集合的二进制次序
    bool is_start, is_finish;  // 是否为开始、接收状态的标志
    State() {                  // 结构体初始化构造函数
      id = 0;
      bin_id = 0;
      is_start = 0;
      is_finish = 0;
    }
  } state[MAXN];  // 状态数组

  int start_num;          // 开始状态数目
  int start_state[MAXN];  // 开始状态在状态集合次序的集合

  int fin_num;          // 接收状态数目
  int fin_state[MAXN];  // 接收状态在状态集合次序的集合

  map<char, int> char_order;  // 字符->字符表次序  转换表
  map<int, int> id_order;     // 状态id->状态数组次序  转换表

  char move[MAXN][MAXN];  // 自动机状态转换表的二维数组

  Automata();                 // 自动机初始化构造函数
  void read();                // 读入文件数据更新自动机
  void showAutomata();        // 打印输出自动机的全部信息
  int inSet(ull*, int, ull);  // 检测子集是否在子集集合中并返回次序
  vector<int> getOrder(ull);  // 获取子集中所有状态的次序
  ull charTrans(ull, char);   // 获取子集通过字符转换形成的子集
  ull search(ull, int, bool*);  // 递归搜索子集通过不限次空字符获得的子集
  ull closure(ull);          // 获取子集闭包closure
  Automata NFA_Trans_DFA();  // NFA确定化函数
  Automata Minimize_DFA();   // DFA最小化函数
};

Automata::Automata() {  // 自动机初始化构造函数
  is_min = letter_num = state_num = start_num = fin_num = 0;
  memset(letter, 0, sizeof(letter));
  memset(start_state, -1, sizeof(start_state));
  memset(fin_state, -1, sizeof(fin_state));
  memset(move, 0, sizeof(move));
}

void Automata::read() {  // 读入文件数据更新自动机
  int pos = -1;          // NFA空字符检测标志
  ifstream infile(IN, ios::in);
  assert(infile.is_open());
  getline(infile, type);                  // 更新自动机类型
  infile >> letter_num;                   // 更新字符表长度
  for (int i = 0; i < letter_num; i++) {  // 更新字符表
    infile >> letter[i];
    if (letter[i] == '_') {  // 如果为空字符
      pos = i;               // 记录空字符次序
      continue;
    }
    char_order[letter[i]] = i;  // 更新字符->次序转换表
  }
  if (pos != -1) {                         // 如果字符表有空字符
    letter[pos] = letter[letter_num - 1];  // 交换空字符与字符表末尾字符的次序
    letter[letter_num - 1] = '_';
    char_order[letter[pos]] = pos;  // 重新调整字符->次序转换表
    char_order['_'] = letter_num - 1;
  }
  infile >> state_num;                   // 更新状态集合数目
  for (int i = 0; i < state_num; i++) {  // 更新状态数组
    infile >> state[i].id;
    state[i].bin_id = 1 << i;
    
    infile >> state[i].is_start >> state[i].is_finish;
    if (state[i].is_start) start_state[start_num++] = i;
    if (state[i].is_finish) fin_state[fin_num++] = i;
    id_order[state[i].id] = i;  // 更新状态id->次序转换表
  }
  while (!infile.eof()) {
    int id1, id2;
    char ch;
    infile >> id1 >> ch >> id2;
    move[id_order[id1]][id_order[id2]] = ch;  // 更新自动机状态转换表的二维数组
  }
  infile.close();
}

vector<int> Automata::getOrder(ull set) {  // 获取子集中所有状态的次序
  int sum = -1;                            // 状态次序初始化为-1
  vector<int> order;
  while (set) {
    ull bit =
        (1 & set);  // 利用000...001与子集set进行位与获得set最低位数字(0或者1)
    sum++;                   // 当前状态次序+1
    if (bit)                 // 如果子集set状态次序sum位为 1
      order.push_back(sum);  // 将状态次序sum压入向量
    set = set >> 1;  // 因为set是unsigned long long型变量，所以逻辑右移 1位
  }
  return order;
}

ull Automata::search(ull ans, int num,
                     bool flag[]) {  // 递归搜索子集通过不限次空字符获得的子集
  for (int i = 0; i < state_num; i++) {  // 遍历状态转换表的二维数组的列
    if (!flag[i] &&
        move[num][i] == '_') {  // 如果转换边未遍历标记且转换字符为空字符
      ans |= state[i].bin_id;  // 将到达状态的二进制表示与子集ans进行位或
      flag[i] = 1;             // 标记此转换边已访问过
      ans |= search(ans, i, flag);  // 从目前到达状态重新开始递归搜索
    }
  }
  return ans;
}

ull Automata::closure(ull ans) {  // 获取子集闭包closure
  vector<int> order = getOrder(ans);  // 调用getOrder()获取子集中所有状态的次序
  bool flag[MAXN];
  memset(flag, 0, sizeof(flag));
  for (int i = 0; i < (int)order.size(); i++)
    ans = search(
        ans, order[i],
        flag);  // 从子集中每个状态开始调用search()搜索通过不限次空字符获得的闭包子集
  return ans;
}

ull Automata::charTrans(ull set, char ch) {  // 获取子集通过字符转换形成的子集
  ull tmp = 0;
  vector<int> order = getOrder(set);  // 调用getOrder()获取子集中所有状态的次序
  for (int i = 0; i < (int)order.size(); i++) {
    for (int j = 0; j < state_num; j++) {
      if (move[order[i]][j] == ch) {  // 如果转换表字符为指定字符
        tmp |= state[j].bin_id;
      }
    }
  }
  return tmp;
}

int Automata::inSet(ull subset[], int size,
                    ull set) {  // 检测子集是否在子集集合中并返回次序
  for (int i = 0; i < size; i++) {
    if (set == subset[i])  // 如果子集set等于子集集合中的一个子集
      return i;            // 返回其在子集集合中的次序
  }
  return -1;  // 不存在则返回 -1
}

Automata Automata::NFA_Trans_DFA() {  // NFA确定化函数
  if (type == "DFA")  // 如果自动机已经是DFA则直接返回自身
    return *this;
  Automata dfa;
  dfa.type = "DFA";
  ull subset[MAXS];
  int pos = 0, size = 0;
  int move_dfa
      [MAXS]
      [MAXN];  // 创建一个子集转换表(因为列号为字符次序，与自动机状态装换表不同)
  memset(move_dfa, -1, sizeof(move_dfa));
  subset[pos] =
      closure(state[start_state[0]]
                  .bin_id);  // 调用closure()求开始状态闭包获得开始子集subset[0]
  size++;                    // 子集集合subset[]大小 +1
  while (1) {
    for (int i = 0; i < letter_num - 1;
         i++) {  // 遍历字符表除空字符外的所有字符
      ull J = charTrans(
          subset[pos],
          letter[i]);  // 获得当前子集subset[pos]的字符letter[i]转换子集 J
      if (!J)          // 如果子集 J 为空
        continue;
      ull I = closure(J);  // 子集 I为子集 J的闭包closure
      int number =
          inSet(subset, size, I);  // number存储子集 I在子集集合subset[]中的次序
      if (number == -1) {  // 如果number == -1即子集 I不在子集集合subset[]中
        subset[size] = I;         // 将子集 I加入到子集集合subset[]中
        move_dfa[pos][i] = size;  // 更新子集转换表
        size++;                   // 子集集合subset[]大小 +1
      } else                      // 集 I存在子集集合subset[]中
        move_dfa[pos][i] = number;  // 更新子集转换表
    }
    pos++;  // 当前子集转换表行 +1
    if (pos ==
        size)  // 当所有子集都存在于子集集合subset[]中，子集转换表行数不再变化
      break;   // 子集转换表创建完成，跳出循环
  }

  if (letter[letter_num - 1] == '_')  // 如果NFA字符表有空字符
    dfa.letter_num = letter_num - 1;  // DFA的字符表不能有空字符，所以字符数目-1
  for (int i = 0; i < dfa.letter_num; i++) {  // 更新DFA字符表
    dfa.letter[i] = letter[i];
    dfa.char_order[dfa.letter[i]] = i;  // 更新字符->字符表次序 转换表
  }
  dfa.start_num = 1;  // DFA开始状态唯一
  dfa.start_state[0] = 0;
  dfa.state[0].is_start = 1;
  dfa.state_num = size;
  for (int i = 0; i < size; i++) {  // 更新DFA的状态数组
    dfa.state[i].id = i;
    dfa.state[i].bin_id = 1 << i;
    dfa.id_order[i] = i;
    for (int j = 0; j < fin_num; j++) {
      if ((subset[i] & state[fin_state[j]].bin_id) > 0) {
        dfa.fin_state[dfa.fin_num++] = i;
        dfa.state[i].is_finish = 1;
        break;
      }
    }
  }
  for (int i = 0; i < size;
       i++) {  // 由获得的子集转换表move_dfa[][]更新DFA的状态转换表move[][]
    for (int j = 0; j < dfa.letter_num; j++) {
      if (move_dfa[i][j] != -1) {
        dfa.move[i][move_dfa[i][j]] = dfa.letter[j];
      }
    }
  }
  return dfa;
}

Automata Automata::Minimize_DFA() {  // DFA最小化函数
  Automata dfa;
  if (type != "DFA")              // 如果自动机不是DFA
    dfa = this->NFA_Trans_DFA();  // 将自动机调用NFA_Trans_DFA()转换的DFA赋予dfa
  else                            // 如果自动机是DFA
    dfa = *this;                  // 将自动机赋予dfa
  int divide[MAXN],
      num = 1;  // 声明子集划分表数组divide[]和子集初始最大划分数num
  memset(divide, 0, sizeof(divide));
  for (int i = 0; i < dfa.fin_num;
       i++)  // 子集划分表divide[]形成基本划分，非接收状态划分为
    // 0，接收状态划分为 1
    divide[dfa.fin_state[i]] = 1;
  for (int i = 0; i <= num; i++) {  // 进入划分循环
    int set[MAXN], ans = 0;  // 声明当前划分集合set[]和当前划分大小ans
    for (int j = 0; j < dfa.state_num; j++)  // 创建划分为 i的当前划分
      if (divide[j] == i) set[ans++] = j;
    for (int j = 0; j < dfa.letter_num;
         j++) {  // 当前划分set[]读入字符表所有字符进行再划分
      int key = -1;
      bool flag = 0;
      for (int k = 0; k < dfa.state_num; k++) {
        if (dfa.move[set[0]][k] == dfa.letter[j]) {  // 查询状态转换表
          key =
              divide[k];  // 当前划分首状态通过字符letter[j]到达状态的划分为 key
          break;
        }
      }
      for (int k = 1; k < ans; k++) {  // 遍历当前划分set[]除首状态外的所有状态
        int value = -1;
        for (int l = 0; l < dfa.state_num; l++) {
          if (dfa.move[set[k]][l] == dfa.letter[j]) {  // 查询状态转换表
            value =
                divide[l];  // 当前划分其他状态通过字符letter[j]到达状态划分为
            // value
            break;
          }
        }
        if (key != value &&
            divide[set[0]] ==
                divide[set[k]]) {  // 当首状态转移状态划分 key 不等于
          // 其他状态转移状态划分
          // value，且此其他状态未进行过再划分与首状态仍在同一划分中
          divide[set[k]] = num + 1;  // 其他状态进入新划分 num+1
          flag = 1;                  // 标志新划分产生
        }
      }
      if (flag)  // 如果新划分产生
        num++;   // 最大划分数 +1
    }
  }

  dfa.is_min = 1;     // 标记DFA已最小化
  dfa.start_num = 1;  // DFA开始状态唯一
  char move_copy[MAXN][MAXN];
  for (int i = 0; i < dfa.state_num; i++) {
    for (int j = 0; j < dfa.state_num; j++) {
      move_copy[i][j] = dfa.move[i][j];
      dfa.move[i][j] = 0;
    }
  }
  for (int i = 0; i <= num; i++) {  // 由划分数进行状态数组更新
    dfa.state[i].id = i;
    dfa.state[i].bin_id = 1 << i;
    dfa.id_order[i] = i;
    dfa.state[i].is_start = 0;
  }
  for (int i = 0; i < dfa.state_num; i++) {  // 更新开始状态
    if (i == dfa.start_state[0]) {
      dfa.start_state[0] = divide[i];
      dfa.state[divide[i]].is_start = 1;
      break;
    }
  }
  dfa.fin_num = 0;
  for (int i = 0; i <= num; i++) {  // 更新接收状态
    dfa.state[i].is_finish = 0;
    for (int j = 0; j < dfa.state_num; j++) {
      if (divide[j] == i && dfa.state[j].is_finish) {
        dfa.fin_state[dfa.fin_num++] = i;
        dfa.state[i].is_finish = 1;
        break;
      }
    }
  }

  for (int i = 0; i < dfa.state_num; i++) {
    for (int j = 0; j < dfa.state_num; j++) {
      if (move_copy[i][j] != 0 && (i == j || divide[i] != divide[j])) {
        dfa.move[divide[i]][divide[j]] =
            move_copy[i][j];  // 更新最小化DFA转换表move[][]
      }
    }
  }
  dfa.state_num = num + 1;  // 最小化DFA状态数等于最大划分数 +1
  return dfa;
}

void Automata::showAutomata() {  // 打印输出自动机的全部信息
  if (is_min == 0)
    cout << "This is a " << type << ":" << endl;
  else
    cout << "This is a minimized " << type << ":" << endl;
  cout << "字符个数：" << letter_num << endl;
  cout << "字符集：";
  for (int i = 0; i < letter_num; i++) cout << letter[i] << " ";
  cout << endl << "状态个数：" << state_num << endl;
  cout << "状态集：";
  for (int i = 0; i < state_num; i++) cout << state[i].id << " ";
  cout << endl << "开始状态：";
  for (int i = 0; i < start_num; i++) cout << state[start_state[i]].id << " ";
  cout << endl << "接收状态：";
  for (int i = 0; i < fin_num; i++) cout << state[fin_state[i]].id << " ";
  cout << endl << type << "状态转换表：" << endl;
  cout << "  ";
  for (int i = 0; i < state_num; i++) cout << state[i].id << " ";
  cout << endl;
  for (int i = 0; i < state_num; i++) {
    cout << state[i].id << " ";
    for (int j = 0; j < state_num; j++) {
      if (move[i][j] == 0)  // 当转换表 状态i->状态j 没有字符可到达时
        cout << '.' << " ";  // 转换表move[i][j]以 . 表示
      else
        cout << move[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

int main() {
  Automata automata[3];  // automata[0]为 NFA  automata[1]为 DFA  automata[2] 为
  // min_DFA
  automata[0].read();          // 读入输入文件更新NFA
  automata[0].showAutomata();  // 打印输出NFA的全部信息

  automata[1] = automata[0].NFA_Trans_DFA();  // DFA为NFA确定化后的自动机
  automata[1].showAutomata();                 // 打印输出DFA的全部信息

  automata[2] = automata[1].Minimize_DFA();  // MIN_DFA为DFA最小化后的自动机
  automata[2].showAutomata();  // 打印输出MIN_DFA的全部信息

  return 0;
}
