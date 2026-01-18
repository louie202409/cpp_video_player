#include <bits/stdc++.h>
#include <windows.h>
#include <time.h>
using namespace std;
ifstream in("badapple.txt");
int arr[361][641];
int tmp[641];
int n;
using namespace std;
void hidecursor()
{
	CONSOLE_CURSOR_INFO cursor_info={1,0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}
void setpos(int i, int j)
{
	HANDLE hout;
	COORD coord;
	coord.X = j;
	coord.Y = i;
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hout, coord);
}
int main()
{
	ios::sync_with_stdio(false);
	hidecursor();
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
   	cfi.dwFontSize.X=4;
    cfi.dwFontSize.Y=8;
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
	cin>>n;
	n=6964;
	int clocknum=clock();
	int stclock=clock();
	int ticks=0;
	while(n--)
	{
		ticks++;
		setpos(1,0);
		for(int i=1;i<=90;i++)
		{
			int k=0;
			in>>k;
			for(int j=1;j<=320;j++)
			{
				tmp[j]=0;
			}
			for(int j=1;j<=k;j++)
			{
				int wow;
				in>>wow;
				tmp[wow]=1;
			}
			for(int j=1;j<=640;j++)
			{
				arr[i][j]=arr[i][j-1]^tmp[j];
			}
		}
		for(int i=1;i<=90;i++)
		{
			string str;
			for(int j=1;j<=320;j++)
			{
				if(arr[i][j]==0)
				{
					str+=' ';
				}
				else
				{
					str+="#";
				}
			}
			cout<<str<<"\n";
		}
		cout<<flush;
		while(clock()-stclock<ticks*1.0*CLOCKS_PER_SEC/30)
		{
			;
		}
		clocknum=clock();
	}
	return 0;
}
