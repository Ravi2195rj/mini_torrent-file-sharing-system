#include <openssl/sha.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <sys/stat.h> 



using namespace std;


struct stat stat_buf;

int size,tempsize;
string tracker1ip="127.0.0.1",tracker2ip="127.0.0.2",tracker1port="123",tracker2port="1234",filename="songfile.mp3",filepath="./songfile.mp3",torrentfilename,torrentfilepath,filesize;

int main()
{ 

  torrentfilename.append(filename);
  torrentfilename.append(".mtorrent");
  torrentfilepath.append("./");
  torrentfilepath.append(torrentfilename);

  stat(filepath.c_str(),&stat_buf);
  size=stat_buf.st_size;
  tempsize=size;
  filesize=to_string(size);

	unsigned char digest[20];
	char mdString[SHA_DIGEST_LENGTH*2];

	string SH1, SH2;
	std::fstream fin,fout;
	fin.open(filepath, ios_base::in|ios_base::binary);
  fout.open(torrentfilepath, ios_base::out);

  fout << tracker1ip.c_str() << ":";
  fout << tracker1port.c_str() << "\n";
  fout << tracker2ip.c_str() << ":";
  fout << tracker2port.c_str() << "\n";
  fout << filepath.c_str() << "\n";
  fout << filesize.c_str() << "\n";
  

    	while (tempsize>0) 
      {   

          if(tempsize>1024*512)
          {
              char buffer[1024*512];
              fin.read(buffer,1024*512);
              SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest); 
              for (int i = 0; i <20; i++)
                  sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
              SH1=mdString;
              SH2.append(SH1.substr(0,20));                  
          }
          else
          {
              char buffer[tempsize];
              fin.read(buffer,tempsize);
              SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest); 
              for (int i = 0; i <20; i++)
                  sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
              SH1=mdString;
              SH2.append(SH1.substr(0,20)); 
          }
          tempsize-=1024*512;
      }


  fout << SH2.c_str();
	//cout<<SH2;
	//cout<<SH2.length();

  return 0;
}
