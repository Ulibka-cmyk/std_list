#include <Arduino.h>
// #include <vector>     // vector
#include <list>
#include <memory>
#include <Printable.h>


// using namespace std;

class TTimerData : public Printable  // для возможности прямой печати

{
  public:
  float t1; 
  float t2; 
  uint32_t time_on; 
  char _tt[20]; // для увеличения размера класса
  String str;
  

  TTimerData(float ptick1_ON,   float ptick2_OFF, uint32_t ptime_on);
  TTimerData( uint32_t ptime_on);

  bool operator >(TTimerData const & a);
  bool operator <(TTimerData const & a);
  
  size_t printTo(Print& p) const;
};


size_t TTimerData::printTo(Print& p) const{
  size_t count;
  count = p.print(F("TTimerData:: t1 : "));
  count += p.print(t1);
  count += p.print(F(", tick2_OFF : "));
  count += p.print(t2);
  count += p.print(F(", time_on : "));
  count += p.print(time_on);
  count += p.print(F(" "));
  count += p.print(str);
  count += p.print(F(" "));
  return count;
}

TTimerData::TTimerData(float ptick1_ON,   float ptick2_OFF, uint32_t ptime_on) :   t1(ptick1_ON), t2(ptick2_OFF), time_on(ptime_on) {}
TTimerData::TTimerData( uint32_t ptime_on) : time_on(ptime_on) {
  str = "str" + String(time_on);
}


bool TTimerData::operator >(TTimerData const & a) {
      return this->time_on > a.time_on;
}

bool TTimerData::operator <(TTimerData const & a) {
      return this->time_on < a.time_on;
}



void PrintHeap(const char * msg) {
   Serial.print(msg);
   Serial.println(ESP.getFreeHeap());
}


void PrintHeap(const __FlashStringHelper * msg) {
   Serial.print(msg);
   Serial.print(" stack : ");
   Serial.print(ESP.getFreeContStack());
   Serial.print(" heap : ");
   Serial.println(ESP.getFreeHeap());
}

// *******************************************
// Вариант 1 : храним указатели на  объекты в List
// *******************************************

typedef std::shared_ptr<TTimerData> TTimerDataPtr    ; // необходимо для автоматического удаления памяти под объектом
typedef std::list<TTimerDataPtr> TTimerDataList;       

void change_String1(TTimerDataList& l)
{
  for(TTimerDataList::iterator it=l.begin();it!=l.end();it++) {
      TTimerDataPtr& a= *it;
      TTimerData& b = *a;
      b.str += "dop******************************************************************************************************************************";
    }
    
}  

void show_TTimerData1(TTimerDataList& l)
{
  
  Serial.println("--------------------");
  for(TTimerDataList::iterator it=l.begin();it!=l.end();it++) {
      
      TTimerData& b = * (*it);

      Serial.print( b.time_on);
      Serial.print(" ");
      Serial.print(b.str);
      Serial.print(" ");
    }
    Serial.println();
}   

void testList1 () {



  PrintHeap(F("testList1 Start : "));
  TTimerDataList lst;

  
   for (uint32_t i = 100 ; i>10;i--) {
       // безопасно создаем умный указатель
       auto new_order = std::make_shared<TTimerData>(0,0,i); 
       lst.push_back(new_order);
    }

   PrintHeap(F("testList1 List заполнен : "));  
   lst.sort();
   PrintHeap(F("testList1 List sort : "));  

  change_String1(lst);
  show_TTimerData1(lst);
  PrintHeap(F("testList1 change_String1 : "));  

   lst.clear();
   PrintHeap(F("testList1 List clear : "));  
}


// *******************************************
// Вариант 2 : храним сами объекты в List
// *******************************************

typedef std::list<TTimerData> TListInside;  

void show_TTimerData(TListInside& l)
{

  // std::list<int>::const_iterator it;
  Serial.println("--------------------");
  for(TListInside::iterator it=l.begin();it!=l.end();it++) {
      Serial.print( it->time_on);
      Serial.print(" ");
      Serial.print(it->str);
      Serial.print(" ");
      yield(); // позволяет ESP8266 выполнить служебные задачи, в частности сбросить Soft reset
    }
    Serial.println();
}   


void change_String(TListInside& l)
{
  for(TListInside::iterator it=l.begin();it!=l.end();it++) {
      it->str += "dop0123456789012345678901234567890123456789"; 
      
    }
    
}   

void testList2() {


   PrintHeap(F("testList2 Start : "));

   TListInside lst;

  for (uint32_t i = 100 ; i>10;i--) {
    lst.emplace_back(0,0,i);  // создает объект внутри List и сразу вызывает конструктор TTimerData
    lst.push_back(  TTimerData(i+200) );
  }
  
  PrintHeap(F("testList2 List заполнен : "));

  
  change_String(lst);
  show_TTimerData(lst);
  
  PrintHeap(F("\ntestList2 change_String : "));

  lst.clear();
  PrintHeap(F("testList2 Конец : "));
}



void setup()
{
  delay(3000); // чтобы монитор успел стартовать
  Serial.begin(115200);
  Serial.println(F("*****************************************************************************************\n"));
  
  PrintHeap("Запуск test : ");

  testList1();
  Serial.println(F("\n======================\n"));

  testList2();

  PrintHeap("\nКонец test : ");  

}

void loop() {}
