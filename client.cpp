//Roll No.: 2018201018
//Name: Ravi Jakhania

//#include "assignment1.h"
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <termios.h>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <ftw.h>
#include <sys/ioctl.h>

using namespace std;
#define PORT 1313

string clientIP_Port,tracker1IP_Port,tracker2IP_Port,logfile,client_IP,client_Port,tracker1_IP,tracker1_Port,tracker2_IP,tracker2_Port;

void findIP(string clientIP_Port,string tracker1IP_Port,string tracker2IP_Port)
{
    size_t found = clientIP_Port.find(':');
    client_IP=clientIP_Port.substr(0,found);
    client_Port=clientIP_Port.substr(found+1);

    size_t found1 = tracker1IP_Port.find(':');
    tracker1_IP=tracker1IP_Port.substr(0,found1);
    tracker1_Port=tracker1IP_Port.substr(found1+1);
    
    size_t found2 = tracker2IP_Port.find(':');
    tracker2_IP=tracker2IP_Port.substr(0,found2);
    tracker2_Port=tracker2IP_Port.substr(found2+1);
}

int connection_with_tracker()
{
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char hello[50] = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
            
        memset(&serv_addr, '0', sizeof(serv_addr)); 
                   
        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(PORT); 
                       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
                   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

   return sock;
}
void gotoNonCanon() //going to non canon
{
    struct termios initial_settings, new_settings;
    FILE *input;
    FILE *output;

    input = fopen("/dev/tty", "r");
    output = fopen("/dev/tty", "w");
    tcgetattr(fileno(input),&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;

    if(tcsetattr(fileno(input), TCSANOW, &new_settings) != 0) 
    {
        fprintf(stderr,"could not set attributes\n");
    }
  
//  tcsetattr(fileno(input),TCSANOW,&initial_settings);
}
void statusbar(string tempstr) //printing status bar and path
{   
    cout<<"\033[1;1m"<<tempstr<<"\033[0m";
    cout<<"\n";
}
    
stack <string> stack1,stack2;
vector <string> Path,Name;

char buffer[1000],path[1000]=".",pop[1000];
// int curser_row=2,curser_column=1,file_num=2,total_files,level=0,search_flag=0,total_rows,total_columns,listend,commandmodeline;
// string dst_root;

// DIR *point_to_directory;
// struct dirent **point_to_file;
// struct stat file_stat;
// struct winsize win;


int main(int argc, char *argv[])
{
    gotoNonCanon(); //Going to Non Canon mode.
    
    cout<<"\033[3J";
    cout<<"\033[H\033[J";
    statusbar("Enter Command:");

    cout<<" ";
    
    clientIP_Port=argv[1];
    tracker1IP_Port=argv[2];
    tracker2IP_Port=argv[3];
    
    //logfile=argv[4];
    // findIP(clientIP_Port,tracker1IP_Port,tracker2IP_Port);

    // cout<<clientIP_Port<<"\n"<<flush;
    // cout<<tracker1IP_Port<<"\n"<<flush;
    // cout<<tracker2IP_Port<<"\n"<<flush;
    
    
    // cout<<client_IP<<"\n"<<flush;
    // cout<<client_Port<<"\n"<<flush;
    // cout<<tracker1_IP<<"\n"<<flush;
    // cout<<tracker1_Port<<"\n"<<flush;
    // cout<<tracker2_IP<<"\n"<<flush;
    // cout<<tracker2_Port<<"\n"<<flush;

        int curser=0,commandmodeline=1,curser_column=15,flag=1;
        cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;      
        while(flag)
        { 
            
            char command[500];
            char buffer[500];
            read(0,buffer,128);
            
            if(buffer[0]==27 && buffer[1]!=91) // Esc key , Going back to normal mode
            {
                flag=0;
            }

            else if(buffer[0]!=27 && buffer[0]!=127 && buffer[0]!=10)  //Entering Input or commands.
            {
                command[curser++]=buffer[0];
                cout<<buffer[0]<<flush;
                curser_column++;
            }

            else if(buffer[0]==127 || buffer[0]==8) // Pressing Back space to Erase text.
            {

                 if(curser_column>15)
                 {
                    command[--curser]=' ';
                    command[curser+1]='\0';
                    // cout<<"\033["<<commandmodeline<<";15H"<<flush;
                    // cout<<command<<flush;
                    // command[curser-1]='\0';
                    //curser--;
                    cout<<"\033["<<commandmodeline<<";15H"<<flush;
                    cout<<command<<flush;
                    command[curser]='\0';
                    cout<<"\033["<<commandmodeline<<";"<<--curser_column<<"H"<<flush;
               }
                  
            }
            else if(buffer[0]==10) // Pressing Enter key to execute commands
            {
                std:: vector<string> v;
                command[curser]='\0';
                char s[1000];
                int j=0,i;

                for(i=0;command[i]!='\0';i++) // Differentiate Operation and Arguments.
                {
                    if(command[i]==' ')
                    {
                        if(command[i-1]==92)
                            s[j++]=command[i];
                        else
                        {
                            s[j]='\0';
                            v.push_back(s); 
                            j=0;    
                        }
                    }
                    else if(command[i]!=92)
                    {
                        s[j++]=command[i];
                    }
                }

                s[j]=command[i];
                v.push_back(s);
                cout<<"\n"<<flush;
                cout<<"\e[2K"<<flush;
                if(v[0]=="share")
                {

                        string local_file_path=v[1];
                        string filename_with_extension=v[2];
                        //cout<<v[1]<<":"<<v[2];
                 //   int sock=connection_with_tracker();
                   // send(sock,"hello",1024,0);
                  //  printf("Hello message sent\n"); 
                    //valread=read( sock , buffer, 1024);

                }



                v.clear();
                cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;   
                cout<<"\e[2K"<<flush;
                statusbar("Enter Command:");

                curser=0,commandmodeline=1,curser_column=15;
                cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;



         } 
         buffer[0]=buffer[1]=buffer[2]=0;
         }  
    return 0;
}