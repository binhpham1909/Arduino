
#include "BMacros.h"
double map(double x, double ca[10])	// Ham noi suy
{
    int id = (int) (x/10);
    return (x - (double)id*10) * (10 + ca[id+1] - ca[id]) / 10 + id*10 + ca[id];
};

int deValue(String& inPut, String& outPut){	// String type {key}={val}
	String key;
    int startS = inPut.indexOf('{',0);
    int endS = inPut.indexOf('}',startS+1);
    key = inPut.substring(startS+1,endS);
    startS = inPut.indexOf('{', endS+1);
    endS = inPut.indexOf('}', startS+1);
    outPut = inPut.substring(startS + 1, endS);
    return key.toInt();
};
int getMemoryFree() {
  // Trong trường hợp này, ta có thể hiểu extern sẽ khai báo một biến toàn cục trong chương trình (nếu chưa có) hoặc include một biến toàn cục đã được extern trước đó
  extern int __heap_start;
  extern int *__brkval; 
  
  //Dấu & phía trước tên biến / tên con trỏ sẽ cho ta biết vị trí ô nhớ mà nó đang đứng
  //Lưu ý: bài viết này không dành cho beginner và bạn cần tưởng tượng một chút để có thể mườn tượng vấn đề
  return (int) SP - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}