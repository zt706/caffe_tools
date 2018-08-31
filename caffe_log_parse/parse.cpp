// regex_match example
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <chrono>
 
using namespace std;

// g++ -std=c++11 reg.cpp;
// g++ -std=c++11 -O3 parse.cpp; 能加速

int main ()
{
  string line;
  //string log_path = "/web/model-zoo/logs/admin/24c28a08-6612-4c8a-8c34-4c9a81f2d58c/0be22ee5-88f7-4a4b-8610-2cb356f50f40/0a0995d4-0722-408a-8cf6-7a173b39422e/train.log";
  string log_path = "/web/model-zoo/logs/admin/24c28a08-6612-4c8a-8c34-4c9a81f2d58c/3ffb73f5-997f-4c72-bcf4-8b2161bb6b40/050934f8-9fb4-45b3-826a-5338976b4d99/train.log";

  auto begin = chrono::high_resolution_clock::now();

  ifstream myfile (log_path);
  //ifstream myfile ("log.txt");

  vector<string> all_lines;
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
		all_lines.push_back(line);	
    }
    myfile.close();
	
  }
  else cout << "Unable to open file"; 

  string iterator;
  string train_loss;

  // fastest method	
  for(auto & str : all_lines)
  {
 		size_t pos_it = str.find("] Iteration ");
 		size_t pos_loss = str.find(", loss = ");
		if(pos_it != string::npos && pos_loss != string::npos)
    	{
      		//cout << str << '\n';
			string it = str.substr(pos_it + strlen("] Iteration "), pos_loss - pos_it - strlen("] Iteration "));
			string loss = str.substr(pos_loss + strlen(", loss = "), strlen(str.c_str()) - pos_loss - strlen(", loss = "));
      		//printf("%s %s \n", it.c_str(), loss.c_str());
			iterator += it + ",";
			train_loss += loss + ",";
		}
  }

  auto end = chrono::high_resolution_clock::now();    
  auto dur = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  cout << "c++ parse log used " << ms << "ms" << endl;

  // test total construct regex time
  auto t0 = chrono::high_resolution_clock::now();    
  for(auto & str : all_lines)
  {
  		std::smatch sm;
  		std::regex e(std::regex("(.*] Iteration )([0-9]+).*(loss = )([0-9].*[0-9]+).*", std::regex::optimize));
  		//std::regex e(std::regex(R"(.* Iteration .*)", std::regex::optimize));
  }
  auto t1 = chrono::high_resolution_clock::now();    
  auto d0 = t1- t0;
  auto m0 = std::chrono::duration_cast<std::chrono::milliseconds>(d0).count();
  cout << "construct regex used: " << m0 << "ms" << endl;
  

  // right method
  std::smatch sm;
  std::regex e(std::regex("(.*] Iteration )([0-9]+).*(loss = )([0-9].*[0-9]+).*", std::regex::optimize));
  //std::regex e(std::regex(R"(.* Iteration .*)", std::regex::optimize));
  for(auto & str : all_lines)
  {
  		if (std::regex_match (str, sm, e));
  }

  auto end2 = chrono::high_resolution_clock::now();    
  auto dur2 = end2 - t1;
  auto ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(dur2).count();
  cout << "regex parse log used " << ms2 << "ms" << endl;

  // error method
  // g++ -std=c++11 -O3 reg.cpp; 也不能比上面的写法快
  // 使用-O3的话和上面的做法是一样的时间,说明 -O3 也不能能优化掉这种重复构造很多次对象的错误做法
  // 这种写法之所以慢,时间都花在重复构造对象上面了 
  for(auto & str : all_lines)
  {
  		std::smatch sm;
  		std::regex e(std::regex("(.*] Iteration )([0-9]+).*(loss = )([0-9].*[0-9]+).*", std::regex::optimize));
  		//std::regex e(std::regex(R"(.* Iteration .*)", std::regex::optimize));
  		if (std::regex_match (str, sm, e));
  }

  auto end3 = chrono::high_resolution_clock::now();    
  auto dur3 = end3 - end2;
  auto ms3 = std::chrono::duration_cast<std::chrono::milliseconds>(dur3).count();
  cout << "badlly regex parse log used " << ms3 << "ms" << endl;

  //cout << iterator << endl;
  //cout << train_loss << endl;

  return 0;
}
