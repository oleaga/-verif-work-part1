// Xonix.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include <conio.h>
#include <dos.h>
#include <iostream>
#include <ctime>

#include <mmsystem.h>				// ���������� ��� ����������� ������
#pragma comment (lib, "winmm.lib")  // ���������� ��� ����������� ������

#include <string.h>
#include <stdlib.h>
#include <direct.h>

#define sizeX 77// ���������� �������� ������� �� �����������, � ������ ��� ������ Scrn - ������ ���������� (������������� j)
#define sizeY 32// ���������� �������� ������� �� ���������, � ������ ��� ������ Scrn - ������ ���������� (������������� i)

const int LenStr=30;
const int StrCount=10;

struct CharAttr
{
	unsigned char Symbol;
	unsigned char Color;
	unsigned short int FieldID;// ����� �������, ������������ ��� �������� ���-�� �������� ��� ���������
};

struct Ghost
{
	COORD CurPos;
	COORD PrevPos;
	short int ID;

};

enum SymbolCode
{
	ChBorder = 0xb0,
	ChLine   = 0xf9,
	ChGhost  = 0xfd,
	ChSpace  = 0x20, // ' ' - ������
	ChHead   = '@'
};

enum KeyCode
	{
		KDown    = 0x50,
		KUp      = 0x48,
		KLeft    = 0x4b,
		KRight   = 0x4d,
		KEnter   = 0x0d,
		KEsc     = 0x1b,
		KDefault = 0x25
	};

struct TextAttribute
	{
		unsigned char SelTextAttr;
		unsigned char BorderTextAttr;
		unsigned char LightSelTextAttr;
		unsigned char LightTextAttr;
		unsigned char XTextAttr;
		unsigned char GhostTextAttr;
		unsigned char HeadTextAttr;
		unsigned char LineTextAttr;
		unsigned char TextAttr;
	} TA;

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
UINT CodePage;// ��� �������� ��������, ������������ �������� �� ���������

int shell();
void PreSet(COORD &SizeWin);
int Menu(COORD Center, int &ItemNumber);
void GotoXY(COORD &Pos);
int Play(COORD SizeWin, unsigned int &Score);
bool EscMsBox(COORD SizeWin,CharAttr *Scrn);
bool TryMsBox(COORD TopLeft, CharAttr Scrn[sizeY][sizeX], int TryCnt);
bool MoveGhosts(Ghost Ghosts[], CharAttr Scrn[sizeY][sizeX]);
bool MoveHead(Ghost *Head, CharAttr Scrn[sizeY][sizeX], int Key, COORD &StartPos, COORD &TopLeft, COORD &BottRight, unsigned int &Score, unsigned int &FieldCount);
void PrintChr(COORD Pos, unsigned char Clr, unsigned char Smbl);
void PrintStr(COORD Pos, unsigned char Clr, char *Str);
void Filling(CharAttr Scrn[sizeY][sizeX], Ghost *Ghs,  unsigned int &Score, unsigned int &FieldCount);
void ReadRecords(char NameList[StrCount][LenStr], unsigned int ScoreList[StrCount]);
void ChangeRecords(unsigned int &Score);
void Records(COORD &SizeWin, unsigned int &Score,char NameList[StrCount][LenStr], unsigned int ScoreList[StrCount]);
void ShowRecords(COORD SizeWin, char NameList[StrCount][LenStr], unsigned int ScoreList[StrCount], unsigned int &Score, short int Index);

int main()
{
	return shell();
}

int shell()
{
	COORD SizeWin;

	unsigned int Score=0;
	unsigned int ScoreList[StrCount];
	int ItN = 0;

	char GoodBye[] = "����� ��������!";
	char NameList[StrCount][LenStr];
	
	TA.BorderTextAttr = 0x07;
	TA.SelTextAttr = 0x70;
	TA.LightTextAttr = 0x0f;
	TA.LightSelTextAttr = 0xf0;
	TA.XTextAttr = 0x0d;
	TA.GhostTextAttr = 0x0e;
	TA.HeadTextAttr = 0x0f;
	TA.LineTextAttr = 0x08;
	TA.TextAttr = 0x07;

	PreSet(SizeWin);
	ReadRecords(NameList, ScoreList);
	while(1)
	{
		Menu(SizeWin, ItN);
		switch(ItN)
		{
			case 0:// PLAY()
				{
					//����� ������� PLAY()
					Play(SizeWin, Score);	
					//break;
				}
			case 1:// RECORDS()
				{
					
					
					Records( SizeWin, Score, NameList, ScoreList);
					//����� ������� RECORDS()

							
					break;
				}
			case 2:// SETTINGS()
				{
					//mciSendString(L"play sound\\orkestr-polya-moria-bez-nazvaniya_(mp3.cc).mp3 repeat",0,0,0);

					//����� ������� SETTINGS()

					system("cls");
					getch();
					//mciSendString(L"stop sound\\orkestr-polya-moria-bez-nazvaniya_(mp3.cc).mp3",0,0,0);		
					break;
				}
			case 3:// EXIT
				{
					system("cls");
					SizeWin.X = (SizeWin.X-strlen(GoodBye))/2;
					SizeWin.Y = SizeWin.Y/2 - 1;
					GotoXY(SizeWin);
					printf(GoodBye);
					
					mciSendString(L"play sound\\����������.wav wait",0,0,0);
					return 0;
				}// EXIT
		}
	};
	return -1;
}

void GotoXY(COORD &Pos)
{
  SetConsoleCursorPosition(handle,Pos);
}

int Menu(COORD Center, int &CurrItem)
{
	int Key = KDefault;

	const int IQ = 4;// ���������� ����� ����
	const int IL = 14;// ����� ������ ����

	char Items[IQ][IL] = {"    ����     ","   �������   ","  ���������  ","    �����    "};// ����� ��������� ������, �� '�����' ������ ���� � ��������� ������!!!
	char MelodyFileName[] = "chimes.wav";

	COORD Pos;

	Center.X /= 2;
	Center.Y /= 2;

	mciSendString(L"play sound\\����.mp3 repeat",0,0,0);

	Pos.X = Center.X-IL/2;
	SetConsoleTextAttribute(handle, TA.BorderTextAttr);
	for(int i = 0; i<IQ; i++)
	{
		if(CurrItem==i) SetConsoleTextAttribute(handle, TA.SelTextAttr);
		Pos.Y = Center.Y-IQ+i*2;
		GotoXY(Pos);
		printf("%s",Items[i]);
		SetConsoleTextAttribute(handle, TA.BorderTextAttr);
	}

	while(!((Key==KEnter) && (CurrItem==IQ-1)))
	{
		Key = getch();
		Pos.X = Center.X-IL/2;
		switch(Key)
		{
			case KUp   : 
			{
				Pos.Y = Center.Y-IQ+2*CurrItem;
				GotoXY(Pos);
				printf("%s",Items[CurrItem]);
				if(CurrItem>0) CurrItem--;
				else CurrItem = IQ-1;
				SetConsoleTextAttribute(handle, TA.SelTextAttr);
				Pos.Y = Center.Y-IQ+2*CurrItem;
				GotoXY(Pos);
				printf("%s",Items[CurrItem]);
				SetConsoleTextAttribute(handle, TA.BorderTextAttr);
				break;
			}
			case KDown : 
			{
				Pos.Y = Center.Y-IQ+2*CurrItem;
				GotoXY(Pos);
				printf("%s",Items[CurrItem]);
				if(CurrItem<IQ-1) CurrItem++;
				else CurrItem = 0;
				SetConsoleTextAttribute(handle, TA.SelTextAttr);
				Pos.Y = Center.Y-IQ+2*CurrItem;
				GotoXY(Pos);
				printf("%s",Items[CurrItem]);
				SetConsoleTextAttribute(handle, TA.BorderTextAttr);
				break;
			}
			case KEnter:
			{
				mciSendString(L"stop sound\\����.mp3",0,0,0);
				return 0;
			}
		}
	}

	mciSendString(L"stop sound\\����.mp3",0,0,0);
	return -1;
}

void PreSet(COORD &SizeWin)
{
	COORD FontSize;
	unsigned int dx;
	unsigned int dy;
	unsigned int sx;
	unsigned int sy; 
	// ������������� �������� �������� ASCII �� ���� � ����� ���������
	CodePage = /*866;//*/GetConsoleOutputCP();

	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	
	CONSOLE_FONT_INFO FontInfo;
	GetCurrentConsoleFont(handle,false,&FontInfo);
	FontSize = GetConsoleFontSize(handle, FontInfo.nFont);
	sx = FontSize.X+1;
	sy = FontSize.Y+1;
	
	SizeWin.X = 10;
	SizeWin.Y = 10;
	SetConsoleScreenBufferSize(handle,SizeWin);
	SizeWin.X = sizeX;
	SizeWin.Y = sizeY;
	dx = (1400 - sx*SizeWin.X)/2;
	dy = ( 750 - sy*SizeWin.Y)/2;

	// ������������� ������� �����
	chdir("..\\Debug\\");
	// ������������� ��������� ����
	SetConsoleTitle(_T("XONIX"));

	// ������ ������. ����� � ������. http://www.cyberforum.ru/cpp-beginners/thread102665.html
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);
	CursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo( handle, &CursorInfo );

	// ���������� � ����������� ����
	HWND hwnd = GetConsoleWindow();
	MoveWindow(hwnd, dx*sx, dy*sy,1,1,true);
    SetConsoleScreenBufferSize(handle,SizeWin);// ��������� ������ �������
	MoveWindow(hwnd, dx, dy, SizeWin.X*sx, SizeWin.Y*sy, true);// ����������� ������� �� ������

}

int Play(COORD SizeWin, unsigned int &Score)
{
	const int GN = 100; // Ghosts number
	const int LOFN = 100;// Lenght Of FileName;

	char LvlDir[] = "levels\\";
	char LvlFileExt[] = ".lvl";
	char LvlFileName[LOFN] = "levels\\0.lvl";
	char LvlFileName2[LOFN]="";
	char str[LOFN];
	char *str2;
	char tryStr[]="�������� �������: ";

	int Key = KDefault;
	int i,j,GCnt,Lvl,TryCnt = 3;
	Score = 0;
	unsigned int FieldCount = 0;
	unsigned int FieldMax = (sizeX-2)*(sizeY-4);
	unsigned int FieldLvl;
	unsigned int minTime;
	unsigned short int GPrison;

	bool isExit = 0;
	bool isFail = 0;

	COORD Center;
	COORD Pos;
	COORD HeadPos;
	COORD StartPos, TopLeft, BottRight;

	FILE* LvlFile;

	Ghost Ghs[GN];

	CharAttr Scrn[sizeY][sizeX];
	//////////////////////////////////////////24 ������ ��� ������ ����
	
	Lvl=1;
	do{
		minTime=60-(Lvl-1)*6;
		itoa(Lvl,LvlFileName,10);
		strcat(LvlFileName,LvlFileExt);
		strcpy(str,LvlDir);
		strcat(str,LvlFileName);
		strcpy(LvlFileName,str);
		LvlFile = fopen (LvlFileName,"r");
		if(LvlFile !=0)
		{
			system("cls");
			mciSendString(L"play sound\\����.mp3 repeat",0,0,0);
			SetConsoleOutputCP(1251);
			Pos.X = 0;
			Pos.Y = 0;
			GotoXY(Pos);
			SetConsoleTextAttribute(handle, TA.XTextAttr);
			printf(" �������:");// 9 ��������, 10-� - ����� ������!
			Pos.X = 10;
			GotoXY(Pos);
			printf("%d",Lvl);

			Pos.X = sizeX - strlen(tryStr)-3;
			GotoXY(Pos);
			printf("%s%d",tryStr,TryCnt);

			char strScore[20]="����: ";
			Pos.X=35;
			Pos.Y=0;
			PrintStr(Pos, TA.XTextAttr, strScore);
			Pos.X=35 + strlen(strScore);
			itoa(Score,strScore,10);
			PrintStr(Pos, TA.XTextAttr, strScore);

			SetConsoleOutputCP(CodePage);
			for(i = 0; i<GN; i++) Ghs[i].ID = -1;
			GCnt = 1;
			for(i = 1; i<sizeY-1; i++)
			{
				Pos.Y = i;
				str2 = fgets(str, sizeX+5, LvlFile);
				for(j = 0; j<sizeX; j++)
				{
					Pos.X = j;
					Scrn[i][j].Symbol = str[j];
					Scrn[i][j].Color = TA.BorderTextAttr;
					Scrn[i][j].FieldID = 0;// ����� �������, ������������ ��� �������� ���-�� �������� ��� ���������
					if(Scrn[i][j].Symbol==ChGhost)
					{
						Scrn[i][j].Color = TA.GhostTextAttr; //
						Ghs[GCnt].ID = GCnt;
						Ghs[GCnt].CurPos.X = j;
						Ghs[GCnt].CurPos.Y = i;
						////////////////////////////////////
						COORD PrvPos[4];
						PrvPos[0].X = Ghs[GCnt].CurPos.X-1;
						PrvPos[0].Y = Ghs[GCnt].CurPos.Y-1;

						PrvPos[1].X = Ghs[GCnt].CurPos.X-1;
						PrvPos[1].Y = Ghs[GCnt].CurPos.Y+1;

						PrvPos[2].X = Ghs[GCnt].CurPos.X+1;
						PrvPos[2].Y = Ghs[GCnt].CurPos.Y+1;

						PrvPos[3].X = Ghs[GCnt].CurPos.X+1;
						PrvPos[3].Y = Ghs[GCnt].CurPos.Y-1;
						////////////////////////////////////
						srand( time(0) );
						unsigned int numb = (rand()%100 + GCnt) * (rand()%10 + GCnt)%4;
						Ghs[GCnt].PrevPos.X = PrvPos[numb].X;
						Ghs[GCnt].PrevPos.Y = PrvPos[numb].Y;
						GCnt++;
					
					}
					else
						if(Scrn[i][j].Symbol == ChHead)
						{
							Scrn[i][j].Symbol = ChBorder;
							//Scrn[i][j].Color = TA.HeadTextAttr;
							Ghs[0].ID = ChBorder;// ������������� ������ ������� '���' ������
							Ghs[0].CurPos.X = j;
							Ghs[0].CurPos.Y = i;
							Ghs[0].PrevPos.X = j;
							Ghs[0].PrevPos.Y = i;
							HeadPos.X = Ghs[0].CurPos.X;
							HeadPos.Y = Ghs[0].CurPos.Y;
						}
					PrintChr(Pos, Scrn[i][j].Color, Scrn[i][j].Symbol);
				}
				PrintChr(Ghs[0].CurPos, TA.HeadTextAttr, ChHead);
			}
			fclose(LvlFile);
			Center.X = SizeWin.X/2;
			Center.Y = SizeWin.Y/2-1;
			GPrison = 24;
			FieldLvl = (GCnt-1)*GPrison;
			do{
				while(Key!=KEsc && !isFail)
				{
					for(int t = 0; t<2; t++)
					{
						if(kbhit()!=0)
						{
							Key = getch();
							if((Key==KLeft||Key==KRight||Key==KUp||Key==KDown)) isFail = MoveHead(Ghs, Scrn, Key, StartPos, TopLeft, BottRight, Score, FieldCount);
							Sleep(minTime);
						}
						else
						{
							if(kbhit()!=0)
							{
								Key = getch();
							}
							Sleep(minTime);
							if(Key != KEsc) Key = KDefault;
							isFail = MoveHead(Ghs, Scrn, Key, StartPos, TopLeft, BottRight, Score, FieldCount);
						}
						if(FieldMax-FieldCount<FieldLvl) 
						{
							int tmp=TryCnt;
							TryCnt=-1;
							TryMsBox(SizeWin, Scrn, TryCnt);
							TryCnt=tmp;
							Key = KDefault;
							break;
						}
					}
					if(FieldMax - FieldCount < FieldLvl)
					{
						mciSendString(L"stop sound\\����.mp3",0,0,0);
						break;
					}
					isFail = MoveGhosts(Ghs, Scrn);
				}
		
				if(isFail)
				{
					HeadPos.X=StartPos.X;
					HeadPos.Y=StartPos.Y;
					TryCnt--;
					SetConsoleTextAttribute(handle, TA.XTextAttr);
					Pos.Y = 0;
					Pos.X = sizeX -3;//�������� �������: 
					GotoXY(Pos);
					printf("%d",TryCnt);

					SetConsoleOutputCP(CodePage);
			
					Ghs[0].CurPos=HeadPos;
					Ghs[0].PrevPos=HeadPos;
					Ghs[0].ID=ChBorder;
					//Scrn[Ghs[0].CurPos.Y][Ghs[0].CurPos.X].Symbol=ChBorder;

					mciSendString(L"pause sound\\����.mp3",0,0,0);
					TryMsBox(SizeWin, Scrn, TryCnt);
					int I,J;
					I=BottRight.X;
					J=BottRight.Y;
					//SetConsoleOutputCP(CodePage);
					for(j=TopLeft.Y;j<=J;j++)
					{
						for(i=TopLeft.X;i<=I;i++)
						{
							if(Scrn[j][i].Symbol==ChLine)
							{
								Pos.X=i;
								Pos.Y=j;
								Scrn[j][i].Color=TA.BorderTextAttr;
								Scrn[j][i].Symbol=ChSpace;
								PrintChr(Pos, Scrn[j][i].Color, Scrn[j][i].Symbol);
							}
						}
					}
					PrintChr(Ghs[0].CurPos, Scrn[Ghs[0].CurPos.Y][Ghs[0].CurPos.X].Color, Scrn[Ghs[0].CurPos.Y][Ghs[0].CurPos.X].Symbol);
					PrintChr(HeadPos, TA.HeadTextAttr, ChHead);
					//SetConsoleOutputCP(1251);
					mciSendString(L"resume sound\\����.mp3",0,0,0);
					isExit=!(TryCnt);
				}
				isFail=0;
		
				if(Key==KEsc)
				{
					mciSendString(L"pause sound\\����.mp3",0,0,0);
					isExit = EscMsBox(SizeWin, &(Scrn[0][0]));
					mciSendString(L"resume sound\\����.mp3",0,0,0);
				}
				if(FieldMax - FieldCount < FieldLvl)
				{
					FieldCount=0;
					break;
				}
				Key = KDefault;
			}while(!isExit);
			if(isExit)
			{
				system("cls");
				mciSendString(L"stop sound\\����.mp3",0,0,0);	
				SetConsoleOutputCP(1251);
				return 0;
			}
		}
		Lvl++;
	}while(LvlFile != 0);
	SetConsoleOutputCP(1251);
	return 0;
}

bool MoveGhosts(Ghost Ghs[], CharAttr Scrn[sizeY][sizeX])
{
	int i;
	COORD Next;
	COORD Templ;
	unsigned char NextSymbol;
	bool GameOver = 0;
	

	for(i = 1; Ghs[i].ID!=-1; i++)// ����������� ��������� ������� � ������� � ������� (������� - ������)
	{
		Next.X = 2 * Ghs[i].CurPos.X - Ghs[i].PrevPos.X;
		Next.Y = 2 * Ghs[i].CurPos.Y - Ghs[i].PrevPos.Y;
		NextSymbol = Scrn[Next.Y][Next.X].Symbol;

		if(NextSymbol == ChSpace || NextSymbol == ChLine)
		{
			Ghs[i].PrevPos.X = Ghs[i].CurPos.X;
			Ghs[i].PrevPos.Y = Ghs[i].CurPos.Y;
			Ghs[i].CurPos.X = Next.X;
			Ghs[i].CurPos.Y = Next.Y;
			if(NextSymbol == ChLine)
				{
					GameOver = 1;
				}
			
		}
		else
		{
			if(NextSymbol == ChBorder)
			{
				if(Scrn[Next.Y] [Ghs[i].CurPos.X].Symbol == ChBorder && Scrn[Ghs[i].CurPos.Y] [Next.X].Symbol != ChBorder)
				{
					Templ.X = Ghs[i].CurPos.X;
					Templ.Y = Ghs[i].CurPos.Y;
					Ghs[i].CurPos.X = Next.X;
					Ghs[i].CurPos.Y = Ghs[i].PrevPos.Y;
					Ghs[i].PrevPos.X = Templ.X;
					Ghs[i].PrevPos.Y = Templ.Y;
					if(Scrn[Ghs[i].CurPos.Y] [Ghs[i].CurPos.X].Symbol == ChLine)
					{
						GameOver = 1;
					}
				}

				if(Scrn[Next.Y] [Ghs[i].CurPos.X].Symbol != ChBorder && Scrn[Ghs[i].CurPos.Y] [Next.X].Symbol == ChBorder)
				{
					Templ.Y = Ghs[i].CurPos.Y;
					Templ.X = Ghs[i].CurPos.X;
					Ghs[i].CurPos.Y = Next.Y;
					Ghs[i].CurPos.X = Ghs[i].PrevPos.X;
					Ghs[i].PrevPos.Y = Templ.Y;
					Ghs[i].PrevPos.X = Templ.X;
					if(Scrn[Ghs[i].CurPos.Y] [Ghs[i].CurPos.X].Symbol == ChLine)
					{
						GameOver = 1;
					}
				}

				if(Scrn[Next.Y] [Ghs[i].CurPos.X].Symbol == ChBorder && Scrn[Ghs[i].CurPos.Y] [Next.X].Symbol == ChBorder)
				{
					Templ.X = Ghs[i].CurPos.X;
					Templ.Y = Ghs[i].CurPos.Y;
					Ghs[i].CurPos.X = Ghs[i].PrevPos.X;
					Ghs[i].CurPos.Y = Ghs[i].PrevPos.Y;
					Ghs[i].PrevPos.X = Templ.X;
					Ghs[i].PrevPos.Y = Templ.Y;
					if(Scrn[Ghs[i].CurPos.Y] [Ghs[i].CurPos.X].Symbol == ChLine)
					{
						GameOver = 1;
					}
				}
				if(Scrn[Next.Y] [Ghs[i].CurPos.X].Symbol != ChBorder && Scrn[Ghs[i].CurPos.Y] [Next.X].Symbol != ChBorder)
				{
					COORD NextPos[2];

					NextPos[0].X = Next.X;
					NextPos[0].Y = Ghs[i].PrevPos.Y;

					NextPos[1].X = Ghs[i].PrevPos.X;
					NextPos[1].Y = Next.Y;

					srand( time(0) );
					unsigned int numb = (rand()%100+i)*(rand()%10+i)%2;
					Ghs[i].PrevPos.X = Ghs[i].CurPos.X;
					Ghs[i].PrevPos.Y = Ghs[i].CurPos.Y;
					Ghs[i].CurPos.X = NextPos[numb].X;
					Ghs[i].CurPos.Y = NextPos[numb].Y;
					if(Scrn[Ghs[i].CurPos.Y] [Ghs[i].CurPos.X].Symbol == ChLine)
					{
						GameOver = 1;
					}
				}
			}
			else
			{
				if(NextSymbol == ChGhost)
				{
					COORD NextPos[2];
					NextPos[0].X = Next.X;
					NextPos[0].Y = Ghs[i].PrevPos.Y;

					NextPos[1].X = Ghs[i].PrevPos.X;
					NextPos[1].Y = Next.Y;

					srand( time(0) );
					unsigned int numb = (rand()%100+i)*(rand()%10+i)%2;
					Ghs[i].PrevPos.X = Ghs[i].CurPos.X;
					Ghs[i].PrevPos.Y = Ghs[i].CurPos.Y;
					Ghs[i].CurPos.X = NextPos[numb].X;
					Ghs[i].CurPos.Y = NextPos[numb].Y;
					if(Scrn[Ghs[i].CurPos.Y] [Ghs[i].CurPos.X].Symbol == ChLine)
					{
						GameOver = 1;
					}
			
				}
				//else
				//{
					// �������� �������� ������������
				//}
			}
		}
		Scrn[Ghs[i].CurPos.Y]  [Ghs[i].CurPos.X].Symbol = ChGhost;
		Scrn[Ghs[i].CurPos.Y]  [Ghs[i].CurPos.X].Color = TA.GhostTextAttr;
		if(Scrn[Ghs[i].PrevPos.Y] [Ghs[i].PrevPos.X].Symbol != ChLine) Scrn[Ghs[i].PrevPos.Y] [Ghs[i].PrevPos.X].Symbol = ChSpace;
		if(Scrn[Ghs[i].PrevPos.Y] [Ghs[i].PrevPos.X].Symbol == ChSpace) Scrn[Ghs[i].PrevPos.Y] [Ghs[i].PrevPos.X].Color = TA.BorderTextAttr;

		PrintChr(Ghs[i].PrevPos, Scrn[ Ghs[i].PrevPos.Y ][ Ghs[i].PrevPos.X ].Color, Scrn[ Ghs[i].PrevPos.Y ][ Ghs[i].PrevPos.X ].Symbol);

		PrintChr(Ghs[i].CurPos, Scrn[ Ghs[i].CurPos.Y ][ Ghs[i].CurPos.X ].Color, Scrn[ Ghs[i].CurPos.Y ][ Ghs[i].CurPos.X ].Symbol);

		if(GameOver) return GameOver;
	}
	return GameOver;
}

bool MoveHead(Ghost *Head, CharAttr Scrn[sizeY][sizeX], int Key, COORD &StartPos, COORD &TopLeft, COORD &BottRight, unsigned int &Score, unsigned int &FieldCount)
{
	//int i;
	COORD Next;
	bool GameOver = 0;
	
	if(Head->ID == ChBorder)
	{
		Scrn[Head->CurPos.Y][Head->CurPos.X].Symbol = Head->ID;
		PrintChr(Head->CurPos, TA.BorderTextAttr, Head->ID);

		Next.X = Head->CurPos.X;
		Next.Y = Head->CurPos.Y;
		switch(Key)
		{
		case KLeft:
			{
				if(Next.X>0)
				{
					Next.X = Next.X-1;
				}
				break;
			}
		case KRight:
			{
				if(Next.X<sizeX-1)
				{
					Next.X = Next.X+1;
				}
				break;
			}
		case KUp:
			{
				if(Next.Y>1)
				{
					Next.Y = Next.Y-1;
				}
				break;
			}
		case KDown:
			{
				if(Next.Y<sizeY-2)
				{
					Next.Y = Next.Y+1;
				}
				break;
			}
			
		}
		if((Scrn[Next.Y][Next.X].Symbol==ChSpace) || (Scrn[Next.Y][Next.X].Symbol==ChGhost))
		{
			Head->ID = ChLine;
			StartPos.X = Head->CurPos.X;
			StartPos.Y = Head->CurPos.Y;
			TopLeft.X=Next.X;
			TopLeft.Y=Next.Y;
			BottRight.X=Next.X;
			BottRight.Y=Next.Y;
			if(Scrn[Next.Y][Next.X].Symbol==ChGhost) GameOver=1;
		}
		else// Scrn[Next.Y][Next.X].Symbol==ChBorder
		{
			Head->PrevPos.X = Next.X;
			Head->PrevPos.Y = Next.Y;
		}
		Head->CurPos.X = Next.X;
		Head->CurPos.Y = Next.Y;
	}
	else // Head->ID = ChLine
	{
		Next.X = Head->CurPos.X-Head->PrevPos.X;// ��� ���� ������ ������� ������� X, � �� ��������� �������
		Next.Y = Head->CurPos.Y-Head->PrevPos.Y;// ��� ���� ������ ������� ������� Y, � �� ��������� �������
		switch(Key)
		{
		case KLeft:
			{
				if(Next.Y!=0)
				{
					Head->PrevPos.X = Head->CurPos.X + 1;
					Head->PrevPos.Y = Head->CurPos.Y;
				}
				break;
			}
		case KRight:
			{
				if(Next.Y!=0)
				{
					Head->PrevPos.X = Head->CurPos.X - 1;
					Head->PrevPos.Y = Head->CurPos.Y;
				}
				break;
			}
		case KUp:
			{
				if(Next.X!=0)
				{
					Head->PrevPos.X = Head->CurPos.X;
					Head->PrevPos.Y = Head->CurPos.Y + 1;
				}
				break;
			}
		case KDown:
			{
				if(Next.X!=0)
				{
					Head->PrevPos.X = Head->CurPos.X;
					Head->PrevPos.Y = Head->CurPos.Y - 1;
				}
				break;
			}			
		}
		Next.X = 2 * Head->CurPos.X - Head->PrevPos.X;
		Next.Y = 2 * Head->CurPos.Y - Head->PrevPos.Y;
		Head->PrevPos.X = Head->CurPos.X;
		Head->PrevPos.Y = Head->CurPos.Y;
		Head->CurPos.X = Next.X;
		Head->CurPos.Y = Next.Y;

		if(TopLeft.X>Head->CurPos.X) TopLeft.X=Head->CurPos.X;
		if(TopLeft.Y>Head->CurPos.Y) TopLeft.Y=Head->CurPos.Y;

		if(BottRight.X<Head->CurPos.X) BottRight.X=Head->CurPos.X;
		if(BottRight.Y<Head->CurPos.Y) BottRight.Y=Head->CurPos.Y;


		Scrn[Head->PrevPos.Y][Head->PrevPos.X].Color=TA.LineTextAttr;
		Scrn[Head->PrevPos.Y][Head->PrevPos.X].Symbol = ChLine;
		PrintChr(Head->PrevPos, Scrn[Head->PrevPos.Y][Head->PrevPos.X].Color, ChLine);
		if(Scrn[Head->CurPos.Y][Head->CurPos.X].Symbol==ChBorder)
		{
			Head->ID = ChBorder;
			Scrn[Head->CurPos.Y][Head->CurPos.X].Color=TA.HeadTextAttr;
			Filling(Scrn, &(*Head),  Score, FieldCount);
		}
		else
		{
			Scrn[Head->CurPos.Y][Head->CurPos.X].Symbol=ChLine;
			Scrn[Head->CurPos.Y][Head->CurPos.X].Color=TA.LineTextAttr;
		}
	}

	Scrn[Head->CurPos.Y][Head->CurPos.X].Symbol = Head->ID;
	PrintChr(Head->CurPos, TA.HeadTextAttr, ChHead);

	return GameOver;
}

bool TryMsBox(COORD TopLeft, CharAttr Scrn[sizeY][sizeX], int TryCnt)
{
	const int MB_N = 5;
	const int MB_M = 35;

	int i,j;

	char MB[MB_N][MB_M] =  {"                                  ",///
							"  �������. ������� ����� ������,  ",///
							"      ����� ���������� ����.      ",///
							"      �������� ������� - �        ",///
							"                                  "};

	COORD Pos;

	SetConsoleOutputCP(1251);
	if(TryCnt < 0)
	{
		TopLeft.X = (TopLeft.X-MB_M+10)/2;
		TopLeft.Y = TopLeft.Y/2-2;
		PrintStr(TopLeft, TA.LightSelTextAttr, "                          ");
		TopLeft.Y++;
		PrintStr(TopLeft, NULL, "    �� ������ �������!    ");
		TopLeft.Y++;
		PrintStr(TopLeft, NULL, "                          ");
		SetConsoleTextAttribute(handle, TA.BorderTextAttr); 
		getch();
	}
	if(TryCnt > 0)
	{
		TopLeft.X = (TopLeft.X-MB_M+1)/2;
		TopLeft.Y = TopLeft.Y/2-3;
	
		Pos.X = TopLeft.X;
		SetConsoleTextAttribute(handle, TA.LightSelTextAttr);
		for(i = 0;i<MB_N;i++)
		{
			Pos.Y = TopLeft.Y+i;
			PrintStr(Pos, NULL, MB[i]);
		}
		SetConsoleOutputCP(CodePage);
		Pos.X = TopLeft.X+25;
		Pos.Y = TopLeft.Y+3;
		GotoXY(Pos);
		SetConsoleTextAttribute(handle, TA.XTextAttr);
		printf("%d", TryCnt);
		SetConsoleTextAttribute(handle, TA.LightSelTextAttr);
		getch();
		
		for(i = 0;i<MB_N;i++)
		{
			for(j = 0;j<MB_M;j++)
			{
				Pos.X = j+TopLeft.X;
				Pos.Y = i+TopLeft.Y;
				PrintChr(Pos, Scrn[Pos.Y][Pos.X].Color, Scrn[Pos.Y][Pos.X].Symbol);

			}
		}
		SetConsoleTextAttribute(handle, TA.BorderTextAttr); 
		//SetConsoleOutputCP(CodePage);
	}
	if(TryCnt == 0)
	{
		TopLeft.X = (TopLeft.X-MB_M+10)/2;
		TopLeft.Y = TopLeft.Y/2-2;
		PrintStr(TopLeft, TA.LightSelTextAttr, "                          ");
		TopLeft.Y++;
		PrintStr(TopLeft, NULL, "       �� ���������       ");
		TopLeft.Y++;
		PrintStr(TopLeft, NULL, "                          ");
		SetConsoleTextAttribute(handle, TA.BorderTextAttr); 
		getch();
	}
	return 0;
}

bool EscMsBox(COORD TopLeft, CharAttr *Scrn)
{
	bool Exit = 0;
	int i, j, Key = KDefault;
	const int MB_N = 5;
	const int MB_M = 35;
	char MB[MB_N][MB_M] =  {"                                  ",///
							"  ��������� ���� � ����� � ����?  ",///
							"                                  ",///
							"             ��  ���              ",///
							"                                  "};
	char WYes[] = " �� ";
	char WNo[] = " ��� ";
	COORD Pos,Yes,No;

	SetConsoleOutputCP(1251);

	TopLeft.X = (TopLeft.X-MB_M+1)/2;
	TopLeft.Y = TopLeft.Y/2-3;
	
	Pos.X = TopLeft.X;
	SetConsoleTextAttribute(handle, TA.LightSelTextAttr);
	for(i = 0;i<MB_N;i++)
	{
		Pos.Y = TopLeft.Y+i;
		PrintStr(Pos, NULL, MB[i]);
	}
	Yes.X = TopLeft.X+12;
	Yes.Y = TopLeft.Y+3;
	No.X = TopLeft.X+16;
	No.Y = TopLeft.Y+3;
	PrintStr(No, TA.LightTextAttr, WNo);
	SetConsoleTextAttribute(handle, TA.LightSelTextAttr);

	while(Key!=KEnter)
	{
		Key = getch();
		switch(Key)
		{
		case KLeft:
			{
				PrintStr(Yes, TA.LightTextAttr, WYes);
				PrintStr(No, TA.LightSelTextAttr, WNo);
				Exit = 1;
				break;
			}
		case KRight:
			{
				PrintStr(No, TA.LightTextAttr, WNo);
				PrintStr(Yes, TA.LightSelTextAttr, WYes);
				Exit = 0;
				break;
			}
		case KEnter:
			{
				SetConsoleOutputCP(CodePage);
				if(Exit)
				{
					SetConsoleTextAttribute(handle, TA.BorderTextAttr);
					
					return Exit;
				}

				for(i = 0;i<MB_N;i++)
				{
					for(j = 0;j<MB_M;j++)
					{
						Pos.X = j+TopLeft.X;
						Pos.Y = i+TopLeft.Y;
						PrintChr(Pos, (Scrn+Pos.X+Pos.Y*sizeX)->Color, (Scrn+Pos.X+Pos.Y*sizeX)->Symbol);
					}
				}
				SetConsoleTextAttribute(handle, TA.BorderTextAttr); 
				SetConsoleOutputCP(CodePage);
				return Exit;
			}
		case KEsc:
			{
				SetConsoleOutputCP(CodePage);
				for(i = 0;i<MB_N;i++)
				{
					for(j = 0;j<MB_M;j++)
					{
						Pos.X = j+TopLeft.X;
						Pos.Y = i+TopLeft.Y;
						PrintChr(Pos, (Scrn+Pos.X+Pos.Y*sizeX)->Color, (Scrn+Pos.X+Pos.Y*sizeX)->Symbol);
					}
				}
				SetConsoleTextAttribute(handle, TA.BorderTextAttr);
				
				return Exit;
			}
		
		}
	};
	SetConsoleOutputCP(CodePage);
	return Exit;

}

void PrintChr(COORD Pos, unsigned char Clr, unsigned char Smbl)
{
	GotoXY(Pos);
	SetConsoleTextAttribute( handle, Clr);
	putchar(Smbl);
}

void PrintStr(COORD Pos, unsigned char Clr, char *Str)
{
	GotoXY(Pos);
	if(Clr!=NULL) SetConsoleTextAttribute( handle, Clr);
	printf(Str);
}

void Filling(CharAttr Scrn[sizeY][sizeX], Ghost *Ghs, unsigned int &Score, unsigned int &FieldCounter)
{
	COORD Pos;
	
	unsigned short int i,j,k;
	unsigned char isFieldCountUse=0;
	unsigned int FieldCount=1;
	for(j=2;j<sizeY-2;j++)
	{
		
		for(i=1;i<sizeX-1;i++)
		{
			// ������� ������: ������ ��� "����"...
			if(Scrn[j][i].Symbol == ChSpace || Scrn[j][i].Symbol == ChGhost) 
			{
				if(Scrn[j][i].Symbol == ChSpace)
				{
					
				}
				// ������� ������: ������ ��� "����"...
				if(Scrn[j-1][i].Symbol == ChSpace || Scrn[j-1][i].Symbol == ChGhost)
				{
					// ������������� ID ���� ��� � �������
					Scrn[j][i].FieldID = Scrn[j-1][i].FieldID;
					// ����������: ������ ��� "����"...
					if(Scrn[j][i-1].Symbol == ChSpace || Scrn[j][i-1].Symbol == ChGhost)
					{
						// ID �������(����) ����������� ������� ������ ��� �������� �������
						if(Scrn[j][i-1].FieldID < Scrn[j][i].FieldID)
						{
							// ������������� ���������� ID
							Scrn[j][i].FieldID = Scrn[j][i-1].FieldID;
							Scrn[j-1][i].FieldID = Scrn[j][i-1].FieldID;
						}
					}
					
				}
				else// ������� ������: �� ������ � �� "����"
				{
					// ����������: ������ ��� "����"...
					if(Scrn[j][i-1].Symbol == ChSpace || Scrn[j][i-1].Symbol == ChGhost)
					{
						// ������������� ID ���� ��� � ����������
						Scrn[j][i].FieldID = Scrn[j][i-1].FieldID;
					}
					else// ����������: �� ������ � �� "����"
					{
						// ������������� ID ���� �� FieldCount
						Scrn[j][i].FieldID=FieldCount;
						// ������ ������� ��� ���� ID ��������������
						isFieldCountUse=1;
					}
					
				}
			}// ������� ������: ������ ��� "����"...
			// ��������� ������: ����� ��� ������
			if(Scrn[j][i+1].Symbol == ChLine || Scrn[j][i+1].Symbol == ChBorder)
			{
				// ����� ID �������������...
				if(isFieldCountUse && i<sizeX-2)
				{
					// ... ����������� ������� ID
					FieldCount++;
					// ������ ������� ��� ���� ID �� ��������������
					isFieldCountUse=0;
				}
			}
		}// for(i=1;i<sizeX-1;i++)
	}// for(j=2;j<sizeY-2;j++)

	isFieldCountUse=0;// ������ ��� ��������� ��������� ���� ����� � ������ ID (��������� �� ����� �����)
	do
	{

		for(i=sizeX-2;i>0;i--)
		{
		
			for(j=2;j<sizeY-2;j++)
			{
				// ������� ������: ������ ��� "����"...
				if(Scrn[j][i].Symbol == ChSpace || Scrn[j][i].Symbol == ChGhost)
				{
					// ������ ������: ������ ��� "����"...
					if(Scrn[j][i+1].Symbol == ChSpace || Scrn[j][i+1].Symbol == ChGhost)
					{
						// ID �������� ������ ��� ������...
						if(Scrn[j][i].FieldID > Scrn[j][i+1].FieldID)
						{
							isFieldCountUse=1;
							// �������� ID � ������� �� �������
							Scrn[j][i].FieldID = Scrn[j][i+1].FieldID;
							// ������� ������: ������ ��� "����"...
							if(Scrn[j-1][i].Symbol == ChSpace || Scrn[j-1][i].Symbol == ChGhost)
							{
								// ID �������� ������ ��� ������...
								if(Scrn[j][i].FieldID > Scrn[j-1][i].FieldID)
								{
									//isFieldCountUse=1;
									Scrn[j][i].FieldID = Scrn[j-1][i].FieldID;
									Scrn[j][i+1].FieldID = Scrn[j-1][i].FieldID;
								}
								else
								{
									// ID �������� ������ ��� ������...
									if(Scrn[j][i].FieldID < Scrn[j-1][i].FieldID)
									{
										//isFieldCountUse=1;
										Scrn[j-1][i].FieldID = Scrn[j][i].FieldID;
									}
								}
							}
							
						}
						else
						{
							// ID �������� ������ ��� ������...
							if(Scrn[j][i].FieldID < Scrn[j][i+1].FieldID)
							{
								isFieldCountUse=1;
								// �������� ID � ������ �� �������
								Scrn[j][i+1].FieldID = Scrn[j][i].FieldID;
							}
							
						}
					}
					else// ������ ������: �� ������ � �� "����"...
					{
						// ������� ������: ������ ��� "����"...
						if(Scrn[j-1][i].Symbol == ChSpace || Scrn[j-1][i].Symbol == ChGhost)
						{
							// ID �������� ������ ��� ������...
							if(Scrn[j][i].FieldID > Scrn[j-1][i].FieldID)
							{
								isFieldCountUse=1;
								Scrn[j][i].FieldID = Scrn[j-1][i].FieldID;
								
							}
							else
							{
								// ID �������� ������ ��� ������...
								if(Scrn[j][i].FieldID < Scrn[j-1][i].FieldID)
								{
									isFieldCountUse=1;
									Scrn[j-1][i].FieldID = Scrn[j][i].FieldID;
									
								}
							}
						}
					}
				}// ������� ������: ������ ��� "����"...
			}// for(j=...)
		}//for(i=...)

		if(isFieldCountUse == 3) break;

		for(j=sizeY-3;j>1;j--)
		{
		
			for(i=sizeX-2;i>0;i--)
			{
				// ������� ������: ������ ��� "����"...
				if(Scrn[j][i].Symbol == ChSpace || Scrn[j][i].Symbol == ChGhost)
				{
					// ������ ������: ������ ��� "����"...
					if(Scrn[j+1][i].Symbol == ChSpace || Scrn[j+1][i].Symbol == ChGhost)
					{
						// ID �������� ������ ��� �����...
						if(Scrn[j][i].FieldID > Scrn[j+1][i].FieldID)
						{
							isFieldCountUse=2;
							// �������� ID � ������� �� �������
							Scrn[j][i].FieldID = Scrn[j+1][i].FieldID;
							// ������ ������: ������ ��� "����"...
							if(Scrn[j][i+1].Symbol == ChSpace || Scrn[j][i+1].Symbol == ChGhost)
							{
								// ID �������� ������ ��� ������...
								if(Scrn[j][i].FieldID > Scrn[j][i+1].FieldID)
								{
									isFieldCountUse=2;
									Scrn[j][i].FieldID = Scrn[j][i+1].FieldID;
									Scrn[j+1][i].FieldID = Scrn[j][i+1].FieldID;
								}
								else
								{
									// ID �������� ������ ��� ������...
									if(Scrn[j][i].FieldID < Scrn[j][i+1].FieldID)
									{
										isFieldCountUse=2;
										Scrn[j][i+1].FieldID = Scrn[j][i].FieldID;
									}
								}
							}

						}
						else
						{
							// ID �������� ������ ��� �����...
							if(Scrn[j][i].FieldID < Scrn[j+1][i].FieldID)
							{
								isFieldCountUse=2;
								// �������� ID � ������ �� �������
								Scrn[j+1][i].FieldID = Scrn[j][i].FieldID;
							}
						}
					}
					else// ������ ������: �� ������ � �� "����"...
					{
						// ������ ������: ������ ��� "����"...
						if(Scrn[j][i+1].Symbol == ChSpace || Scrn[j][i+1].Symbol == ChGhost)
						{
							// ID �������� ������ ��� ������...
							if(Scrn[j][i].FieldID > Scrn[j][i+1].FieldID)
							{
								isFieldCountUse=2;
								Scrn[j][i].FieldID = Scrn[j][i+1].FieldID;
							}
							else
							{
								// ID �������� ������ ��� ������...
								if(Scrn[j][i].FieldID < Scrn[j][i+1].FieldID)
								{
									isFieldCountUse=2;
									Scrn[j][i+1].FieldID = Scrn[j][i].FieldID;
								}
							}
						}
					}
				}// ������� ������: ������ ��� "����"...
			}// for(i=...)
		}//for(j=...)
		
		if(isFieldCountUse == 4) break;

		for(i=1;i<sizeX-1;i++)
		{
		
			for(j=sizeY-3;j>1;j--)
			{
				// ������� ������: ������ ��� "����"...
				if(Scrn[j][i].Symbol == ChSpace || Scrn[j][i].Symbol == ChGhost)
				{
					// ����� ������: ������ ��� "����"...
					if(Scrn[j][i-1].Symbol == ChSpace || Scrn[j][i-1].Symbol == ChGhost)
					{
						// ID �������� ������ ��� �����...
						if(Scrn[j][i].FieldID > Scrn[j][i-1].FieldID)
						{
							isFieldCountUse=3;
							// �������� ID � ������� �� �������
							Scrn[j][i].FieldID = Scrn[j][i-1].FieldID;
							// ������ ������: ������ ��� "����"...
							if(Scrn[j+1][i].Symbol == ChSpace || Scrn[j+1][i].Symbol == ChGhost)
							{
								// ID �������� ������ ��� �����...
								if(Scrn[j][i].FieldID > Scrn[j+1][i].FieldID)
								{
									isFieldCountUse=3;
									Scrn[j][i].FieldID = Scrn[j+1][i].FieldID;
									Scrn[j][i-1].FieldID = Scrn[j+1][i].FieldID;
								}
								else
								{
									// ID �������� ������ ��� �����...
									if(Scrn[j][i].FieldID < Scrn[j+1][i].FieldID)
									{
										isFieldCountUse=3;
										Scrn[j+1][i].FieldID = Scrn[j][i].FieldID;
									}
								}
							}

						}
						else
						{
							// ID �������� ������ ��� �����...
							if(Scrn[j][i].FieldID < Scrn[j][i-1].FieldID)
							{
								isFieldCountUse=3;
								// �������� ID � ����� �� �������
								Scrn[j][i-1].FieldID = Scrn[j][i].FieldID;
							}
						}
					}
					else// ����� ������: �� ������ � �� "����"...
					{
						// ������ ������: ������ ��� "����"...
						if(Scrn[j+1][i].Symbol == ChSpace || Scrn[j+1][i].Symbol == ChGhost)
						{
							// ID �������� ������ ��� �����...
							if(Scrn[j][i].FieldID > Scrn[j+1][i].FieldID)
							{
								isFieldCountUse=3;
								Scrn[j][i].FieldID = Scrn[j+1][i].FieldID;
							}
							else
							{
								// ID �������� ������ ��� �����...
								if(Scrn[j][i].FieldID < Scrn[j+1][i].FieldID)
								{
									isFieldCountUse=3;
									Scrn[j+1][i].FieldID = Scrn[j][i].FieldID;
								}
							}
						}
					}
				}// ������� ������: ������ ��� "����"...
			}// for(j=...)
		}//for(i=...)

		if(isFieldCountUse == 1) break;

		for(j=2;j<sizeY-2;j++)
		{
		
			for(i=1;i<sizeX-1;i++)
			{
				// ������� ������: ������ ��� "����"...
				if(Scrn[j][i].Symbol == ChSpace || Scrn[j][i].Symbol == ChGhost)
				{
					// ������� ������: ������ ��� "����"...
					if(Scrn[j-1][i].Symbol == ChSpace || Scrn[j-1][i].Symbol == ChGhost)
					{
						// ID �������� ������ ��� ������...
						if(Scrn[j][i].FieldID > Scrn[j-1][i].FieldID)
						{
							isFieldCountUse=4;
							// �������� ID � ������� �� �������
							Scrn[j][i].FieldID = Scrn[j-1][i].FieldID;
							// ����� ������: ������ ��� "����"...
							if(Scrn[j][i-1].Symbol == ChSpace || Scrn[j][i-1].Symbol == ChGhost)
							{
								// ID �������� ������ ��� �����...
								if(Scrn[j][i].FieldID > Scrn[j][i-1].FieldID)
								{
									isFieldCountUse=4;
									Scrn[j][i].FieldID = Scrn[j][i-1].FieldID;
									Scrn[j-1][i].FieldID = Scrn[j][i-1].FieldID;
								}
								else
								{
									// ID �������� ������ ��� �����...
									if(Scrn[j][i].FieldID < Scrn[j][i-1].FieldID)
									{
										isFieldCountUse=4;
										Scrn[j][i-1].FieldID = Scrn[j][i].FieldID;
									}
								}
							}

						}
						else
						{
							// ID �������� ������ ��� ������...
							if(Scrn[j][i].FieldID < Scrn[j-1][i].FieldID)
							{
								isFieldCountUse=4;
								// �������� ID � ������� �� �������
								Scrn[j-1][i].FieldID = Scrn[j][i].FieldID;
							}
						}
					}
					else// ������� ������: �� ������ � �� "����"...
					{
						// ����� ������: ������ ��� "����"...
						if(Scrn[j][i-1].Symbol == ChSpace || Scrn[j][i-1].Symbol == ChGhost)
						{
							// ID �������� ������ ��� �����...
							if(Scrn[j][i].FieldID > Scrn[j][i-1].FieldID)
							{
								isFieldCountUse=4;
								Scrn[j][i].FieldID = Scrn[j][i-1].FieldID;
							}
							else
							{
								// ID �������� ������ ��� �����...
								if(Scrn[j][i].FieldID < Scrn[j][i-1].FieldID)
								{
									isFieldCountUse=4;
									Scrn[j][i-1].FieldID = Scrn[j][i].FieldID;
								}
							}
						}
					}
				}// ������� ������: ������ ��� "����"...
			}// for(i=...)
		}//for(j=...)
		
		if(isFieldCountUse == 2) break;
	}
	while(isFieldCountUse);

	unsigned short int curField=0;
	for(j=2;j<sizeY-2;j++)
	{
		for(i=1;i<sizeX-1;i++)
		{
			if(Scrn[j][i].Symbol == ChSpace)
			{

				if(Scrn[j][i].FieldID == curField) continue;
				else
				{
					for(k=1; (Ghs+k)->ID>0; k++)
					{
					
						if(Scrn[j][i].FieldID == Scrn[ (Ghs+k)->CurPos.Y ][ (Ghs+k)->CurPos.X ].FieldID) 
						{
							curField=Scrn[j][i].FieldID;
							break;
						}
					}
					if((Ghs+k)->ID<0)
					{
						Scrn[j][i].FieldID = 0;
						Pos.X=i;
						Pos.Y=j;
						Scrn[j][i].Symbol = ChBorder;
						Scrn[j][i].Color = TA.BorderTextAttr;
						PrintChr(Pos, TA.BorderTextAttr, ChBorder);
						Score++;
						FieldCounter++;
						
					}
				}
			}
			if(Scrn[j][i].Symbol == ChLine)
			{
				FieldCounter++;
				Pos.X=i;
				Pos.Y=j;
				Scrn[j][i].Symbol = ChBorder;
				Scrn[j][i].Color = TA.BorderTextAttr;
				PrintChr(Pos, TA.BorderTextAttr, ChBorder);
				
			}
			

		}
	}

	char strScore[20]="����: ";
	SetConsoleOutputCP(1251);
	Pos.X=35;
	Pos.Y=0;
	PrintStr(Pos, TA.XTextAttr, strScore);
	SetConsoleOutputCP(CodePage);
	Pos.X=35 + strlen(strScore);
	itoa(Score,strScore,10);
	PrintStr(Pos, TA.XTextAttr, strScore);
				
}

void ReadRecords(char NameList[StrCount][LenStr], unsigned int ScoreList[StrCount])
{
	int i,j,k;

	FILE* RecFile;

	char RecFileName[]="levels\\records.txt";
	char str[LenStr], str2[LenStr];

	RecFile = fopen (RecFileName,"r");
	if(RecFile != 0)
	{
		for(i=0; i<StrCount; i++)
		{ 
			fgets(str, LenStr, RecFile);
			for(j=0; str[j] != '\t'; j++)
			{
				NameList[i][j]=str[j];
			}
			NameList[i][j++]='\0';
			
			for(k=0; str[j]>47 && str[j]<58;k++, j++)
			{
				str2[k]=str[j];
			}
			str2[k]='\0';
			ScoreList[i] = atoi(str2);
		}
		
		fclose(RecFile);
	}

}

void ChangeRecords(unsigned int &Score)
{
	

}

void Records(COORD &SizeWin, unsigned int &Score,char NameList[StrCount][LenStr], unsigned int ScoreList[StrCount])
{
	short int i;
	unsigned int tmp;
	char Name[21]="��",str[21]="";

	mciSendString(L"play sound\\�������.mp3 repeat",0,0,0);
	system("cls");
	
	if(Score==0)
	{
		ShowRecords(SizeWin, NameList, ScoreList, Score, -1);
	}
	else
	{
		if(ScoreList[StrCount-1]<Score)
		{
			ScoreList[StrCount-1] = Score;
			strcpy(NameList[StrCount-1], Name);
			for(i=StrCount-2; (ScoreList[i]<Score) && (i>-1); i--)
			{
				tmp=ScoreList[i];
				ScoreList[i]=ScoreList[i+1];
				ScoreList[i+1]=tmp;

				strcpy(str,NameList[i]);
				strcpy(NameList[i],NameList[i+1]);
				strcpy(NameList[i+1],str);
			}
		}
		ShowRecords(SizeWin, NameList, ScoreList, Score, i+1);
	}
	while(1)
	{
		if(getch() == KEsc)
		{
			system("cls");
			mciSendString(L"stop sound\\�������.mp3",0,0,0);
			Score=0;
			return;
		}
	}
	
}

void ShowRecords(COORD SizeWin, char NameList[StrCount][LenStr],unsigned int  ScoreList[StrCount], unsigned int &Score, short int Index)
{
	int i;
	COORD Pos;

	SizeWin.Y=(SizeWin.Y-20)/2;
	SizeWin.X=SizeWin.X/2-10;
	SetConsoleTextAttribute(handle,TA.BorderTextAttr);
	for(i=0; i<StrCount; i++)
	{
		Pos.X=SizeWin.X;
		Pos.Y=SizeWin.Y+2*i;
		GotoXY(Pos);
		if(Index == i)
		{
			SetConsoleTextAttribute(handle,TA.GhostTextAttr);
			printf(">> %6s\t%d  << ��� ���������",NameList[i],ScoreList[i]);
			SetConsoleTextAttribute(handle,TA.BorderTextAttr);
		}
		else printf("   %6s\t%d",NameList[i],ScoreList[i]);
	}
	Pos.X=SizeWin.X-7;
	Pos.Y=SizeWin.Y+22;
	GotoXY(Pos);
	SetConsoleTextAttribute(handle,TA.LineTextAttr);
	printf("������� 'Esc', ����� ����� � ����");
	SetConsoleTextAttribute(handle,TA.BorderTextAttr);
}
