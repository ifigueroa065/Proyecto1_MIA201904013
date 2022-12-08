#ifndef SHARED_H
#define SHARED_H

#include <string>
#include <bits/stdc++.h>

using namespace std;

class Shared
{
public:
    Shared();
    string upper(string s);
    string lower(string s);
    bool compare(string s1, string s2);
    void handler(string title, string message);
    bool confirmation(string title, string message);
    void response(string title, string message);

};

#endif // END OF DECLARATION