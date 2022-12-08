#ifndef MOUNT_H
#define MOUNT_H

#include <string>
#include <bits/stdc++.h>
#include "../lib/shared.h"
#include "../lib/structs.h"
#include "../lib/disco.h"

using namespace std;

class Mount
{
    public:
    Mount();
    typedef struct _MP
    {
        char letter;
        char status = '0';
        char name[16];
    }MountedPartition;

    typedef struct _MD{
        char path[150];
        char status = '0';
        MountedPartition mpartitions[26];
    }MountedDisk;

    MountedDisk mounted[99];

    void mount(vector<string> command);
    void unmount(vector<string> command);
    void mount(string path, string name);
    void unmount(string id);
    void listmount();

    Structs::Partition getMount(string id, string *p);

    private:
    Disk dsk;
    Shared shared;
    vector<char> alfabeto = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

};



#endif