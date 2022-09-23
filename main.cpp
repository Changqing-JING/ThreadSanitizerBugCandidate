#include <thread>
#include <mutex>
#include <atomic>

std::mutex m1;
std::mutex m2;
std::mutex m3;

std::atomic<int> counter {0};

void foo(){
  std::lock_guard<std::mutex> lock1(m1);
  std::lock_guard<std::mutex> lock2(m2);
}

void xoo(){
  foo();
  std::lock_guard<std::mutex> lock2(m2);
  std::lock_guard<std::mutex> lock3(m3);
}

void yoo(){
  std::unique_lock<std::mutex> lock3(m3);
  std::unique_lock<std::mutex> lock1(m1);
}

int main(){
  std::thread th1([]{
    while(true){
      xoo();
      if(counter>10){
        break;
      }
    }
  });

  std::thread th2([]{
    while(true){
      yoo();
      counter++;
      if(counter>10){
        break;
      }
    }
  });

  th1.join();
  th2.join();
}