#include <sys/socket.h>
#include <arpa/inet.h>//тут працавала і з <netinet/in.h>, але для падабенства з кліентам падключыў больш шырокую арпу.
#include <stdio.h>

int main ()
{
   int mySocket;
   mySocket = socket (AF_INET, SOCK_STREAM, 0);

   if (mySocket < 0)
   {
      printf ("ошибка вызова socket\n");
      return 1;
   }

   int bad;
   struct sockaddr_in localAddress;
   localAddress.sin_family = AF_INET;
   localAddress.sin_port = htons (7500);
   localAddress.sin_addr.s_addr = htonl (INADDR_ANY);
   bad = bind (mySocket, (struct sockaddr *) &localAddress, sizeof (localAddress));

   if (bad < 0)
   {
      printf ("ошибка вызова bind\n");
      return 1;
   }

   bad = listen (mySocket, 5);

   if (bad)
   {
      printf ("ошибка вызова listen\n");
      return 1;
   }

   //*
   int conectionSocket;
   conectionSocket = accept (mySocket, NULL, NULL);

   if (conectionSocket < 0)
   {
      printf ("ошибка вызова accept\n");
      return 1;
   }
   //*/

   ////////////////
   // больш складаны варыянт, які атрымлівае адрас кліента
   /*
   int conectionSocket;
   struct sockaddr_in clientAddress;
   int sizeClientAddress;

   conectionSocket = accept (mySocket, (struct sockaddr *)&clientAddress, &sizeClientAddress);

   if (conectionSocket < 0)
   {
      printf ("ошибка вызова accept\n");
      return 1;
   }
   printf("local ip %s\n",inet_ntoa(localAddress.sin_addr));
   printf("local port %d\n",htons(localAddress.sin_port));
   printf("local domain(family) %s\n\n",(localAddress.sin_family==AF_INET?"AF_INET":"others"));
   printf("client ip %s\n",inet_ntoa(clientAddress.sin_addr));
   printf("client port %d\n",htons(clientAddress.sin_port));
   printf("client domain(family) %s\n\n\n",(clientAddress.sin_family==AF_INET?"AF_INET":"others"));
   //*/

   char buf[5];
   bad = recv (conectionSocket, buf, 5, 0);

   if (bad <= 0)
   {
      printf ("ошибка вызова recv\n");
      return 1;
   }

   printf ("%s\n", buf);
   int i;

   for (i = 0; i < 5; ++i)
      scanf ("%c", &buf[i]);

   bad = send (conectionSocket, buf, 5, 0);

   if (bad <= 0)
   {
      printf ("ошибка вызова send\n");
      return 1;
   }

   return 0;
}
