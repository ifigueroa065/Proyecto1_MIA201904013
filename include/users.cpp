#include "../lib/users.h"

#include <iostream>
#include "string"

using namespace std;

Users::Users() {
}

bool Users::login(vector<string> context, Mount m) {
    mount = m;
    string id="";
    string usuario="";
    string password="";

    for (auto current : context) {
        string id_ = shared.lower(current.substr(0, current.find('=')));
        current.erase(0, id_.length() + 1);
        if (current.substr(0, 1) == "\"") {
            current = current.substr(1, current.length() - 2);
        }
        if (shared.compare(id_, "id")) {
                id = current;
        } else if (shared.compare(id_, "usuario") || shared.compare(id_, "usr")) {
                usuario = current;
        } else if (shared.compare(id_, "password") || shared.compare(id_, "pwd")) {
                password = current;
        }
    }

    if (id=="" || usuario=="" || password=="") {
        shared.handler("LOGIN", "requiere ciertos parámetros obligatorios");
        return false;
    }
    return login(usuario, password, id);
    /*if (required.size() != 0) {
        shared.handler("LOGIN", "requiere ciertos parámetros obligatorios");
        return false;
    }
    return login(usuario, password, id);
    */
}

bool Users::login(string u, string p, string id) {
    try {
        string path;
        Structs::Partition partition = mount.getmount(id, &path);

        Structs::Superblock super;
        FILE *rfile = fopen(path.c_str(), "rb");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Structs::Superblock), 1, rfile);


        Structs::Fileblock fb;
        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fread(&fb, sizeof(Structs::Fileblock), 1, rfile);
        fclose(rfile);

        string txt;
        txt += fb.b_content;

        vector<string> vctr = getElements(txt, '\n');
        for (string line:vctr) {
            if (line[2] == 'U' || line[2] == 'u') {
                vector<string> in = getElements(line, ',');
                if (shared.compare(in[3], u) && shared.compare(in[4], p)) {
                    shared.response("LOGIN", "logueado correctamente");
                    logged.id = id;
                    logged.user = u;
                    logged.password = p;
                    logged.uid = stoi(in[0]);
                    return true;
                }
            }
        }
        throw runtime_error("no hay credenciales similares");
    }
    catch (exception &e) {
        shared.handler("LOGIN", e.what());
        return false;
    }
}

vector<string> Users::getElements(string txt, char c) {
    vector<string> v;
    string line;
    if (c == ',') {
        txt.push_back(',');
    }
    for (char &x: txt) {
        if (x == c) {
            v.push_back(line);
            line = "";
            continue;
        }
        line += x;
    }

    if (v.empty()) {
        throw runtime_error("no hay archivo txt");
    }
    return v;
}

bool Users::logout() {
    shared.response("LOGOUT", "hasta luego " + logged.user);
    logged = User();
    return false;
}

void Users::grp(vector<string> context, string action) {
    vector<string> required = {"name"};
    string name;

    for (int i = 0; i < context.size(); i++) {
        string current = context.at(i);
        string id_ = current.substr(0, current.find("="));
        current.erase(0, id_.length() + 1);
        if (current.substr(0, 1) == "\"") {
            current = current.substr(1, current.length() - 2);
        }

        if (shared.compare(id_, "name")) {
            if (count(required.begin(), required.end(), id_)) {
                auto itr = find(required.begin(), required.end(), id_);
                required.erase(itr);
                name = current;
            }
        }
    }

    if (required.size() != 0) {
        shared.handler(action + "GRP", "requiere ciertos parámetros obligatorios");
        return;
    }
    if (shared.compare(action, "MK")) {
        mkgrp(name);
    } else {
        rmgrp(name);
    }
}

void Users::mkgrp(string n) {
    try {
        if (!(shared.compare(logged.user, "root"))) {
            throw runtime_error("no se puede realizar la acción sin el usuario root");
        }
        string path;
        Structs::Partition partition = mount.getmount(logged.id, &path);

        Structs::Superblock super;
        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Structs::Superblock), 1, rfile);

        Structs::Fileblock fb;
        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fread(&fb, sizeof(Structs::Fileblock), 1, rfile);

        string txt;
        txt += fb.b_content;

        vector<string> vctr = getElements(txt, '\n');
        int c = 0;
        for (string line:vctr) {
            if ((line[2] == 'G' || line[2] == 'g')) {
                c++;
                vector<string> in = getElements(line, ',');
                if (in[2] == n) {
                    if (line[0] == '0') {

                    } else {
                        throw runtime_error("el grupo ya existe");
                    }
                }
            }
        }
        txt += to_string(c + 1) + ",G," + n + "\n";
        strcpy(fb.b_content, txt.c_str());
        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fwrite(&fb, sizeof(Structs::Fileblock), 1, rfile);
        fclose(rfile);
        shared.response("MKGRP", "grupo creado correctamente");
    }
    catch (exception &e) {
        shared.handler("MKGRP", e.what());
    }
}

void Users::rmgrp(string n) {
    try {
        if (!(shared.compare(logged.user, "root"))) {
            throw runtime_error("no se puede realizar la acción sin el usuario root");
        }
        string path;
        Structs::Partition partition = mount.getmount(logged.id, &path);

        Structs::Superblock super;
        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Structs::Superblock), 1, rfile);


        Structs::Fileblock fb;
        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fread(&fb, sizeof(Structs::Fileblock), 1, rfile);

        string txt;
        txt += fb.b_content;

        bool exist = false;
        vector<string> vctr = getElements(txt, '\n');
        txt = "";
        for (string line:vctr) {
            if ((line[2] == 'G' || line[2] == 'g') && line[0] != '0') {
                vector<string> in = getElements(line, ',');
                if (in[2] == n) {
                    exist = true;
                    txt += to_string(0) + ",G," + in[2] + "\n";
                    continue;
                }
            }
            txt += line + "\n";
        }
        if (!exist) {
            throw runtime_error("el grupo no existe");
        }
        strcpy(fb.b_content, txt.c_str());

        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fwrite(&fb, sizeof(Structs::Fileblock), 1, rfile);
        fclose(rfile);
        shared.response("RMGRP", "grupo eliminado correctamente");
    }
    catch (exception &e) {
        shared.handler("RMGRP", e.what());
    }
}

void Users::usr(vector<string> context, string action) {
    vector<string> required;
    if (shared.compare(action, "MK")) {
        required = {"usr", "pwd", "grp"};
    } else {
        required = {"usr"};
    }
    string usr;
    string pwd;
    string grp;

    for (int i = 0; i < context.size(); i++) {
        string current = context.at(i);
        string id_ = current.substr(0, current.find("="));
        current.erase(0, id_.length() + 1);
        if (current.substr(0, 1) == "\"") {
            current = current.substr(1, current.length() - 2);
        }

        if (shared.compare(id_, "usr")) {
            if (count(required.begin(), required.end(), id_)) {
                auto itr = find(required.begin(), required.end(), id_);
                required.erase(itr);
                usr = current;
            }
        } else if (shared.compare(id_, "pwd")) {
            if (count(required.begin(), required.end(), id_)) {
                auto itr = find(required.begin(), required.end(), id_);
                required.erase(itr);
                pwd = current;
            }
        } else if (shared.compare(id_, "grp")) {
            if (count(required.begin(), required.end(), id_)) {
                auto itr = find(required.begin(), required.end(), id_);
                required.erase(itr);
                grp = current;
            }
        }
    }

    if (required.size() != 0) {
        shared.handler(action + "GRP", "requiere ciertos parámetros obligatorios");
        return;
    }
    if (shared.compare(action, "MK")) {
        mkusr(usr, pwd, grp);
    } else {
        rmusr(usr);
    }
}

void Users::mkusr(string usr, string pwd, string grp) {
    try {
        if (!(shared.compare(logged.user, "root"))) {
            throw runtime_error("no se puede realizar la acción sin el usuario root");
        }
        string path;
        Structs::Partition partition = mount.getmount(logged.id, &path);

        Structs::Superblock super;
        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Structs::Superblock), 1, rfile);


        Structs::Fileblock fb;
        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fread(&fb, sizeof(Structs::Fileblock), 1, rfile);

        string txt;
        txt += fb.b_content;

        vector<string> vctr = getElements(txt, '\n');
        int c = 0;
        for (string line:vctr) {
            if ((line[2] == 'U' || line[2] == 'u') && line[0] != '0') {
                c++;
                vector<string> in = getElements(line, ',');
                if (in[3] == usr) {
                    throw runtime_error("el usuario ya existe");
                }
            }
        }
        txt += to_string(c + 1) + ",U," + grp + "," + usr + "," + pwd + "\n";
        strcpy(fb.b_content, txt.c_str());

        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fwrite(&fb, sizeof(Structs::Fileblock), 1, rfile);

        fclose(rfile);
        shared.response("MKUSR", "usuario creado correctamente");
    }
    catch (exception &e) {
        shared.handler("MKUSR", e.what());
    }
}

void Users::rmusr(string usr) {
    try {
        if (!(shared.compare(logged.user, "root"))) {
            throw runtime_error("no se puede realizar la acción sin el usuario root");
        }
        string path;
        Structs::Partition partition = mount.getmount(logged.id, &path);

        Structs::Superblock super;
        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Structs::Superblock), 1, rfile);


        Structs::Fileblock fb;
        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fread(&fb, sizeof(Structs::Fileblock), 1, rfile);

        string txt;
        txt += fb.b_content;

        bool exist = false;
        vector<string> vctr = getElements(txt, '\n');
        txt = "";
        for (string line:vctr) {
            if ((line[2] == 'U' || line[2] == 'u') && line[0] != '0') {
                vector<string> in = getElements(line, ',');
                if (in[3] == usr) {
                    exist = true;
                    txt += to_string(0) + ",U," + in[2] + "," + in[3] + "," + in[4] + "\n";
                    continue;
                }
            }
            txt += line + "\n";
        }
        if (!exist) {
            throw runtime_error("el usuario no existe");
        }
        strcpy(fb.b_content, txt.c_str());

        fseek(rfile, super.s_block_start + sizeof(Structs::Folderblock), SEEK_SET);
        fwrite(&fb, sizeof(Structs::Fileblock), 1, rfile);
        fclose(rfile);
        shared.response("RMUSR", "usuario eliminado correctamente");
    }
    catch (exception &e) {
        shared.handler("RMUSR", e.what());
    }
}

