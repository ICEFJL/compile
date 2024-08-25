#include "global.h"

int main() {
  char place[100];
  cout << "输入txt文件路径";  // 输入源程序路径
  cin >> place;
  word_analysis(place);  // 调用词法分析 将结果保存至中间文件
  cout << "词法分析结果已保存至中间文件" << endl;
  return 0;
}