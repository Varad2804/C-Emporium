#include<bits/stdc++.h>
using namespace std;
//this class has conatins some special algorithms..required for computing..
//Note: Our sorting algorithm would still work if any new product will be addded.. 
class Algos{
    public:static vector<string> Sort(int x,int y,vector<string>v,string onbasis)//Sorts the given subvector with given index ..
    {
        //typical bubble sort algo for sorting part of vector given its indices(start and end)....
        for(int i=x;i<y;i++)
        {
            for(int j=x;j<y-(i-x);j++)
            {
                if(compare(v[j],v[j+1],onbasis))
                {
                    swap(v[j],v[j+1]);//swap if price of this next element is greater than the next elememt or swap if this Name is lexographically larger than the next element Name.
                }
            }
        }
        return v;
    }
    public:static vector<string> split(string s,string delim)
    {
        //splits the line //statements into their corresponding words...
        vector<string>v;//this vectore will store all the words...
        int start, end = -1*delim.size();
        do
        {
            start = end + delim.size();
            end = s.find(delim, start);
            v.push_back(s.substr(start, end - start));
        }while (end != -1);
        
        return v;
    }
    static bool compare(string s1,string s2,string onbasis)
    {
        //compares the two given string either on the basis of the price or name...
        vector<string>temp1,temp2;//stores the splits of the s1,s2...
        temp1=split(s1," ");
        temp2=split(s2," ");
        if(onbasis=="Price")
        {
            return stof(temp1[4])>stof(temp2[4]);//we have price at 4 th index of the string according to the model given in problem statement for products...
        }
        else if(onbasis=="Name")
        {
            return temp1[2].compare(temp2[2])>0;//we have Name at 2 th index of the string according to the model given in problem statement for product..
        }
        return false;
    }
};