#include "../lib/mount.h"

#include <iostream>
#include <stdlib.h>
#include "string"
#include <locale>

using namespace std;

Mount::Mount(){}

void Mount::mount(vector<string> command){
    if(command.empty()){
        listmount();
        return;
    }
    vector<string> required = { "path", "name" };
    string path;
    string name;

    for (auto current : command){
        string id = shared.lower(current.substr(0, current.find("=")));
        current.erase(0, id.length() + 1);
        if(current.substr(0,1) == "\""){
            current.substr(1,current.length()-2);
        };

        if(shared.compare(id, "name")){
            if(count(required.begin(), required.end(), id)){
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                name = current;
            }
        } else if (shared.compare(id, "path")) {
            if (count(required.begin(), required.end(), id)) {
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                path = current;
            }
        }
    };
    if (required.size() != 0) {
        shared.handler("MOUNT", "requiere ciertos parámetros obligatorios");
        return;
    };
    mount(path, name);
}

void Mount::mount(string p, string n){
    try{
        FILE *validate = fopen(p.c_str(), "r");
        if (validate == NULL) {
            shared.handler("MOUNT", "Disco no existe");
        }
        Structs::MBR disk;
        rewind(validate);
        fread(&disk, sizeof(Structs::MBR), 1, validate);
        fclose(validate);
        
        Structs::Partition partition = dsk.findby(disk, n, p);
        if (partition.part_type == 'E') {
            vector<Structs::EBR> ebrs = dsk.getlogics(partition, p);
            if (!ebrs.empty()) {
                Structs::EBR ebr = ebrs.at(0);
                n = ebr.part_name;
                //shared.handler("", "se montará una partición lógica");
            } else {
                throw runtime_error("No se puede montar una extendida");
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mounted[i].path == p) {
                for (int j = 0; j < 26; j++) {
                    if (Mount::mounted[i].mpartitions[j].status == '0') {
                        mounted[i].mpartitions[j].status = '1';
                        mounted[i].mpartitions[j].letter = alfabeto.at(j);
                        strcpy(mounted[i].mpartitions[j].name, n.c_str());
                        string re = to_string(i + 1) + alfabeto.at(j);
                        shared.response("MOUNT", "se ha realizado correctamente el mount -id=65" + re);
                        return;
                    }
                }
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mounted[i].status == '0') {
                mounted[i].status = '1';
                strcpy(mounted[i].path, p.c_str());
                for (int j = 0; j < 26; j++) {
                    if (Mount::mounted[i].mpartitions[j].status == '0') {
                        mounted[i].mpartitions[j].status = '1';
                        mounted[i].mpartitions[j].letter = alfabeto.at(j);
                        strcpy(mounted[i].mpartitions[j].name, n.c_str());
                        string re = to_string(i + 1) + alfabeto.at(j);
                        shared.response("MOUNT", "se ha realizado correctamente el mount -id=65" + re);
                        return;
                    }
                }
            }
        }

    }catch(exception &e){
        shared.handler("MOUNT", e.what());
        return;
    }

}

void Mount::listmount() {
    cout << "\n<-------------------------- MOUNTS -------------------------->"
        << endl;
    for (int i = 0; i < 99; i++) {
        for (int j = 0; j < 26; j++) {
            if (mounted[i].mpartitions[j].status == '1') {
                cout << "> 65" << i + 1 << alfabeto.at(j) << ", " << mounted[i].mpartitions[j].name << endl;
            }
        }
    }
}