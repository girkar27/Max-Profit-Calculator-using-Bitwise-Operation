#include <iostream>
#include <typeinfo>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>
#include <sys/stat.h>

using namespace std;
//declaring global variables for market and sample pricelist
string market_price_file;
string price_list_file;
map<string, int> marketprice_map;

bool is_integer(string s)
{
  for (int i = 0; i < s.length(); i++){
      if (isdigit(s[i]) == false){
          return false;
      }
  }   
  return true;
}

int typecast_to_int(string line){
  stringstream sstream(line);
  stringstream ss;
  int val;
  ss << line; ss >> val;
  return val;
}

void return_key_value(string line, string &cardName, string &cardCost){
  char* char_line = new char [line.length() + 1];
  strcpy(char_line, line.c_str());
  char* each_word = strtok(char_line, " "); cardName = each_word;
  each_word = strtok(NULL, " "); cardCost = each_word;

}

void write_op_file(int set_count_var, int maxProfit, vector<string> &max_subset, double time_taken, bool ignore_flag, string err_msg){
  fstream op_file;
  string line;
  int card_line_counter = 0;
  op_file.open("output.txt",ios::app);
  if (op_file.is_open()){
    int subset_size = max_subset.size();
    
    if (ignore_flag){
      op_file << err_msg << "\n";
    }else{
      op_file << set_count_var << " " << maxProfit << " " << subset_size << " " << time_taken << "\n";
      for (int i=0;i < subset_size;i++){
        op_file << max_subset[i] << "\n";
      }
    }
  }
  op_file.close();
}


int price_list_comparison(){
  fstream price_list_f;
  string line;
  int card_line_counter = 0;
  map<string, int> price_list_set_map; vector<string> price_list_vector; 
  int set_count_var; int set_weight; int line_counter=0; string err_msg = "";
  
  // empty file validation start-----------------
  price_list_f.open(price_list_file,ios::in);
  if (price_list_f.is_open()){
    price_list_f.seekg(0, ios::end); 
    int length = price_list_f.tellg();
    if (length == 0){
      err_msg = "Empty price_list file...MAxProfit cannot be computed";
      // cout << err_msg;
      bool ignore_flag=true;
      vector<string> temp;
      int maxProfit= 0;
      double time_taken = 0.0;
      write_op_file(set_count_var, maxProfit, temp, time_taken, ignore_flag, err_msg);
      price_list_f.close();
      exit(0);
    }
  }
  price_list_f.close();
  // file validation complete-------------------------

  price_list_f.open(price_list_file,ios::in);
  if (price_list_f.is_open()){ 
    int total_set_weight = 0;
    while(getline(price_list_f , line)){
        string cardName; string cardCost;
        if (line == ""){
          continue;
        }
        return_key_value(line, cardName, cardCost);
        int cardCost_int = stoi(cardCost);       
        bool new_set = false;
        //set condition...
        if ((is_integer(cardName)) && is_integer(cardCost)){
          new_set = true;
          set_count_var = stoi(cardName);
          set_weight = cardCost_int;
          card_line_counter = 0;
          continue;
        }

        card_line_counter ++;
        if (card_line_counter > set_count_var){
          cout << "SetCount :" << set_count_var << " Weight :" << set_weight <<  
          " Error: baseball set count and no of cards in set unmatching" << endl;
          continue;
        }

        total_set_weight += cardCost_int; 
        price_list_set_map[cardName] = cardCost_int;
        price_list_vector.push_back(cardName);
        // cout << "dictv" << price_list_set_map[cardName] << endl;
        line_counter ++;
        // generate power set and append logic...
        if (line_counter == set_count_var){
          if (price_list_vector.size() > 0){
            int max_cumulative_cost = 0;bool ignore_flag = false;int maxProfit = 0;int market_cumulation=0;
            vector<string> max_subset; string error_card; 

            //for best case complexity.....where total cost < weight .. print all cards.           
            if (total_set_weight <= set_weight){
              clock_t start, end;
              start =  clock();
              
              bool ignore_flag=false;
              int temp_maxProfit = 0;
              for(int i=0;i < price_list_vector.size(); i++){
                string card = price_list_vector[i];

                if(marketprice_map.find(card) == marketprice_map.end()){
                    // cout << "SetCount=" << set_count_var << endl; 
                    // cout << "Baseball card " << card << " not present in marketprice file --> ignoring subset\n" << endl;
                    err_msg = "Error->Card: " + card + " not present in marketprice file --> ignoring subset";
                    ignore_flag = true;
                    error_card =  card;
                    break;
                }
                temp_maxProfit += marketprice_map[card];
              }
              end  = clock();
              double time_taken = double(end - start)/double(CLOCKS_PER_SEC);
              setprecision(10);
              
              cout << "Best Case Complexity Hit... when cumulative purchase cost is less than than set weight"  <<  endl;
              int maxProfit = temp_maxProfit - total_set_weight;

              write_op_file(set_count_var, maxProfit, price_list_vector, time_taken, ignore_flag, err_msg); 
              // cout << "max profit-----" << set_count_var << " " <<  market_cumulation << " " << max_cumulative_cost << " " << max_subset.size() << " max_cumulative_cost " << maxProfit  << endl;
              line_counter = 0;
              price_list_vector.clear();
              price_list_set_map.clear();
              continue;
            }
            //best case end....

            clock_t start, end;
            start =  clock();

            //outer loop from 0 to 2^n.........
            for (int i=0; i<(1<<set_count_var);i++){
              if (ignore_flag){
                break;
              }
              int ccost = 0;
              int temp_market_cumulation = 0;
              vector<string> temp_subset;
  
              //inner loop to calculate setss...
              for (int j=0; j<set_count_var; j++){
                if((1<<j) &i){
                  string card = price_list_vector[j];
                  ccost += price_list_set_map[card];

                  if(marketprice_map.find(card) == marketprice_map.end()){
                    // cout << "SetCount=" << set_count_var << endl; 
                    // cout << "Baseball card " << card << " not present in marketprice file --> ignoring subset\n" << endl;
                    err_msg = "Error->Card: " + card + " not present in marketprice file --> ignoring subset";
                    ignore_flag = true;
                    error_card =  card;
                    break;
                  }else{
                    // calculating profit for each card
                    int profit_each_card = marketprice_map[card] - price_list_set_map[card];
                    temp_market_cumulation += profit_each_card;
                    temp_subset.push_back(card);
                  }
                }
              }
              // calculating maximum profit for each subset
              if ((!ignore_flag) && (ccost <= set_weight) && (temp_market_cumulation > maxProfit)){  
                max_cumulative_cost = ccost;
                maxProfit = temp_market_cumulation;
                max_subset = temp_subset;  
              }
            }
            end  = clock();
            double time_taken = double(end - start)/double(CLOCKS_PER_SEC);
            setprecision(10);
            // cout << "Time taken by program is : " << fixed << setprecision(10) << time_taken;
            // cout << " sec " << endl;
            write_op_file(set_count_var, maxProfit, max_subset, time_taken, ignore_flag, err_msg); 
            
            ignore_flag=false; 
            line_counter = 0;
            price_list_vector.clear();
            price_list_set_map.clear();
            }         
        }
    }
  }
  price_list_f.close();
  return 0;
}
bool generate_market_file_map(string &market_price_file){
  fstream market_price_f;
  
  market_price_f.open(market_price_file,ios::in);
  
  if (market_price_f.is_open()){
    string line;
    bool first = true;
    int total_card_count;
    while(getline(market_price_f , line)){
      stringstream sstream(line);
      if (line == ""){
          continue;
      }
      if (first == true){
        first = false;
        total_card_count = stoi(line);
      } 
      int counter = 0;
      string def_key;
      int def_value = -1;
      while (getline(sstream, line, ' ')){
        if ((def_value == -1) && (counter % 2 == 0)){
          def_key = line;
          marketprice_map[def_key] = def_value; 
        }
        else{
          // typecasting----
          int market_cost = typecast_to_int(line);
          marketprice_map[def_key] = market_cost;
          def_key="NULL";
        }  
        counter ++;
      }
    }
  }    
  market_price_f.close();
  return true;
}
int main(int argc, char** argv){
  if(argc != 3){
    cout << "Args missing: Needed <program1> <market-price-file> <price-list-file> " << endl;
    exit(0);
  }

  market_price_file =  argv[1]; price_list_file =  argv[2];  
  string file;
  for(int i=0;i<2;i++){
    if (i == 0){
      file = market_price_file; 
    }else{
      file = price_list_file;
    }

    const char* dir = &file[0];
    struct stat sb;
    if (stat(dir, &sb) != 0){
      cout << file <<  " Directory does not exist.";
      exit(0);
    }
  }
  const char* dir = "output.txt";
  struct stat sb;
  if (stat(dir, &sb) == 0){
    int result = remove("output.txt");
  }
  bool is_market_file_generated = generate_market_file_map(market_price_file);
  int custom = price_list_comparison();
  cout << "-----------------------------Program Executed------------------------------------" << endl;
}