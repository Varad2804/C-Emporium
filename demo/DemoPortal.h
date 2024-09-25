#include"../ecomm/Portal.cpp"
#include<bits/stdc++.h>
#ifndef DemoPortal_H
#define DemoPortal_H
using namespace std;
class DemoPortal:public Portal{
	public:
		int PortalID;
		map<int,string>m;
		static int RequestID;
		DemoPortal(int);
    	// Invoked by main or driver class
		// sends command to Platform (by writing to PortalToPlatform)
		// Each command line in the file should have a PortalID as the first token 
		// and a RequestID as second token. 
		// PortalID is unique to each instance of Portal
		// Each request from a portal should have a unique ID
		virtual void processUserCommand(string);
		// checks for pending responses (in PortalToPlatform)
		// Displays response
		virtual void checkResponse();
};
#endif