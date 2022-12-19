#include "../lib/filemanager.h"

#include <iostream>
#include "string"

using namespace std;

FileManager::FileManager() {
}

void FileManager::mkdir(vector<string> context, Structs::Partition partition, string pth) {
    vector<string> required = {"path"};
    bool p = false;
    string path;

    for (auto current : context) {
        string id = shared.lower(current.substr(0, current.find('=')));
        current.erase(0, id.length() + 1);
        if (current.substr(0, 1) == "\"") {
            current = current.substr(1, current.length() - 2);
        }

        if (shared.compare(id, "path")) {
            if (count(required.begin(), required.end(), id)) {
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                path = current;
            }
        } else if (shared.compare(id, "p")) {
            p = true;
        }
    }
    if (!required.empty()) {
        shared.handler("MKDIR", "requiere ciertos parámetros obligatorios");
        return;
    }
    vector<string> tmp = getpath(path);
    mkdir(tmp, p, partition, pth);
}

void FileManager::mkdir(vector<string> path, bool p, Structs::Partition partition, string pth) {
    try {
        Structs::Superblock spr;
        Structs::Inodes inode;
        Structs::Folderblock folder;
        Structs::Pointerblock pointer;
        FILE *bfile = fopen(pth.c_str(), "rb+");

        fseek(bfile, partition.part_start, SEEK_SET);
        fread(&spr, sizeof(Structs::Superblock), 1, bfile);

        fseek(bfile, spr.s_inode_start, SEEK_SET);
        fread(&inode, sizeof(Structs::Inodes), 1, bfile);

        string newf;
        if (path.empty()) {
            throw runtime_error("no se ha brindado una path válida");
        }
        int past;
        int bi;
        int bb;
        bool fnd = false;
        Structs::Inodes inodetmp;
        Structs::Folderblock foldertmp;

        newf = path.back();
        int father = 0;
        path.pop_back();
        string stack;
        for (int v = 0; v < path.size(); ++v) {
            fnd = false;
            for (int i = 0; i < 15; ++i) {
                if (i < 12) {
                    if (inode.i_block[i] != -1) {
                        folder = Structs::Folderblock();
                        fseek(bfile, spr.s_block_start + (sizeof(Structs::Folderblock) * inode.i_block[i]), SEEK_SET);
                        fread(&folder, sizeof(Structs::Folderblock), 1, bfile);
                        for (int j = 0; j < 4; j++) {
                            if (shared.compare(folder.b_content[j].b_name, path.at(v))) {
                                fnd = true;
                                father = folder.b_content[j].b_inodo;
                                inode = Structs::Inodes();
                                fseek(bfile,
                                      spr.s_inode_start + (sizeof(Structs::Inodes) * folder.b_content[j].b_inodo),
                                      SEEK_SET);
                                fread(&inode, sizeof(Structs::Inodes), 1, bfile);
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                } else if (i == 12) {
                    if (inode.i_block[i] != -1) {
                        pointer = Structs::Pointerblock();
                        fseek(bfile, spr.s_block_start + (sizeof(Structs::Pointerblock) * inode.i_block[i]), SEEK_SET);
                        fread(&pointer, sizeof(Structs::Pointerblock), 1, bfile);

                        for (int b_pointer : pointer.b_pointers) {
                            if (b_pointer != -1) {
                                folder = Structs::Folderblock();
                                fseek(bfile, spr.s_block_start + (sizeof(Structs::Folderblock) * b_pointer),
                                      SEEK_SET);
                                fread(&folder, sizeof(Structs::Folderblock), 1, bfile);
                                for (auto &j : folder.b_content) {
                                    if (shared.compare(j.b_name, path.at(v))) {
                                        fnd = true;
                                        father = j.b_inodo;
                                        inode = Structs::Inodes();
                                        fseek(bfile,
                                              spr.s_inode_start + (sizeof(Structs::Inodes) * j.b_inodo),
                                              SEEK_SET);
                                        fread(&inode, sizeof(Structs::Inodes), 1, bfile);
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                } else if (i == 13) {
                    if (inode.i_block[i] != -1) {
                        pointer = Structs::Pointerblock();
                        fseek(bfile, spr.s_block_start + (sizeof(Structs::Pointerblock) * inode.i_block[i]), SEEK_SET);
                        fread(&pointer, sizeof(Structs::Pointerblock), 1, bfile);

                        for (int b_pointer : pointer.b_pointers) {
                            if (b_pointer != -1) {
                                Structs::Pointerblock pointer2;
                                fseek(bfile, spr.s_block_start + (sizeof(Structs::Pointerblock) * inode.i_block[i]),
                                      SEEK_SET);
                                fread(&pointer2, sizeof(Structs::Pointerblock), 1, bfile);
                                for (int b_pointer2 : pointer2.b_pointers) {
                                    if (b_pointer2 != -1) {
                                        folder = Structs::Folderblock();
                                        fseek(bfile, spr.s_block_start + (sizeof(Structs::Folderblock) * b_pointer2),
                                              SEEK_SET);
                                        fread(&folder, sizeof(Structs::Folderblock), 1, bfile);
                                        for (auto &j : folder.b_content) {
                                            if (shared.compare(j.b_name, path.at(v))) {
                                                fnd = true;
                                                father = j.b_inodo;
                                                inode = Structs::Inodes();
                                                fseek(bfile,
                                                      spr.s_inode_start + (sizeof(Structs::Inodes) * j.b_inodo),
                                                      SEEK_SET);
                                                fread(&inode, sizeof(Structs::Inodes), 1, bfile);
                                                break;
                                            }
                                        }
                                    } else {
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            }
            if (!fnd) {
                if (p) {
                    stack += "/" + path.at(v);
                    mkdir(getpath(stack), false, partition, pth);
                    v = -1;
                    fseek(bfile,
                          spr.s_inode_start,
                          SEEK_SET);
                    fread(&inode, sizeof(Structs::Inodes), 1, bfile);
                } else {
                    throw runtime_error("no es posible crear el directorio");
                }
            }
        }

        fnd = false;
        for (int i = 0; i < 15; ++i) {
            if (inode.i_block[i] != -1) {
                if (i < 12) {
                    fseek(bfile, spr.s_block_start + (sizeof(Structs::Folderblock) * inode.i_block[i]), SEEK_SET);
                    fread(&folder, sizeof(Structs::Folderblock), 1, bfile);
                    for (int j = 0; j < 4; ++j) {
                        if (folder.b_content[j].b_inodo == -1) {
                            past = inode.i_block[i];
                            bi = getfree(spr, pth, "BI");
                            bb = getfree(spr, pth, "BB");

                            inodetmp.i_uid = 1;
                            inodetmp.i_gid = 1;
                            inodetmp.i_size = sizeof(sizeof(Structs::Folderblock));
                            inodetmp.i_atime = spr.s_umtime;
                            inodetmp.i_ctime = spr.s_umtime;
                            inodetmp.i_mtime = spr.s_umtime;
                            inodetmp.i_type = 0;
                            inodetmp.i_perm = 664;
                            inodetmp.i_block[0] = bb;

                            strcpy(foldertmp.b_content[0].b_name, ".");
                            foldertmp.b_content[0].b_inodo = bi;
                            strcpy(foldertmp.b_content[1].b_name, "..");
                            foldertmp.b_content[1].b_inodo = father;
                            strcpy(foldertmp.b_content[2].b_name, "-");
                            strcpy(foldertmp.b_content[3].b_name, "-");

                            folder.b_content[j].b_inodo = bi;
                            strcpy(folder.b_content[j].b_name, newf.c_str());
                            fnd = true;
                            i = 20;
                            break;
                        }
                    }

                }
            }
        }

        if (!fnd) {
            for (int i = 0; i < 15; ++i) {
                if (inode.i_block[i] == -1) {
                    if (i < 12) {

                        bi = getfree(spr, pth, "BI");
                        past = getfree(spr, pth, "BB");

                        folder = Structs::Folderblock();
                        strcpy(folder.b_content[0].b_name, ".");
                        folder.b_content[0].b_inodo = bi;
                        strcpy(folder.b_content[1].b_name, "..");
                        folder.b_content[1].b_inodo = father;
                        folder.b_content[2].b_inodo = bi;
                        strcpy(folder.b_content[2].b_name, newf.c_str());
                        strcpy(folder.b_content[3].b_name, "-");

                        inode.i_block[i] = past;
                        updatebm(spr, pth, "BB");

                        bb = getfree(spr, pth, "BB");
                        inodetmp.i_uid = 1;
                        inodetmp.i_gid = 1;
                        inodetmp.i_size = sizeof(sizeof(Structs::Folderblock));
                        inodetmp.i_atime = spr.s_umtime;
                        inodetmp.i_ctime = spr.s_umtime;
                        inodetmp.i_mtime = spr.s_umtime;
                        inodetmp.i_type = 0;
                        inodetmp.i_perm = 664;
                        inodetmp.i_block[0] = bb;

                        strcpy(foldertmp.b_content[0].b_name, ".");
                        foldertmp.b_content[0].b_inodo = bi;
                        strcpy(foldertmp.b_content[1].b_name, "..");
                        foldertmp.b_content[1].b_inodo = father;
                        strcpy(foldertmp.b_content[2].b_name, "-");
                        strcpy(foldertmp.b_content[3].b_name, "-");

                        fseek(bfile, spr.s_inode_start + (sizeof(Structs::Inodes) * father), SEEK_SET);
                        fwrite(&inode, sizeof(Structs::Inodes), 1, bfile);
                        break;
                    }
                }
            }


        }

        fseek(bfile, spr.s_inode_start + (sizeof(Structs::Inodes) * bi), SEEK_SET);
        fwrite(&inodetmp, sizeof(Structs::Inodes), 1, bfile);

        fseek(bfile, spr.s_block_start + (sizeof(Structs::Folderblock) * bb), SEEK_SET);
        fwrite(&foldertmp, sizeof(Structs::Folderblock), 1, bfile);

        fseek(bfile, spr.s_block_start + (sizeof(Structs::Folderblock) * past), SEEK_SET);
        fwrite(&folder, sizeof(Structs::Folderblock), 1, bfile);

        updatebm(spr, pth, "BI");
        updatebm(spr, pth, "BB");

        if (spr.s_filesystem_type == 3) {
            string event;
            for (string ttpp: path) {
                event += "/" + ttpp;
            }
            event += "/" + newf;
            Structs::Journaling joutmp;
            Structs::Journaling jouraux;
            strcpy(joutmp.content, event.c_str());
            strcpy(joutmp.path, event.data());
            joutmp.type = 0;
            joutmp.size = sizeof(event) + sizeof(Structs::Folderblock);
            strcpy(joutmp.operation, "mkdir");
            joutmp.date = spr.s_umtime;

            FILE *rec = fopen(pth.c_str(), "rb+");
            fseek(rec, partition.part_start + sizeof(Structs::Superblock), SEEK_SET);

            int position = 0;
            while (true) {
                fread(&rec, sizeof(Structs::Journaling), 1, rec);
                if (jouraux.type == -1) {
                    break;
                }
                position++;
            }

            fseek(bfile, partition.part_start + (sizeof(Structs::Superblock) + sizeof(Structs::Journaling) * position),
                  SEEK_SET);
            fwrite(&joutmp, sizeof(Structs::Journaling), 1, bfile);
            fclose(rec);
        }

        shared.response("MKDR", "se ha creado el directorio");
        fclose(bfile);
    }
    catch (exception &e) {
        shared.handler("MKDIR", e.what());
        return;
    }

}

vector<string> FileManager::getpath(string s) {
    vector<string> result;
    if (s.empty()) {
        return result;
    }

    s.push_back('/');
    string tmp;
    int status = -1;
    for (char &c : s) {
        if (status != -1) {
            if (status == 2 && c == '\"') {
                status = 3;
                continue;
            } else if (status == 1) {
                if (c == '\"') {
                    status = 2;
                    continue;
                } else if (c == '/') {
                    continue;
                }
                status = 3;
            }

            if ((status == 3) && c == '/') {
                status = 1;
                result.push_back(tmp);
                tmp = "";
                continue;
            }
            tmp += c;
        } else if (c == '/') {
            status = 1;
        }
    }
    return result;
}

int FileManager::getfree(Structs::Superblock spr, string pth, string t) {
    char ch = 'x';
    FILE *file = fopen(pth.c_str(), "rb");
    if (t == "BI") {
        fseek(file, spr.s_bm_inode_start, SEEK_SET);
        for (int i = 0; i < spr.s_inodes_count; i++) {
            fread(&ch, sizeof(ch), 1, file);
            if (ch == '0') {
                fclose(file);
                return i;
            }
        }
    } else {
        fseek(file, spr.s_bm_block_start, SEEK_SET);
        for (int i = 0; i < spr.s_blocks_count; i++) {
            fread(&ch, sizeof(ch), 1, file);
            if (ch == '0') {
                fclose(file);
                return i;
            }
        }
    }
    fclose(file);
    return -1;
}

void FileManager::updatebm(Structs::Superblock spr, string pth, string t) {
    char ch = 'x';
    char one = '1';
    int num;
    FILE *file = fopen(pth.c_str(), "rb+");
    if (t == "BI") {
        fseek(file, spr.s_bm_inode_start, SEEK_SET);
        for (int i = 0; i < spr.s_inodes_count; i++) {
            fread(&ch, sizeof(ch), 1, file);
            if (ch == '0') {
                num = i;
                break;
            }
        }
        fseek(file, spr.s_bm_inode_start + num, SEEK_SET);
        fwrite(&one, sizeof(one), 1, file);
    } else {
        fseek(file, spr.s_bm_block_start, SEEK_SET);
        for (int i = 0; i < spr.s_blocks_count; i++) {
            fread(&ch, sizeof(ch), 1, file);
            if (ch == '0') {
                num = i;
                break;
            }
        }
        fseek(file, spr.s_bm_block_start + num, SEEK_SET);
        fwrite(&one, sizeof(one), 1, file);
    }
    fclose(file);
}

void FileManager::newfileblock(Structs::Superblock super){
    
}