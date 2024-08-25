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
#define IN "in_1.txt"    // �����ļ�
#define OUT "out_1.txt"  // ����ļ�
using namespace std;

class Automata {  // �Զ�����
 public:
  string type;  // �Զ������ͣ�NFA DFA
  bool is_min;  // �Ƿ���С���ı�־

  int letter_num;     // �ַ�����
  char letter[MAXN];  // �ַ�������

  int state_num;               // ״̬������Ŀ
  struct State {               // ����״̬��Ϣ�Ľṹ��
    int id;                    // ״̬id
    ull bin_id;                // ��״̬���ϵĶ����ƴ���
    bool is_start, is_finish;  // �Ƿ�Ϊ��ʼ������״̬�ı�־
    State() {                  // �ṹ���ʼ�����캯��
      id = 0;
      bin_id = 0;
      is_start = 0;
      is_finish = 0;
    }
  } state[MAXN];  // ״̬����

  int start_num;          // ��ʼ״̬��Ŀ
  int start_state[MAXN];  // ��ʼ״̬��״̬���ϴ���ļ���

  int fin_num;          // ����״̬��Ŀ
  int fin_state[MAXN];  // ����״̬��״̬���ϴ���ļ���

  map<char, int> char_order;  // �ַ�->�ַ������  ת����
  map<int, int> id_order;     // ״̬id->״̬�������  ת����

  char move[MAXN][MAXN];  // �Զ���״̬ת����Ķ�ά����

  Automata();                 // �Զ�����ʼ�����캯��
  void read();                // �����ļ����ݸ����Զ���
  void showAutomata();        // ��ӡ����Զ�����ȫ����Ϣ
  int inSet(ull*, int, ull);  // ����Ӽ��Ƿ����Ӽ������в����ش���
  vector<int> getOrder(ull);  // ��ȡ�Ӽ�������״̬�Ĵ���
  ull charTrans(ull, char);   // ��ȡ�Ӽ�ͨ���ַ�ת���γɵ��Ӽ�
  ull search(ull, int, bool*);  // �ݹ������Ӽ�ͨ�����޴ο��ַ���õ��Ӽ�
  ull closure(ull);          // ��ȡ�Ӽ��հ�closure
  Automata NFA_Trans_DFA();  // NFAȷ��������
  Automata Minimize_DFA();   // DFA��С������
};

Automata::Automata() {  // �Զ�����ʼ�����캯��
  is_min = letter_num = state_num = start_num = fin_num = 0;
  memset(letter, 0, sizeof(letter));
  memset(start_state, -1, sizeof(start_state));
  memset(fin_state, -1, sizeof(fin_state));
  memset(move, 0, sizeof(move));
}

void Automata::read() {  // �����ļ����ݸ����Զ���
  int pos = -1;          // NFA���ַ�����־
  ifstream infile(IN, ios::in);
  assert(infile.is_open());
  getline(infile, type);                  // �����Զ�������
  infile >> letter_num;                   // �����ַ�����
  for (int i = 0; i < letter_num; i++) {  // �����ַ���
    infile >> letter[i];
    if (letter[i] == '_') {  // ���Ϊ���ַ�
      pos = i;               // ��¼���ַ�����
      continue;
    }
    char_order[letter[i]] = i;  // �����ַ�->����ת����
  }
  if (pos != -1) {                         // ����ַ����п��ַ�
    letter[pos] = letter[letter_num - 1];  // �������ַ����ַ���ĩβ�ַ��Ĵ���
    letter[letter_num - 1] = '_';
    char_order[letter[pos]] = pos;  // ���µ����ַ�->����ת����
    char_order['_'] = letter_num - 1;
  }
  infile >> state_num;                   // ����״̬������Ŀ
  for (int i = 0; i < state_num; i++) {  // ����״̬����
    infile >> state[i].id;
    state[i].bin_id = 1 << i;
    
    infile >> state[i].is_start >> state[i].is_finish;
    if (state[i].is_start) start_state[start_num++] = i;
    if (state[i].is_finish) fin_state[fin_num++] = i;
    id_order[state[i].id] = i;  // ����״̬id->����ת����
  }
  while (!infile.eof()) {
    int id1, id2;
    char ch;
    infile >> id1 >> ch >> id2;
    move[id_order[id1]][id_order[id2]] = ch;  // �����Զ���״̬ת����Ķ�ά����
  }
  infile.close();
}

vector<int> Automata::getOrder(ull set) {  // ��ȡ�Ӽ�������״̬�Ĵ���
  int sum = -1;                            // ״̬�����ʼ��Ϊ-1
  vector<int> order;
  while (set) {
    ull bit =
        (1 & set);  // ����000...001���Ӽ�set����λ����set���λ����(0����1)
    sum++;                   // ��ǰ״̬����+1
    if (bit)                 // ����Ӽ�set״̬����sumλΪ 1
      order.push_back(sum);  // ��״̬����sumѹ������
    set = set >> 1;  // ��Ϊset��unsigned long long�ͱ����������߼����� 1λ
  }
  return order;
}

ull Automata::search(ull ans, int num,
                     bool flag[]) {  // �ݹ������Ӽ�ͨ�����޴ο��ַ���õ��Ӽ�
  for (int i = 0; i < state_num; i++) {  // ����״̬ת����Ķ�ά�������
    if (!flag[i] &&
        move[num][i] == '_') {  // ���ת����δ���������ת���ַ�Ϊ���ַ�
      ans |= state[i].bin_id;  // ������״̬�Ķ����Ʊ�ʾ���Ӽ�ans����λ��
      flag[i] = 1;             // ��Ǵ�ת�����ѷ��ʹ�
      ans |= search(ans, i, flag);  // ��Ŀǰ����״̬���¿�ʼ�ݹ�����
    }
  }
  return ans;
}

ull Automata::closure(ull ans) {  // ��ȡ�Ӽ��հ�closure
  vector<int> order = getOrder(ans);  // ����getOrder()��ȡ�Ӽ�������״̬�Ĵ���
  bool flag[MAXN];
  memset(flag, 0, sizeof(flag));
  for (int i = 0; i < (int)order.size(); i++)
    ans = search(
        ans, order[i],
        flag);  // ���Ӽ���ÿ��״̬��ʼ����search()����ͨ�����޴ο��ַ���õıհ��Ӽ�
  return ans;
}

ull Automata::charTrans(ull set, char ch) {  // ��ȡ�Ӽ�ͨ���ַ�ת���γɵ��Ӽ�
  ull tmp = 0;
  vector<int> order = getOrder(set);  // ����getOrder()��ȡ�Ӽ�������״̬�Ĵ���
  for (int i = 0; i < (int)order.size(); i++) {
    for (int j = 0; j < state_num; j++) {
      if (move[order[i]][j] == ch) {  // ���ת�����ַ�Ϊָ���ַ�
        tmp |= state[j].bin_id;
      }
    }
  }
  return tmp;
}

int Automata::inSet(ull subset[], int size,
                    ull set) {  // ����Ӽ��Ƿ����Ӽ������в����ش���
  for (int i = 0; i < size; i++) {
    if (set == subset[i])  // ����Ӽ�set�����Ӽ������е�һ���Ӽ�
      return i;            // ���������Ӽ������еĴ���
  }
  return -1;  // �������򷵻� -1
}

Automata Automata::NFA_Trans_DFA() {  // NFAȷ��������
  if (type == "DFA")  // ����Զ����Ѿ���DFA��ֱ�ӷ�������
    return *this;
  Automata dfa;
  dfa.type = "DFA";
  ull subset[MAXS];
  int pos = 0, size = 0;
  int move_dfa
      [MAXS]
      [MAXN];  // ����һ���Ӽ�ת����(��Ϊ�к�Ϊ�ַ��������Զ���״̬װ����ͬ)
  memset(move_dfa, -1, sizeof(move_dfa));
  subset[pos] =
      closure(state[start_state[0]]
                  .bin_id);  // ����closure()��ʼ״̬�հ���ÿ�ʼ�Ӽ�subset[0]
  size++;                    // �Ӽ�����subset[]��С +1
  while (1) {
    for (int i = 0; i < letter_num - 1;
         i++) {  // �����ַ�������ַ���������ַ�
      ull J = charTrans(
          subset[pos],
          letter[i]);  // ��õ�ǰ�Ӽ�subset[pos]���ַ�letter[i]ת���Ӽ� J
      if (!J)          // ����Ӽ� J Ϊ��
        continue;
      ull I = closure(J);  // �Ӽ� IΪ�Ӽ� J�ıհ�closure
      int number =
          inSet(subset, size, I);  // number�洢�Ӽ� I���Ӽ�����subset[]�еĴ���
      if (number == -1) {  // ���number == -1���Ӽ� I�����Ӽ�����subset[]��
        subset[size] = I;         // ���Ӽ� I���뵽�Ӽ�����subset[]��
        move_dfa[pos][i] = size;  // �����Ӽ�ת����
        size++;                   // �Ӽ�����subset[]��С +1
      } else                      // �� I�����Ӽ�����subset[]��
        move_dfa[pos][i] = number;  // �����Ӽ�ת����
    }
    pos++;  // ��ǰ�Ӽ�ת������ +1
    if (pos ==
        size)  // �������Ӽ����������Ӽ�����subset[]�У��Ӽ�ת�����������ٱ仯
      break;   // �Ӽ�ת��������ɣ�����ѭ��
  }

  if (letter[letter_num - 1] == '_')  // ���NFA�ַ����п��ַ�
    dfa.letter_num = letter_num - 1;  // DFA���ַ������п��ַ��������ַ���Ŀ-1
  for (int i = 0; i < dfa.letter_num; i++) {  // ����DFA�ַ���
    dfa.letter[i] = letter[i];
    dfa.char_order[dfa.letter[i]] = i;  // �����ַ�->�ַ������ ת����
  }
  dfa.start_num = 1;  // DFA��ʼ״̬Ψһ
  dfa.start_state[0] = 0;
  dfa.state[0].is_start = 1;
  dfa.state_num = size;
  for (int i = 0; i < size; i++) {  // ����DFA��״̬����
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
       i++) {  // �ɻ�õ��Ӽ�ת����move_dfa[][]����DFA��״̬ת����move[][]
    for (int j = 0; j < dfa.letter_num; j++) {
      if (move_dfa[i][j] != -1) {
        dfa.move[i][move_dfa[i][j]] = dfa.letter[j];
      }
    }
  }
  return dfa;
}

Automata Automata::Minimize_DFA() {  // DFA��С������
  Automata dfa;
  if (type != "DFA")              // ����Զ�������DFA
    dfa = this->NFA_Trans_DFA();  // ���Զ�������NFA_Trans_DFA()ת����DFA����dfa
  else                            // ����Զ�����DFA
    dfa = *this;                  // ���Զ�������dfa
  int divide[MAXN],
      num = 1;  // �����Ӽ����ֱ�����divide[]���Ӽ���ʼ��󻮷���num
  memset(divide, 0, sizeof(divide));
  for (int i = 0; i < dfa.fin_num;
       i++)  // �Ӽ����ֱ�divide[]�γɻ������֣��ǽ���״̬����Ϊ
    // 0������״̬����Ϊ 1
    divide[dfa.fin_state[i]] = 1;
  for (int i = 0; i <= num; i++) {  // ���뻮��ѭ��
    int set[MAXN], ans = 0;  // ������ǰ���ּ���set[]�͵�ǰ���ִ�Сans
    for (int j = 0; j < dfa.state_num; j++)  // ��������Ϊ i�ĵ�ǰ����
      if (divide[j] == i) set[ans++] = j;
    for (int j = 0; j < dfa.letter_num;
         j++) {  // ��ǰ����set[]�����ַ��������ַ������ٻ���
      int key = -1;
      bool flag = 0;
      for (int k = 0; k < dfa.state_num; k++) {
        if (dfa.move[set[0]][k] == dfa.letter[j]) {  // ��ѯ״̬ת����
          key =
              divide[k];  // ��ǰ������״̬ͨ���ַ�letter[j]����״̬�Ļ���Ϊ key
          break;
        }
      }
      for (int k = 1; k < ans; k++) {  // ������ǰ����set[]����״̬�������״̬
        int value = -1;
        for (int l = 0; l < dfa.state_num; l++) {
          if (dfa.move[set[k]][l] == dfa.letter[j]) {  // ��ѯ״̬ת����
            value =
                divide[l];  // ��ǰ��������״̬ͨ���ַ�letter[j]����״̬����Ϊ
            // value
            break;
          }
        }
        if (key != value &&
            divide[set[0]] ==
                divide[set[k]]) {  // ����״̬ת��״̬���� key ������
          // ����״̬ת��״̬����
          // value���Ҵ�����״̬δ���й��ٻ�������״̬����ͬһ������
          divide[set[k]] = num + 1;  // ����״̬�����»��� num+1
          flag = 1;                  // ��־�»��ֲ���
        }
      }
      if (flag)  // ����»��ֲ���
        num++;   // ��󻮷��� +1
    }
  }

  dfa.is_min = 1;     // ���DFA����С��
  dfa.start_num = 1;  // DFA��ʼ״̬Ψһ
  char move_copy[MAXN][MAXN];
  for (int i = 0; i < dfa.state_num; i++) {
    for (int j = 0; j < dfa.state_num; j++) {
      move_copy[i][j] = dfa.move[i][j];
      dfa.move[i][j] = 0;
    }
  }
  for (int i = 0; i <= num; i++) {  // �ɻ���������״̬�������
    dfa.state[i].id = i;
    dfa.state[i].bin_id = 1 << i;
    dfa.id_order[i] = i;
    dfa.state[i].is_start = 0;
  }
  for (int i = 0; i < dfa.state_num; i++) {  // ���¿�ʼ״̬
    if (i == dfa.start_state[0]) {
      dfa.start_state[0] = divide[i];
      dfa.state[divide[i]].is_start = 1;
      break;
    }
  }
  dfa.fin_num = 0;
  for (int i = 0; i <= num; i++) {  // ���½���״̬
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
            move_copy[i][j];  // ������С��DFAת����move[][]
      }
    }
  }
  dfa.state_num = num + 1;  // ��С��DFA״̬��������󻮷��� +1
  return dfa;
}

void Automata::showAutomata() {  // ��ӡ����Զ�����ȫ����Ϣ
  if (is_min == 0)
    cout << "This is a " << type << ":" << endl;
  else
    cout << "This is a minimized " << type << ":" << endl;
  cout << "�ַ�������" << letter_num << endl;
  cout << "�ַ�����";
  for (int i = 0; i < letter_num; i++) cout << letter[i] << " ";
  cout << endl << "״̬������" << state_num << endl;
  cout << "״̬����";
  for (int i = 0; i < state_num; i++) cout << state[i].id << " ";
  cout << endl << "��ʼ״̬��";
  for (int i = 0; i < start_num; i++) cout << state[start_state[i]].id << " ";
  cout << endl << "����״̬��";
  for (int i = 0; i < fin_num; i++) cout << state[fin_state[i]].id << " ";
  cout << endl << type << "״̬ת����" << endl;
  cout << "  ";
  for (int i = 0; i < state_num; i++) cout << state[i].id << " ";
  cout << endl;
  for (int i = 0; i < state_num; i++) {
    cout << state[i].id << " ";
    for (int j = 0; j < state_num; j++) {
      if (move[i][j] == 0)  // ��ת���� ״̬i->״̬j û���ַ��ɵ���ʱ
        cout << '.' << " ";  // ת����move[i][j]�� . ��ʾ
      else
        cout << move[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

int main() {
  Automata automata[3];  // automata[0]Ϊ NFA  automata[1]Ϊ DFA  automata[2] Ϊ
  // min_DFA
  automata[0].read();          // ���������ļ�����NFA
  automata[0].showAutomata();  // ��ӡ���NFA��ȫ����Ϣ

  automata[1] = automata[0].NFA_Trans_DFA();  // DFAΪNFAȷ��������Զ���
  automata[1].showAutomata();                 // ��ӡ���DFA��ȫ����Ϣ

  automata[2] = automata[1].Minimize_DFA();  // MIN_DFAΪDFA��С������Զ���
  automata[2].showAutomata();  // ��ӡ���MIN_DFA��ȫ����Ϣ

  return 0;
}
