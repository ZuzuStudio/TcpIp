#include <sys/socket.h>
#include <arpa/inet.h> //нужен для структуры sockaddr_in; а также для фунуций htons и inet_addr
//#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <hzw/queue.h>

using namespace std;
using namespace hzw;

enum State { FINISH, START, NORMAL, CONTROL, DISCONNECT, ENDING};

char catchChar();

int main(int argc, char **argv)
{
   /* int mySocket;

    mySocket = socket(AF_INET, SOCK_STREAM, 0);

    if(mySocket < 0)
    {
       cerr << "ошибка вызова socket\n";
       return 1;
    }

    int bad;
    struct sockaddr_in myAddress;
    myAddress.sin_family = AF_INET;
    myAddress.sin_port = htons(7500);

    if(argc == 1)
       myAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    else
       myAddress.sin_addr.s_addr = inet_addr(argv[1]);

    bad = connect(mySocket, (struct sockaddr *) &myAddress, sizeof(myAddress));

    if(bad)
    {
       cerr << "ошибка вызова connect\n";
       return 1;
    }*/

   try
   {
      Queue<char> qInput, qSend, qRecipe, qOutput, qChecked;
      bool silentWindow[2] = {true, true};
      char ch;
      char controlWord[20] = "";
      State state = FINISH;

      do
      {

         //console input
         ch = catchChar();

         if(ch != EOF)
         {
            qInput.enqueue(ch);
         }

         //qInput
         if(!qInput.isEmpty())
            if('/' == qInput.onFront())
               state = CONTROL;// TO control state
            else
            {
               if(!qChecked.isEmpty())
               {
                  qSend.enqueue(qChecked.onFront());
                  qChecked.dequeue();
               }
               else
               {
                  qSend.enqueue(qInput.onFront());
                  qInput.dequeue();
               }

            }

         //qChecked
         if(!qInput.isEmpty())
         {
            char smallString[2] = {'\0', '\0'};
            smallString[0] = qInput.onFront();
            qChecked.enqueue(qInput.onFront());
            qInput.dequeue();
            strcat(controlWord, smallString);

            if(!strcmp(smallString, "/exit"))
            {
               state = DISCONNECT;
               qChecked.clear();
               strcpy(controlWord, "");
            }
            else
            {
               if(strlen(controlWord) > 7)
               {
                  state = NORMAL;
                  strcpy(controlWord, "");
               }
            }
         }

         //qSend normall
         if(!qSend.isEmpty())
         {
            int bad = send(mySocket, qSend.onFront(), 1, 0);

            if(bad <= 0)
            {
               perror("ошибка вызова send\n");
               return 1;
            }

            qSend.dequeue();
         }
         else
         {
            int bad = send(mySocket, '\16', 1, 0);

            if(bad <= 0)
            {
               perror("ошибка вызова send\n");
               return 1;
            }
         }

      }
      while(state);


   }
   catch(exception &e)
   {
      perror("произошло следующее исключение:\n");
      perror(e.what());
      return 1;
   }

   /*
   int len = 5;
   bad = send(mySocket, "kuku\0", len, 0);

   if(bad <= 0)
   {
      cerr << "ошибка вызова send\n";
      return 1;
   }

   char buf[ len ] ;

   bad = recv(mySocket, buf, len, 0) ;

   if(bad <= 0)
   {
      cerr << "ошибка вызова recv\n";
      return 1;
   }
   else
      cout << buf << endl;
      */

   return 0;

}

char catchChar()
{
   char ch;
   int oldConsoleFlag;
   termios oldTermios, newTermios;
   tcgetattr(STDIN_FILENO, &oldTermios);
   newTermios = oldTermios;
   newTermios.c_lflag &= ~ICANON;
   tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
   oldConsoleFlag = fcntl(STDIN_FILENO, F_GETFL, 0);
   fcntl(STDIN_FILENO, F_SETFL, oldConsoleFlag | O_NONBLOCK);
   ch = getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
   fcntl(STDIN_FILENO, F_SETFL, oldConsoleFlag);
   return ch;
}




