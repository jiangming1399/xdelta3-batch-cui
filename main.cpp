#include <iostream>
#include <fstream>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <vector>
#define DEBUG 1
using namespace std;

vector <string> filePath;

string getAbsPath(string relativePath) {
    char m_szInitDir[_MAX_PATH]; //初始化一个BUFF,用于存储程序运行路径。
    string path;
    char rPath[_MAX_PATH];

    strcpy(rPath,relativePath.c_str());
    getcwd(m_szInitDir,_MAX_PATH); //获取当前程序路径

    if (_fullpath(m_szInitDir,rPath,_MAX_PATH) == NULL)
        return NULL;
    path.assign(m_szInitDir);
    return path;
}

bool fileExists(string path) {
    char rPath[_MAX_PATH];
    strcpy(rPath,path.c_str());//将 string 转换为 char
    return (access(rPath, F_OK ) == 0);
}


bool searchFile(string lPath, string aPath, string fileName) {
    long fHandle;
    string pathTemp;
    struct _finddata_t fileinfo;
    char rPath[_MAX_PATH];
    strcpy(rPath,(lPath+aPath+fileName).c_str());

    fileinfo.attrib = _A_SUBDIR ;

    fHandle=_findfirst(rPath,&fileinfo);
    if(fHandle==-1)return false;
    pathTemp.assign(fileinfo.name);
    if((fileinfo.attrib&_A_SUBDIR)!=_A_SUBDIR)
        filePath.push_back(aPath+pathTemp);
    while(!_findnext(fHandle,&fileinfo)){
        pathTemp.assign(fileinfo.name);
        if((fileinfo.attrib&_A_SUBDIR)!=_A_SUBDIR)
            filePath.push_back(aPath+pathTemp);
    }
    _findclose(fHandle);

    fileinfo.attrib = _A_SUBDIR;

    strcpy(rPath,(lPath + aPath + "*").c_str());
    fHandle=_findfirst(rPath,&fileinfo);

    if(fHandle==-1)return false;
    pathTemp.assign(fileinfo.name);

    if(pathTemp!=".."&&pathTemp!="."&&pathTemp!=".\\"&&pathTemp!="..\\")
        searchFile(lPath, aPath + pathTemp + "\\", fileName);

    while(!_findnext(fHandle,&fileinfo)){
        pathTemp.assign(fileinfo.name);
        if(pathTemp!=".."&&pathTemp!="."&&pathTemp!=".\\"&&pathTemp!="..\\")
            searchFile(lPath, aPath + pathTemp + "\\", fileName);
    }
}

string string_replace(string& s1,const string& s2,const string& s3)
{
    string s4 = s1;
	string::size_type pos=0;
	string::size_type a=s2.size();
	string::size_type b=s3.size();
	while((pos=s4.find(s2,pos))!=string::npos)
	{
		s4.replace(pos,a,s3);
		pos+=b;
	}
	return s4;

}

int main(int argc, char *argv[])
{
    #ifdef DEBUG

    //cout << getAbsPath("asd") << endl;
    //searchFile("E:\\BaiduYunDownload\\","","*");
    //for(int i=0;i<filePath.size();i++){
     //   cout << filePath[i] << endl;
    //}
    #endif //Test case

    string path[3];
    string enterArgs,commandArgs = "-e -9 -S djw -vfs";
    ofstream batchFile;

    cout << "########################################" << endl;
    cout << "#                                      #" << endl;
    cout << "#                                      #" << endl;
    cout << "#          xdelta3 batch CUI           #" << endl;
    cout << "#                                      #" << endl;
    cout << "#                                      #" << endl;
    cout << "########################################" << endl;

    if(!fileExists(getAbsPath("xdelta3.exe"))) {
        cout << getAbsPath("xdelta3.exe") << " was missing!" << endl;
        return -1;
    }

    if(argc!=4) {
        cout << "Usage: batch.exe <Source Directory> <Target Directory> <Output Directory>" << endl;
        return -1;
    }
    //convert to absolute path and add splash
    for(int i=0;i<3;i++){
        path[i].assign(argv[i+1]);
        path[i] = getAbsPath(path[i]);
        if(path[i][path[i].size()-1]!='\\')
            path[i]+='\\';
    }

    //test case
    for(int i=0;i<2;i++){
        if(!fileExists(path[i])){
            cout << "\"" << path[i] << "\" is an invalid path!";
            return -1;
        }
    }
    cout << "Searching files..." ;
    searchFile(path[0],"","*"); //寻找文件
    cout << "  Done" << endl << endl;

    while (1) {
        cout << "The args is:" << endl << "xdelta3.exe " << commandArgs << endl << endl;
        cout << "Enter Y to use it, or enter another then press ENTER." << endl << endl;
        cout << "xdelta3.exe ";
        cin >> enterArgs;
        if(enterArgs=="Y"||enterArgs=="y") break;
        else commandArgs = enterArgs;
    }

    cout << "Writting out create.bat...  " ;

    //ofstream batchFileC(path[2]+"Create.bat");
    batchFile.open((path[2] + "Create.bat").c_str(),ios_base::out);

    if (!batchFile.is_open()){
        cout << "CANNOT OPEN " << path[2] + "Create.bat" << endl;
    }

    batchFile << "@echo off" << endl << "echo Start creating patch file" << endl << "mkdir patch" << endl;
    for(int i=0;i<filePath.size();i++){
       batchFile << "xdelta3.exe " << commandArgs << " \"" << path[0] + filePath[i] << "\" \"" << path[1] + filePath[i] << "\" \"" << path[2] + "patch\\" + string_replace(filePath[i],"\\","[&slash;]") << ".vcdiff\"" << endl;
    }
    batchFile << "echo Patching done!" << endl << "pause" << endl;

    batchFile.close();
    cout << "SUCCESS" << endl;

    cout << "Writting out patch.bat...  " ;
    batchFile.open((path[2] + "Patch.bat").c_str(),ios_base::out);

    if (!batchFile.is_open()){
        cout << "CANNOT OPEN " << path[2] + "Patch.bat" << endl;
    }

    batchFile << "@echo off" << endl << "echo Start creating patch file" << endl;
    for(int i=0;i<filePath.size();i++){
       batchFile << "xdelta3.exe -d -vs " << filePath[i] << " " << "patch\\" + string_replace(filePath[i],"\\","[&slash;]") << ".vcdiff" << " " << filePath[i] << endl;
    }
    batchFile << "echo Patching done!" << endl << "pause" << endl;

    batchFile.close();
    cout << "SUCCESS" << endl << endl << endl;
    cout << "Please copy the \"xdelta3.exe\" to " << path[2] << endl;
    return 0;
}


