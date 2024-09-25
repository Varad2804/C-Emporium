#include"DemoPortal.h"
#include<fstream>
#include"Algos.cpp"
#include<bits/stdc++.h>
using namespace std;
int DemoPortal::RequestID=1;
DemoPortal::DemoPortal(int ID): Portal()//constructs a new portal..
{
    this->PortalID=ID;//initializes the Portal ID to the default value ..(1)..
}
void DemoPortal::checkResponse()
{
    ifstream Myfile("PlatformToPortal.txt");//making obj of ifstream with file as "PLatformToPortal.txt"
    string myText;
    int count=0;
    vector<string>v;
    while(getline(Myfile,myText))//read the files.....
    {
        count++;
        v.push_back(myText);//push all the responses in the vector
    }
    if(count==0)//if count ==0 implies there has been no response form the PLatform till now...
    {
        cout<<"No Response from Portal"<<endl;
        return;
    }
    vector<string>temp;//this vector is used to store the split format of the query...
    for(int i=0;i<count;i++)
    {
        temp=Algos::split(v[i]," ");////this vector is used to store the split format of the query...
        int firstindex=0,secondindex=0;
        //firstindex and second index coorespond to the particular subvector we need to sort...as there might a group of Responses back to back for List request...
        if(this->m.count(stoi(temp[1]))>0)
        {
            int firstindexRequestID=stoi(temp[1]);//RequestID of the firstelemnt which is a response of Listing query...
            firstindex=i;
            while(i<count)//loop runs till we get elements with requestID==firstRequestID... 
            {
                temp=Algos::split(v[i]," ");
                int presentRequestID=stoi(temp[1]);//RequestID of the cuurent element...
                if(presentRequestID!=firstindexRequestID)
                {
                    i--;
                    break;
                }
                i++;
            }
            secondindex=min(i,count-1);//secondindex is minimum of i,count-1...I took this condition so that we dont end up assigning secondindex to a outofbound index...
            v=Algos::Sort(firstindex,secondindex,v,this->m[firstindexRequestID]);
            // for(int i=0;i<v.size();i++)
            // {
            //     cout<<v[i]<<endl;
            // }
        }
    }
    for(int i=0;i<count;i++)
    {
        temp=Algos::split(v[i]," ");
        if(this->m.count(stoi(temp[1]))>0)//this condition is for those requests whose requests was of the type of Listing products..
        {
            cout<<"ProductName:"<<temp[2]<<" ProductID:"<<temp[3]<<" Price:"<<temp[4]<<" Available_Pieces:"<<temp[5]<<endl;
        }
        else//printing for other type of requests...
        {
            for(int j=2;j<temp.size();j++)
            {
                cout<<temp[j]<<" ";
            }
            cout<<endl;
        }
    }
    //as we have checked the PlatformToPortal.txt file now we must remove those requests for which we processes and printed
    ofstream Myfile1;
    Myfile1.open("PlatformToPortal.txt",std::ofstream::out | std::ofstream::trunc);
    Myfile1.close();
    Myfile.close();
}
void DemoPortal::processUserCommand(string query)
{
    vector<string>temp;//this vector is used to store the split format of the query...
    temp=Algos::split(query," ");
    freopen("PortalToPlatform.txt","a",stdout);//open the file PortalToPlatform to write and stdout to write now in PortalToPlatform.txt
    if(query[0]=='L')
    {
        //PortalID RequestID List Category...
        //also we need not send sortoreder type to portal...
        cout<<PortalID<<" "<<RequestID<<" "<<temp[0]<<" "<<temp[1]<<endl;
        vector<string>splits=Algos::split(query," ");
        this->m[RequestID]=splits[2];//we add this to dictionary as we need to sort the response of this type of query after we reieve from platform
    }
    els
    {
        //PortalID RequestID query..either start or Buy ProductID numitems..
        cout<<PortalID<<" "<<RequestID<<" "<<query<<endl;
    }
    freopen("/dev/tty","a",stdout);//returning the stdout to terminal...
    RequestID++;
    cout<<"Request has been passed on to the Platform..."<<endl;
}