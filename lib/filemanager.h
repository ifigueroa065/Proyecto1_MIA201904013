#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <bits/stdc++.h>

#include "../lib/shared.h"
#include "../lib/structs.h"
#include "../lib/mount.h"

using namespace std;

class FileManager {
public:
    FileManager();

    void mkdir(vector<string> context, Structs::Partition partition, string p);


    vector<string> getpath(string s);

    int getfree(Structs::Superblock spr, string pth, string t);

    void updatebm(Structs::Superblock spr, string pth, string t);

    void newfileblock(Structs::Superblock super);
private:
    void mkdir(vector<string> path, bool p, Structs::Partition partition, string pth);


    Mount mount;
    Shared shared;
};

#endif // END OF DECLARATION