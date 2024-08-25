#include <string>

#include "global.h"

/*判断并查询保留字种别码*/
int ReserveWordId(string strToken) {
  if (ReserveWord.find(strToken) != ReserveWord.end()) {
    return ReserveWord[strToken];
  } else {
    return -1;
  }
}
/*判断并查询界符种别码*/
int DelimeterId(string strToken) {
  if (Delimiter.find(strToken) != Delimiter.end()) {
    return Delimiter[strToken];
  } else {
    return -1;
  }
}
/*判断并查询算符种别码*/
int OperatorId(string strToken) {
  if (Operator.find(strToken) != Operator.end()) {
    return Operator[strToken];
  } else {
    return -1;
  }
}

/* 核心词法分析程序 */
int scan(string r, string &strToken, int &p) {
  int sym = -1;      // 种别码
  char ch = r[p++];  // 读r当前的字符，且p++

  while (ch == ' ' || ch == '\n') {  // 跳过空格和换行符和文件结束符
    if (ch == ' ') col++;
    if (ch == '\n') {
      row++;
      col = 1;
    }
    ch = r[p++];
  }
  strToken.clear();         // 清空strToken
  strToken.push_back(ch);   // 存入strToken
  if (ch == '#') return 0;  // 读到#，则返回0(程序结束
  /* 判断保留字或标识符 */
  if (isalpha(ch)) {  // 读入的第一个字符为字母

    while (isalpha(r[p])) {  // 继续读r,若为字母则循环，并存至strToken
      strToken.push_back(r[p++]);
    }
    while (isdigit(r[p]) || isalpha(r[p])) {  // 若为数字，且之后为数字或字母
      strToken.push_back(r[p++]);
    }
    sym = ReserveWordId(strToken);
    if (sym == -1) {
      sym = 32;
    }

  }
  /* 判断数字 */
  else if (isdigit(ch)) {  // 读入的第一个字符为数字
    while (isdigit(r[p])) {
      strToken.push_back(r[p++]);
    }
    sym = 31;
  }
  /* 判断界符 */
  else if (ch == ';' || ch == ',' || ch == '(' || ch == ')') {
    sym = DelimeterId(strToken);
  } else if (ch == ':') {
    if (r[p] == '=') {
      strToken.push_back(r[p++]);
      sym = DelimeterId(strToken);
    } else {
      sym = -2;
    }
  }
  /* 判断算符 */
  else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=') {
    sym = OperatorId(strToken);
  } else if (ch == '<') {
    if (r[p] == '>' || r[p] == '=') {
      strToken.push_back(r[p++]);
    }
    sym = OperatorId(strToken);
  } else if (ch == '>') {
    if (r[p] == '=') {
      strToken.push_back(r[p++]);
    }
    sym = OperatorId(strToken);
  }
  // 不能被以上词法分析识别，则出错
  else {
    sym = -1;
  }
  return sym;
}

/*读文件并调用词法分析程序,将结果保存至writefile.txt*/
void word_analysis(string place) {
  string str;
  ifstream file;
  file.open(place, ios::in);
  if (!file) {
    cerr << "Open File Fail" << endl;
    exit(1);
  }
  string temp;
  while (getline(file, temp)) str += temp + "\n";
  str += "#";
  file.close();

  for (int i = 1; i <= 15; i++) {
    symMap[i] = "保留字";
  }

  for (int i = 16; i <= 20; i++) {
    symMap[i] = "界符";
  }

  for (int i = 21; i <= 30; i++) {
    symMap[i] = "算符";
  }

  ofstream ofile;
  ofile.open("writefile.txt", ios::out | ios::binary);
  if (!ofile) {
    cerr << "Open File Fail" << endl;
    exit(1);
  }
  int sym = 0;
  string strToken;
  int p = 0;
  while (true) {
    sym = scan(str, strToken, p);
    if (sym == 0) {
      break;
    } else {
      ofile << '\n';
    }

    if (sym == -1 || sym == -2) {
      cout << "row " << row << " col " << col << " " << symMap[sym] << " "
           << strToken;
      col++;
    } else if (symMap.count(sym)) {
      ofile << symMap[sym] << " " << strToken << " " << sym << "$" << row;
    }
    col += strToken.size();
  }
  ofile.close();
}