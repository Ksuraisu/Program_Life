#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <random>
using namespace std;

//max life level
const int mll = 12;
/*[0] - size_x, [1] - size_y, [2] - ����������� ����, [3] - ��������� ���������� ������
���-�� ������� : [4] - ���. ��� ����������� �����, [5] - ����. ��� ����������� �����, [6] - ��� ���������� �����*/
int simsettings[7]{ 21, 21, 0, 1, 2, 3, 3 };
/*[0] - "��������" ������, [1] - ����� ���������, [2] - �������� ���������*/
int programsettings[3]{ 1, 1, 400 };

void acout(string whatToAnimate, bool endLine = true, int color = 15, int delay = programsettings[0]) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	for (int i = 0; i < whatToAnimate.length(); i++) {
		cout << whatToAnimate[i];
		Sleep(delay);
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	if (endLine) cout << endl;
}

void createMap() {
	ofstream fout("work.dat");
	mt19937 engine;
	random_device device;
	engine.seed(device());
	for (int y = 0; y < simsettings[1]; y++) {
		for (int x = 0; x < simsettings[0]; x++)
			fout << engine() % 2;
		fout << endl;
	}
	fout.close();
}

//�������, ������������ ��������� � work.out
void writeGen(int **current, int nofgen, int countLifes) {
	ofstream fout("work.out", ios_base::app);
	if (nofgen != 1) fout << "\n";
	fout << "��������� #" << nofgen << "\n";
	fout << "�����: " << countLifes << "\n";
	for (int y = 0; y < simsettings[1]; y++) {
		fout << '|';
		for (int x = 0; x < simsettings[0]; x++) {
			//X - ������ �����
			if (current[y][x] == -1) fout << 'X' << '|';
			else fout << current[y][x] << '|';
		}
		fout << endl;
	}
	fout.close();
}

int checkNeighbors(int **current, int y, int x) {
	int neighbors = 0;
	int s_y = y, s_x = x;
	bool yih = false, yil = false, xih = false, xil = false;
	if (simsettings[2]) {
		if (y - 1 < 0) {
			s_y = simsettings[1]; yil = true;
		} else if (y + 1 >= simsettings[1]) {
			s_y = -1; yih = true;
		} else s_y = y;

		if (x - 1 < 0) {
			s_x = simsettings[0]; xil = true;
		} else if (x + 1 >= simsettings[0]) {
			s_x = -1;
			xih = true;
		} else s_x = x;
	}

	//�������� �������������� �������� ������
	if (!yil && s_y - 1 >= 0 && current[s_y - 1][x] > 0 && current[s_y - 1][x] <= mll) neighbors++;
	if (!xih && s_x + 1 < simsettings[0] && current[y][s_x + 1] > 0 && current[y][s_x + 1] <= mll) neighbors++;
	if (!yih && s_y + 1 < simsettings[1] && current[s_y + 1][x] > 0 && current[s_y + 1][x] <= mll) neighbors++;
	if (!xil && s_x - 1 >= 0 && current[y][s_x - 1] > 0 && current[y][s_x - 1] <= mll) neighbors++;
	//�������� ������������ �������� ������
	if ((!yil && !xih) && (s_y - 1 >= 0 && s_x + 1 < simsettings[0]) && current[s_y - 1][s_x + 1] > 0 && current[s_y - 1][s_x + 1] <= mll) neighbors++;
	if ((!yih && !xih) && (s_y + 1 < simsettings[1] && s_x + 1 < simsettings[0]) && current[s_y + 1][s_x + 1] > 0 && current[s_y + 1][s_x + 1] <= mll) neighbors++;
	if ((!yih && !xil) && (s_y + 1 < simsettings[1] && s_x - 1 >= 0) && current[s_y + 1][s_x - 1] > 0 && current[s_y + 1][s_x - 1] <= mll) neighbors++;
	if ((!yil && !xih) && (s_y - 1 >= 0 && s_x - 1 >= 0) && current[s_y - 1][s_x - 1] > 0 && current[s_y - 1][s_x - 1] <= mll) neighbors++;

	return neighbors;
}

void simout(int **current);
void CUI();

void save() {
	ofstream fout("settings.txt");
	string output;
	for (int i = 0; i < sizeof(programsettings) / sizeof(programsettings[0]); i++)
		fout << programsettings[i] << '\n';
	for (int i = 0; i < sizeof(simsettings) / sizeof(simsettings[0]); i++)
		fout << simsettings[i] << '\n';
}
void read() {
	ifstream fin("settings.txt");
	if (!fin.is_open()) return;
	string input;
	for (int i = 0; i < sizeof(programsettings) / sizeof(programsettings[0]); i++) {
		getline(fin, input);
		programsettings[i] = stoi(input);

	}
	for (int i = 0; i < sizeof(simsettings) / sizeof(simsettings[0]); i++) {
		getline(fin, input);
		simsettings[i] = stoi(input);
	}
}
void simulation(int nofgens) {
	ofstream oworkdat("work.dat");
	ifstream iworkdat("work.dat");
	//�������� ����������� work.out ����� ������� � ���
	ofstream clearworkout("work.out", ios_base::trunc); clearworkout.close();
	int **p_current = new int *[simsettings[1]], **p_future = new int *[simsettings[1]];
	for (int i = 0; i < simsettings[1]; i++) p_current[i] = new int[simsettings[0]];
	for (int i = 0; i < simsettings[1]; i++) p_future[i] = new int[simsettings[0]];
	string input;

	//������������� ��������
	for (int y = 0; y < simsettings[1]; y++) {
		for (int x = 0; x < simsettings[0]; x++) {
			p_future[y][x] = 0; p_current[y][x] = 0;
		}
	}
	createMap();

	//������ ������� ��������� � ������
	for (int y = 0; getline(iworkdat, input); y++) {
		for (int x = 0; x < simsettings[0]; x++) {
			if (input[x] == '1') p_current[y][x] = 1;
			/*����������: � ������� current ������������ ��������� ������ ��������
			1 - ���� ������, 0 - ����������� ������, -1 - ������ �����*/
			else if (input[x] == '0') p_current[y][x] = -1;
		}
	}

	iworkdat.close(); oworkdat.close();
	//�����������
	ofstream log("log.txt", ios::trunc);
	for (int nofgen = 1; nofgen <= nofgens; nofgen++) {
		for (int y = 0; y < simsettings[1]; y++) {
			for (int x = 0; x < simsettings[0]; x++) {
				//�������� �������� �� ��������� ���������� ������
				int neighbours = checkNeighbors(p_current, y, x);
				log << neighbours << ' ';
				if (simsettings[3]) {
					if (p_current[y][x] > 0 && p_current[y][x] <= mll) {
						if (neighbours >= simsettings[4] && neighbours <= simsettings[5]) p_future[y][x]++;
						else p_future[y][x] = -1;
					} else if (p_current[y][x] == -1) {
						if (neighbours == simsettings[6]) p_future[y][x]++;
					} else if (p_current[y][x] > mll) {
						p_future[y][x] = -1;
					} else if (p_current[y][x] == 0) {
						p_future[y][x]++;
					}
				} else {
					if (p_current[y][x] == 1 && neighbours >= simsettings[4] && neighbours <= simsettings[5]) p_future[y][x] = 1;
					else if (p_current[y][x] == -1 && neighbours == simsettings[6]) p_future[y][x] = 1;
					else p_future[y][x] = -1;
				}
			}
			log << endl;
		}
		for (int i = 0; i < simsettings[0] + simsettings[0] - 1; i++) log << '-'; log << endl;
		//������ �����
		int countLifes = 0;
		for (int y = 0; y < simsettings[1]; y++)
			for (int x = 0; x < simsettings[0]; x++) {
				if (p_current[y][x] >= 0) countLifes++;
			}

		//������ ��������� � ����
		writeGen(p_current, nofgen, countLifes);
		system("cls");

		//��������������� ��������� � �������
		if (programsettings[1] != 3) {
			simout(p_current);
			if (programsettings[1] == 1) Sleep(programsettings[2]);
			else system("pause");
		}
		//��������� ��������� � ������ ������ ����
		if (countLifes == 0) {
			cout << '\n' << "��� ������� ������." << '\n';

			break;
		}

		//������ "��������" � "���������"
		for (int y = 0; y < simsettings[1]; y++)
			for (int x = 0; x < simsettings[0]; x++)
				p_current[y][x] = p_future[y][x];
	}
	cout << '\n' << "��������� ��������� ���� ������." << '\n';
	for (int i = 0; i < simsettings[1]; i++) {
		delete p_current[i]; delete p_future[i];
	}
	delete[] p_current; delete[] p_future;
	system("pause");
}
int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ~ENABLE_PROCESSED_INPUT);
	read();
	CUI();
	save();
	return 0;
}

void simSettings() {
	acout("������� ��������� ���������:");
	acout("  0 - ����� � ������� ����");
	acout("  1 - ������ ����: ", false);
	acout(to_string(simsettings[0]), false);
	acout("x", false);
	acout(to_string(simsettings[1]));

	acout("  2 - ����������� ����: ", false);
	if (simsettings[2] == 1) acout("��������");
	else acout("���������");

	acout("  3 - ��������� ���������� ������ ��������: ", false);
	if (simsettings[3] == 1) acout("��������");
	else acout("���������");

	acout("  4 - ���������� ������� ������������ ��� ����������� �����: ", false);
	acout(to_string(simsettings[4]), false); acout(" - ", false); acout(to_string(simsettings[5]));
	acout("  5 - ���������� ������� ������������ ��� ���������� �����: ", false); acout(to_string(simsettings[6]));
	acout("��� �� ������ ��������: ", false);
	string input; int iinput;
	cin >> input;
	try
	{
		iinput = stoi(input);
	} catch (...) {
		system("cls");
		int animation = programsettings[0];
		programsettings[0] = 0;
		simSettings();
		programsettings[0] = animation;
		return;
	}
simchoose:
	switch (iinput)
	{
	case 0:
		system("cls");
		save();
		return;
	case 1:
		acout("��������! �� ������������� ������ ���� ������ 50x50");
		acout("������� ����� ������ ����: ");
		acout("X: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		if (iinput > 0) simsettings[0] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		acout("Y: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		if (iinput > 0) simsettings[1] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		system("cls");
		simSettings();
		return;
	case 2:
		if (simsettings[2] == 1) simsettings[2] = 0;
		else simsettings[2] = 1;
		system("cls");
		simSettings();
		return;
	case 3:
		if (simsettings[3] == 1) simsettings[3] = 0;
		else simsettings[3] = 1;
		system("cls");
		simSettings();
		return;
	case 4:
		acout("������� ����� ���������� ������� ��� ����������� �����:");
		acout("��: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		if (iinput > -1 && iinput < 9) simsettings[4] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
			break;
		}
		acout("��: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		if (iinput > -1 && iinput < 9) simsettings[5] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
			break;
		}
		system("cls");
		simSettings();
		return;
	case 5:
		acout("������� ����� ���������� ������� ��� ���������� �����: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
		}
		if (iinput > -1 && iinput < 9) simsettings[6] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto simchoose;
			break;
		}
		system("cls");
		simSettings();
		return;
	default:
		acout("�������� ����! ���������� �����.");
		system("pause");
		system("cls");
		goto simchoose;
		break;
	}
}

void programSettings() {
	acout("������� ��������� ���������:");
	acout("  0 - ����� � ������� ����");
	acout("  1 - \"��������\" ������: ", false);
	if (programsettings[0] == 1) acout("��������");
	else acout("���������");
	acout("  2 - ����� ���������: ", false);
	if (programsettings[1] == 1) acout("��������������");
	else if (programsettings[1] == 2) acout("������");
	else acout("������ ������ � ����");
	acout("  3 - �������� ���������� ���� � �������������� ������: ", false); acout(to_string(programsettings[2]), false); acout("��");
	acout("��� �� ������ ��������: ", false);
	string input; int iinput;
	cin >> input;
	try
	{
		iinput = stoi(input);
	} catch (...) {
		system("cls");
		int animation = programsettings[0];
		programsettings[0] = 0;
		programSettings();
		programsettings[0] = animation;
		return;
	}
programchoose:
	switch (iinput)
	{
	case 0:
		system("cls");
		save();
		return;
	case 1:
		if (programsettings[0] == 1) programsettings[0] = 0;
		else programsettings[0] = 1;
		system("cls");
		programSettings();
		return;
	case 2:
		acout("�������� ����� ���������: ");
		acout("  1 - ��������������"); acout("  2 - ������"); acout("  3 - ������ ������ � ����"); acout("��� �����: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto programchoose;
		}
		if (iinput > 0 && iinput < 4) programsettings[1] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto programchoose;
		}
		system("cls");
		programSettings();
		return;
	case 3:
		acout("��������! �� ������������� ������� �������� ������ 100��!");
		acout("������� ����� �������� ��������� � �������������: ", false);
		cin >> input;
		try {
			iinput = stoi(input);
		} catch (...) {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto programchoose;
		}
		if (iinput > -1) programsettings[2] = iinput;
		else {
			acout("�������� ����! ���������� �����.");
			system("pause");
			system("cls");
			goto programchoose;
		}
		system("cls");
		programSettings();
		return;
	default:
		acout("�������� ����! ���������� �����.");
		system("pause");
		system("cls");
		goto programchoose;
		break;
	}
}
void help() {
	acout("������ ��������� ��������� ������������ ��������� ������� ����������� ����������� ������ ������� � 1970-�� ����");
	acout("������ �� ������ � ���������: https://ru.wikipedia.org/wiki/����_\"�����\"");
	acout("");
	acout("�������� ������� ����:");
	acout("����� �������� ���� - ����, ������ �������� ������������ �������������");
	acout("������ ������ ������ ���� ���� ������ �������� ������. ��� ����� ���� ������ ��� �� ������");
	acout("� ������ ��������� ������ ��������� �������� ��������� �������, ��������� ����� ��������");
	acout("�� ��������� ������ ������ ������ ���� 2 ��� 3 ����� ������, ��� ���� ����� ��� ���������� ����");
	acout("����� ������ ������� �� ��������������� ��� �� �����������");
	acout("� ������ ������ ����� ����� ��������� �����, ���� ������ �� ����� 3 ������(�� ���������)");
	acout("��������� ������������ ����:");
	acout(" - ���������� ��� - �� ��������� �������� �������������");
	acout(" - ��� ������ �� ���� ������");
	acout("");
	acout("����� ��������� ����� ������������ ��������� ������� ������");
	acout("������� ���������� ������:");
	acout("  ", false, 255); acout(" - ��������� ������� ������ ����� 0");
	acout("  ", false, 187); acout(" - ��������� ������� ������ �� 1 �� 3");
	acout("  ", false, 153); acout(" - ��������� ������� ������ �� 4 �� 6");
	acout("  ", false, 17); acout(" - ��������� ������� ������ �� 7 �� 10");
	acout("  ", false, 102); acout(" - ��������� ������� ������ ����� 10");
	acout("  ", false, 204); acout(" - ��������� ������� ������ ����� 11");
	acout("  ", false, 68); acout(" - ��������� ������� ������ ����� 12");
	
	acout("\n");
	acout("��������� ��������� � ��������� �������� � ������������� ����\n");
	acout("�����������: ������ ��������");
	system("pause");
}
void CUI() {
	string input;
	int choose, nofgens;
	system("cls");
	acout("��������� \"�����\" v0.1.0");
	acout("  0 - ����� �� ���������");
	acout("  1 - ���������");
	acout("  2 - ��������� ���������");
	acout("  3 - ��������� ���������");
	acout("  4 - ������");
	acout("��� �����: ", false);
	cin >> input;
	try {
		choose = stoi(input);
	} catch (...) {
		acout("�������� ����! ���������� �����.");
		system("pause");
		system("cls");
		CUI();
		return;
	}

	system("cls");
	switch (choose) {
	case 0:
		save();
		system("cls");
		return;
	case 1:
		cout << "������� ���������� ���������: ";
		cin >> input;
		try {
			nofgens = stoi(input);
		} catch (...) {
			cout << "�������� ����! ���������� �����.\n";
			system("pause");
			system("cls");
			CUI();
			return;
		}
		system("cls");
		simulation(nofgens);
		CUI();
		return;
	case 2:
		simSettings();
		CUI();
		return;
	case 3:
		programSettings();
		CUI();
		return;
	case 4:
		help();
		CUI();
		return;
	default:
		acout("�� ������� ����� �� �� ������! ����������, ������ ����������� ��� �����.");
		system("pause");
		CUI();
		return;
	}
}

void simout(int **current) {
	//TODO: ����������� ��������� ������ ������� ����
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int y = 0; y < simsettings[1]; y++) {
		for (int x = 0; x < simsettings[0]; x++) {
			if (current[y][x] > 0 && current[y][x] < 4) SetConsoleTextAttribute(hConsole, 187);
			else if (current[y][x] > 3 && current[y][x] < 7) SetConsoleTextAttribute(hConsole, 153);
			else if (current[y][x] > 6 && current[y][x] < 10) SetConsoleTextAttribute(hConsole, 17);
			else if (current[y][x] == -1) SetConsoleTextAttribute(hConsole, 0);
			else if (current[y][x] == 0) SetConsoleTextAttribute(hConsole, 255);
			else if (current[y][x] == 10) SetConsoleTextAttribute(hConsole, 102);
			else if (current[y][x] == 11) SetConsoleTextAttribute(hConsole, 204);
			else if (current[y][x] == 12) SetConsoleTextAttribute(hConsole, 68);
			cout << "  ";
		}
		SetConsoleTextAttribute(hConsole, 15);
		cout << '\n';
	}
	SetConsoleTextAttribute(hConsole, 15);
}